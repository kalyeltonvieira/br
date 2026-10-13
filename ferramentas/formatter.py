# -*- coding: utf-8 -*-
"""
Formatador de Código BR (brasil formatar)
Padroniza estilo de código com regras de indentação e espaçamento
"""
import re
from typing import List, Tuple

class FormatadorBR:
    """Formatador de código BR"""
    
    def __init__(self):
        self.indentacao = 4  # Espaços por nível
        self.max_linha = 100  # Comprimento máximo de linha
    
    def formatar(self, codigo: str) -> str:
        """Formata código BR completo"""
        linhas = codigo.splitlines()
        linhas_formatadas = []
        
        nivel_indentacao = 0
        em_bloco = False
        
        for i, linha in enumerate(linhas):
            linha = linha.rstrip()
            
            # Pular linhas vazias (mas manter uma entre blocos)
            if not linha.strip():
                if linhas_formatadas and linhas_formatadas[-1].strip():
                    linhas_formatadas.append("")
                continue
            
            # Remover espaços extras no início
            linha = linha.lstrip()
            
            # Detectar fechamento de bloco
            if linha.strip() == "fim":
                nivel_indentacao = max(0, nivel_indentacao - 1)
            
            # Aplicar indentação
            indentacao = " " * (nivel_indentacao * self.indentacao)
            linha_formatada = indentacao + linha
            
            # Aplicar regras de espaçamento
            linha_formatada = self._aplicar_regras_espacamento(linha_formatada)
            
            # Quebrar linha longa se necessário
            linha_formatada = self._quebrar_linha_longa(linha_formatada)
            
            linhas_formatadas.append(linha_formatada)
            
            # Detectar abertura de bloco
            if self._abre_bloco(linha):
                nivel_indentacao += 1
                em_bloco = True
        
        # Remover linhas vazias consecutivas extras
        resultado = []
        linha_anterior_vazia = False
        
        for linha in linhas_formatadas:
            if not linha.strip():
                if not linha_anterior_vazia:
                    resultado.append(linha)
                    linha_anterior_vazia = True
            else:
                resultado.append(linha)
                linha_anterior_vazia = False
        
        return "\n".join(resultado)
    
    def _abre_bloco(self, linha: str) -> bool:
        """Verifica se a linha abre um bloco"""
        palavras_bloco = [
            "se ", "enquanto ", "para cada ", "tente ", "execute em paralelo",
            "função ", "lista", "registro", "explicacao", "objetivo", "teste "
        ]
        
        for palavra in palavras_bloco:
            if linha.strip().startswith(palavra):
                return True
        
        return False
    
    def _aplicar_regras_espacamento(self, linha: str) -> str:
        """Aplica regras de espaçamento"""
        # Espaço após operadores
        linha = re.sub(r'(\+|\-|\*|\/|==|!=|<=|>=|<|>)', r' \1 ', linha)
        
        # Espaço em atribuições
        linha = re.sub(r'(\w+)=(\S)', r'\1 = \2', linha)
        
        # Espaço após vírgulas
        linha = re.sub(r',(\S)', r', \1', linha)
        
        # Espaço após dois pontos em registros
        linha = re.sub(r':(\S)', r': \1', linha)
        
        # Remover espaços múltiplos
        linha = re.sub(r'  +', ' ', linha)
        
        # Espaço obrigatório em "é"
        linha = re.sub(r'(\w+)é', r'\1 é ', linha)
        
        return linha.strip()
    
    def _quebrar_linha_longa(self, linha: str) -> str:
        """Quebra linha longa em múltiplas linhas"""
        if len(linha) <= self.max_linha:
            return linha
        
        # Tentar quebrar em operadores
        operadores = [" + ", " - ", " * ", " / ", " == ", " != ", " <= ", " >= ", " < ", " > "]
        
        for op in operadores:
            if op in linha:
                partes = linha.split(op)
                if len(partes) == 2:
                    return partes[0] + "\n" + " " * 8 + op + partes[1]
        
        # Tentar quebrar em vírgulas
        if "," in linha:
            partes = linha.split(",")
            if len(partes) > 1:
                resultado = partes[0]
                for parte in partes[1:]:
                    resultado += ",\n" + " " * 8 + parte
                return resultado
        
        return linha

def formatar_arquivo(caminho_entrada: str, caminho_saida: Optional[str] = None) -> str:
    """Formata um arquivo BR"""
    with open(caminho_entrada, "r", encoding="utf-8") as f:
        codigo = f.read()
    
    formatador = FormatadorBR()
    codigo_formatado = formatador.formatar(codigo)
    
    if caminho_saida:
        with open(caminho_saida, "w", encoding="utf-8") as f:
            f.write(codigo_formatado)
        return f"Arquivo formatado: {caminho_saida}"
    
    return codigo_formatado

def main():
    """Ponto de entrada CLI do formatador"""
    import sys
    
    if len(sys.argv) < 2:
        print("Uso: brasil formatar <arquivo.br> [saida.br]")
        sys.exit(1)
    
    caminho_entrada = sys.argv[1]
    caminho_saida = sys.argv[2] if len(sys.argv) > 2 else None
    
    if not caminho_saida:
        caminho_saida = caminho_entrada  # Sobrescrever original
    
    resultado = formatar_arquivo(caminho_entrada, caminho_saida)
    print(resultado)

if __name__ == "__main__":
    main()
