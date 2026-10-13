# -*- coding: utf-8 -*-
"""
Gerador de Documentação (brasil documentar) para Linguagem BR
Gera documentação automática a partir de blocos explicacao e código fonte
"""
import os
import re
from typing import List, Dict, Optional, Tuple
from dataclasses import dataclass
from lexer import LexerBR
from parser import ParserBR
from br_ast import *

@dataclass
class DocumentacaoFuncao:
    nome: str
    parametros: List[str]
    descricao: str
    linha: int
    tipo_retorno: Optional[str] = None

@dataclass
class DocumentacaoModulo:
    nome: str
    descricao: str
    funcoes: List[DocumentacaoFuncao]
    variaveis: List[str]

class GeradorDocumentacaoBR:
    """Gerador de documentação para código BR"""
    
    def __init__(self):
        self.modulos: List[DocumentacaoModulo] = []
        self.blocos_explicacao: Dict[str, str] = {}
    
    def gerar_arquivo(self, caminho: str, formato: str = "markdown") -> str:
        """Gera documentação para um arquivo BR"""
        with open(caminho, "r", encoding="utf-8") as f:
            codigo = f.read()
        
        # Tokenizar e parsear
        lexer = LexerBR(codigo)
        tokens = lexer.tokenizar()
        
        parser = ParserBR(tokens)
        ast = parser.parse()
        
        # Extrair documentação
        nome_modulo = os.path.basename(caminho).replace(".br", "").replace(".brm", "")
        descricao = ""
        funcoes = []
        variaveis = []
        
        for no in ast:
            if isinstance(no, BlocoExplicacao):
                descricao = no.conteudo
                self.blocos_explicacao[nome_modulo] = no.conteudo
            
            elif isinstance(no, DeclaracaoFuncao):
                doc_funcao = DocumentacaoFuncao(
                    nome=no.nome,
                    parametros=no.parametros,
                    descricao="",  # Pode ser extraída de bloco explicacao antes da função
                    linha=no.posicao.linha,
                    tipo_retorno=no.tipo_retorno.value if no.tipo_retorno else None
                )
                funcoes.append(doc_funcao)
            
            elif isinstance(no, DeclaracaoVariavel):
                variaveis.append(no.nome)
        
        modulo = DocumentacaoModulo(nome_modulo, descricao, funcoes, variaveis)
        self.modulos.append(modulo)
        
        # Gerar saída no formato solicitado
        if formato == "markdown":
            return self._gerar_markdown(modulo)
        elif formato == "html":
            return self._gerar_html(modulo)
        elif formato == "json":
            return self._gerar_json(modulo)
        else:
            return self._gerar_markdown(modulo)
    
    def gerar_diretorio(self, diretorio: str, formato: str = "markdown", saida: str = "documentacao"):
        """Gera documentação para todos os arquivos em um diretório"""
        if not os.path.exists(diretorio):
            print(f"Diretório '{diretorio}' não encontrado.")
            return
        
        os.makedirs(saida, exist_ok=True)
        
        for arquivo in os.listdir(diretorio):
            if arquivo.endswith(".br") or arquivo.endswith(".brm"):
                caminho = os.path.join(diretorio, arquivo)
                doc = self.gerar_arquivo(caminho, formato)
                
                nome_saida = arquivo.replace(".br", "").replace(".brm", "")
                if formato == "markdown":
                    caminho_saida = os.path.join(saida, f"{nome_saida}.md")
                elif formato == "html":
                    caminho_saida = os.path.join(saida, f"{nome_saida}.html")
                else:
                    caminho_saida = os.path.join(saida, f"{nome_saida}.json")
                
                with open(caminho_saida, "w", encoding="utf-8") as f:
                    f.write(doc)
                
                print(f"Documentação gerada: {caminho_saida}")
    
    def _gerar_markdown(self, modulo: DocumentacaoModulo) -> str:
        """Gera documentação em formato Markdown"""
        md = f"# {modulo.nome}\n\n"
        
        if modulo.descricao:
            md += f"{modulo.descricao}\n\n"
        
        if modulo.funcoes:
            md += "## Funções\n\n"
            for funcao in modulo.funcoes:
                md += f"### {funcao.nome}\n\n"
                
                if funcao.parametros:
                    params_str = ", ".join(funcao.parametros)
                    md += f"**Parâmetros:** `{params_str}`\n\n"
                
                if funcao.tipo_retorno:
                    md += f"**Retorno:** `{funcao.tipo_retorno}`\n\n"
                
                md += f"**Linha:** {funcao.linha}\n\n"
                md += "---\n\n"
        
        if modulo.variaveis:
            md += "## Variáveis\n\n"
            for var in modulo.variaveis:
                md += f"- `{var}`\n"
            md += "\n"
        
        return md
    
    def _gerar_html(self, modulo: DocumentacaoModulo) -> str:
        """Gera documentação em formato HTML"""
        html = f"""<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <title>{modulo.nome} - Documentação BR</title>
    <style>
        body {{ font-family: Arial, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }}
        h1 {{ color: #333; border-bottom: 2px solid #007acc; }}
        h2 {{ color: #555; margin-top: 30px; }}
        h3 {{ color: #666; }}
        code {{ background: #f4f4f4; padding: 2px 6px; border-radius: 3px; }}
        .funcao {{ background: #f9f9f9; padding: 15px; margin: 10px 0; border-radius: 5px; }}
    </style>
</head>
<body>
    <h1>{modulo.nome}</h1>
"""
        
        if modulo.descricao:
            html += f"    <p>{modulo.descricao}</p>\n"
        
        if modulo.funcoes:
            html += "    <h2>Funções</h2>\n"
            for funcao in modulo.funcoes:
                html += f"    <div class=\"funcao\">\n"
                html += f"        <h3>{funcao.nome}</h3>\n"
                
                if funcao.parametros:
                    params_str = ", ".join(funcao.parametros)
                    html += f"        <p><strong>Parâmetros:</strong> <code>{params_str}</code></p>\n"
                
                if funcao.tipo_retorno:
                    html += f"        <p><strong>Retorno:</strong> <code>{funcao.tipo_retorno}</code></p>\n"
                
                html += f"        <p><strong>Linha:</strong> {funcao.linha}</p>\n"
                html += f"    </div>\n"
        
        if modulo.variaveis:
            html += "    <h2>Variáveis</h2>\n"
            html += "    <ul>\n"
            for var in modulo.variaveis:
                html += f"        <li><code>{var}</code></li>\n"
            html += "    </ul>\n"
        
        html += """
</body>
</html>
"""
        return html
    
    def _gerar_json(self, modulo: DocumentacaoModulo) -> str:
        """Gera documentação em formato JSON"""
        import json
        
        data = {
            "nome": modulo.nome,
            "descricao": modulo.descricao,
            "funcoes": [
                {
                    "nome": f.nome,
                    "parametros": f.parametros,
                    "tipo_retorno": f.tipo_retorno,
                    "linha": f.linha
                }
                for f in modulo.funcoes
            ],
            "variaveis": modulo.variaveis
        }
        
        return json.dumps(data, indent=2, ensure_ascii=False)

