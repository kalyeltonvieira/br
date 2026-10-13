# -*- coding: utf-8 -*-
"""
Analisador Semântico e Sistema de Tipos para Linguagem BR
Implementa inferência de tipos baseada em Hindley-Milner modificada
"""
from typing import Dict, List, Optional, Set, Any
from br_ast import *
from dataclasses import dataclass

class ErroSemantico(Exception):
    def __init__(self, mensagem: str, linha: int, coluna: int):
        self.mensagem = mensagem
        self.linha = linha
        self.coluna = coluna
        super().__init__(f"Erro Semântico [Linha {linha}, Coluna {coluna}]: {mensagem}")

@dataclass
class VariavelInfo:
    nome: str
    tipo: Any = None
    eh_constante: bool = False
    foi_usada: bool = False
    escopo: int = 0

class AnalisadorSemanticoBR:
    def __init__(self):
        self.escopos = [{}]  # Pilha de escopos
        self.escopo_atual = 0
        self.funcoes = {}  # nome -> {param: tipo}
        self.classes = {}  
        self.erros = []
        self.avisos = []
        
    def adicionar_erro(self, mensagem: str, posicao):
        self.erros.append(ErroSemantico(mensagem, posicao.linha, posicao.coluna))
    
    def adicionar_aviso(self, mensagem: str):
        self.avisos.append(mensagem)
    
    def entrar_escopo(self):
        self.escopos.append({})
        self.escopo_atual += 1
    
    def sair_escopo(self):
        # Verificar variáveis não usadas
        escopo = self.escopos.pop()
        self.escopo_atual -= 1
        
        for nome, info in escopo.items():
            if not info.foi_usada and not nome.startswith("_"):
                self.adicionar_aviso(f"Variável '{nome}' declarada mas nunca usada (linha {info.escopo})")

    def declarar_variavel(self, nome: str, tipo, eh_constante: bool, posicao):
        # Verificar redeclaração no mesmo escopo
        escopo_atual = self.escopos[-1]
        if nome in escopo_atual:
            self.adicionar_erro(f"Variável '{nome}' já declarada neste escopo", posicao)
            return
        
        escopo_atual[nome] = VariavelInfo(
            nome=nome,
            tipo=tipo,
            eh_constante=eh_constante,
            escopo=posicao.linha
        )

    def atribuir_variavel(self, nome: str, tipo, posicao):
        for escopo in reversed(self.escopos):
            if nome in escopo:
                info = escopo[nome]
                if info.eh_constante:
                    self.adicionar_erro(f"Nao e possivel alterar constante '{nome}'", posicao)
                    return
                if info.tipo == TipoBR.DESCONHECIDO and tipo != TipoBR.DESCONHECIDO:
                    info.tipo = tipo
                return

        self.escopos[-1][nome] = VariavelInfo(
            nome=nome,
            tipo=tipo,
            eh_constante=False,
            escopo=posicao.linha
        )
    
    def obter_variavel(self, nome: str, posicao):
        for escopo in reversed(self.escopos):
            if nome in escopo:
                variavel = escopo[nome]
                variavel.foi_usada = True
                return variavel
        
        self.adicionar_erro(f"Variável '{nome}' não declarada", posicao)
        return None
    
    def inferir_tipo(self, no):
        """Inferência de tipos usando Hindley-Milner simplificado"""
        if isinstance(no, LiteralInteiro):
            return TipoBR.INTEIRO
        elif isinstance(no, LiteralDecimal):
            return TipoBR.DECIMAL
        elif isinstance(no, LiteralTexto):
            return TipoBR.TEXTO
        elif isinstance(no, LiteralBooleano):
            return TipoBR.BOOLEANO
        elif isinstance(no, LiteralNulo):
            return TipoBR.NULO
        elif isinstance(no, Variavel):
            info = self.obter_variavel(no.nome, no.posicao)
            return info.tipo if info else TipoBR.DESCONHECIDO
        elif isinstance(no, ListaLiteral):
            for elem in no.elementos:
                self.inferir_tipo(elem)
            return TipoBR.LISTA
        elif isinstance(no, ComprehensionStmt):
            self.inferir_tipo(no.iteravel)
            if no.condicao is not None:
                self.inferir_tipo(no.condicao)
            self.inferir_tipo(no.expressao)
            return TipoBR.LISTA
        elif isinstance(no, RegistroLiteral):
            return TipoBR.REGISTRO
        elif isinstance(no, Binario):
            tipo_esq = self.inferir_tipo(no.esquerda)
            tipo_dir = self.inferir_tipo(no.direita)
            
            # Operações aritméticas
            if no.operador in ["+", "-", "*", "/"]:
                if tipo_esq == TipoBR.DESCONHECIDO or tipo_dir == TipoBR.DESCONHECIDO:
                    return TipoBR.DESCONHECIDO
                if no.operador == "+" and (tipo_esq == TipoBR.TEXTO or tipo_dir == TipoBR.TEXTO):
                    return TipoBR.TEXTO  # Concatenação com conversão para texto
                elif tipo_esq in [TipoBR.INTEIRO, TipoBR.DECIMAL] and tipo_dir in [TipoBR.INTEIRO, TipoBR.DECIMAL]:
                    return TipoBR.DECIMAL if (tipo_esq == TipoBR.DECIMAL or tipo_dir == TipoBR.DECIMAL) else TipoBR.INTEIRO
                else:
                    self.adicionar_erro(f"Operador '{no.operador}' não pode ser aplicado a tipos {tipo_esq.value} e {tipo_dir.value}", no.posicao)
                    return TipoBR.DESCONHECIDO
            
            # Operações de comparação
            elif no.operador in ["igual", "maior que", "menor que", "<", ">", "==", "!=", "<=", ">="]:
                if tipo_esq != tipo_dir and not (tipo_esq in [TipoBR.INTEIRO, TipoBR.DECIMAL] and tipo_dir in [TipoBR.INTEIRO, TipoBR.DECIMAL]):
                    self.adicionar_erro(f"Comparação entre tipos incompatíveis: {tipo_esq.value} e {tipo_dir.value}", no.posicao)
                return TipoBR.BOOLEANO
            
            return TipoBR.DESCONHECIDO
        
        elif isinstance(no, ChamadaFuncao):
            if no.nome in self.funcoes:
                return TipoBR.DESCONHECIDO  # Tipo de retorno não rastreado nesta versão
            info = self.obter_variavel(no.nome, no.posicao)
            if info:
                return TipoBR.DESCONHECIDO
            self.adicionar_erro(f"Função '{no.nome}' não declarada", no.posicao)
            return TipoBR.DESCONHECIDO
        
        elif isinstance(no, ChamadaMetodo):
            # Verificar se é um serviço nativo
            return TipoBR.DESCONHECIDO
        
        elif isinstance(no, AcessoPropriedade):
            tipo_obj = self.inferir_tipo(no.objeto)
            if tipo_obj in (TipoBR.REGISTRO, TipoBR.DESCONHECIDO):
                return TipoBR.DESCONHECIDO
            self.adicionar_erro(f"Tentativa de acessar propriedade em tipo não-registro: {tipo_obj.value}", no.posicao)
            return TipoBR.DESCONHECIDO
        
        elif isinstance(no, AcessoIndice):
            self.inferir_tipo(no.objeto)
            self.inferir_tipo(no.indice)
            return TipoBR.DESCONHECIDO

        elif isinstance(no, IAPergunte):
            return TipoBR.TEXTO
        
        elif isinstance(no, IAAgente):
            return TipoBR.REGISTRO
        
        elif isinstance(no, ConteStmt):
            self.inferir_tipo(no.inicio)
            self.inferir_tipo(no.fim)
            return TipoBR.INTEIRO
        
        elif isinstance(no, DeclaracaoTarefa):
            return TipoBR.DESCONHECIDO

        elif isinstance(no, ExpressaoTarefa):
            self.inferir_tipo(no.expressao)
            return TipoBR.DESCONHECIDO
        
        return TipoBR.DESCONHECIDO
    
    def verificar_tipo(self, no, tipo_esperado, posicao):
        """Verifica se o tipo inferido corresponde ao tipo esperado"""
        tipo_inferido = self.inferir_tipo(no)
        
        if tipo_esperado != TipoBR.DESCONHECIDO and tipo_inferido != TipoBR.DESCONHECIDO:
            # Coerção de int para decimal é permitida
            if tipo_esperado == TipoBR.DECIMAL and tipo_inferido == TipoBR.INTEIRO:
                return
            # Coerção de nulo para qualquer tipo é permitida
            if tipo_inferido == TipoBR.NULO:
                return
            if tipo_esperado != tipo_inferido:
                self.adicionar_erro(f"Tipo incompatível: esperava {tipo_esperado.value}, mas obteve {tipo_inferido.value}", posicao)
    
    def analisar(self, nos):
        """Analisa uma lista de nós AST"""
        self.erros = []
        self.avisos = []
        
        for no in nos:
            self.analisar_no(no)
        
        # Verificar variáveis não usadas no escopo global
        self.sair_escopo()
        
        return len(self.erros) == 0
    
    def analisar_no(self, no):
        if isinstance(no, DeclaracaoVariavel):
            tipo = no.tipo_estatico if no.tipo_estatico else self.inferir_tipo(no.valor)
            if getattr(no, "eh_atribuicao", False) and not no.eh_constante:
                self.atribuir_variavel(no.nome, tipo, no.posicao)
            else:
                self.declarar_variavel(no.nome, tipo, no.eh_constante, no.posicao)
            self.analisar_no(no.valor)
            
            if no.tipo_estatico:
                self.verificar_tipo(no.valor, no.tipo_estatico, no.posicao)

        elif isinstance(no, AtribuicaoAlvo):
            if isinstance(no.alvo, AcessoPropriedade):
                self.analisar_no(no.alvo.objeto)
            elif isinstance(no.alvo, AcessoIndice):
                self.analisar_no(no.alvo.objeto)
                self.analisar_no(no.alvo.indice)
            self.analisar_no(no.valor)
        
        elif isinstance(no, DeclaracaoFuncao):
            # Registrar função
            tipos_param = {}
            for param in no.parametros:
                tipos_param[param] = TipoBR.DESCONHECIDO
            self.funcoes[no.nome] = tipos_param
            
            # Analisar corpo da função
            self.entrar_escopo()
            for param in no.parametros:
                self.declarar_variavel(param, TipoBR.DESCONHECIDO, False, no.posicao)
            
            for stmt in no.corpo:
                self.analisar_no(stmt)
            
            self.sair_escopo()
        
        elif isinstance(no, DeclaracaoTeste):
            self.entrar_escopo()
            for stmt in no.corpo:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, BlocoExplicacao):
            pass  # Documentação, não precisa de análise
        
        elif isinstance(no, BlocoObjetivo):
            pass  # IA de compilação, não precisa de análise

        elif isinstance(no, DeclaracaoImportacao):
            if no.simbolos:
                for simbolo in no.simbolos:
                    self.declarar_variavel(simbolo, TipoBR.DESCONHECIDO, True, no.posicao)
            else:
                nome_local = no.alias or no.modulo.split("/")[-1].split("\\")[-1]
                if nome_local.endswith(".br"):
                    nome_local = nome_local[:-3]
                self.declarar_variavel(nome_local, TipoBR.DESCONHECIDO, True, no.posicao)

        elif isinstance(no, DeclaracaoClasse):
            self.declarar_variavel(no.nome, TipoBR.DESCONHECIDO, True, no.posicao)
            if no.metodos:
                for metodo in no.metodos:
                    self.funcoes[metodo.nome] = {p: TipoBR.DESCONHECIDO for p in metodo.parametros}
                    self.entrar_escopo()
                    self.declarar_variavel("isto", TipoBR.REGISTRO, False, metodo.posicao)
                    for param in metodo.parametros:
                        self.declarar_variavel(param, TipoBR.DESCONHECIDO, False, metodo.posicao)
                    for stmt in metodo.corpo:
                        self.analisar_no(stmt)
                    self.sair_escopo()
        
        elif isinstance(no, SeStmt):
            self.analisar_no(no.condicao)
            self.verificar_tipo(no.condicao, TipoBR.BOOLEANO, no.posicao)
            
            self.entrar_escopo()
            for stmt in no.corpo_se:
                self.analisar_no(stmt)
            self.sair_escopo()
            
            self.entrar_escopo()
            for stmt in no.corpo_senao:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, EnquantoStmt):
            self.analisar_no(no.condicao)
            self.verificar_tipo(no.condicao, TipoBR.BOOLEANO, no.posicao)
            
            self.entrar_escopo()
            for stmt in no.corpo:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, ParaCadaStmt):
            self.analisar_no(no.lista)
            tipo_lista = self.inferir_tipo(no.lista)
            
            self.entrar_escopo()
            tipo_elem = TipoBR.DESCONHECIDO
            if tipo_lista == TipoBR.LISTA:
                tipo_elem = TipoBR.DESCONHECIDO  # Não conseguimos inferir o tipo do elemento nesta versão
            self.declarar_variavel(no.variavel, tipo_elem, False, no.posicao)
            
            for stmt in no.corpo:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, TenteCaptureStmt):
            self.entrar_escopo()
            for stmt in no.corpo_tente:
                self.analisar_no(stmt)
            self.sair_escopo()
            
            self.entrar_escopo()
            self.declarar_variavel(no.variavel_erro, TipoBR.REGISTRO, False, no.posicao)
            for stmt in no.corpo_capture:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, ExecuteParaleloStmt):
            for bloco in no.blocos:
                self.entrar_escopo()
                for stmt in bloco:
                    self.analisar_no(stmt)
                self.sair_escopo()

        elif isinstance(no, RepitaVezesStmt):
            self.analisar_no(no.vezes)
            self.entrar_escopo()
            for stmt in no.corpo:
                self.analisar_no(stmt)
            self.sair_escopo()

        elif isinstance(no, EscolhaCasoStmt):
            self.analisar_no(no.expressao)
            for caso in no.casos:
                self.analisar_no(caso["valor"])
                self.entrar_escopo()
                for stmt in caso["corpo"]:
                    self.analisar_no(stmt)
                self.sair_escopo()
            self.entrar_escopo()
            for stmt in no.padrao:
                self.analisar_no(stmt)
            self.sair_escopo()

        elif isinstance(no, CorrespondenciaStmt):
            self.analisar_no(no.valor)
            for caso in no.casos:
                self.analisar_no(caso["valor"])
                guarda = caso.get("condicao")
                if guarda is not None:
                    self.analisar_no(guarda)
                    self.verificar_tipo(guarda, TipoBR.BOOLEANO, guarda.posicao)
                self.entrar_escopo()
                for stmt in caso["corpo"]:
                    self.analisar_no(stmt)
                self.sair_escopo()
            self.entrar_escopo()
            for stmt in no.padrao:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, MostreStmt):
            self.analisar_no(no.expressao)
        
        elif isinstance(no, RetornarStmt):
            self.analisar_no(no.valor)
        
        elif isinstance(no, SalveStmt):
            self.analisar_no(no.expressao)
        
        elif isinstance(no, RemovaStmt):
            self.analisar_no(no.expressao)
        
        elif isinstance(no, EspereStmt):
            self.analisar_no(no.valor)
            self.analisar_no(no.esperado)
        
        elif isinstance(no, ExpressaoStmt):
            self.analisar_no(no.expressao)
        
        elif isinstance(no, Binario):
            self.analisar_no(no.esquerda)
            self.analisar_no(no.direita)
            self.inferir_tipo(no)
        
        elif isinstance(no, Unario):
            self.analisar_no(no.operando)
            self.inferir_tipo(no)
        
        elif isinstance(no, ChamadaFuncao):
            for arg in no.argumentos:
                self.analisar_no(arg)
        
        elif isinstance(no, ChamadaMetodo):
            if no.objeto:
                self.analisar_no(no.objeto)
            for arg in no.argumentos:
                self.analisar_no(arg)
        
        elif isinstance(no, AcessoPropriedade):
            self.analisar_no(no.objeto)

        elif isinstance(no, AcessoIndice):
            self.analisar_no(no.objeto)
            self.analisar_no(no.indice)
        
        elif isinstance(no, ListaLiteral):
            for elem in no.elementos:
                self.analisar_no(elem)

        elif isinstance(no, ComprehensionStmt):
            self.analisar_no(no.iteravel)
            self.entrar_escopo()
            self.declarar_variavel(no.variavel, TipoBR.DESCONHECIDO, False, no.posicao)
            if no.condicao is not None:
                self.analisar_no(no.condicao)
                self.verificar_tipo(no.condicao, TipoBR.BOOLEANO, no.condicao.posicao)
            self.analisar_no(no.expressao)
            self.sair_escopo()
        
        elif isinstance(no, RegistroLiteral):
            for prop in no.propriedades.values():
                self.analisar_no(prop)
        
        elif isinstance(no, IAPergunte):
            self.analisar_no(no.prompt)
        
        elif isinstance(no, IAAgente):
            for prop in no.propriedades.values():
                self.analisar_no(prop)
        
        elif isinstance(no, ConteStmt):
            self.analisar_no(no.inicio)
            self.analisar_no(no.fim)
            self.entrar_escopo()
            self.declarar_variavel(no.variavel, TipoBR.INTEIRO, False, no.posicao)
            for stmt in no.corpo:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, DeclaracaoTarefa):
            self.funcoes[no.nome] = {p: TipoBR.DESCONHECIDO for p in no.parametros}
            self.entrar_escopo()
            for param in no.parametros:
                self.declarar_variavel(param, TipoBR.DESCONHECIDO, False, no.posicao)
            for stmt in no.corpo:
                self.analisar_no(stmt)
            self.sair_escopo()
        
        elif isinstance(no, ExpressaoTarefa):
            self.analisar_no(no.expressao)

def analisar_semantica(nos):
    analisador = AnalisadorSemanticoBR()
    analisador.entrar_escopo()  # Escopo global
    sucesso = analisador.analisar(nos)
    return sucesso, analisador.erros, analisador.avisos
