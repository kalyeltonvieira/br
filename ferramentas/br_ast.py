# -*- coding: utf-8 -*-
"""
Estrutura de AST (Árvore de Sintaxe Abstrata) para Linguagem BR
Define todos os nós da árvore sintática com informações de localização
"""
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any
from enum import Enum

class TipoBR(Enum):
    INTEIRO = "inteiro"
    DECIMAL = "decimal"
    TEXTO = "texto"
    BOOLEANO = "booleano"
    NULO = "nulo"
    LISTA = "lista"
    REGISTRO = "registro"
    DESCONHECIDO = "desconhecido"
    TAREFA = "tarefa"
    ERRO = "erro"

class TipoNo(Enum):
    PROGRAMA = "programa"
    BLOCO = "bloco"
    LITERAL_INTEIRO = "literal_inteiro"
    LITERAL_DECIMAL = "literal_decimal"
    LITERAL_TEXTO = "literal_texto"
    LITERAL_BOOLEANO = "literal_booleano"
    LITERAL_NULO = "literal_nulo"
    VARIAVEL = "variavel"
    FSTRING = "fstring"
    BINARIO = "binario"
    UNARIO = "unario"
    CHAMADA_FUNCAO = "chamada_funcao"
    CHAMADA_METODO = "chamada_metodo"
    ACESSO_PROPRIEDADE = "acesso_propriedade"
    ACESSO_INDICE = "acesso_indice"
    LISTA_LITERAL = "lista_literal"
    REGISTRO_LITERAL = "registro_literal"
    REGISTRO_INLINE = "registro_inline"
    IA_PERGUNTE = "ia_pergunte"
    IA_AGENTE = "ia_agente"
    EXPRESSAO_TAREFA = "expressao_tarefa"
    DECLARACAO_VARIAVEL = "declaracao_variavel"
    ATRIBUICAO_ALVO = "atribuicao_alvo"
    DECLARACAO_FUNCAO = "declaracao_funcao"
    DECLARACAO_TAREFA = "declaracao_tarefa"
    DECLARACAO_TESTE = "declaracao_teste"
    BLOCO_EXPLICACAO = "bloco_explicacao"
    BLOCO_OBJETIVO = "bloco_objetivo"
    DECLARACAO_IMPORTACAO = "declaracao_importacao"
    DECLARACAO_CONTRATO = "declaracao_contrato"
    DECLARACAO_CLASSE = "declaracao_classe"
    EXPRESSAO_STMT = "expressao_stmt"
    MOSTRE_STMT = "mostre_stmt"
    RETORNAR_STMT = "retornar_stmt"
    SALVE_STMT = "salve_stmt"
    REMOVA_STMT = "remova_stmt"
    ESPERE_STMT = "espere_stmt"
    SE_STMT = "se_stmt"
    ENQUANTO_STMT = "enquanto_stmt"
    PARA_CADA_STMT = "para_cada_stmt"
    TENTE_CAPTURE_STMT = "tente_capture_stmt"
    EXECUTE_PARALELO_STMT = "execute_paralelo_stmt"
    REPITA_VEZES_STMT = "repita_vezes_stmt"
    ESCOLHA_CASO_STMT = "escolha_caso_stmt"
    CONTE_STMT = "conte_stmt"
    CRIAR_SITE_STMT = "criar_site_stmt"
    PARE_STMT = "pare_stmt"
    CRIAR_JANELA_STMT = "criar_janela_stmt"
    # Novos tipos
    COMPREHENSION = "comprehension"
    GENERATOR = "generator"
    DECORATOR = "decorator"
    CONTEXT_MANAGER = "context_manager"
    CORRESPONDENCIA = "correspondencia"
    DOCSTRING = "docstring"

@dataclass
class Posicao:
    linha: int
    coluna: int
    def __repr__(self):
        return f"{self.linha}:{self.coluna}"

@dataclass
class NoAST:
    posicao: Posicao
    tipo_no: str = field(init=False)
    def aceitar(self, visitante):
        raise NotImplementedError

# ============ Expressões ============
@dataclass
class LiteralInteiro(NoAST):
    valor: int
    def __post_init__(self):
        self.tipo_no = TipoNo.LITERAL_INTEIRO.value
    def aceitar(self, visitante):
        return visitante.visitar_literal_inteiro(self)

@dataclass
class LiteralDecimal(NoAST):
    valor: float
    def __post_init__(self):
        self.tipo_no = TipoNo.LITERAL_DECIMAL.value
    def aceitar(self, visitante):
        return visitante.visitar_literal_decimal(self)

@dataclass
class LiteralTexto(NoAST):
    valor: str
    def __post_init__(self):
        self.tipo_no = TipoNo.LITERAL_TEXTO.value
    def aceitar(self, visitante):
        return visitante.visitar_literal_texto(self)

