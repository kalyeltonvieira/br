# -*- coding: utf-8 -*-
"""
Test Runner para Linguagem BR
Executa testes automatizados em arquivos .brt
"""
import sys
import os
from pathlib import Path
from typing import List, Dict, Tuple

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from lexer import LexerBR
from parser import ParserBR
from semantic_analyzer import AnalisadorSemanticoBR, analisar_semantica
from optimizer import otimizar_codigo
from codegen import gerar_bytecode
from maquina_virtual import MaquinaVirtualBR

class ResultadoTeste:
    def __init__(self, nome: str, sucesso: bool, mensagem: str = ""):
        self.nome = nome
        self.sucesso = sucesso
        self.mensagem = mensagem
    
    def __repr__(self):
        status = "✓ PASSOU" if self.sucesso else "✗ FALHOU"
        return f"{status}: {self.nome} - {self.mensagem}"

class TestRunnerBR:
    def __init__(self):
        self.resultados: List[ResultadoTeste] = []
        self.testes_totais = 0
        self.testes_passados = 0
        self.testes_falhados = 0
    
    def executar_arquivo_teste(self, caminho: str) -> List[ResultadoTeste]:
        """Executa todos os testes em um arquivo .brt"""
        try:
            with open(caminho, 'r', encoding='utf-8') as f:
                codigo = f.read()
        except Exception as e:
            return [ResultadoTeste(caminho, False, f"Erro ao ler arquivo: {e}")]
        
        # Tokenizar
        lexer = LexerBR(codigo)
        tokens = lexer.tokenizar()
        
        # Parsear
        parser = ParserBR(tokens)
        ast = parser.parse()
        
        # Analisar semanticamente
        analisador = AnalisadorSemanticoBR()
        analisador.entrar_escopo()
        sucesso, erros, avisos = analisar_semantica(ast)
        
        if not sucesso:
            return [ResultadoTeste(caminho, False, f"Erros semânticos: {erros}")]
        
        # Otimizar
        ast_otimizado, relatorio = otimizar_codigo(ast)
        
        # Gerar bytecode
        bytecode = gerar_bytecode(ast_otimizado)
        
        # Executar na VM
        vm = MaquinaVirtualBR(bytecode)
        
        try:
            vm.executar()
            # Verificar se há testes definidos
            if hasattr(vm, 'testes') and vm.testes:
                resultados = []
                for teste in vm.testes:
                    resultado = self._executar_teste_individual(vm, teste)
                    resultados.append(resultado)
                    self.resultados.append(resultado)
                return resultados
            else:
                return [ResultadoTeste(caminho, False, "Nenhum teste definido")]
        except Exception as e:
            return [ResultadoTeste(caminho, False, f"Erro de execução: {e}")]
    
    def _executar_teste_individual(self, vm, teste: Dict) -> ResultadoTeste:
        """Executa um teste individual"""
        nome = teste.get('nome', 'Teste sem nome')
        descricao = teste.get('descricao', '')
        
        try:
            # Executar o corpo do teste
            if 'corpo' in teste:
                vm.executar_instrucoes(teste['corpo'])
            
            # Verificar asserções
            if 'asserts' in teste:
                for assert_info in teste['asserts']:
                    tipo = assert_info.get('tipo')
                    esperado = assert_info.get('esperado')
                    obtido = assert_info.get('obtido')
                    
                    if tipo == 'igual':
                        if obtido != esperado:
                            return ResultadoTeste(nome, False, f"Esperava {esperado}, obteve {obtido}")
                    elif tipo == 'verdadeiro':
                        if not obtido:
                            return ResultadoTeste(nome, False, f"Esperava verdadeiro, obteve {obtido}")
                    elif tipo == 'falso':
                        if obtido:
                            return ResultadoTeste(nome, False, f"Esperava falso, obteve {obtido}")
            
            return ResultadoTeste(nome, True, descricao)
        except Exception as e:
            return ResultadoTeste(nome, False, f"Erro ao executar teste: {e}")
    
    def executar_diretorio_testes(self, diretorio: str) -> List[ResultadoTeste]:
        """Executa todos os testes em um diretório"""
        resultados_totais = []
        caminho = Path(diretorio)
        
        if not caminho.exists():
            return [ResultadoTeste(diretorio, False, "Diretório não existe")]
        
        for arquivo in caminho.glob('*.brt'):
            resultados = self.executar_arquivo_teste(str(arquivo))
            resultados_totais.extend(resultados)
        
        return resultados_totais
    
    def relatorio_final(self) -> str:
        """Gera relatório final dos testes"""
        self.testes_totais = len(self.resultados)
        self.testes_passados = sum(1 for r in self.resultados if r.sucesso)
        self.testes_falhados = self.testes_totais - self.testes_passados
        
        relatorio = f"""
=== RELATÓRIO DE TESTES BR ===
Total de testes: {self.testes_totais}
Testes passados: {self.testes_passados}
Testes falhados: {self.testes_falhados}
Taxa de sucesso: {(self.testes_passados / self.testes_totais * 100) if self.testes_totais > 0 else 0:.1f}%

"""
        
        if self.testes_falhados > 0:
            relatorio += "=== TESTES FALHADOS ===\n"
            for resultado in self.resultados:
                if not resultado.sucesso:
                    relatorio += f"  {resultado}\n"
            relatorio += "\n"
        
        return relatorio

def main():
    if len(sys.argv) < 2:
        print("Uso: python test_runner.py <arquivo.brt ou diretorio>")
        sys.exit(1)
    
    caminho = sys.argv[1]
    runner = TestRunnerBR()
    
    if Path(caminho).is_file():
        resultados = runner.executar_arquivo_teste(caminho)
    else:
        resultados = runner.executar_diretorio_testes(caminho)
    
    for resultado in resultados:
        print(resultado)
    
    print(runner.relatorio_final())
    
    if runner.testes_falhados > 0:
        sys.exit(1)

if __name__ == "__main__":
    main()
