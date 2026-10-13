# -*- coding: utf-8 -*-
"""
Linter Avançado para Linguagem BR
Verifica qualidade, estilo e possíveis erros no código
"""
import re
from typing import List, Dict, Any, Optional
from lexer import LexerBR, TipoToken
from br_ast import *

class RegraLinter:
    def __init__(self, codigo, nome, descricao, nivel="aviso"):
        self.codigo = codigo
        self.nome = nome
        self.descricao = descricao
        self.nivel = nivel  # erro, aviso, sugestao
    
    def verificar(self, linhas, ast) -> List[Dict[str, Any]]:
        raise NotImplementedError

class LinterAvancado:
    def __init__(self):
        self.regras = []
        self._registrar_regras()
    
    def _registrar_regras(self):
        self.regras = [
            RegraVariavelNaoUsada(),
            RegraFuncaoSemRetorno(),
            RegraComplexidadeAlta(),
            RegraNomeDescritivo(),
            RegraTamanhoFuncao(),
            RegraAninhamentoProfundo(),
            RegraComentarioExplicativo(),
            RegraVariavelGlobal(),
            RegraImportNaoUsado(),
            RegraStringLiteral(),
        ]
    
    def lint(self, codigo: str) -> List[Dict[str, Any]]:
        """Executa todas as regras no código"""
        linhas = codigo.split('\n')
        problemas = []
        
        # Parseia o código
        try:
            from parser_completo import parse_codigo
            ast = parse_codigo(codigo)
        except:
            ast = []
        
        for regra in self.regras:
            try:
                resultados = regra.verificar(linhas, ast)
                problemas.extend(resultados)
            except:
                pass
        
        return problemas
    
    def reportar(self, problemas: List[Dict[str, Any]], formato="texto"):
        """Reporta problemas encontrados"""
        if formato == "texto":
            for p in problemas:
                nivel = p.get('nivel', 'aviso')
                icone = {'erro': '❌', 'aviso': '⚠️', 'sugestao': '💡'}.get(nivel, 'ℹ️')
                print(f"{icone} [{nivel.upper()}] Linha {p.get('linha', '?')}: {p.get('mensagem', '')}")
                if 'sugestao' in p:
                    print(f"   💡 Sugestão: {p['sugestao']}")
        
        elif formato == "json":
            import json
            print(json.dumps(problemas, indent=2, ensure_ascii=False))
        
        return problemas

class RegraVariavelNaoUsada(RegraLinter):
    def __init__(self):
        super().__init__("BR001", "variavel-nao-usada", "Variável declarada mas nunca usada", "aviso")
    
    def verificar(self, linhas, ast):
        problemas = []
        variaveis_declaradas = set()
        variaveis_usadas = set()
        
        for no in ast:
            self._coletar_variaveis(no, variaveis_declaradas, variaveis_usadas)
        
        nao_usadas = variaveis_declaradas - variaveis_usadas
        for v in nao_usadas:
            if not v.startswith('_'):
                problemas.append({
                    "codigo": self.codigo,
                    "nome": self.nome,
                    "nivel": self.nivel,
                    "linha": 0,
                    "mensagem": f"Variável '{v}' declarada mas nunca usada",
                    "sugestao": f"Remova a variável '{v}' ou use-a no código"
                })
        
        return problemas
    
    def _coletar_variaveis(self, no, declaradas, usadas):
        if isinstance(no, DeclaracaoVariavel):
            declaradas.add(no.nome)
        elif isinstance(no, Variavel):
            usadas.add(no.nome)
        for attr in dir(no):
            val = getattr(no, attr)
            if isinstance(val, list):
                for item in val:
                    if isinstance(item, NoAST):
                        self._coletar_variaveis(item, declaradas, usadas)

