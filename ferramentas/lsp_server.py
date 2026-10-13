# -*- coding: utf-8 -*-
"""
LSP (Language Server Protocol) para Linguagem BR
Fornece autocomplete, diagnóstico, go-to-definition, hover
"""
import sys
import json
from typing import List, Dict, Any, Optional
from lexer import LexerBR, TipoToken
from br_ast import *
from parser_completo import parse_codigo

class LSPServerBR:
    def __init__(self):
        self.arquivos: Dict[str, str] = {}
        self.asts: Dict[str, List[NoAST]] = {}
        self.simbolos: Dict[str, List[Dict[str, Any]]] = {}
        self.workspace = ""
    
    def iniciar(self):
        """Inicia o servidor LSP"""
        print("[LSP-BR] Servidor iniciado", flush=True)
        while True:
            try:
                linha = sys.stdin.readline()
                if not linha:
                    break
                
                # Lê cabeçalho Content-Length
                if linha.startswith("Content-Length:"):
                    length = int(linha.split(":")[1].strip())
                    sys.stdin.readline()  # blank line
                    conteudo = sys.stdin.read(length)
                    self._processar_mensagem(json.loads(conteudo))
            except Exception as e:
                print(f"[LSP-BR] Erro: {e}", flush=True)
    
    def _enviar(self, mensagem: dict):
        """Envia mensagem JSON-RPC"""
        conteudo = json.dumps(mensagem, ensure_ascii=False)
        sys.stdout.write(f"Content-Length: {len(conteudo)}\r\n\r\n{conteudo}")
        sys.stdout.flush()
    
    def _processar_mensagem(self, msg: dict):
        metodo = msg.get("method", "")
        id_ = msg.get("id")
        params = msg.get("params", {})
        
        if metodo == "initialize":
            self._inicializar(id_, params)
        elif metodo == "textDocument/didOpen":
            self._abrir_documento(params)
        elif metodo == "textDocument/didChange":
            self._alterar_documento(params)
        elif metodo == "textDocument/completion":
            self._completar(id_, params)
        elif metodo == "textDocument/hover":
            self._hover(id_, params)
        elif metodo == "textDocument/definition":
            self._definicao(id_, params)
        elif metodo == "textDocument/references":
            self._referencias(id_, params)
        elif metodo == "textDocument/rename":
            self._renomear(id_, params)
        elif metodo == "textDocument/documentSymbol":
            self._simbolos_documento(id_, params)
        elif metodo == "textDocument/codeAction":
            self._acoes_codigo(id_, params)
        elif metodo == "shutdown":
            self._desligar(id_)
    
    def _inicializar(self, id_: int, params: dict):
        self.workspace = params.get("rootUri", "")
        self._enviar({
            "id": id_,
            "result": {
                "capabilities": {
                    "textDocumentSync": 1,
                    "completionProvider": {
                        "triggerCharacters": [".", "(", "\"", "f"],
                        "resolveProvider": True
                    },
                    "hoverProvider": True,
                    "definitionProvider": True,
                    "referencesProvider": True,
                    "renameProvider": True,
                    "documentSymbolProvider": True,
                    "codeActionProvider": True
                },
                "serverInfo": {
                    "name": "LSP-BR",
                    "version": "1.0.0"
                }
            }
        })
    
    def _abrir_documento(self, params: dict):
        uri = params["textDocument"]["uri"]
        texto = params["textDocument"]["text"]
        self.arquivos[uri] = texto
        self._analisar(uri, texto)
        self._enviar_diagnosticos(uri)
    
    def _alterar_documento(self, params: dict):
        uri = params["textDocument"]["uri"]
        for change in params.get("contentChanges", []):
            texto = change.get("text", "")
            self.arquivos[uri] = texto
            self._analisar(uri, texto)
        self._enviar_diagnosticos(uri)
    
    def _analisar(self, uri: str, texto: str):
        """Analisa o código e extrai símbolos"""
        try:
            ast = parse_codigo(texto)
            self.asts[uri] = ast
            self.simbolos[uri] = self._extrair_simbolos(ast)
        except Exception as e:
            self.asts[uri] = []
            self.simbolos[uri] = []
    
    def _extrair_simbolos(self, ast: List[NoAST]) -> List[Dict[str, Any]]:
        simbolos = []
        for no in ast:
            if isinstance(no, DeclaracaoFuncao):
                simbolos.append({
                    "nome": no.nome,
                    "tipo": "função",
                    "linha": no.posicao.linha,
                    "detalhes": f"({', '.join(no.parametros)})"
                })
            elif isinstance(no, DeclaracaoVariavel):
                simbolos.append({
                    "nome": no.nome,
                    "tipo": "constante" if no.eh_constante else "variável",
                    "linha": no.posicao.linha,
                    "detalhes": str(no.tipo_estatico.value if no.tipo_estatico else "")
                })
            elif isinstance(no, DeclaracaoClasse):
                simbolos.append({
                    "nome": no.nome,
                    "tipo": "classe",
                    "linha": no.posicao.linha,
                    "detalhes": f"herda {no.classe_mae}" if no.classe_mae else ""
                })
        return simbolos
    
    def _enviar_diagnosticos(self, uri: str):
        """Envia diagnósticos (erros) para o editor"""
        diagnosticos = []
        
        try:
            from linter_avancado import LinterAvancado
            linter = LinterAvancado()
            problemas = linter.lint(self.arquivos.get(uri, ""))
            
            for p in problemas:
                diagnosticos.append({
                    "range": {
                        "start": {"line": max(0, p.get("linha", 1) - 1), "character": 0},
                        "end": {"line": max(0, p.get("linha", 1) - 1), "character": 100}
                    },
                    "severity": 2 if p.get("nivel") == "erro" else 3,
                    "message": p.get("mensagem", ""),
                    "source": "br-lint"
                })
        except:
            pass
        
        self._enviar({
            "method": "textDocument/publishDiagnostics",
            "params": {
                "uri": uri,
                "diagnostics": diagnosticos
            }
        })
    
    def _completar(self, id_: int, params: dict):
        """Fornece autocomplete"""
        uri = params["textDocument"]["uri"]
        linha = params["position"]["line"]
        caractere = params["position"]["character"]
        
        items = [
            {"label": "função", "kind": 2, "detail": "Declaração de função", "insertText": "função nome\n    \nfim"},
            {"label": "se", "kind": 14, "detail": "Condicional se", "insertText": "se  então\n    \nfim"},
            {"label": "enquanto", "kind": 14, "detail": "Loop enquanto", "insertText": "enquanto \n    \nfim"},
            {"label": "para", "kind": 14, "detail": "Loop para cada", "insertText": "para cada  em \n    \nfim"},
            {"label": "repita", "kind": 14, "detail": "Loop repita", "insertText": "repita  vezes\n    \nfim"},
            {"label": "conte", "kind": 14, "detail": "Loop conte", "insertText": "conte de 1 até 10 como i\n    \nfim"},
            {"label": "classe", "kind": 2, "detail": "Declaração de classe", "insertText": "classe Nome\n    \nfim"},
            {"label": "tente", "kind": 14, "detail": "Tratamento de erro", "insertText": "tente\n    \ncapture erro\n    \nfim"},
            {"label": "escolha", "kind": 14, "detail": "Switch/case", "insertText": "escolha \n    caso \n        \n    padrao\n        \nfim"},
            {"label": "tarefa", "kind": 2, "detail": "Tarefa assíncrona", "insertText": "tarefa nome\n    \nfim"},
            {"label": "mostre", "kind": 2, "detail": "Mostrar valor", "insertText": "mostre "},
            {"label": "devolva", "kind": 2, "detail": "Retornar valor", "insertText": "devolva "},
            {"label": "importar", "kind": 2, "detail": "Importar módulo", "insertText": "importar \"\""},
            {"label": "constante", "kind": 2, "detail": "Declarar constante", "insertText": "constante NOME é "},
            {"label": "fim", "kind": 14, "detail": "Fim de bloco", "insertText": "fim"},
            {"label": "f\"\"", "kind": 2, "detail": "F-string", "insertText": "f\"{}\""},
            {"label": "com", "kind": 14, "detail": "Context manager", "insertText": "com  como \n    \nfim"},
            {"label": "corresponda", "kind": 14, "detail": "Pattern matching", "insertText": "corresponda \n    caso \n        \n    padrao\n        \nfim"},
        ]
        
        # Adiciona símbolos do documento
        for s in self.simbolos.get(uri, []):
            items.append({
                "label": s["nome"],
                "kind": 6 if s["tipo"] == "função" else 13,
                "detail": s["tipo"],
                "insertText": s["nome"]
            })
        
        self._enviar({"id": id_, "result": {"isIncomplete": False, "items": items}})
    
    def _hover(self, id_: int, params: dict):
        """Fornece informações ao passar mouse"""
        uri = params["textDocument"]["uri"]
        linha = params["position"]["line"]
        
        # Busca símbolo na linha
        for s in self.simbolos.get(uri, []):
            if s["linha"] == linha + 1:
                self._enviar({
                    "id": id_,
                    "result": {
                        "contents": {
                            "kind": "markdown",
                            "value": f"**{s['nome']}** ({s['tipo']})\n\n{s['detalhes']}"
                        }
                    }
                })
                return
        
        self._enviar({"id": id_, "result": None})
    
    def _definicao(self, id_: int, params: dict):
        """Vai para definição do símbolo"""
        uri = params["textDocument"]["uri"]
        palavra = self._palavra_em(uri, params["position"])
        
        if palavra:
            for s in self.simbolos.get(uri, []):
                if s["nome"] == palavra:
                    self._enviar({
                        "id": id_,
                        "result": {
                            "uri": uri,
                            "range": {
                                "start": {"line": s["linha"] - 1, "character": 0},
                                "end": {"line": s["linha"] - 1, "character": len(s["nome"])}
                            }
                        }
                    })
                    return
        
        self._enviar({"id": id_, "result": None})
    
    def _referencias(self, id_: int, params: dict):
        self._enviar({"id": id_, "result": []})
    
    def _renomear(self, id_: int, params: dict):
        self._enviar({"id": id_, "result": None})
    
    def _simbolos_documento(self, id_: int, params: dict):
        uri = params["textDocument"]["uri"]
        simbolos = []
        
        for s in self.simbolos.get(uri, []):
            simbolos.append({
                "name": s["nome"],
                "kind": 12 if s["tipo"] == "função" else 13,
                "range": {
                    "start": {"line": s["linha"] - 1, "character": 0},
                    "end": {"line": s["linha"] - 1, "character": len(s["nome"])}
                },
                "selectionRange": {
                    "start": {"line": s["linha"] - 1, "character": 0},
                    "end": {"line": s["linha"] - 1, "character": len(s["nome"])}
                }
            })
        
        self._enviar({"id": id_, "result": simbolos})
    
    def _acoes_codigo(self, id_: int, params: dict):
        self._enviar({"id": id_, "result": []})
    
    def _desligar(self, id_: int):
        self._enviar({"id": id_, "result": None})
        sys.exit(0)
    
    def _palavra_em(self, uri: str, position: dict) -> Optional[str]:
        """Obtém a palavra na posição"""
        texto = self.arquivos.get(uri, "")
        linhas = texto.split('\n')
        linha = position["line"]
        caractere = position["character"]
        
        if linha < len(linhas):
            linha_texto = linhas[linha]
            # Encontra palavra no caractere
            inicio = caractere
            while inicio > 0 and (linha_texto[inicio-1].isalnum() or linha_texto[inicio-1] == '_'):
                inicio -= 1
            fim = caractere
            while fim < len(linha_texto) and (linha_texto[fim].isalnum() or linha_texto[fim] == '_'):
                fim += 1
            if inicio < fim:
                return linha_texto[inicio:fim]
        
        return None

if __name__ == "__main__":
    server = LSPServerBR()
    server.iniciar()