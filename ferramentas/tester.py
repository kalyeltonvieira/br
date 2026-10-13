# -*- coding: utf-8 -*-
"""
Framework de Testes (brasil testar) para Linguagem BR
Executa testes automatizados com relatórios detalhados
"""
import sys
import os
import time
from typing import List, Dict, Any, Optional, Tuple
from dataclasses import dataclass
from lexer import LexerBR
from parser import ParserBR
from codegen import gerar_bytecode
from maquina_virtual import MaquinaVirtualBR
from semantic_analyzer import analisar_semantica
from encoding_utils import ler_texto

@dataclass
class ResultadoTeste:
    nome: str
    sucesso: bool
    duracao: float
    erro: Optional[str] = None
    saida: Optional[str] = None

@dataclass
class SuiteTestes:
    nome: str
    testes: List[ResultadoTeste]
    duracao_total: float

class FrameworkTestesBR:
    """Framework de testes para BR"""
    
    def __init__(self):
        self.suites: List[SuiteTestes] = []
        self.antes_de_cada: Optional[List] = None
        self.depois_de_cada: Optional[List] = None
        self.antes_de_todos: Optional[List] = None
        self.depois_de_todos: Optional[List] = None
    
    def executar_arquivo(self, caminho: str) -> SuiteTestes:
        """Executa testes de um arquivo BR"""
        codigo = ler_texto(caminho)
        
        # Tokenizar e parsear
        lexer = LexerBR(codigo)
        tokens = lexer.tokenizar()
        
        parser = ParserBR(tokens)
        ast = parser.parse()
        if parser.erros:
            resultados = [
                ResultadoTeste(
                    "analise_sintatica",
                    False,
                    0.0,
                    erro="\n".join(str(erro) for erro in parser.erros)
                )
            ]
            suite = SuiteTestes(os.path.basename(caminho), resultados, 0.0)
            self.suites.append(suite)
            return suite
        
        # Extrair declarações de teste
        sucesso, erros, _avisos = analisar_semantica(ast)
        if not sucesso:
            resultados = [
                ResultadoTeste(
                    "analise_semantica",
                    False,
                    0.0,
                    erro="\n".join(str(erro) for erro in erros)
                )
            ]
            suite = SuiteTestes(os.path.basename(caminho), resultados, 0.0)
            self.suites.append(suite)
            return suite

        bytecode = gerar_bytecode(ast)
        testes = bytecode.get("testes", [])
        
        # Executar cada teste
        resultados = []
        inicio_suite = time.time()
        
        for teste in testes:
            resultado = self._executar_teste(teste, caminho, bytecode)
            resultados.append(resultado)
        
        duracao_total = time.time() - inicio_suite
        
        nome_suite = os.path.basename(caminho).replace(".br", "").replace(".brm", "")
        suite = SuiteTestes(nome_suite, resultados, duracao_total)
        self.suites.append(suite)
        
        return suite
    
    def _executar_teste(self, teste, arquivo_origem: str, bytecode: Dict[str, Any]) -> ResultadoTeste:
        """Executa um teste individual"""
        inicio = time.time()
        nome = teste.get("nome", "teste_anonimo")
        vm = None
        
        try:
            vm = MaquinaVirtualBR(
                {
                    "constantes": bytecode.get("constantes", []),
                    "instrucoes": bytecode.get("instrucoes", []),
                    "funcoes": bytecode.get("funcoes", {}),
                    "testes": [],
                    "documentacoes": bytecode.get("documentacoes", []),
                },
                diretorio_base=os.path.dirname(os.path.abspath(arquivo_origem))
            )
            vm.executar()
            vm.executar_instrucoes(teste.get("instrucoes", []))
            
            # Capturar saída se houver
            saida = str(vm.pilha[-1]) if vm.pilha else None
            
            duracao = time.time() - inicio
            
            return ResultadoTeste(nome, True, duracao, saida=saida)
        
        except AssertionError as e:
            duracao = time.time() - inicio
            erro = vm.formatar_erro(e) if vm is not None else str(e)
            return ResultadoTeste(nome, False, duracao, erro=erro)
        
        except Exception as e:
            duracao = time.time() - inicio
            erro = vm.formatar_erro(e) if vm is not None else str(e)
            return ResultadoTeste(nome, False, duracao, erro=f"Erro inesperado: {erro}")
    
    def executar_diretorio(self, diretorio: str) -> List[SuiteTestes]:
        """Executa todos os testes em um diretório"""
        suites = []
        
        if not os.path.exists(diretorio):
            print(f"Diretório '{diretorio}' não encontrado.")
            return suites
        
        for arquivo in os.listdir(diretorio):
            if arquivo.endswith(".br") or arquivo.endswith(".brm"):
                caminho = os.path.join(diretorio, arquivo)
                suite = self.executar_arquivo(caminho)
                suites.append(suite)
        
        return suites
    
    def relatorio_suite(self, suite: SuiteTestes) -> str:
        """Gera relatório de uma suite de testes"""
        total = len(suite.testes)
        passou = sum(1 for t in suite.testes if t.sucesso)
        falhou = total - passou
        
        relatorio = f"""
Suite: {suite.nome}
Tempo total: {suite.duracao_total:.3f}s
Resultados: {passou}/{total} passou ({falhou} falhou)
"""
        
        for teste in suite.testes:
            status = "✓ PASSOU" if teste.sucesso else "✗ FALHOU"
            status = "[PASSOU]" if teste.sucesso else "[FALHOU]"
            relatorio += f"  {status}: {teste.nome} ({teste.duracao:.3f}s)"
            if teste.erro:
                relatorio += f"\n    Erro: {teste.erro}"
            if teste.saida:
                relatorio += f"\n    Saída: {teste.saida}"
            relatorio += "\n"
        
        return relatorio
    
    def relatorio_completo(self) -> str:
        """Gera relatório completo de todas as suites"""
        if not self.suites:
            return "Nenhum teste executado."
        
        total_testes = sum(len(s.testes) for s in self.suites)
        total_passou = sum(sum(1 for t in s.testes if t.sucesso) for s in self.suites)
        total_falhou = total_testes - total_passou
        tempo_total = sum(s.duracao_total for s in self.suites)
        
        relatorio = f"""
╔══════════════════════════════════════════════════════════════╗
║                  Relatório de Testes BR                       ║
╚══════════════════════════════════════════════════════════════╝

Suites executadas: {len(self.suites)}
Total de testes: {total_testes}
Passou: {total_passou}
Falhou: {total_falhou}
Tempo total: {tempo_total:.3f}s

"""
        
        for suite in self.suites:
            relatorio += self.relatorio_suite(suite)
        
        return relatorio

