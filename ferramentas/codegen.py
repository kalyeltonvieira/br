# -*- coding: utf-8 -*-
"""
Gerador de Bytecode para Linguagem BR
Traduz AST otimizado para instruções da VM BR no formato BRC1
"""
import struct
from typing import List, Dict, Any, Optional
from br_ast import *

OPCODE_MAP = {
    "EMPILHAR_CONST": 1,
    "DEFINIR_VAR": 2,
    "OBTER_VAR": 3,
    "OBTER_PROPRIEDADE": 4,
    "SOMAR": 5,
    "SUBTRAIR": 6,
    "MULTIPLICAR": 7,
    "DIVIDIR": 8,
    "IGUAL_A": 9,
    "MAIOR_QUE": 10,
    "RETORNAR": 11,
    "ESPERE_IGUAL": 12,
    "CHAMAR_METODO": 13,
    "CHAMAR_SERVICO": 14,
    "CRIAR_LISTA_BLOCO": 15,
    "LISTA_ELEMENTO": 16,
    "CRIAR_REGISTRO_BLOCO": 17,
    "REGISTRO_PROPRIEDADE": 18,
    "IA_PERGUNTE_BLOCO": 19,
    "IA_AGENTE_BLOCO": 20,
    "TENTE_CAPTURE_BLOCO": 21,
    "PARALELO_EXECUTE": 22,
    "CONDICIONAL_SE": 23,
    "ENQUANTO_BLOCO": 24,
    "MENOR_QUE": 25,
    "IMPORTAR_MODULO": 26,
    "IMPORTAR_SIMBOLOS": 27,
    "CHAMAR_METODO_OBJETO": 28,
    "DECLARAR_CLASSE": 29,
    "REPITA_VEZES": 30,
    "ESCOLHA_CASO": 31,
    "DESCARTAR_TOPO": 32,
    "NAO_IGUAL": 33,
    "MAIOR_OU_IGUAL": 34,
    "MENOR_OU_IGUAL": 35,
    "OBTER_INDICE": 36,
    "ATRIBUIR_VAR": 37,
    "DEFINIR_PROPRIEDADE": 38,
    "DEFINIR_INDICE": 39,
    "CONTE_BLOCO": 40,
    "AGUARDAR_TAREFA": 41,
    "PARA_CADA_BLOCO": 42,
    "QUEBRAR_LOOP": 43,
}

