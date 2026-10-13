# -*- coding: utf-8 -*-
"""
Framework Web para Linguagem BR
Framework web completo estilo Flask/Django
"""
import json
import socket
import threading
from typing import Dict, List, Any, Callable, Optional
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs

class Rota:
    def __init__(self, path: str, metodo: str = "GET"):
        self.path = path
        self.metodo = metodo.upper()
        self.funcao: Optional[Callable] = None
        self.template: Optional[str] = None
    
    def __repr__(self):
        return f"Rota({self.metodo} {self.path})"

class SiteBR:
    def __init__(self, nome: str = ""):
        self.nome = nome
        self.porta = 8080
        self.rotas: List[Rota] = []
        self.middlewares: List[Callable] = []
        self.templates: Dict[str, str] = {}
        self.estaticos: Dict[str, bytes] = {}
        self.server: Optional[HTTPServer] = None
    
    def rota(self, path: str, metodo: str = "GET"):
        """Decorator para registrar rota"""
        r = Rota(path, metodo)
        self.rotas.append(r)
        return r
    
    def middleware(self, func: Callable):
        """Adiciona middleware"""
        self.middlewares.append(func)
        return func
    
    def template(self, nome: str, conteudo: str):
        """Adiciona template"""
        self.templates[nome] = conteudo
    
    def estatico(self, path: str, conteudo: bytes, tipo: str = "text/html"):
        """Adiciona arquivo estático"""
        self.estaticos[path] = conteudo
    
    def iniciar(self, porta: int = 8080):
        """Inicia o servidor"""
        self.porta = porta
        handler = self._criar_handler()
        self.server = HTTPServer(("0.0.0.0", porta), handler)
        print(f"[SiteBR] Servidor iniciado em http://localhost:{porta}")
        print(f"[SiteBR] Rotas registradas: {len(self.rotas)}")
        self.server.serve_forever()
    
    def iniciar_async(self, porta: int = 8080):
        """Inicia servidor em thread separada"""
        t = threading.Thread(target=self.iniciar, args=(porta,), daemon=True)
        t.start()
        return t
    
    def parar(self):
        """Para o servidor"""
        if self.server:
            self.server.shutdown()
            print("[SiteBR] Servidor parado")
    
    def _criar_handler(self):
        site = self
        
        class HandlerBR(BaseHTTPRequestHandler):
            def do_GET(self):
                site._processar_requisicao(self, "GET")
            
            def do_POST(self):
                site._processar_requisicao(self, "POST")
            
            def do_PUT(self):
                site._processar_requisicao(self, "PUT")
            
            def do_DELETE(self):
                site._processar_requisicao(self, "DELETE")
            
            def log_message(self, format, *args):
                print(f"[SiteBR] {self.client_address[0]} - {format % args}")
        
        return HandlerBR
    
    def _processar_requisicao(self, handler, metodo):
        path = urlparse(handler.path).path
        query = urlparse(handler.path).query
        params = parse_qs(query)
        
        # Arquivos estáticos
        if path in self.estaticos:
            handler.send_response(200)
            handler.send_header("Content-type", "text/html")
            handler.end_headers()
            handler.wfile.write(self.estaticos[path])
            return
        
        # Busca rota
        for rota in self.rotas:
            if rota.path == path and rota.metodo == metodo:
                # Executa middlewares
                for mw in self.middlewares:
                    resultado = mw({"path": path, "metodo": metodo, "params": params})
                    if resultado:
                        handler.send_response(200)
                        handler.send_header("Content-type", "application/json")
                        handler.end_headers()
                        handler.wfile.write(json.dumps(resultado).encode())
                        return
                
                # Lê corpo da requisição
                corpo = None
                if metodo in ("POST", "PUT"):
                    length = int(handler.headers.get('Content-Length', 0))
                    if length > 0:
                        corpo = handler.rfile.read(length).decode('utf-8')
                
                # Executa função da rota
                try:
                    resultado = rota.funcao(params, corpo) if rota.funcao else {"mensagem": "Rota sem função"}
                    
                    # Renderiza template se houver
                    if rota.template and rota.template in self.templates:
                        html = self.templates[rota.template]
                        if isinstance(resultado, dict):
                            for k, v in resultado.items():
                                html = html.replace("{{" + k + "}}", str(v))
                        handler.send_response(200)
                        handler.send_header("Content-type", "text/html; charset=utf-8")
                        handler.end_headers()
                        handler.wfile.write(html.encode('utf-8'))
                    else:
                        handler.send_response(200)
                        handler.send_header("Content-type", "application/json; charset=utf-8")
                        handler.end_headers()
                        handler.wfile.write(json.dumps(resultado, ensure_ascii=False).encode('utf-8'))
                except Exception as e:
                    handler.send_response(500)
                    handler.send_header("Content-type", "application/json")
                    handler.end_headers()
                    handler.wfile.write(json.dumps({"erro": str(e)}).encode())
                return
        
        # 404
        handler.send_response(404)
        handler.send_header("Content-type", "application/json")
        handler.end_headers()
        handler.wfile.write(json.dumps({"erro": "Rota não encontrada", "path": path}).encode())

# ============ Exemplo de uso ============
def exemplo_site():
    """Cria um site de exemplo"""
    site = SiteBR("Meu Site")
    
    @site.rota("/", "GET")
    def home(params, corpo):
        return {"mensagem": "Bem-vindo ao Site BR!", "versao": "1.0"}
    
    @site.rota("/api/usuarios", "GET")
    def listar_usuarios(params, corpo):
        return {"usuarios": [
            {"id": 1, "nome": "João", "email": "joao@email.com"},
            {"id": 2, "nome": "Maria", "email": "maria@email.com"}
        ]}
    
    @site.rota("/api/usuarios", "POST")
    def criar_usuario(params, corpo):
        dados = json.loads(corpo) if corpo else {}
        return {"criado": True, "usuario": dados}
    
    @site.rota("/sobre", "GET")
    def sobre(params, corpo):
        return {"nome": "Framework Web BR", "versao": "1.0.0", "autor": "Linguagem BR"}
    
    # Adiciona template
    site.template("index", """
    <html>
    <head><title>{{titulo}}</title></head>
    <body>
        <h1>{{mensagem}}</h1>
        <p>Bem-vindo ao {{nome}}</p>
    </body>
    </html>
    """)
    
    return site

if __name__ == "__main__":
    site = exemplo_site()
    print("[Framework Web] Iniciando servidor de exemplo...")
    print("[Framework Web] Acesse http://localhost:8080")
    site.iniciar()