class RegraFuncaoSemRetorno(RegraLinter):
    def __init__(self):
        super().__init__("BR002", "funcao-sem-retorno", "Função que não retorna valor explicitamente", "aviso")
    
    def verificar(self, linhas, ast):
        problemas = []
        for no in ast:
            if isinstance(no, DeclaracaoFuncao):
                tem_retorno = any(isinstance(s, RetornarStmt) for s in no.corpo)
                if not tem_retorno and no.nome not in ('mostre', 'defina'):
                    problemas.append({
                        "codigo": self.codigo,
                        "nome": self.nome,
                        "nivel": self.nivel,
                        "linha": no.posicao.linha,
                        "mensagem": f"Função '{no.nome}' não possui retorno explícito",
                        "sugestao": "Adicione 'devolva' ao final da função ou declare que ela retorna nulo"
                    })
        return problemas

class RegraComplexidadeAlta(RegraLinter):
    def __init__(self):
        super().__init__("BR003", "complexidade-alta", "Função com complexidade ciclomática alta", "sugestao")
    
    def verificar(self, linhas, ast):
        problemas = []
        for no in ast:
            if isinstance(no, DeclaracaoFuncao):
                complexidade = self._calcular_complexidade(no.corpo)
                if complexidade > 10:
                    problemas.append({
                        "codigo": self.codigo,
                        "nome": self.nome,
                        "nivel": self.nivel,
                        "linha": no.posicao.linha,
                        "mensagem": f"Função '{no.nome}' tem complexidade {complexidade} (limite: 10)",
                        "sugestao": "Considere dividir a função em funções menores"
                    })
        return problemas
    
    def _calcular_complexidade(self, corpo):
        complexidade = 1
        for stmt in corpo:
            if isinstance(stmt, (SeStmt, EnquantoStmt, ParaCadaStmt, EscolhaCasoStmt)):
                complexidade += 1
        return complexidade

class RegraNomeDescritivo(RegraLinter):
    def __init__(self):
        super().__init__("BR004", "nome-descritivo", "Nome de variável/função muito curto ou não descritivo", "sugestao")
    
    def verificar(self, linhas, ast):
        problemas = []
        for no in ast:
            if isinstance(no, DeclaracaoVariavel):
                if len(no.nome) < 3 and no.nome not in ('i', 'j', 'k', 'x', 'y', 'z', 'n', 'm'):
                    problemas.append({
                        "codigo": self.codigo,
                        "nome": self.nome,
                        "nivel": self.nivel,
                        "linha": no.posicao.linha,
                        "mensagem": f"Nome de variável '{no.nome}' muito curto",
                        "sugestao": f"Use um nome mais descritivo para '{no.nome}'"
                    })
            elif isinstance(no, DeclaracaoFuncao):
                if len(no.nome) < 4:
                    problemas.append({
                        "codigo": self.codigo,
                        "nome": self.nome,
                        "nivel": self.nivel,
                        "linha": no.posicao.linha,
                        "mensagem": f"Nome de função '{no.nome}' muito curto",
                        "sugestao": f"Use um nome mais descritivo para '{no.nome}'"
                    })
        return problemas

class RegraTamanhoFuncao(RegraLinter):
    def __init__(self):
        super().__init__("BR005", "funcao-muito-longa", "Função com muitas linhas", "sugestao")
    
    def verificar(self, linhas, ast):
        problemas = []
        for no in ast:
            if isinstance(no, DeclaracaoFuncao):
                if len(no.corpo) > 30:
                    problemas.append({
                        "codigo": self.codigo,
                        "nome": self.nome,
                        "nivel": self.nivel,
                        "linha": no.posicao.linha,
                        "mensagem": f"Função '{no.nome}' tem {len(no.corpo)} instruções (limite: 30)",
                        "sugestao": "Considere dividir em funções menores"
                    })
        return problemas

class RegraAninhamentoProfundo(RegraLinter):
    def __init__(self):
        super().__init__("BR006", "aninhamento-profundo", "Blocos muito aninhados", "sugestao")
    
    def verificar(self, linhas, ast):
        problemas = []
        for no in ast:
            profundidade = self._profundidade_maxima(no)
            if profundidade > 4:
                problemas.append({
                    "codigo": self.codigo,
                    "nome": self.nome,
                    "nivel": self.nivel,
                    "linha": no.posicao.linha,
                    "mensagem": f"Aninhamento de {profundidade} níveis (limite: 4)",
                    "sugestao": "Extraia blocos internos para funções separadas"
                })
        return problemas
    
    def _profundidade_maxima(self, no, nivel=0):
        if nivel > 10:
            return nivel
        max_nivel = nivel
        if isinstance(no, (SeStmt, EnquantoStmt, ParaCadaStmt)):
            for stmt in getattr(no, 'corpo', []) or getattr(no, 'corpo_se', []):
                max_nivel = max(max_nivel, self._profundidade_maxima(stmt, nivel + 1))
        return max_nivel

