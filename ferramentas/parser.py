# -*- coding: utf-8 -*-
"""
Parser (Analisador Sintático) para Linguagem BR
Usa descida recursiva com Pratt Parsing para expressões
"""
from typing import List, Optional, Dict, Any
from lexer import LexerBR, Token, TipoToken
from br_ast import *
from fstring_processor import processar_fstring

class ErroSintaxe(Exception):
    def __init__(self, mensagem: str, linha: int, coluna: int):
        self.mensagem = mensagem
        self.linha = linha
        self.coluna = coluna
        super().__init__(f"Erro Sintático [Linha {linha}, Coluna {coluna}]: {mensagem}")

class ParserBR:
    def __init__(self, tokens):
        self.tokens = tokens
        self.posicao = 0
        self.erros = []
        self.ultimo_bloco_fechou_com_fim = False
        
    def token_atual(self) -> Token:
        if self.posicao < len(self.tokens):
            return self.tokens[self.posicao]
        return self.tokens[-1]
    
    def avancar(self) -> Token:
        if self.posicao < len(self.tokens):
            token = self.tokens[self.posicao]
            self.posicao += 1
            return token
        return self.tokens[-1]
    
    def verificar_identificador(self) -> bool:
        token = self.token_atual()
        tipo = token.tipo
        if tipo == TipoToken.IDENTIFICADOR:
            return True
        ignorar_como_id = {
            TipoToken.FIM, TipoToken.SE, TipoToken.ENTAO, TipoToken.SENAO,
            TipoToken.ENQUANTO, TipoToken.PARA, TipoToken.CADA, TipoToken.EM,
            TipoToken.REPITA, TipoToken.VEZES, TipoToken.CONTE, TipoToken.ATE,
            TipoToken.DE, TipoToken.COMO, TipoToken.E,
            TipoToken.TENTE, TipoToken.CAPTURE, TipoToken.DEVOLVA,
            TipoToken.MOSTRE, TipoToken.ESPERE, TipoToken.EXECUTE, TipoToken.PARALELO,
            TipoToken.CONSTANTE, TipoToken.IMPORT, TipoToken.IMPORTE,
            TipoToken.LITERAL_VERDADEIRO, TipoToken.LITERAL_FALSO, TipoToken.LITERAL_NULO,
            TipoToken.EOF
        }
        if tipo not in ignorar_como_id:
            valor = token.valor
            if valor and (valor[0].isalpha() or valor[0] == '_'):
                token.tipo = TipoToken.IDENTIFICADOR
                return True
        return False

    def verificar(self, tipo: TipoToken) -> bool:
        if tipo == TipoToken.IDENTIFICADOR:
            return self.verificar_identificador()
        return self.token_atual().tipo == tipo
    
    def consumir(self, tipo: TipoToken, mensagem: str = "") -> Token:
        if self.verificar(tipo):
            return self.avancar()
        token = self.token_atual()
        raise ErroSintaxe(mensagem or f"Esperava {tipo.name}, mas encontrou {token.tipo.name}", token.linha, token.coluna)
    
    def verificar_palavra(self, palavra: str) -> bool:
        return self.token_atual().tipo == TipoToken.IDENTIFICADOR and self.token_atual().valor == palavra
    
    def consumir_palavra(self, palavra: str) -> Token:
        if self.verificar_palavra(palavra):
            return self.avancar()
        token = self.token_atual()
        raise ErroSintaxe(f"Esperava '{palavra}', mas encontrou '{token.valor}'", token.linha, token.coluna)

    def pular_novas_linhas(self):
        while self.verificar(TipoToken.NOVA_LINHA):
            self.avancar()

    def eh_fim_instrucao(self):
        return self.token_atual().tipo in {
            TipoToken.NOVA_LINHA,
            TipoToken.INDENTAR,
            TipoToken.DESINDENTAR,
            TipoToken.FIM,
            TipoToken.EOF,
        }

    def consumir_indentacao_bloco(self, mensagem: str):
        self.pular_novas_linhas()
        return self.consumir(TipoToken.INDENTAR, mensagem)
    
    def sincronizar(self):
        while not self.verificar(TipoToken.EOF):
            if self.token_atual().tipo == TipoToken.NOVA_LINHA:
                self.avancar()
                return
            if self.token_atual().tipo == TipoToken.DESINDENTAR:
                return
            if self.token_atual().tipo == TipoToken.FIM:
                self.avancar()
                return
            self.avancar()
    
    def parse(self):
        declaracoes = []
        while not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.EOF):
                break
            try:
                decl = self.declaracao()
                if decl:
                    declaracoes.append(decl)
            except ErroSintaxe as e:
                print(str(e))
                self.erros.append(e)
                posicao_antes = self.posicao
                self.sincronizar()
                if self.posicao == posicao_antes and not self.verificar(TipoToken.EOF):
                    self.avancar()
        return declaracoes
    
    def declaracao(self):
        self.pular_novas_linhas()
        if self.verificar(TipoToken.EXPLICACAO):
            return self.bloco_explicacao()
        if self.verificar(TipoToken.IMPORT) or self.verificar(TipoToken.DE):
            return self.declaracao_importacao()
        if self.verificar(TipoToken.OBJETIVO):
            return self.bloco_objetivo()
        if self.verificar(TipoToken.TESTE):
            return self.declaracao_teste()
        if self.verificar(TipoToken.FUNCAO):
            return self.declaracao_funcao()
        if self.verificar(TipoToken.CLASSE):
            return self.declaracao_classe()
        if self.verificar(TipoToken.CONSTANTE):
            return self.declaracao_constante()
        if self.verificar(TipoToken.VARIAVEL) or self._parece_declaracao_variavel_explicita():
            return self.declaracao_variavel_explicita()
        if self.verificar(TipoToken.TENTE):
            return self.tente_capture_stmt()
        if self.verificar(TipoToken.SE):
            return self.se_stmt()
        if self.verificar(TipoToken.ENQUANTO):
            return self.enquanto_stmt()
        if self.verificar(TipoToken.PARA):
            return self.para_cada_stmt()
        if self.verificar(TipoToken.EXECUTE):
            return self.execute_paralelo_stmt()
        if self.verificar(TipoToken.PARE):
            return self.pare_stmt()
        if self.verificar(TipoToken.REPITA):
            return self.repita_vezes_stmt()
        if self.verificar(TipoToken.CONTE):
            return self.conte_stmt()
        if self.verificar(TipoToken.ESCOLHA):
            return self.escolha_caso_stmt()
        if self.verificar(TipoToken.CORRESPONDA):
            return self.correspondencia_stmt()
        if self.verificar(TipoToken.TAREFA):
            return self.declaracao_tarefa()
        return self.declaracao_variavel_ou_instrucao()
    
    def bloco_explicacao(self):
        inicio = self.avancar()
        linhas = []
        while not self.verificar(TipoToken.FIM) and not self.verificar(TipoToken.EOF):
            linhas.append(self.token_atual().valor)
            self.avancar()
        self.consumir(TipoToken.FIM, "Esperava 'fim' para fechar o bloco explicacao")
        return BlocoExplicacao(Posicao(inicio.linha, inicio.coluna), "\n".join(linhas))
    
    def bloco_objetivo(self):
        inicio = self.avancar()
        linhas = []
        while not self.verificar(TipoToken.FIM) and not self.verificar(TipoToken.EOF):
            linhas.append(self.token_atual().valor)
            self.avancar()
        self.consumir(TipoToken.FIM, "Esperava 'fim' para fechar o bloco objetivo")
        return BlocoObjetivo(Posicao(inicio.linha, inicio.coluna), "\n".join(linhas))
    
    def declaracao_importacao(self):
        inicio = self.avancar()
        if inicio.tipo == TipoToken.IMPORT:
            modulo_token = self.consumir(TipoToken.LITERAL_TEXTO, "Esperava nome do modulo entre aspas")
            alias = None
            if self.verificar(TipoToken.COMO):
                self.avancar()
                alias = self._nome_importacao("Esperava alias do modulo")
            return DeclaracaoImportacao(Posicao(inicio.linha, inicio.coluna), modulo_token.valor, alias=alias)
        modulo_token = self.consumir(TipoToken.LITERAL_TEXTO, "Esperava nome do modulo entre aspas")
        self.consumir(TipoToken.IMPORTE, "Esperava 'importe' apos nome do modulo")
        simbolos = []
        while not self.verificar(TipoToken.EOF) and not self.verificar(TipoToken.NOVA_LINHA) and not self.verificar(TipoToken.INDENTAR) and not self.verificar(TipoToken.DESINDENTAR):
            simbolos.append(self._nome_importacao("Esperava nome importado"))
            if self.verificar(TipoToken.E) or self.verificar(TipoToken.VIRGULA):
                self.avancar()
                continue
            break
        return DeclaracaoImportacao(Posicao(inicio.linha, inicio.coluna), modulo_token.valor, simbolos=simbolos)
    
    def _nome_importacao(self, mensagem: str) -> str:
        if self.verificar(TipoToken.LITERAL_TEXTO) or self.verificar(TipoToken.IDENTIFICADOR):
            return self.avancar().valor
        token = self.token_atual()
        raise ErroSintaxe(mensagem, token.linha, token.coluna)
    
    def declaracao_teste(self):
        inicio = self.avancar()
        nome_token = self.consumir(TipoToken.LITERAL_TEXTO, "Esperava nome do teste entre aspas")
        corpo = self.bloco()
        return DeclaracaoTeste(Posicao(inicio.linha, inicio.coluna), nome_token.valor, corpo)
    
    def declaracao_funcao(self):
        inicio = self.avancar()
        nome_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da funcao")
        parametros = []
        if self.verificar(TipoToken.RECEBENDO) or self.verificar_palavra("recebendo"):
            self.avancar()
            while not self.verificar(TipoToken.FIM) and not self.verificar(TipoToken.NOVA_LINHA) and not self.verificar(TipoToken.INDENTAR):
                param_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome do parametro")
                parametros.append(param_token.valor)
                if self.verificar(TipoToken.E) or self.verificar(TipoToken.VIRGULA):
                    self.avancar()
        corpo = self.bloco()
        return DeclaracaoFuncao(Posicao(inicio.linha, inicio.coluna), nome_token.valor, parametros, corpo)
    
    def declaracao_classe(self):
        inicio = self.avancar()
        nome_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da classe")
        classe_mae = None
        if self.verificar(TipoToken.HERDA):
            self.avancar()
            classe_mae = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da classe base").valor
        self.consumir_indentacao_bloco("Esperava indentacao no corpo da classe")
        campos = []
        metodos = []
        while not self.verificar(TipoToken.DESINDENTAR) and not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.DESINDENTAR) or self.verificar(TipoToken.EOF):
                break
            if self.verificar(TipoToken.METODO) or self.verificar_palavra("metodo") or self.verificar_palavra("método"):
                self.avancar()
                metodo_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome do metodo")
                params_metodo = []
                if self.verificar(TipoToken.RECEBENDO) or self.verificar_palavra("recebendo"):
                    self.avancar()
                    while not self.verificar(TipoToken.FIM) and not self.verificar(TipoToken.NOVA_LINHA) and not self.verificar(TipoToken.INDENTAR):
                        param_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome do parametro")
                        params_metodo.append(param_token.valor)
                        if self.verificar(TipoToken.E) or self.verificar(TipoToken.VIRGULA):
                            self.avancar()
                corpo_metodo = self.bloco()
                metodos.append(DeclaracaoFuncao(Posicao(metodo_token.linha, metodo_token.coluna), metodo_token.valor, params_metodo, corpo_metodo))
                continue
            if self.verificar(TipoToken.IDENTIFICADOR):
                campo = self.avancar().valor
                campos.append(campo)
                if self.verificar(TipoToken.DOIS_PONTOS):
                    self.avancar()
                    if self.verificar(TipoToken.IDENTIFICADOR) or self.verificar(TipoToken.TEXTO) or self.verificar(TipoToken.INTEIRO) or self.verificar(TipoToken.DECIMAL) or self.verificar(TipoToken.BOOLEANO):
                        self.avancar()
                continue
            self.avancar()
        self.consumir(TipoToken.DESINDENTAR, "Esperava fim do corpo da classe")
        if self.verificar(TipoToken.FIM):
            self.avancar()
        classe = DeclaracaoClasse(Posicao(inicio.linha, inicio.coluna), nome_token.valor, campos, classe_mae=classe_mae)
        classe.metodos = metodos
        return classe
    
    def declaracao_constante(self):
        inicio = self.avancar()
        nome_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da constante")
        self.consumir(TipoToken.IGUAL, "Esperava 'e' apos nome da constante")
        valor = self.expressao()
        return DeclaracaoVariavel(Posicao(inicio.linha, inicio.coluna), nome_token.valor, None, valor, eh_constante=True)
    
    def declaracao_variavel_explicita(self):
        inicio = self.avancar()
        nome_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da variavel")
        tipo = None
        if self.verificar(TipoToken.DOIS_PONTOS):
            self.avancar()
            tipo_token = self.avancar()
            tipo = self._tipo_estatico(tipo_token)
        self.consumir(TipoToken.IGUAL, "Esperava '=' apos nome da variavel")
        valor = self.expressao()
        return DeclaracaoVariavel(Posicao(inicio.linha, inicio.coluna), nome_token.valor, tipo, valor)
    
    def _parece_declaracao_variavel_explicita(self):
        if not self.verificar_palavra("variavel"):
            return False
        lookahead = self.posicao + 1
        if lookahead >= len(self.tokens):
            return False
        if self.tokens[lookahead].tipo != TipoToken.IDENTIFICADOR:
            return False
        depois_nome = lookahead + 1
        if depois_nome >= len(self.tokens):
            return False
        return self.tokens[depois_nome].tipo in (TipoToken.IGUAL, TipoToken.DOIS_PONTOS)
    
    def _tipo_estatico(self, tipo_token):
        tipo_map = {"inteiro": TipoBR.INTEIRO, "decimal": TipoBR.DECIMAL, "texto": TipoBR.TEXTO, "booleano": TipoBR.BOOLEANO, "lista": TipoBR.LISTA, "registro": TipoBR.REGISTRO}
        return tipo_map.get(tipo_token.valor, TipoBR.DESCONHECIDO)
    
    def declaracao_variavel_ou_instrucao(self):
        if self.verificar(TipoToken.IDENTIFICADOR):
            lookahead = self.posicao + 1
            if lookahead < len(self.tokens):
                prox_token = self.tokens[lookahead]
                if prox_token.tipo == TipoToken.DOIS_PONTOS:
                    return self.declaracao_variavel_com_tipo()
                elif prox_token.tipo == TipoToken.IGUAL:
                    return self.declaracao_variavel()
            atribuicao = self.tentar_atribuicao_alvo()
            if atribuicao:
                return atribuicao
        if self.verificar(TipoToken.ISTO):
            atribuicao = self.tentar_atribuicao_alvo()
            if atribuicao:
                return atribuicao
        return self.instrucao()
    
    def tentar_atribuicao_alvo(self):
        posicao_inicial = self.posicao
        try:
            alvo = self.chamada()
            if isinstance(alvo, (AcessoPropriedade, AcessoIndice)) and self.verificar(TipoToken.IGUAL):
                self.avancar()
                valor = self.expressao()
                return AtribuicaoAlvo(alvo.posicao, alvo, valor)
        except ErroSintaxe:
            pass
        self.posicao = posicao_inicial
        return None
    
    def declaracao_variavel_com_tipo(self):
        nome_token = self.avancar()
        self.consumir(TipoToken.DOIS_PONTOS, "Esperava ':' apos nome da variavel")
        tipo_token = self.avancar()
        tipo = self._tipo_estatico(tipo_token)
        self.consumir(TipoToken.IGUAL, "Esperava 'e' apos tipo da variavel")
        valor = self.expressao()
        return DeclaracaoVariavel(Posicao(nome_token.linha, nome_token.coluna), nome_token.valor, tipo, valor)
    
    def declaracao_variavel(self):
        nome_token = self.avancar()
        self.consumir(TipoToken.IGUAL, "Esperava 'e' apos nome da variavel")
        valor = self.expressao()
        return DeclaracaoVariavel(Posicao(nome_token.linha, nome_token.coluna), nome_token.valor, None, valor, eh_atribuicao=True)
    
    def instrucao(self):
        if self.verificar(TipoToken.MOSTRE):
            return self.mostre_stmt()
        if self.verificar(TipoToken.DEVOLVA):
            return self.retornar_stmt()
        if self.verificar(TipoToken.SALVE):
            return self.salve_stmt()
        if self.verificar(TipoToken.REMOVA):
            return self.remova_stmt()
        if self.verificar(TipoToken.ESPERE):
            return self.espere_stmt()
        return ExpressaoStmt(Posicao(self.token_atual().linha, self.token_atual().coluna), self.expressao())
    
    def mostre_stmt(self):
        inicio = self.avancar()
        return MostreStmt(Posicao(inicio.linha, inicio.coluna), self.expressao())
    
    def retornar_stmt(self):
        inicio = self.avancar()
        if self.eh_fim_instrucao():
            valor = LiteralNulo(Posicao(inicio.linha, inicio.coluna))
        else:
            valor = self.expressao()
        return RetornarStmt(Posicao(inicio.linha, inicio.coluna), valor)
    
    def salve_stmt(self):
        inicio = self.avancar()
        return SalveStmt(Posicao(inicio.linha, inicio.coluna), self.expressao())
    
    def remova_stmt(self):
        inicio = self.avancar()
        return RemovaStmt(Posicao(inicio.linha, inicio.coluna), self.expressao())
    
    def espere_stmt(self):
        inicio = self.avancar()
        valor = self.expressao_ate_igual_a()
        self.consumir_palavra("igual")
        self.consumir_palavra("a")
        esperado = self.expressao()
        return EspereStmt(Posicao(inicio.linha, inicio.coluna), valor, esperado)
    
    def expressao_ate_igual_a(self):
        inicio = self.posicao
        fim = inicio
        while fim < len(self.tokens) - 1 and self.tokens[fim].tipo not in {TipoToken.NOVA_LINHA, TipoToken.INDENTAR, TipoToken.DESINDENTAR, TipoToken.FIM, TipoToken.EOF}:
            if self.tokens[fim].valor == "igual" and self.tokens[fim + 1].valor == "a":
                break
            fim += 1
        if fim == inicio:
            token = self.token_atual()
            raise ErroSintaxe("Esperava expressao antes de 'igual a'", token.linha, token.coluna)
        sub_tokens = self.tokens[inicio:fim] + [Token(TipoToken.EOF, "EOF", self.tokens[fim-1].linha, self.tokens[fim-1].coluna)]
        sub_parser = ParserBR(sub_tokens)
        valor = sub_parser.expressao()
        self.posicao = fim
        return valor
    
    def se_stmt(self, eh_senao_se=False):
        inicio = self.avancar()
        condicao = self.expressao()
        self.consumir(TipoToken.ENTAO, "Esperava 'entao' apos condicao do se")
        corpo_se = self.bloco()
        fechou_com_fim = self.ultimo_bloco_fechou_com_fim
        corpo_senao = []
        if self.verificar(TipoToken.SENAO):
            self.avancar()
            if self.verificar(TipoToken.SE):
                corpo_senao = [self.se_stmt(eh_senao_se=True)]
                fechou_com_fim = True
            else:
                corpo_senao = self.bloco()
                fechou_com_fim = self.ultimo_bloco_fechou_com_fim
        if not eh_senao_se and not fechou_com_fim:
            token = self.token_atual()
            raise ErroSintaxe("Esperava 'fim' para fechar o bloco se", token.linha, token.coluna)
        return SeStmt(Posicao(inicio.linha, inicio.coluna), condicao, corpo_se, corpo_senao)
    
    def enquanto_stmt(self):
        inicio = self.avancar()
        condicao = self.expressao()
        return EnquantoStmt(Posicao(inicio.linha, inicio.coluna), condicao, self.bloco())
    
    def para_cada_stmt(self):
        inicio = self.avancar()
        self.consumir(TipoToken.CADA, "Esperava 'cada' apos 'para'")
        var_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da variavel")
        self.consumir(TipoToken.EM, "Esperava 'em' apos variavel do para cada")
        lista = self.expressao()
        return ParaCadaStmt(Posicao(inicio.linha, inicio.coluna), var_token.valor, lista, self.bloco())
    
    def tente_capture_stmt(self):
        inicio = self.avancar()
        corpo_tente = self.bloco()
        var_erro = "erro"
        corpo_capture = []
        if self.verificar(TipoToken.CAPTURE):
            self.avancar()
            if self.verificar(TipoToken.IDENTIFICADOR):
                var_erro = self.avancar().valor
            corpo_capture = self.bloco()
        return TenteCaptureStmt(Posicao(inicio.linha, inicio.coluna), corpo_tente, var_erro, corpo_capture)
    
    def execute_paralelo_stmt(self):
        inicio = self.avancar()
        self.consumir(TipoToken.EM, "Esperava 'em' apos 'execute'")
        self.consumir(TipoToken.PARALELO, "Esperava 'paralelo' apos 'execute em'")
        corpo = self.bloco()
        blocos = [[stmt] for stmt in corpo]
        return ExecuteParaleloStmt(Posicao(inicio.linha, inicio.coluna), blocos)
    
    def pare_stmt(self):
        inicio = self.avancar()
        return PareStmt(Posicao(inicio.linha, inicio.coluna))

    def repita_vezes_stmt(self):
        inicio = self.avancar()
        vezes = self.expressao()
        self.consumir(TipoToken.VEZES, "Esperava 'vezes' apos expressao do repita")
        return RepitaVezesStmt(Posicao(inicio.linha, inicio.coluna), vezes, self.bloco())
    
    def conte_stmt(self):
        inicio = self.avancar()
        self.consumir(TipoToken.DE, "Esperava 'de' apos 'conte'")
        inicio_expr = self.expressao()
        self.consumir(TipoToken.ATE, "Esperava 'ate' ou 'ate' apos expressao inicial")
        fim_expr = self.expressao()
        if self.verificar(TipoToken.COMO):
            self.avancar()
        else:
            self.consumir_palavra("como")
        var_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da variavel do contador")
        return ConteStmt(Posicao(inicio.linha, inicio.coluna), inicio_expr, fim_expr, var_token.valor, self.bloco())
    
    def escolha_caso_stmt(self):
        inicio = self.avancar()
        expressao = self.expressao()
        self.consumir_indentacao_bloco("Esperava indentacao no corpo do escolha")
        casos = []
        padrao = []
        while not self.verificar(TipoToken.DESINDENTAR) and not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.DESINDENTAR) or self.verificar(TipoToken.EOF):
                break
            if self.verificar(TipoToken.CASO):
                caso_inicio = self.avancar()
                valor = self.expressao()
                casos.append({"posicao": Posicao(caso_inicio.linha, caso_inicio.coluna), "valor": valor, "corpo": self.bloco()})
                continue
            if self.verificar(TipoToken.PADRAO):
                self.avancar()
                padrao = self.bloco()
                continue
            self.avancar()
        self.consumir(TipoToken.DESINDENTAR, "Esperava fim do corpo do escolha")
        if self.verificar(TipoToken.FIM):
            self.avancar()
        return EscolhaCasoStmt(Posicao(inicio.linha, inicio.coluna), expressao, casos, padrao)

    def correspondencia_stmt(self):
        inicio = self.avancar()
        valor = self.expressao()
        self.consumir_indentacao_bloco("Esperava indentacao no corpo do corresponda")
        casos = []
        padrao = []
        while not self.verificar(TipoToken.DESINDENTAR) and not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.DESINDENTAR) or self.verificar(TipoToken.EOF):
                break
            if self.verificar(TipoToken.CASO):
                caso_inicio = self.avancar()
                valor_caso = self.expressao()
                condicao_extra = None
                if self.verificar(TipoToken.SE):
                    self.avancar()
                    condicao_extra = self.expressao()
                casos.append({
                    "posicao": Posicao(caso_inicio.linha, caso_inicio.coluna),
                    "valor": valor_caso,
                    "corpo": self.bloco(),
                    "condicao": condicao_extra,
                })
                continue
            if self.verificar(TipoToken.PADRAO):
                self.avancar()
                padrao = self.bloco()
                continue
            self.avancar()
        self.consumir(TipoToken.DESINDENTAR, "Esperava fim do corpo do corresponda")
        if self.verificar(TipoToken.FIM):
            self.avancar()
        return CorrespondenciaStmt(Posicao(inicio.linha, inicio.coluna), valor, casos, padrao)
    
    def declaracao_tarefa(self):
        inicio = self.avancar()
        nome_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da tarefa")
        parametros = []
        if self.verificar(TipoToken.RECEBENDO) or self.verificar_palavra("recebendo"):
            self.avancar()
            while not self.verificar(TipoToken.FIM) and not self.verificar(TipoToken.NOVA_LINHA) and not self.verificar(TipoToken.INDENTAR):
                param_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome do parametro")
                parametros.append(param_token.valor)
                if self.verificar(TipoToken.E) or self.verificar(TipoToken.VIRGULA):
                    self.avancar()
        return DeclaracaoTarefa(Posicao(inicio.linha, inicio.coluna), nome_token.valor, parametros, self.bloco())
    
    def bloco(self):
        self.consumir_indentacao_bloco("Esperava indentacao apos declaracao")
        instrucoes = []
        while not self.verificar(TipoToken.DESINDENTAR) and not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.DESINDENTAR) or self.verificar(TipoToken.EOF):
                break
            try:
                stmt = self.declaracao()
                instrucoes.append(stmt)
            except ErroSintaxe as e:
                print(str(e))
                self.erros.append(e)
                posicao_antes = self.posicao
                self.sincronizar()
                if self.posicao == posicao_antes and not self.verificar(TipoToken.EOF):
                    self.avancar()
        self.consumir(TipoToken.DESINDENTAR, "Esperava fim do bloco")
        if self.verificar(TipoToken.FIM):
            self.avancar()
            self.ultimo_bloco_fechou_com_fim = True
        else:
            self.ultimo_bloco_fechou_com_fim = False
        return instrucoes
    
    # ============ Expressões (Pratt Parsing) ============
    
    def expressao(self):
        return self.associatividade()
    
    def associatividade(self):
        return self.binario_esquerda(self.soma, ["igual", "maior que", "menor que", "<", ">", "==", "!=", "<=", ">="])
    
    def soma(self):
        return self.binario_esquerda(self.multiplicacao, ["+", "-"])
    
    def multiplicacao(self):
        return self.binario_esquerda(self.unario, ["*", "/"])
    
    def binario_esquerda(self, func, ops):
        esquerda = func()
        while self.token_atual().valor in ops:
            op_token = self.avancar()
            direita = func()
            esquerda = Binario(Posicao(op_token.linha, op_token.coluna), esquerda, op_token.valor, direita)
        return esquerda
    
    def unario(self):
        if self.token_atual().valor in ["-", "nao"]:
            op_token = self.avancar()
            return Unario(Posicao(op_token.linha, op_token.coluna), op_token.valor, self.unario())
        return self.chamada()
    
    def chamada(self):
        expr = self.primario()
        while True:
            if self.verificar(TipoToken.PONTO):
                ponto = self.avancar()
                prop_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da propriedade")
                expr = AcessoPropriedade(Posicao(ponto.linha, ponto.coluna), expr, prop_token.valor)
            elif self.verificar(TipoToken.COLCHETE_ESQ):
                inicio = self.avancar()
                # Check if it's a slice starting with None, e.g. [:5] or [:]
                if self.verificar(TipoToken.DOIS_PONTOS):
                    self.avancar()
                    inicio_expr = LiteralNulo(Posicao(inicio.linha, inicio.coluna))
                    if self.verificar(TipoToken.COLCHETE_DIR):
                        fim_expr = LiteralNulo(Posicao(inicio.linha, inicio.coluna))
                    else:
                        fim_expr = self.expressao()
                    self.consumir(TipoToken.COLCHETE_DIR, "Esperava ']' apos fatia")
                    indice = FatiaExpr(Posicao(inicio.linha, inicio.coluna), inicio_expr, fim_expr)
                else:
                    indice = self.expressao()
                    # Check if there is a colon (slice) after the expression
                    if self.verificar(TipoToken.DOIS_PONTOS):
                        self.avancar()
                        if self.verificar(TipoToken.COLCHETE_DIR):
                            fim_expr = LiteralNulo(Posicao(inicio.linha, inicio.coluna))
                        else:
                            fim_expr = self.expressao()
                        self.consumir(TipoToken.COLCHETE_DIR, "Esperava ']' apos fatia")
                        indice = FatiaExpr(Posicao(inicio.linha, inicio.coluna), indice, fim_expr)
                    else:
                        self.consumir(TipoToken.COLCHETE_DIR, "Esperava ']' apos indice")
                expr = AcessoIndice(Posicao(inicio.linha, inicio.coluna), expr, indice)
            elif self.verificar(TipoToken.PARENTESE_ESQ):
                args = self.argumentos_parenteses()
                if isinstance(expr, AcessoPropriedade):
                    expr = ChamadaMetodo(expr.posicao, expr.objeto, expr.propriedade, args)
                elif isinstance(expr, Variavel):
                    expr = ChamadaFuncao(expr.posicao, expr.nome, args)
                else:
                    raise ErroSintaxe("Chamada invalida", self.token_atual().linha, self.token_atual().coluna)
            elif isinstance(expr, (Variavel, AcessoPropriedade)) and self._inicia_argumento_natural():
                args = self.argumentos_naturais()
                if isinstance(expr, AcessoPropriedade):
                    expr = ChamadaMetodo(expr.posicao, expr.objeto, expr.propriedade, args)
                else:
                    expr = ChamadaFuncao(expr.posicao, expr.nome, args)
            elif self.verificar(TipoToken.IDENTIFICADOR):
                lookahead = self.posicao + 1
                if lookahead < len(self.tokens) and self.tokens[lookahead].tipo in [TipoToken.LITERAL_TEXTO, TipoToken.IDENTIFICADOR]:
                    metodo_token = self.avancar()
                    args = []
                    while not self.verificar(TipoToken.EOF) and not self.verificar(TipoToken.NOVA_LINHA) and not self.verificar(TipoToken.INDENTAR) and not self.verificar(TipoToken.DESINDENTAR):
                        try:
                            arg = self.expressao()
                            args.append(arg)
                            if self.verificar(TipoToken.E):
                                self.avancar()
                            else:
                                break
                        except:
                            break
                    expr = ChamadaMetodo(Posicao(metodo_token.linha, metodo_token.coluna), expr, metodo_token.valor, args)
                    continue
                break
            else:
                break
        return expr

    def _inicia_argumento_natural(self):
        return self.token_atual().tipo in {
            TipoToken.IDENTIFICADOR,
            TipoToken.LITERAL_TEXTO,
            TipoToken.LITERAL_INTEIRO,
            TipoToken.LITERAL_DECIMAL,
            TipoToken.LITERAL_VERDADEIRO,
            TipoToken.LITERAL_FALSO,
            TipoToken.LITERAL_NULO,
            TipoToken.AGUARDE,
            TipoToken.LISTA,
            TipoToken.REGISTRO,
            TipoToken.SUBTRACAO,
            TipoToken.NAO,
            TipoToken.PARENTESE_ESQ,
            TipoToken.COLCHETE_ESQ,
            TipoToken.CHAVE_ESQ,
        }

    def argumentos_naturais(self):
        args = [self.expressao()]
        while self.verificar(TipoToken.VIRGULA) or self.verificar(TipoToken.E):
            self.avancar()
            if self.eh_fim_instrucao():
                break
            args.append(self.expressao())
        return args
    
    def argumentos_parenteses(self):
        self.consumir(TipoToken.PARENTESE_ESQ, "Esperava '('")
        self.pular_novas_linhas()
        args = []
        if self.verificar(TipoToken.PARENTESE_DIR):
            self.avancar()
            return args
        while not self.verificar(TipoToken.PARENTESE_DIR) and not self.verificar(TipoToken.EOF):
            args.append(self.expressao())
            if self.verificar(TipoToken.VIRGULA) or self.verificar(TipoToken.E):
                self.avancar()
                self.pular_novas_linhas()
                continue
            break
        self.pular_novas_linhas()
        self.consumir(TipoToken.PARENTESE_DIR, "Esperava ')' apos argumentos")
        return args
    
    def primario(self):
        token = self.token_atual()
        if self.verificar(TipoToken.LITERAL_INTEIRO):
            return LiteralInteiro(Posicao(token.linha, token.coluna), int(self.avancar().valor))
        if self.verificar(TipoToken.LITERAL_DECIMAL):
            return LiteralDecimal(Posicao(token.linha, token.coluna), float(self.avancar().valor))
        if self.verificar(TipoToken.LITERAL_TEXTO):
            return LiteralTexto(Posicao(token.linha, token.coluna), self.avancar().valor)
        if self.verificar(TipoToken.FSTRING):
            token_fstring = self.avancar()
            return processar_fstring(token_fstring)
        if self.verificar(TipoToken.LITERAL_VERDADEIRO):
            self.avancar()
            return LiteralBooleano(Posicao(token.linha, token.coluna), True)
        if self.verificar(TipoToken.LITERAL_FALSO):
            self.avancar()
            return LiteralBooleano(Posicao(token.linha, token.coluna), False)
        if self.verificar(TipoToken.LITERAL_NULO):
            self.avancar()
            return LiteralNulo(Posicao(token.linha, token.coluna))
        if self.verificar(TipoToken.COLCHETE_ESQ):
            return self.lista_inline()
        if self.verificar(TipoToken.CHAVE_ESQ):
            return self.registro_inline()
        if self.verificar(TipoToken.LISTA):
            return self.lista_literal()
        if self.verificar(TipoToken.REGISTRO):
            return self.registro_literal()
        if self.verificar(TipoToken.AGUARDE):
            inicio = self.avancar()
            return ExpressaoTarefa(Posicao(inicio.linha, inicio.coluna), self.expressao())
        if self.verificar(TipoToken.BUSQUE):
            return self.busque_expr()
        if self.verificar(TipoToken.ISTO):
            self.avancar()
            return Variavel(Posicao(token.linha, token.coluna), "isto")
        if self.verificar_palavra("pergunte"):
            return self.ia_pergunte()
        if self.verificar_palavra("crie"):
            return self.ia_agente()
        if self.verificar(TipoToken.IDENTIFICADOR):
            return Variavel(Posicao(token.linha, token.coluna), self.avancar().valor)
        if self.verificar(TipoToken.PARENTESE_ESQ):
            self.avancar()
            expr = self.expressao()
            self.consumir(TipoToken.PARENTESE_DIR, "Esperava ')' apos expressao")
            return expr
        raise ErroSintaxe(f"Expressao inesperada: {token.tipo.name}", token.linha, token.coluna)

    def busque_expr(self):
        inicio = self.avancar()
        while not self.eh_fim_instrucao():
            self.avancar()
        return ChamadaMetodo(Posicao(inicio.linha, inicio.coluna), None, "busque", [])
    
    def lista_inline(self):
        inicio = self.avancar()
        elementos = []
        if self.verificar(TipoToken.COLCHETE_DIR):
            self.avancar()
            return ListaLiteral(Posicao(inicio.linha, inicio.coluna), elementos)
        pos_salva = self.posicao
        try:
            primeira_expr = self.expressao()
            if self.verificar(TipoToken.PARA):
                self.avancar()
                self.consumir(TipoToken.CADA, "Esperava 'cada'")
                var_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava variavel")
                self.consumir(TipoToken.EM, "Esperava 'em'")
                iteravel = self.expressao()
                condicao = None
                if self.verificar(TipoToken.SE):
                    self.avancar()
                    condicao = self.expressao()
                self.consumir(TipoToken.COLCHETE_DIR, "Esperava ']'")
                return ComprehensionStmt(
                    Posicao(inicio.linha, inicio.coluna),
                    primeira_expr,
                    var_token.valor,
                    iteravel,
                    condicao,
                )
        except ErroSintaxe:
            pass
        self.posicao = pos_salva
        while not self.verificar(TipoToken.COLCHETE_DIR) and not self.verificar(TipoToken.EOF):
            elementos.append(self.expressao())
            if self.verificar(TipoToken.VIRGULA) or self.verificar(TipoToken.E):
                self.avancar()
                continue
            break
        self.consumir(TipoToken.COLCHETE_DIR, "Esperava ']' apos lista")
        return ListaLiteral(Posicao(inicio.linha, inicio.coluna), elementos)
    
    def registro_inline(self):
        inicio = self.avancar()
        propriedades = {}
        if self.verificar(TipoToken.CHAVE_DIR):
            self.avancar()
            return RegistroLiteral(Posicao(inicio.linha, inicio.coluna), propriedades)
        while not self.verificar(TipoToken.CHAVE_DIR) and not self.verificar(TipoToken.EOF):
            chave = str(self._chave_registro_inline())
            self.consumir(TipoToken.DOIS_PONTOS, "Esperava ':' apos chave do registro")
            propriedades[chave] = self.expressao()
            if self.verificar(TipoToken.VIRGULA) or self.verificar(TipoToken.E):
                self.avancar()
                continue
            break
        self.consumir(TipoToken.CHAVE_DIR, "Esperava '}' apos registro")
        return RegistroLiteral(Posicao(inicio.linha, inicio.coluna), propriedades)
    
    def _chave_registro_inline(self):
        token = self.token_atual()
        tipos_chave = {TipoToken.LITERAL_TEXTO, TipoToken.IDENTIFICADOR, TipoToken.TEXTO, TipoToken.INTEIRO, TipoToken.DECIMAL, TipoToken.BOOLEANO, TipoToken.LISTA, TipoToken.REGISTRO}
        if token.tipo in tipos_chave:
            return self.avancar().valor
        raise ErroSintaxe("Esperava chave de registro", token.linha, token.coluna)
    
    def lista_literal(self):
        inicio = self.avancar()
        self.consumir_indentacao_bloco("Esperava indentacao apos 'lista'")
        elementos = []
        while not self.verificar(TipoToken.DESINDENTAR) and not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.DESINDENTAR) or self.verificar(TipoToken.EOF):
                break
            elementos.append(self.expressao())
        self.consumir(TipoToken.DESINDENTAR, "Esperava fim da lista")
        self.consumir(TipoToken.FIM, "Esperava 'fim' apos lista")
        return ListaLiteral(Posicao(inicio.linha, inicio.coluna), elementos)
    
    def registro_literal(self):
        inicio = self.avancar()
        self.consumir_indentacao_bloco("Esperava indentacao apos 'registro'")
        propriedades = {}
        while not self.verificar(TipoToken.DESINDENTAR) and not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.DESINDENTAR) or self.verificar(TipoToken.EOF):
                break
            prop_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperava nome da propriedade")
            self.consumir(TipoToken.DOIS_PONTOS, "Esperava ':' apos nome da propriedade")
            propriedades[prop_token.valor] = self.expressao()
        self.consumir(TipoToken.DESINDENTAR, "Esperava fim do registro")
        self.consumir(TipoToken.FIM, "Esperava 'fim' apos registro")
        return RegistroLiteral(Posicao(inicio.linha, inicio.coluna), propriedades)
    
    def ia_pergunte(self):
        inicio = self.avancar()
        if self.verificar(TipoToken.PARA):
            self.avancar()
        else:
            self.consumir_palavra("para")
        self.consumir_palavra("ia")
        self.pular_novas_linhas()
        if self.verificar(TipoToken.INDENTAR):
            self.consumir_indentacao_bloco("Esperava indentacao apos 'pergunte para ia'")
            self.pular_novas_linhas()
            prompt = self.expressao()
            self.pular_novas_linhas()
            self.consumir(TipoToken.DESINDENTAR, "Esperava fim do bloco de pergunta")
            self.consumir(TipoToken.FIM, "Esperava 'fim' apos pergunta para ia")
            return IAPergunte(Posicao(inicio.linha, inicio.coluna), prompt)
        return IAPergunte(Posicao(inicio.linha, inicio.coluna), self.expressao())
    
    def ia_agente(self):
        inicio = self.avancar()
        self.consumir_palavra("agente")
        esp = "geral"
        if self.verificar(TipoToken.LITERAL_TEXTO):
            esp = self.avancar().valor
        self.consumir_indentacao_bloco("Esperava indentacao apos agente")
        props = {}
        while not self.verificar(TipoToken.DESINDENTAR) and not self.verificar(TipoToken.EOF):
            self.pular_novas_linhas()
            if self.verificar(TipoToken.DESINDENTAR) or self.verificar(TipoToken.EOF):
                break
            prop_token = self.consumir(TipoToken.IDENTIFICADOR, "Esperada propriedade do agente")
            if self.verificar(TipoToken.DOIS_PONTOS):
                self.avancar()
            props[prop_token.valor] = self.expressao()
        self.consumir(TipoToken.DESINDENTAR, "Esperava fim do agente")
        self.consumir(TipoToken.FIM, "Esperava 'fim' apos agente")
        return IAAgente(Posicao(inicio.linha, inicio.coluna), esp, props)

def parse_codigo(codigo):
    lexer = LexerBR(codigo)
    tokens = lexer.tokenizar()
    parser = ParserBR(tokens)
    return parser.parse()
