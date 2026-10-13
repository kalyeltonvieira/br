# -*- coding: utf-8 -*-
"""
Linter (brasil analisar) para Linguagem BR
Analisa código estático para detectar más práticas e erros lógicos potenciais
"""
import re
from typing import List, Dict, Tuple, Optional
from dataclasses import dataclass
from lexer import LexerBR, TipoToken
from parser import ParserBR
from br_ast import *

@dataclass
class AvisoLinter:
    linha: int
    coluna: int
    severidade: str  # "erro", "aviso", "sugestao"
    codigo: str
    mensagem: str
    sugestao: Optional[str] = None

class LinterBR:
    """Linter estático para código BR"""
    
    def __init__(self):
        self.avisos: List[AvisoLinter] = []
        self.variaveis_declaradas: Dict[str, Tuple[int, int]] = {}
        self.variaveis_usadas: set = set()
        self.funcoes_declaradas: Dict[str, Tuple[int, int]] = {}
        self.constantes_declaradas: set = set()
    
    def analisar(self, codigo: str, arquivo: str = "desconhecido") -> List[AvisoLinter]:
        """Analisa código BR e retorna lista de avisos"""
        self.avisos = []
        self.variaveis_declaradas = {}
        self.variaveis_usadas = set()
        self.funcoes_declaradas = {}
        self.constantes_declaradas = set()
        
        # Tokenizar e parsear
        lexer = LexerBR(codigo)
        tokens = lexer.tokenizar()
        
        parser = ParserBR(tokens)
        try:
            ast = parser.parse()
        except Exception as e:
            # Erro de parse já será reportado pelo compilador
            return self.avisos
        
        # Análise de código morto
        self._detectar_codigo_morto(ast)
        
        # Análise de variáveis não utilizadas
        self._detectar_variaveis_nao_usadas(ast)
        
        # Análise de shadowing
        self._detectar_shadowing(ast)
        
        # Análise de segurança de IA
        self._detectar_seguranca_ia(ast)
        
        # Análise de estilo
        self._detectar_problemas_estilo(codigo)
        
        return self.avisos
    
    def _detectar_codigo_morto(self, nos: List[NoAST]):
        """Detecta código que nunca será executado"""
        for no in nos:
            if isinstance(no, SeStmt):
                # Verificar se condição é sempre verdadeira ou falsa
                if isinstance(no.condicao, LiteralBooleano):
                    if no.condicao.valor:
                        self._adicionar_aviso(
                            no.condicao.posicao.linha,
                            no.condicao.posicao.coluna,
                            "aviso",
                            "CODIGO_MORTO",
                            "Condição do 'se' é sempre verdadeira, o bloco 'senão' nunca será executado",
                            "Considere remover o bloco 'senão' ou tornar a condição dinâmica"
                        )
                    else:
                        self._adicionar_aviso(
                            no.condicao.posicao.linha,
                            no.condicao.posicao.coluna,
                            "aviso",
                            "CODIGO_MORTO",
                            "Condição do 'se' é sempre falsa, o bloco 'se' nunca será executado",
                            "Considere tornar a condição dinâmica ou remover o bloco"
                        )
            
            elif isinstance(no, EnquantoStmt):
                # Verificar se loop é infinito
                if isinstance(no.condicao, LiteralBooleano) and no.condicao.valor:
                    self._adicionar_aviso(
                        no.condicao.posicao.linha,
                        no.condicao.posicao.coluna,
                        "aviso",
                        "LOOP_INFINITO",
                        "Loop 'enquanto' com condição sempre verdadeira pode causar loop infinito",
                        "Adicione uma condição de parada ou use 'se' para verificar condição de saída"
                    )
            
            elif isinstance(no, RetornarStmt):
                # Código após retorno nunca será executado no mesmo escopo
                self._adicionar_aviso(
                    no.posicao.linha,
                    no.posicao.coluna,
                    "aviso",
                    "CODIGO_MORTO",
                    "Código após 'devolva' nunca será executado",
                    "Mova o código antes do retorno ou remova-o"
                )
    
    def _detectar_variaveis_nao_usadas(self, nos: List[NoAST]):
        """Detecta variáveis declaradas mas nunca usadas"""
        for no in nos:
            if isinstance(no, DeclaracaoVariavel):
                self.variaveis_declaradas[no.nome] = (no.posicao.linha, no.posicao.coluna)
                if no.eh_constante:
                    self.constantes_declaradas.add(no.nome)
            
            elif isinstance(no, Variavel):
                self.variaveis_usadas.add(no.nome)
        
        # Verificar variáveis não usadas
        for nome, (linha, coluna) in self.variaveis_declaradas.items():
            if nome not in self.variaveis_usadas and not nome.startswith("_"):
                self._adicionar_aviso(
                    linha,
                    coluna,
                    "aviso",
                    "VARIAVEL_NAO_USADA",
                    f"Variável '{nome}' declarada mas nunca usada",
                    "Remova a declaração ou use a variável. Use prefixo '_' se intencional"
                )
    
    def _detectar_shadowing(self, nos: List[NoAST]):
        """Detecta shadowing de variáveis"""
        escopos = []
        
        def visitar_no(no: NoAST, profundidade: int = 0):
            if isinstance(no, DeclaracaoVariavel):
                for escopo in reversed(escopos):
                    if no.nome in escopo:
                        self._adicionar_aviso(
                            no.posicao.linha,
                            no.posicao.coluna,
                            "aviso",
                            "SHADOWING",
                            f"Variável '{no.nome}' faz shadow de variável em escopo externo",
                            "Renomeie a variável para evitar confusão"
                        )
                        break
                escopos.append({no.nome})
            
            elif isinstance(no, DeclaracaoFuncao):
                escopos.append({no.nome})
            
            elif isinstance(no, SeStmt):
                visitar_no(no.condicao, profundidade + 1)
                for stmt in no.corpo_se:
                    visitar_no(stmt, profundidade + 1)
                for stmt in no.corpo_senao:
                    visitar_no(stmt, profundidade + 1)
                if escopos:
                    escopos.pop()
                return
            
            elif isinstance(no, EnquantoStmt):
                visitar_no(no.condicao, profundidade + 1)
                for stmt in no.corpo:
                    visitar_no(stmt, profundidade + 1)
                if escopos:
                    escopos.pop()
                return
        
        for no in nos:
            visitar_no(no)
    
    def _detectar_seguranca_ia(self, nos: List[NoAST]):
        """Detecta problemas de segurança em prompts de IA"""
        patterns_perigosos = [
            r"senha\s*=",
            r"password\s*=",
            r"token\s*=",
            r"api_key\s*=",
            r"ignore\s+previous",
            r"desconsidere\s+instruções",
        ]
        
        for no in nos:
            if isinstance(no, IAPergunte):
                if isinstance(no.prompt, LiteralTexto):
                    prompt = no.prompt.valor.lower()
                    for pattern in patterns_perigosos:
                        if re.search(pattern, prompt):
                            self._adicionar_aviso(
                                no.posicao.linha,
                                no.posicao.coluna,
                                "erro",
                                "IA_INSEGURA",
                                "Prompt de IA contém pattern potencialmente perigoso",
                                "Remova informações sensíveis do prompt ou use variáveis"
                            )
            
            elif isinstance(no, IAAgente):
                # Verificar se agente tem permissões excessivas
                if len(no.propriedades) > 10:
                    self._adicionar_aviso(
                        no.posicao.linha,
                        no.posicao.coluna,
                        "aviso",
                        "AGENTE_COMPLEXO",
                        f"Agente de IA tem muitas propriedades ({len(no.propriedades)})",
                        "Considere simplificar o agente ou dividir responsabilidades"
                    )
    
    def _detectar_problemas_estilo(self, codigo: str):
        """Detecta problemas de estilo"""
        linhas = codigo.splitlines()
        
        for i, linha in enumerate(linhas):
            # Linhas muito longas
            if len(linha) > 120:
                self._adicionar_aviso(
                    i + 1,
                    0,
                    "sugestao",
                    "LINHA_LONGA",
                    f"Linha muito longa ({len(linha)} caracteres)",
                    "Considere quebrar a linha em múltiplas linhas"
                )
            
            # Tabulações (deve usar espaços)
            if "\t" in linha:
                self._adicionar_aviso(
                    i + 1,
                    0,
                    "sugestao",
                    "TABULACAO",
                    "Linha contém tabulação (use 4 espaços)",
                    "Substitua tabulações por 4 espaços"
                )
            
            # Espaços em branco no final da linha
            if linha.endswith(" ") or linha.endswith("\t"):
                self._adicionar_aviso(
                    i + 1,
                    len(linha.rstrip()),
                    "sugestao",
                    "ESPACO_FINAL",
                    "Linha tem espaços em branco no final",
                    "Remova os espaços em branco"
                )
    
    def _adicionar_aviso(self, linha: int, coluna: int, severidade: str, codigo: str, mensagem: str, sugestao: Optional[str] = None):
        """Adiciona um aviso à lista"""
        self.avisos.append(AvisoLinter(linha, coluna, severidade, codigo, mensagem, sugestao))
    
    def relatorio(self) -> str:
        """Gera relatório de análise"""
        if not self.avisos:
            return "✓ Nenhum problema encontrado. Código limpo!"
        
        erros = [a for a in self.avisos if a.severidade == "erro"]
        avisos = [a for a in self.avisos if a.severidade == "aviso"]
        sugestoes = [a for a in self.avisos if a.severidade == "sugestao"]
        
        relatorio = f"""
Relatório de Análise Estática:
- Erros: {len(erros)}
- Avisos: {len(avisos)}
- Sugestões: {len(sugestoes)}
"""
        
        if erros:
            relatorio += "\nErros:\n"
            for aviso in erros:
                relatorio += f"  [{aviso.codigo}] Linha {aviso.linha}: {aviso.mensagem}\n"
                if aviso.sugestao:
                    relatorio += f"    → {aviso.sugestao}\n"
        
        if avisos:
            relatorio += "\nAvisos:\n"
            for aviso in avisos:
                relatorio += f"  [{aviso.codigo}] Linha {aviso.linha}: {aviso.mensagem}\n"
                if aviso.sugestao:
                    relatorio += f"    → {aviso.sugestao}\n"
        
        if sugestoes:
            relatorio += "\nSugestões:\n"
            for aviso in sugestoes:
                relatorio += f"  [{aviso.codigo}] Linha {aviso.linha}: {aviso.mensagem}\n"
                if aviso.sugestao:
                    relatorio += f"    → {aviso.sugestao}\n"
        
        return relatorio

def analisar_arquivo(caminho: str) -> str:
    """Analisa um arquivo BR"""
    with open(caminho, "r", encoding="utf-8") as f:
        codigo = f.read()
    
    linter = LinterBR()
    avisos = linter.analisar(codigo, caminho)
    
    return linter.reatorio()

def main():
    """Ponto de entrada CLI do linter"""
    import sys
    
    if len(sys.argv) < 2:
        print("Uso: brasil analisar <arquivo.br>")
        sys.exit(1)
    
    caminho = sys.argv[1]
    
    if not os.path.exists(caminho):
        print(f"Erro: Arquivo '{caminho}' não encontrado.")
        sys.exit(1)
    
    relatorio = analisar_arquivo(caminho)
    print(relatorio)
    
    # Exit com código de erro se houver erros
    linter = LinterBR()
    with open(caminho, "r", encoding="utf-8") as f:
        codigo = f.read()
    avisos = linter.analisar(codigo, caminho)
    
    erros = [a for a in avisos if a.severidade == "erro"]
    if erros:
        sys.exit(1)

if __name__ == "__main__":
    main()