class RegraComentarioExplicativo(RegraLinter):
    def __init__(self):
        super().__init__("BR007", "faltando-comentario", "Função sem comentário explicativo", "sugestao")
    
    def verificar(self, linhas, ast):
        problemas = []
        for no in ast:
            if isinstance(no, DeclaracaoFuncao) and len(no.corpo) > 5:
                if not no.docstring:
                    problemas.append({
                        "codigo": self.codigo,
                        "nome": self.nome,
                        "nivel": self.nivel,
                        "linha": no.posicao.linha,
                        "mensagem": f"Função '{no.nome}' sem documentação",
                        "sugestao": "Adicione um bloco explicacao ou docstring à função"
                    })
        return problemas

class RegraVariavelGlobal(RegraLinter):
    def __init__(self):
        super().__init__("BR008", "variavel-global", "Variável global declarada", "aviso")
    
    def verificar(self, linhas, ast):
        problemas = []
        for no in ast:
            if isinstance(no, DeclaracaoVariavel) and not no.eh_constante:
                problemas.append({
                    "codigo": self.codigo,
                    "nome": self.nome,
                    "nivel": self.nivel,
                    "linha": no.posicao.linha,
                    "mensagem": f"Variável global '{no.nome}'",
                    "sugestao": "Considere usar constante ou encapsular em função"
                })
        return problemas

class RegraImportNaoUsado(RegraLinter):
    def __init__(self):
        super().__init__("BR009", "import-nao-usado", "Módulo importado mas não usado", "aviso")
    
    def verificar(self, linhas, ast):
        problemas = []
        imports = []
        usados = set()
        
        for no in ast:
            if isinstance(no, DeclaracaoImportacao):
                imports.append(no)
            elif isinstance(no, ChamadaFuncao):
                usados.add(no.nome)
        
        for imp in imports:
            nome_modulo = imp.alias or imp.modulo.split('/')[-1].replace('.br', '')
            if nome_modulo not in usados and nome_modulo not in [str(s) for s in usados]:
                problemas.append({
                    "codigo": self.codigo,
                    "nome": self.nome,
                    "nivel": self.nivel,
                    "linha": imp.posicao.linha,
                    "mensagem": f"Módulo '{imp.modulo}' importado mas não usado",
                    "sugestao": f"Remova 'importar \"{imp.modulo}\"'"
                })
        
        return problemas

class RegraStringLiteral(RegraLinter):
    def __init__(self):
        super().__init__("BR010", "string-literal", "String literal que poderia ser F-string", "sugestao")
    
    def verificar(self, linhas, ast):
        problemas = []
        for i, linha in enumerate(linhas, 1):
            if '+' in linha and ('"' in linha or "'" in linha):
                if 'f"' not in linha and "f'" not in linha:
                    if any(v in linha for v in [' + ', '+ ']):
                        problemas.append({
                            "codigo": self.codigo,
                            "nome": self.nome,
                            "nivel": self.nivel,
                            "linha": i,
                            "mensagem": "Concatenação de strings pode ser substituída por F-string",
                            "sugestao": "Use f\"...{var}...\" ao invés de \"...\" + var + \"...\""
                        })
        return problemas

def lint_arquivo(caminho):
    """Executa linter em um arquivo"""
    with open(caminho, 'r', encoding='utf-8') as f:
        codigo = f.read()
    
    linter = LinterAvancado()
    problemas = linter.lint(codigo)
    linter.reportar(problemas)
    return problemas

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        lint_arquivo(sys.argv[1])
    else:
        print("Uso: python linter_avancado.py <arquivo.br>")