@dataclass
class FString(NoAST):
    texto: str
    expressoes: List[Any]
    def __post_init__(self):
        self.tipo_no = TipoNo.FSTRING.value
    def aceitar(self, visitante):
        return visitante.visitar_fstring(self)

@dataclass
class LiteralBooleano(NoAST):
    valor: bool
    def __post_init__(self):
        self.tipo_no = TipoNo.LITERAL_BOOLEANO.value
    def aceitar(self, visitante):
        return visitante.visitar_literal_booleano(self)

@dataclass
class LiteralNulo(NoAST):
    def __post_init__(self):
        self.tipo_no = TipoNo.LITERAL_NULO.value
    def aceitar(self, visitante):
        return visitante.visitar_literal_nulo(self)

@dataclass
class Variavel(NoAST):
    nome: str
    tipo_estatico: Optional[TipoBR] = None
    def __post_init__(self):
        self.tipo_no = TipoNo.VARIAVEL.value
    def aceitar(self, visitante):
        return visitante.visitar_variavel(self)

@dataclass
class AcessoPropriedade(NoAST):
    objeto: NoAST
    propriedade: str
    def __post_init__(self):
        self.tipo_no = TipoNo.ACESSO_PROPRIEDADE.value
    def aceitar(self, visitante):
        return visitante.visitar_acesso_propriedade(self)

@dataclass
class AcessoIndice(NoAST):
    objeto: NoAST
    indice: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.ACESSO_INDICE.value
    def aceitar(self, visitante):
        return visitante.visitar_acesso_indice(self)

@dataclass
class ChamadaMetodo(NoAST):
    objeto: Optional[NoAST]
    metodo: str
    argumentos: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.CHAMADA_METODO.value
    def aceitar(self, visitante):
        return visitante.visitar_chamada_metodo(self)

@dataclass
class ChamadaFuncao(NoAST):
    nome: str
    argumentos: List[NoAST]
    decorators: List[Any] = field(default_factory=list)
    def __post_init__(self):
        self.tipo_no = TipoNo.CHAMADA_FUNCAO.value
    def aceitar(self, visitante):
        return visitante.visitar_chamada_funcao(self)

@dataclass
class Binario(NoAST):
    esquerda: NoAST
    operador: str
    direita: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.BINARIO.value
    def aceitar(self, visitante):
        return visitante.visitar_binario(self)

@dataclass
class Unario(NoAST):
    operador: str
    operando: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.UNARIO.value
    def aceitar(self, visitante):
        return visitante.visitar_unario(self)

@dataclass
class ListaLiteral(NoAST):
    elementos: List[NoAST]
    comprehension: Optional[Dict[str, Any]] = None
    def __post_init__(self):
        self.tipo_no = TipoNo.LISTA_LITERAL.value
    def aceitar(self, visitante):
        return visitante.visitar_lista_literal(self)