def executar_testes(caminho: str) -> str:
    """Executa testes de um arquivo ou diretório"""
    framework = FrameworkTestesBR()
    
    if os.path.isfile(caminho):
        suite = framework.executar_arquivo(caminho)
        return framework.relatorio_suite(suite)
    elif os.path.isdir(caminho):
        suites = framework.executar_diretorio(caminho)
        return framework.relatorio_completo()
    else:
        return f"Erro: '{caminho}' não é um arquivo ou diretório válido."

def main():
    """Ponto de entrada CLI do framework de testes"""
    if len(sys.argv) < 2:
        print("Uso: brasil testar <arquivo.br | diretorio>")
        sys.exit(1)
    
    caminho = sys.argv[1]
    
    # Executa uma unica vez para preservar tempos, saidas e exit code.
    
    # Exit com código de erro se houver falhas
    framework = FrameworkTestesBR()
    if os.path.isfile(caminho):
        suite = framework.executar_arquivo(caminho)
        print(framework.relatorio_suite(suite))
        falhou = sum(1 for t in suite.testes if not t.sucesso)
        if falhou > 0:
            sys.exit(1)
    elif os.path.isdir(caminho):
        suites = framework.executar_diretorio(caminho)
        print(framework.relatorio_completo())
        total_falhou = sum(sum(1 for t in s.testes if not t.sucesso) for s in suites)
        if total_falhou > 0:
            sys.exit(1)
    else:
        print(f"Erro: '{caminho}' nao e um arquivo ou diretorio valido.")
        sys.exit(1)

if __name__ == "__main__":
    main()
