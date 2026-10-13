# -*- coding: utf-8 -*-
"""
Debugger Interativo para Linguagem BR
Suporta breakpoints, step, watch, call stack
"""
import sys
import os
from typing import List, Dict, Any, Optional
from lexer import LexerBR, TipoToken
from br_ast import *

class Breakpoint:
    def __init__(self, arquivo: str, linha: int, condicao: Optional[str] = None):
        self.arquivo = arquivo
        self.linha = linha
        self.condicao = condicao
        self.ativo = True
    
    def __repr__(self):
        return f"Breakpoint({self.arquivo}:{self.linha})"

class DebuggerBR:
    def __init__(self):
        self.breakpoints: List[Breakpoint] = []
        self.pilha_chamadas: List[str] = []
        self.variaveis: Dict[str, Any] = {}
        self.pausado = False
        self.linha_atual = 0
        self.arquivo_atual = ""
        self.modo_passo = False
        self.historico_comandos: List[str] = []
    
    def adicionar_breakpoint(self, arquivo: str, linha: int, condicao: Optional[str] = None):
        bp = Breakpoint(arquivo, linha, condicao)
        self.breakpoints.append(bp)
        print(f"[Debugger] Breakpoint adicionado: {arquivo}:{linha}")
        return bp
    
    def remover_breakpoint(self, arquivo: str, linha: int):
        self.breakpoints = [bp for bp in self.breakpoints 
                           if not (bp.arquivo == arquivo and bp.linha == linha)]
        print(f"[Debugger] Breakpoint removido: {arquivo}:{linha}")
    
    def listar_breakpoints(self):
        print("[Debugger] Breakpoints:")
        for bp in self.breakpoints:
            status = "✅" if bp.ativo else "⏸️"
            cond = f" se {bp.condicao}" if bp.condicao else ""
            print(f"  {status} {bp.arquivo}:{bp.linha}{cond}")
    
    def verificar_breakpoint(self, arquivo: str, linha: int) -> Optional[Breakpoint]:
        for bp in self.breakpoints:
            if bp.arquivo == arquivo and bp.linha == linha and bp.ativo:
                if bp.condicao:
                    # Avalia condição (simplificado)
                    try:
                        if eval(bp.condicao, {"__builtins__": {}}, self.variaveis):
                            return bp
                        return None
                    except:
                        return bp
                return bp
        return None
    
    def pausar(self, arquivo: str, linha: int, motivo: str = "breakpoint"):
        self.pausado = True
        self.arquivo_atual = arquivo
        self.linha_atual = linha
        print(f"\n[Debugger] ⏸️ Pausado em {arquivo}:{linha} ({motivo})")
        self._mostrar_contexto()
        self._loop_comandos()
    
    def _mostrar_contexto(self):
        """Mostra contexto atual"""
        print(f"\n📁 Arquivo: {self.arquivo_atual}")
        print(f"📍 Linha: {self.linha_atual}")
        print(f"📚 Pilha: {' > '.join(self.pilha_chamadas[-5:]) if self.pilha_chamadas else '(principal)'}")
        if self.variaveis:
            print(f"📊 Variáveis:")
            for nome, valor in list(self.variaveis.items())[:10]:
                print(f"    {nome} = {valor}")
    
    def _loop_comandos(self):
        """Loop interativo de comandos do debugger"""
        while self.pausado:
            try:
                cmd = input("(br-dbg) ").strip()
                self.historico_comandos.append(cmd)
                
                if cmd == "continuar" or cmd == "c":
                    self.pausado = False
                    self.modo_passo = False
                elif cmd == "passo" or cmd == "p":
                    self.pausado = False
                    self.modo_passo = True
                elif cmd == "entrar" or cmd == "e":
                    self.pausado = False
                    self.modo_passo = True
                    # TODO: entrar em função
                elif cmd == "sair" or cmd == "s":
                    self.pausado = False
                    self.modo_passo = False
                    # TODO: sair da função atual
                elif cmd == "variaveis" or cmd == "v":
                    self._mostrar_contexto()
                elif cmd == "pilha" or cmd == "stack":
                    print("📚 Pilha de chamadas:")
                    for i, frame in enumerate(self.pilha_chamadas):
                        print(f"  #{i} {frame}")
                elif cmd.startswith("inspecionar ") or cmd.startswith("i "):
                    nome = cmd.split(" ", 1)[1]
                    if nome in self.variaveis:
                        print(f"  {nome} = {self.variaveis[nome]}")
                    else:
                        print(f"  Variável '{nome}' não encontrada")
                elif cmd.startswith("break ") or cmd.startswith("b "):
                    args = cmd.split(" ", 1)[1]
                    if ":" in args:
                        arq, linha = args.split(":")
                        self.adicionar_breakpoint(arq, int(linha))
                    else:
                        self.adicionar_breakpoint(self.arquivo_atual, int(args))
                elif cmd == "breakpoints" or cmd == "bp":
                    self.listar_breakpoints()
                elif cmd.startswith("remover ") or cmd.startswith("r "):
                    args = cmd.split(" ", 1)[1]
                    if ":" in args:
                        arq, linha = args.split(":")
                        self.remover_breakpoint(arq, int(linha))
                    else:
                        self.remover_breakpoint(self.arquivo_atual, int(args))
                elif cmd == "ajuda" or cmd == "h" or cmd == "help":
                    self._mostrar_ajuda()
                elif cmd == "sair" or cmd == "quit" or cmd == "q":
                    print("[Debugger] Encerrando debugger")
                    sys.exit(0)
                else:
                    print("Comando desconhecido. Digite 'ajuda' para ajuda.")
                    
            except KeyboardInterrupt:
                print("\n[Debugger] Interrompido")
                self.pausado = False
            except EOFError:
                self.pausado = False
    
    def _mostrar_ajuda(self):
        print("""
Comandos do Debugger BR:
  continuar (c)    - Continuar execução
  passo (p)        - Executar próxima linha
  entrar (e)       - Entrar em função
  sair (s)         - Sair da função atual
  variaveis (v)    - Mostrar variáveis
  pilha (stack)    - Mostrar pilha de chamadas
  inspecionar (i)  - Inspecionar variável
  break (b)        - Adicionar breakpoint (arquivo:linha)
  breakpoints (bp) - Listar breakpoints
  remover (r)      - Remover breakpoint
  ajuda (h)        - Mostrar esta ajuda
  sair (q)         - Sair do debugger
        """)
    
    def executar_com_breakpoints(self, arquivo: str, codigo: str):
        """Executa código com breakpoints"""
        print(f"[Debugger] Iniciando depuração de {arquivo}")
        print(f"[Debugger] {len(self.breakpoints)} breakpoints ativos")
        
        linhas = codigo.split('\n')
        self.arquivo_atual = arquivo
        
        for i, linha in enumerate(linhas, 1):
            self.linha_atual = i
            
            # Verifica breakpoint
            bp = self.verificar_breakpoint(arquivo, i)
            if bp:
                self.pausar(arquivo, i, f"breakpoint {bp}")
            
            # Modo passo
            if self.modo_passo:
                self.pausar(arquivo, i, "passo")
        
        print("[Debugger] Execução concluída")