class GeradorBytecodeBR:
    def __init__(self):
        self.constantes = []
        self.instrucoes = []
        self.funcoes = {}
        self.classes = {}
        self.testes = []
        self.documentacoes = []
        
    def adicionar_constante(self, valor: Any) -> int:
        """Adiciona uma constante ao pool e retorna seu índice"""
        for i, const in enumerate(self.constantes):
            if type(const) == type(valor) and const == valor:
                return i
        self.constantes.append(valor)
        return len(self.constantes) - 1
    
    def gerar(self, nos):
        """Gera bytecode a partir de uma lista de nós AST"""
        for no in nos:
            self.gerar_no(no)
        
        return {
            "constantes": self.constantes,
            "instrucoes": self.instrucoes,
            "funcoes": self.funcoes,
            "classes": self.classes,
            "testes": self.testes,
            "documentacoes": self.documentacoes
        }

    def gerar_instrucoes_no(self, no):
        instrucoes_salvas = self.instrucoes
        self.instrucoes = []
        self.gerar_no(no)
        instrucoes = self.instrucoes
        self.instrucoes = instrucoes_salvas
        return instrucoes

    def gerar_instrucoes_bloco(self, nos):
        instrucoes_salvas = self.instrucoes
        self.instrucoes = []
        for no in nos:
            self.gerar_no(no)
        instrucoes = self.instrucoes
        self.instrucoes = instrucoes_salvas
        return instrucoes

    def gerar_fluxo_correspondencia(self, var_temp, casos, padrao):
        fluxo = self.gerar_instrucoes_bloco(padrao)
        for caso in reversed(casos):
            valor_insts = self.gerar_instrucoes_no(caso["valor"])
            condicao_igualdade = [["OBTER_VAR", var_temp], *valor_insts, ["IGUAL_A", None]]
            corpo_caso = self.gerar_instrucoes_bloco(caso["corpo"])
            guarda = caso.get("condicao")
            if guarda is not None:
                corpo_caso = [[
                    "CONDICIONAL_SE",
                    {
                        "condicao": self.gerar_instrucoes_no(guarda),
                        "se": corpo_caso,
                        "senao": fluxo,
                    }
                ]]
            fluxo = [[
                "CONDICIONAL_SE",
                {
                    "condicao": condicao_igualdade,
                    "se": corpo_caso,
                    "senao": fluxo,
                }
            ]]
        return fluxo

    def gerar_instrucoes_funcao(self, no):
        instrucoes_salvas = self.instrucoes
        self.instrucoes = []
        for param in reversed(no.parametros):
            self.instrucoes.append(["DEFINIR_VAR", param])
        for stmt in no.corpo:
            self.gerar_no(stmt)
        instrucoes = self.instrucoes
        self.instrucoes = instrucoes_salvas
        return instrucoes
    
    def gerar_no(self, no):
        """Gera bytecode para um nó AST"""
        if isinstance(no, PareStmt):
            self.instrucoes.append(["QUEBRAR_LOOP", None])

        elif isinstance(no, DeclaracaoVariavel):
            self.gerar_no(no.valor)
            # Se tem tipo explícito inteiro/decimal e não é literal, tenta converter
            if no.tipo_estatico is not None and no.tipo_estatico in (TipoBR.INTEIRO, TipoBR.DECIMAL):
                tipo_nome = "INTEIRO" if no.tipo_estatico == TipoBR.INTEIRO else "DECIMAL"
                self.instrucoes.append(["CHAMAR_SERVICO", "SISTEMA", tipo_nome, 1])
            opcode = "ATRIBUIR_VAR" if getattr(no, "eh_atribuicao", False) and not no.eh_constante else "DEFINIR_VAR"
            self.instrucoes.append([opcode, no.nome])

        elif isinstance(no, AtribuicaoAlvo):
            if isinstance(no.alvo, AcessoPropriedade):
                self.gerar_no(no.alvo.objeto)
                self.gerar_no(no.valor)
                self.instrucoes.append(["DEFINIR_PROPRIEDADE", no.alvo.propriedade])
            elif isinstance(no.alvo, AcessoIndice):
                self.gerar_no(no.alvo.objeto)
                self.gerar_no(no.alvo.indice)
                self.gerar_no(no.valor)
                self.instrucoes.append(["DEFINIR_INDICE", None])

        elif isinstance(no, DeclaracaoImportacao):
            if no.simbolos:
                self.instrucoes.append(["IMPORTAR_SIMBOLOS", no.modulo, no.simbolos])
            else:
                nome_local = no.alias or no.modulo.split("/")[-1].split("\\")[-1]
                if nome_local.endswith(".br"):
                    nome_local = nome_local[:-3]
                self.instrucoes.append(["IMPORTAR_MODULO", no.modulo, nome_local])

        elif isinstance(no, DeclaracaoClasse):
            metodos = {}
            for metodo in no.metodos:
                metodos[metodo.nome] = {
                    "parametros": metodo.parametros,
                    "instrucoes": self.gerar_instrucoes_funcao(metodo),
                }
            classe_info = {
                "campos": no.campos,
                "metodos": metodos,
                "classe_mae": no.classe_mae,
            }
            self.classes[no.nome] = classe_info
            self.instrucoes.append(["DECLARAR_CLASSE", no.nome, no.campos, metodos, no.classe_mae])
        
        elif isinstance(no, DeclaracaoFuncao):
            self.funcoes[no.nome] = self.gerar_instrucoes_funcao(no)
        
        elif isinstance(no, DeclaracaoTeste):
            instrucoes_salvas = self.instrucoes
            self.instrucoes = []
            for stmt in no.corpo:
                self.gerar_no(stmt)
            self.testes.append({
                "nome": no.nome,
                "instrucoes": self.instrucoes
            })
            self.instrucoes = instrucoes_salvas
        
        elif isinstance(no, BlocoExplicacao):
            self.documentacoes.append(no.conteudo)
        
        elif isinstance(no, BlocoObjetivo):
            pass
        
        elif isinstance(no, SeStmt):
            cond_insts = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = cond_insts
            self.gerar_no(no.condicao)
            self.instrucoes = instrucoes_salvas
            
            insts_se = []
            self.instrucoes = insts_se
            for stmt in no.corpo_se:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            
            insts_senao = []
            self.instrucoes = insts_senao
            for stmt in no.corpo_senao:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            
            self.instrucoes.append(["CONDICIONAL_SE", {
                "condicao": cond_insts,
                "se": insts_se,
                "senao": insts_senao
            }])
        
        elif isinstance(no, EnquantoStmt):
            cond_insts = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = cond_insts
            self.gerar_no(no.condicao)
            self.instrucoes = instrucoes_salvas
            
            corpo_loop = []
            self.instrucoes = corpo_loop
            for stmt in no.corpo:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            
            self.instrucoes.append(["ENQUANTO_BLOCO", {
                "condicao": cond_insts,
                "corpo": corpo_loop
            }])
        
        elif isinstance(no, ParaCadaStmt):
            lista_insts = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = lista_insts
            self.gerar_no(no.lista)
            self.instrucoes = instrucoes_salvas
            
            corpo_loop = []
            self.instrucoes = corpo_loop
            for stmt in no.corpo:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            
            self.instrucoes.append(["PARA_CADA_BLOCO", {
                "var_item": no.variavel,
                "lista_insts": lista_insts,
                "corpo": corpo_loop
            }])
        
        elif isinstance(no, TenteCaptureStmt):
            corpo_tente = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = corpo_tente
            for stmt in no.corpo_tente:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            
            corpo_capture = []
            self.instrucoes = corpo_capture
            for stmt in no.corpo_capture:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            
            self.instrucoes.append(["TENTE_CAPTURE_BLOCO", {
                "tente": corpo_tente,
                "var_erro": no.variavel_erro,
                "capture": corpo_capture
            }])
        
        elif isinstance(no, ExecuteParaleloStmt):
            blocos = []
            for bloco in no.blocos:
                insts_bloco = []
                instrucoes_salvas = self.instrucoes
                self.instrucoes = insts_bloco
                for stmt in bloco:
                    self.gerar_no(stmt)
                self.instrucoes = instrucoes_salvas
                blocos.append(insts_bloco)
            
            self.instrucoes.append(["PARALELO_EXECUTE", blocos])

        elif isinstance(no, RepitaVezesStmt):
            self.gerar_no(no.vezes)
            corpo = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = corpo
            for stmt in no.corpo:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            self.instrucoes.append(["REPITA_VEZES", corpo])

        elif isinstance(no, EscolhaCasoStmt):
            self.gerar_no(no.expressao)
            casos = []
            instrucoes_salvas = self.instrucoes
            for caso in no.casos:
                valor_insts = []
                self.instrucoes = valor_insts
                self.gerar_no(caso["valor"])
                corpo_insts = []
                self.instrucoes = corpo_insts
                for stmt in caso["corpo"]:
                    self.gerar_no(stmt)
                casos.append({"valor": valor_insts, "corpo": corpo_insts})
            padrao_insts = []
            self.instrucoes = padrao_insts
            for stmt in no.padrao:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            self.instrucoes.append(["ESCOLHA_CASO", {"casos": casos, "padrao": padrao_insts}])

        elif isinstance(no, CorrespondenciaStmt):
            var_temp = f"_correspondencia_{len(self.instrucoes)}"
            self.gerar_no(no.valor)
            self.instrucoes.append(["DEFINIR_VAR", var_temp])
            self.instrucoes.extend(self.gerar_fluxo_correspondencia(var_temp, no.casos, no.padrao))
        
        elif isinstance(no, MostreStmt):
            self.gerar_no(no.expressao)
            self.instrucoes.append(["CHAMAR_SERVICO", "SISTEMA", "MOSTRAR", 1])
        
        elif isinstance(no, RetornarStmt):
            self.gerar_no(no.valor)
            self.instrucoes.append(["RETORNAR", None])
        
        elif isinstance(no, SalveStmt):
            self.gerar_no(no.expressao)
            self.instrucoes.append(["CHAMAR_SERVICO", "BANCO", "SALVAR", 1])
        
        elif isinstance(no, RemovaStmt):
            self.gerar_no(no.expressao)
            self.instrucoes.append(["CHAMAR_SERVICO", "BANCO", "REMOVER", 1])
        
        elif isinstance(no, EspereStmt):
            self.gerar_no(no.valor)
            self.gerar_no(no.esperado)
            self.instrucoes.append(["ESPERE_IGUAL", None])
        
        elif isinstance(no, ExpressaoStmt):
            self.gerar_no(no.expressao)
            self.instrucoes.append(["DESCARTAR_TOPO", None])
        
        elif isinstance(no, LiteralInteiro):
            idx = self.adicionar_constante(no.valor)
            self.instrucoes.append(["EMPILHAR_CONST", idx])
        
        elif isinstance(no, LiteralDecimal):
            idx = self.adicionar_constante(no.valor)
            self.instrucoes.append(["EMPILHAR_CONST", idx])
        
        elif isinstance(no, LiteralTexto):
            idx = self.adicionar_constante(no.valor)
            self.instrucoes.append(["EMPILHAR_CONST", idx])
        
        elif isinstance(no, LiteralBooleano):
            idx = self.adicionar_constante(no.valor)
            self.instrucoes.append(["EMPILHAR_CONST", idx])
        
        elif isinstance(no, LiteralNulo):
            idx = self.adicionar_constante(None)
            self.instrucoes.append(["EMPILHAR_CONST", idx])
        
        elif isinstance(no, Variavel):
            self.instrucoes.append(["OBTER_VAR", no.nome])
        
        elif isinstance(no, Binario):
            self.gerar_no(no.esquerda)
            self.gerar_no(no.direita)
            
            op_map = {
                "+": "SOMAR",
                "-": "SUBTRAIR",
                "*": "MULTIPLICAR",
                "/": "DIVIDIR",
                "igual": "IGUAL_A",
                "maior que": "MAIOR_QUE",
                "menor que": "MENOR_QUE",
                "<": "MENOR_QUE",
                ">": "MAIOR_QUE",
                "==": "IGUAL_A",
                "!=": "NAO_IGUAL",
                "<=": "MENOR_OU_IGUAL",
                ">=": "MAIOR_OU_IGUAL",
            }
            
            opcode = op_map.get(no.operador)
            if opcode:
                self.instrucoes.append([opcode, None])
        
        elif isinstance(no, Unario):
            if no.operador == "-":
                idx_zero = self.adicionar_constante(0)
                self.instrucoes.append(["EMPILHAR_CONST", idx_zero])
                self.gerar_no(no.operando)
                self.instrucoes.append(["SUBTRAIR", None])
            else:
                self.gerar_no(no.operando)
        
        elif isinstance(no, ChamadaFuncao):
            for arg in no.argumentos:
                self.gerar_no(arg)
            self.instrucoes.append(["CHAMAR_METODO", no.nome, len(no.argumentos)])
        
        elif isinstance(no, ChamadaMetodo):
            if no.objeto:
                self.gerar_no(no.objeto)
            for arg in no.argumentos:
                self.gerar_no(arg)
            
            servico_map = {
                "mostre": ("SISTEMA", "MOSTRAR"),
                "salve": ("BANCO", "SALVAR"),
                "remova": ("BANCO", "REMOVER"),
                "leia_arquivo": ("ARQUIVOS", "LER"),
                "escreva_arquivo": ("ARQUIVOS", "ESCREVER"),
                "escreva_arquivo_binario": ("ARQUIVOS", "ESCREVER_BINARIO"),
                "texto_substituir": ("TEXTO", "SUBSTITUIR"),
                "dividir_por": ("TEXTO", "DIVIDIR_POR"),
                "dividir_primeiro": ("TEXTO", "DIVIDIR_PRIMEIRO"),
                "divida_texto": ("TEXTO", "DIVIDIR"),
                "adicione_item": ("LISTA", "ADICIONAR"),
                "obter_item": ("LISTA", "OBTER"),
                "tamanho_lista": ("LISTA", "TAMANHO"),
                "definir_propriedade": ("REGISTRO", "DEFINIR"),
                "obter_propriedade": ("REGISTRO", "OBTER"),
                "tipo_de": ("SISTEMA", "TIPO"),
                "para_inteiro": ("SISTEMA", "INTEIRO"),
                "para_decimal": ("SISTEMA", "DECIMAL"),
                "texto_contem": ("TEXTO", "CONTEM"),
                "texto_contem_inicio": ("TEXTO", "CONTEM_INICIO"),
                "acesse": ("WEB", "ACESSE"),
                "busque": ("BANCO", "BUSQUE_TODOS_CLIENTES"),
            }
            
            if no.metodo in servico_map:
                servico, metodo = servico_map[no.metodo]
                self.instrucoes.append(["CHAMAR_SERVICO", servico, metodo, len(no.argumentos)])
            elif no.objeto:
                self.instrucoes.append(["CHAMAR_METODO_OBJETO", no.metodo, len(no.argumentos)])
            else:
                self.instrucoes.append(["CHAMAR_SERVICO", "IA", "PERGUNTAR_AGENTE", len(no.argumentos)])
        
        elif isinstance(no, AcessoPropriedade):
            self.gerar_no(no.objeto)
            self.instrucoes.append(["OBTER_PROPRIEDADE", no.propriedade])

        elif isinstance(no, ConteStmt):
            self.gerar_no(no.inicio)
            self.gerar_no(no.fim)
            corpo = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = corpo
            for stmt in no.corpo:
                self.gerar_no(stmt)
            self.instrucoes = instrucoes_salvas
            self.instrucoes.append(["CONTE_BLOCO", {
                "variavel": no.variavel,
                "corpo": corpo
            }])

        elif isinstance(no, DeclaracaoTarefa):
            self.funcoes["__tarefa_" + no.nome] = self.gerar_instrucoes_funcao(no)

        elif isinstance(no, ExpressaoTarefa):
            self.gerar_no(no.expressao)
            self.instrucoes.append(["AGUARDAR_TAREFA", None])

        elif isinstance(no, FatiaExpr):
            self.gerar_no(no.inicio)
            self.gerar_no(no.fim)
            self.instrucoes.append(["CHAMAR_SERVICO", "SISTEMA", "FATIA", 2])

        elif isinstance(no, AcessoIndice):
            self.gerar_no(no.objeto)
            self.gerar_no(no.indice)
            self.instrucoes.append(["OBTER_INDICE", None])
        
        elif isinstance(no, ListaLiteral):
            corpo_lista = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = corpo_lista
            for elem in no.elementos:
                self.gerar_no(elem)
                self.instrucoes.append(["LISTA_ELEMENTO", None])
            self.instrucoes = instrucoes_salvas
            
            var_temp = f"_lista_{len(self.instrucoes)}"
            self.instrucoes.append(["CRIAR_LISTA_BLOCO", var_temp, corpo_lista])
            self.instrucoes.append(["OBTER_VAR", var_temp])

        elif isinstance(no, ComprehensionStmt):
            lista_insts = self.gerar_instrucoes_no(no.iteravel)
            corpo_iteracao = self.gerar_instrucoes_no(no.expressao)
            corpo_iteracao.append(["LISTA_ELEMENTO", None])
            if no.condicao is not None:
                corpo_iteracao = [[
                    "CONDICIONAL_SE",
                    {
                        "condicao": self.gerar_instrucoes_no(no.condicao),
                        "se": corpo_iteracao,
                        "senao": [],
                    }
                ]]
            corpo_lista = [[
                "PARA_CADA_BLOCO",
                {
                    "var_item": no.variavel,
                    "lista_insts": lista_insts,
                    "corpo": corpo_iteracao,
                }
            ]]
            var_temp = f"_lista_comp_{len(self.instrucoes)}"
            self.instrucoes.append(["CRIAR_LISTA_BLOCO", var_temp, corpo_lista])
            self.instrucoes.append(["OBTER_VAR", var_temp])
        
        elif isinstance(no, RegistroLiteral):
            corpo_registro = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = corpo_registro
            for prop_nome, prop_valor in no.propriedades.items():
                self.gerar_no(prop_valor)
                self.instrucoes.append(["REGISTRO_PROPRIEDADE", prop_nome])
            self.instrucoes = instrucoes_salvas
            
            var_temp = f"_registro_{len(self.instrucoes)}"
            self.instrucoes.append(["CRIAR_REGISTRO_BLOCO", var_temp, corpo_registro])
            self.instrucoes.append(["OBTER_VAR", var_temp])
        
        elif isinstance(no, IAPergunte):
            corpo_ia = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = corpo_ia
            self.gerar_no(no.prompt)
            self.instrucoes = instrucoes_salvas
            
            var_temp = f"_ia_pergunte_{len(self.instrucoes)}"
            self.instrucoes.append(["IA_PERGUNTE_BLOCO", var_temp, corpo_ia])
            self.instrucoes.append(["OBTER_VAR", var_temp])
        
        elif isinstance(no, IAAgente):
            corpo_agente = []
            instrucoes_salvas = self.instrucoes
            self.instrucoes = corpo_agente
            for prop_nome, prop_valor in no.propriedades.items():
                self.gerar_no(prop_valor)
                self.instrucoes.append(["REGISTRO_PROPRIEDADE", prop_nome])
            self.instrucoes = instrucoes_salvas
            
            var_temp = f"_ia_agente_{len(self.instrucoes)}"
            self.instrucoes.append(["IA_AGENTE_BLOCO", var_temp, corpo_agente])
            self.instrucoes.append(["OBTER_VAR", var_temp])


