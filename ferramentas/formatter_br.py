# -*- coding: utf-8 -*-
"""
Formatador Automático para Linguagem BR
Aplica formatação canônica ao código BR
"""
import re
from typing import List

class FormatadorBR:
    def __init__(self, indentacao=4):
        self.indentacao = indentacao
        self.nivel = 0
    
    def formatar(self, codigo: str) -> str:
        """Formata código BR"""
        linhas = codigo.split('\n')
        resultado = []
        self.nivel = 0
        pula_linha = False
        
        for i, linha in enumerate(linhas):
            linha_original = linha
            linha_strip = linha.strip()
            
            # Pula linhas vazias
            if not linha_strip:
                if pula_linha:
                    resultado.append('')
                    pula_linha = False
                continue
            
            # Verifica se linha reduz indentação
            if self._inicia_bloco(linha_strip):
                resultado.append(self._indentar() + linha_strip)
                self.nivel += 1
                pula_linha = True
            elif self._fecha_bloco(linha_strip):
                self.nivel = max(0, self.nivel - 1)
                resultado.append(self._indentar() + linha_strip)
                pula_linha = True
            else:
                resultado.append(self._indentar() + linha_strip)
                pula_linha = False
        
        return '\n'.join(resultado)
    
    def _indentar(self) -> str:
        return ' ' * (self.nivel * self.indentacao)
    
    def _inicia_bloco(self, linha: str) -> bool:
        """Verifica se linha inicia um bloco"""
        palavras = ['função', 'funcao', 'se', 'enquanto', 'para', 'repita', 
                    'conte', 'tente', 'escolha', 'execute', 'tarefa',
                    'classe', 'metodo', 'método', 'lista', 'registro',
                    'explicacao', 'objetivo', 'teste', 'com', 'corresponda',
                    'crie']
        for p in palavras:
            if linha.startswith(p) or linha.startswith(p.upper()):
                return True
        return False
    
    def _fecha_bloco(self, linha: str) -> bool:
        """Verifica se linha fecha um bloco"""
        return linha == 'fim' or linha.startswith('fim ') or linha.startswith('senão') or linha.startswith('senao')

def formatar_arquivo(caminho):
    """Formata um arquivo .br"""
    with open(caminho, 'r', encoding='utf-8') as f:
        codigo = f.read()
    
    formatador = FormatadorBR()
    codigo_formatado = formatador.formatar(codigo)
    
    with open(caminho, 'w', encoding='utf-8') as f:
        f.write(codigo_formatado)
    
    print(f"[Formatador] Arquivo formatado: {caminho}")
    return codigo_formatado

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        formatar_arquivo(sys.argv[1])
    else:
        print("Uso: python formatter_br.py <arquivo.br>")