def cli_debugger():
    """CLI do debugger"""
    import sys
    args = sys.argv[2:]  # skip "br" command
    
    debugger = DebuggerBR()
    
    if not args:
        print("Uso: br debug <arquivo.br> [opções]")
        print("\nOpções:")
        print("  -b, --break <linha>     Adicionar breakpoint")
        print("  -c, --condition <cond>  Condição do breakpoint")
        print("\nExemplos:")
        print("  br debug programa.br")
        print("  br debug programa.br -b 10")
        print("  br debug programa.br -b 15 -c \"x > 10\"")
        return
    
    arquivo = args[0]
    if not os.path.exists(arquivo):
        print(f"Erro: Arquivo '{arquivo}' não encontrado")
        return
    
    # Processa argumentos
    i = 1
    while i < len(args):
        if args[i] in ('-b', '--break') and i + 1 < len(args):
            debugger.adicionar_breakpoint(arquivo, int(args[i + 1]))
            i += 2
        elif args[i] in ('-c', '--condition') and i + 1 < len(args):
            if debugger.breakpoints:
                debugger.breakpoints[-1].condicao = args[i + 1]
            i += 2
        else:
            i += 1
    
    with open(arquivo, 'r', encoding='utf-8') as f:
        codigo = f.read()
    
    debugger.executar_com_breakpoints(arquivo, codigo)

if __name__ == "__main__":
    cli_debugger()