def serializar_brc1(obj: Any) -> bytes:
    """Serializa um objeto para o formato binário BRC1"""
    if isinstance(obj, bool):
        return struct.pack('>BB', 4, 1 if obj else 0)
    elif obj is None:
        return struct.pack('>B', 5)
    elif isinstance(obj, int):
        return struct.pack('>Bi', 1, obj)
    elif isinstance(obj, float):
        return struct.pack('>Bd', 2, obj)
    elif isinstance(obj, str):
        utf = obj.encode('utf-8')
        return struct.pack('>BI', 3, len(utf)) + utf
    elif isinstance(obj, list):
        if len(obj) > 0 and isinstance(obj[0], str) and obj[0] in OPCODE_MAP:
            op_id = OPCODE_MAP[obj[0]]
            args = obj[1:]
            res = struct.pack('>BBB', 8, op_id, len(args))
            for arg in args:
                res += serializar_brc1(arg)
            return res
        else:
            res = struct.pack('>BI', 6, len(obj))
            for item in obj:
                res += serializar_brc1(item)
            return res
    elif isinstance(obj, dict):
        res = struct.pack('>BI', 7, len(obj))
        for k, v in obj.items():
            res += serializar_brc1(k)
            res += serializar_brc1(v)
        return res
    raise TypeError(f"Tipo não serializável: {type(obj)}")


def salvar_bytecode_brc1(bytecode, caminho):
    """Salva bytecode no formato binário BRC1"""
    serialized = b"BRC1" + serializar_brc1(bytecode)
    with open(caminho, "wb") as f:
        f.write(serialized)


def gerar_bytecode(nos):
    gerador = GeradorBytecodeBR()
    return gerador.gerar(nos)