@dataclass
class RegistroLiteral(NoAST):
    propriedades: Dict[str, NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.REGISTRO_LITERAL.value
    def aceitar(self, visitante):
        return visitante.visitar_registro_literal(self)

@dataclass
class IAPergunte(NoAST):
    prompt: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.IA_PERGUNTE.value
    def aceitar(self, visitante):
        return visitante.visitar_ia_pergunte(self)

@dataclass
class IAAgente(NoAST):
    especialidade: str
    propriedades: Dict[str, NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.IA_AGENTE.value
    def aceitar(self, visitante):
        return visitante.visitar_ia_agente(self)

@dataclass
class ExpressaoTarefa(NoAST):
    expressao: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.EXPRESSAO_TAREFA.value
    def aceitar(self, visitante):
        return visitante.visitar_expressao_tarefa(self)

# ============ Declarações ============
@dataclass
class DeclaracaoVariavel(NoAST):
    nome: str
    tipo_estatico: Optional[TipoBR]
    valor: NoAST
    eh_constante: bool = False
    eh_atribuicao: bool = False
    def __post_init__(self):
        self.tipo_no = TipoNo.DECLARACAO_VARIAVEL.value
    def aceitar(self, visitante):
        return visitante.visitar_declaracao_variavel(self)

@dataclass
class AtribuicaoAlvo(NoAST):
    alvo: NoAST
    valor: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.ATRIBUICAO_ALVO.value
    def aceitar(self, visitante):
        return visitante.visitar_atribuicao_alvo(self)

@dataclass
class DeclaracaoFuncao(NoAST):
    nome: str
    parametros: List[str]
    corpo: List[NoAST]
    tipo_retorno: Optional[TipoBR] = None
    decorators: List[Any] = field(default_factory=list)
    docstring: Optional[str] = None
    gerador: bool = False
    def __post_init__(self):
        self.tipo_no = TipoNo.DECLARACAO_FUNCAO.value
    def aceitar(self, visitante):
        return visitante.visitar_declaracao_funcao(self)

@dataclass
class DeclaracaoTarefa(NoAST):
    nome: str
    parametros: List[str]
    corpo: List[NoAST]
    async_fn: bool = False
    def __post_init__(self):
        self.tipo_no = TipoNo.DECLARACAO_TAREFA.value
    def aceitar(self, visitante):
        return visitante.visitar_declaracao_tarefa(self)

@dataclass
class DeclaracaoTeste(NoAST):
    nome: str
    corpo: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.DECLARACAO_TESTE.value
    def aceitar(self, visitante):
        return visitante.visitar_declaracao_teste(self)

@dataclass
class BlocoExplicacao(NoAST):
    conteudo: str
    def __post_init__(self):
        self.tipo_no = TipoNo.BLOCO_EXPLICACAO.value
    def aceitar(self, visitante):
        return visitante.visitar_bloco_explicacao(self)

@dataclass
class BlocoObjetivo(NoAST):
    objetivo: str
    def __post_init__(self):
        self.tipo_no = TipoNo.BLOCO_OBJETIVO.value
    def aceitar(self, visitante):
        return visitante.visitar_bloco_objetivo(self)

@dataclass
class DeclaracaoImportacao(NoAST):
    modulo: str
    alias: Optional[str] = None
    simbolos: Optional[List[str]] = None
    def __post_init__(self):
        self.tipo_no = TipoNo.DECLARACAO_IMPORTACAO.value
    def aceitar(self, visitante):
        return visitante.visitar_declaracao_importacao(self)

@dataclass
class DeclaracaoContrato(NoAST):
    nome: str
    funcoes: List[Dict[str, Any]]
    def __post_init__(self):
        self.tipo_no = TipoNo.DECLARACAO_CONTRATO.value
    def aceitar(self, visitante):
        return visitante.visitar_declaracao_contrato(self)

@dataclass
class DeclaracaoClasse(NoAST):
    nome: str
    campos: List[str]
    metodos: List[Any] = field(default_factory=list)
    classe_mae: Optional[str] = None
    docstring: Optional[str] = None
    def __post_init__(self):
        self.tipo_no = TipoNo.DECLARACAO_CLASSE.value
    def aceitar(self, visitante):
        return visitante.visitar_declaracao_classe(self)

# ============ Instruções ============
@dataclass
class ExpressaoStmt(NoAST):
    expressao: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.EXPRESSAO_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_expressao_stmt(self)

@dataclass
class MostreStmt(NoAST):
    expressao: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.MOSTRE_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_mostre_stmt(self)

@dataclass
class RetornarStmt(NoAST):
    valor: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.RETORNAR_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_retornar_stmt(self)

@dataclass
class SalveStmt(NoAST):
    expressao: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.SALVE_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_salve_stmt(self)

@dataclass
class RemovaStmt(NoAST):
    expressao: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.REMOVA_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_remova_stmt(self)

@dataclass
class EspereStmt(NoAST):
    valor: NoAST
    esperado: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.ESPERE_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_espere_stmt(self)

@dataclass
class SeStmt(NoAST):
    condicao: NoAST
    corpo_se: List[NoAST]
    corpo_senao: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.SE_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_se_stmt(self)

@dataclass
class EnquantoStmt(NoAST):
    condicao: NoAST
    corpo: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.ENQUANTO_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_enquanto_stmt(self)

@dataclass
class ParaCadaStmt(NoAST):
    variavel: str
    lista: NoAST
    corpo: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.PARA_CADA_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_para_cada_stmt(self)

@dataclass
class TenteCaptureStmt(NoAST):
    corpo_tente: List[NoAST]
    variavel_erro: str
    corpo_capture: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.TENTE_CAPTURE_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_tente_capture_stmt(self)

@dataclass
class ExecuteParaleloStmt(NoAST):
    blocos: List[List[NoAST]]
    def __post_init__(self):
        self.tipo_no = TipoNo.EXECUTE_PARALELO_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_execute_paralelo_stmt(self)

@dataclass
class RepitaVezesStmt(NoAST):
    vezes: NoAST
    corpo: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.REPITA_VEZES_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_repita_vezes_stmt(self)

@dataclass
class EscolhaCasoStmt(NoAST):
    expressao: NoAST
    casos: List[Dict[str, Any]]
    padrao: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.ESCOLHA_CASO_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_escolha_caso_stmt(self)

@dataclass
class ConteStmt(NoAST):
    inicio: NoAST
    fim: NoAST
    variavel: str
    corpo: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.CONTE_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_conte_stmt(self)

@dataclass
class PareStmt(NoAST):
    def __post_init__(self):
        self.tipo_no = TipoNo.PARE_STMT.value
    def aceitar(self, visitante):
        return visitante.visitar_pare_stmt(self)

# ============ NOVOS: Funcionalidades Modernas ============
@dataclass
class ComprehensionStmt(NoAST):
    """List comprehension: [expr para cada var em lista]"""
    expressao: NoAST
    variavel: str
    iteravel: NoAST
    condicao: Optional[NoAST] = None
    def __post_init__(self):
        self.tipo_no = TipoNo.COMPREHENSION.value
    def aceitar(self, visitante):
        return visitante.visitar_comprehension(self)

@dataclass
class GeneratorExpr(NoAST):
    """Generator expression: (expr para cada var em lista)"""
    expressao: NoAST
    variavel: str
    iteravel: NoAST
    def __post_init__(self):
        self.tipo_no = TipoNo.GENERATOR.value
    def aceitar(self, visitante):
        return visitante.visitar_generator(self)

@dataclass
class DecoratorExpr(NoAST):
    """Decorator: @nome"""
    nome: str
    argumentos: List[NoAST] = field(default_factory=list)
    def __post_init__(self):
        self.tipo_no = TipoNo.DECORATOR.value
    def aceitar(self, visitante):
        return visitante.visitar_decorator(self)

@dataclass
class ContextManagerStmt(NoAST):
    """Context manager: com expressao como var"""
    expressao: NoAST
    variavel: Optional[str]
    corpo: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.CONTEXT_MANAGER.value
    def aceitar(self, visitante):
        return visitante.visitar_context_manager(self)

@dataclass
class CorrespondenciaStmt(NoAST):
    """Pattern matching: corresponda valor caso ..."""
    valor: NoAST
    casos: List[Dict[str, Any]]
    padrao: List[NoAST]
    def __post_init__(self):
        self.tipo_no = TipoNo.CORRESPONDENCIA.value
    def aceitar(self, visitante):
        return visitante.visitar_correspondencia(self)

@dataclass
class DocstringStmt(NoAST):
    """Docstring: \"\"\"...\"\"\" """
    conteudo: str
    def __post_init__(self):
        self.tipo_no = TipoNo.DOCSTRING.value
    def aceitar(self, visitante):
        return visitante.visitar_docstring(self)

@dataclass
class FatiaExpr(NoAST):
    inicio: Optional[NoAST]
    fim: Optional[NoAST]
    def __post_init__(self):
        self.tipo_no = "fatia"
    def aceitar(self, visitante):
        # We can implement a default visitor fallback
        if hasattr(visitante, "visitar_fatia"):
            return visitante.visitar_fatia(self)
        return None

# ============ Serialização JSON ============
def para_json(no: NoAST) -> Dict[str, Any]:
    if isinstance(no, LiteralInteiro):
        return {"tipo": "LiteralInteiro", "valor": no.valor, "posicao": {"linha": no.posicao.linha, "coluna": no.posicao.coluna}}
    elif isinstance(no, LiteralDecimal):
        return {"tipo": "LiteralDecimal", "valor": no.valor, "posicao": {"linha": no.posicao.linha, "coluna": no.posicao.coluna}}
    elif isinstance(no, LiteralTexto):
        return {"tipo": "LiteralTexto", "valor": no.valor, "posicao": {"linha": no.posicao.linha, "coluna": no.posicao.coluna}}
    elif isinstance(no, FString):
        return {"tipo": "FString", "texto": no.texto, "expressoes": no.expressoes, "posicao": {"linha": no.posicao.linha, "coluna": no.posicao.coluna}}
    elif isinstance(no, LiteralBooleano):
        return {"tipo": "LiteralBooleano", "valor": no.valor, "posicao": {"linha": no.posicao.linha, "coluna": no.posicao.coluna}}
    elif isinstance(no, LiteralNulo):
        return {"tipo": "LiteralNulo", "posicao": {"linha": no.posicao.linha, "coluna": no.posicao.coluna}}
    elif isinstance(no, Variavel):
        return {"tipo": "Variavel", "nome": no.nome, "tipo_estatico": no.tipo_estatico.value if no.tipo_estatico else None}
    elif isinstance(no, DeclaracaoVariavel):
        return {"tipo": "DeclaracaoVariavel", "nome": no.nome, "tipo_estatico": no.tipo_estatico.value if no.tipo_estatico else None, "valor": para_json(no.valor)}
    return {"tipo": "Desconhecido", "posicao": {"linha": no.posicao.linha, "coluna": no.posicao.coluna}}