def gerar_documentacao(caminho: str, formato: str = "markdown", saida: Optional[str] = None) -> str:
    """Gera documentação para arquivo ou diretório"""
    gerador = GeradorDocumentacaoBR()
    
    if os.path.isfile(caminho):
        doc = gerador.gerar_arquivo(caminho, formato)
        
        if saida:
            with open(saida, "w", encoding="utf-8") as f:
                f.write(doc)
            return f"Documentação gerada: {saida}"
        
        return doc
    
    elif os.path.isdir(caminho):
        saida_dir = saida or "documentacao"
        gerador.gerar_diretorio(caminho, formato, saida_dir)
        return f"Documentação gerada em: {saida_dir}"
    
    else:
        return f"Erro: '{caminho}' não é um arquivo ou diretório válido."

def main():
    """Ponto de entrada CLI do gerador de documentação"""
    import sys
    
    if len(sys.argv) < 2:
        print("Uso: brasil documentar <arquivo.br | diretorio> [formato] [saida]")
        print("Formatos: markdown (padrão), html, json")
        sys.exit(1)
    
    caminho = sys.argv[1]
    formato = sys.argv[2] if len(sys.argv) > 2 else "markdown"
    saida = sys.argv[3] if len(sys.argv) > 3 else None
    
    resultado = gerar_documentacao(caminho, formato, saida)
    print(resultado)

if __name__ == "__main__":
    main()
