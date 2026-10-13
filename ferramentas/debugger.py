# -*- coding: utf-8 -*-
"""
Depurador (Debugger) para Linguagem BR
Permite depurar código BR com breakpoints, stepping e inspeção de estado
"""
import sys
import os
from typing import Dict, List, Optional, Set, Tuple, Any
from dataclasses import dataclass, field
from enum import Enum
from maquina_virtual import MaquinaVirtualBR

class ComandoDepurador(Enum):
    CONTINUAR = "continuar"
    PASSO = "passo"
    ENTRAR = "entrar"
    SAIR = "sair"
    PONTO_PARADA = "ponto_parada"
    LISTAR = "listar"
    PILHA = "pilha"
    VARIAVEIS = "variaveis"
    INSPECIONAR = "inspecionar"
    AJUDA = "ajuda"
    SAIR = "sair"

@dataclass
class Breakpoint:
    arquivo: str
    linha: int
    condicao: Optional[str] = None
    ativo: bool = True
    hit_count: int = 0

@dataclass
class FramePilha:
    nome_funcao: str
    arquivo: str
    linha: int
    variaveis: Dict[str, Any]

class DebuggerBR:
    """Depurador interativo para código BR"""
    
    def __init__(self, bytecode: Dict[str, Any], arquivo_origem: str = "desconhecido"):
        self.bytecode = bytecode
        self.arquivo_origem = arquivo_origem
        self.vm = MaquinaVirtualBR(bytecode)
        
        self.breakpoints: Dict[Tuple[str, int], Breakpoint] = {}
        self.pilha_frames: List[FramePilha] = []
        self.pausado: bool = False
        self.linha_atual: int = 0
        self.modo_step: Optional[str] = None  # "passo", "entrar", "sair"
        self.profundidade_step: int = 0
        
        self.codigo_fonte: List[str] = []
        if os.path.exists(arquivo_origem):
            with open(arquivo_origem, "r", encoding="utf-8") as f:
                self.codigo_fonte = f.read().splitlines()
    
    def adicionar_breakpoint(self, arquivo: str, linha: int, condicao: Optional[str] = None):
        """Adiciona um breakpoint"""
        chave = (arquivo, linha)
        self.breakpoints[chave] = Breakpoint(arquivo, linha, condicao)
        print(f"Breakpoint adicionado em {arquivo}:{linha}")
    
    def remover_breakpoint(self, arquivo: str, linha: int):
        """Remove um breakpoint"""
        chave = (arquivo, linha)
        if chave in self.breakpoints:
            del self.breakpoints[chave]
            print(f"Breakpoint removido de {arquivo}:{linha}")
        else:
            print(f"Breakpoint não encontrado em {arquivo}:{linha}")
    
    def listar_breakpoints(self):
        """Lista todos os breakpoints ativos"""
        if not self.breakpoints:
            print("Nenhum breakpoint definido.")
            return
        
        print("Breakpoints ativos:")
        for bp in self.breakpoints.values():
            status = "ativo" if bp.ativo else "inativo"
            cond_str = f" (condição: {bp.condicao})" if bp.condicao else ""
            print(f"  {bp.arquivo}:{bp.linha} [{status}] - hits: {bp.hit_count}{cond_str}")
    
    def verificar_breakpoint(self, arquivo: str, linha: int) -> bool:
        """Verifica se há breakpoint na linha atual"""
        chave = (arquivo, linha)
        if chave in self.breakpoints:
            bp = self.breakpoints[chave]
            if bp.ativo:
                bp.hit_count += 1
                return True
        return False
    
    def capturar_estado(self):
        """Captura o estado atual da VM para inspeção"""
        frame = FramePilha(
            nome_funcao="<global>",
            arquivo=self.arquivo_origem,
            linha=self.linha_atual,
            variaveis=self.vm.escopos[-1].copy() if self.vm.escopos else {}
        )
        self.pilha_frames.append(frame)
    
    def exibir_contexto(self, linha: int):
        """Exibe o contexto do código ao redor da linha atual"""
        inicio = max(0, linha - 3)
        fim = min(len(self.codigo_fonte), linha + 2)
        
        print(f"\nContexto em {self.arquivo_origem}:{linha}:")
        for i in range(inicio, fim):
            prefixo = ">>> " if i == linha - 1 else "    "
            linha_codigo = self.codigo_fonte[i] if i < len(self.codigo_fonte) else ""
            print(f"{prefixo}{i + 1:4d} | {linha_codigo}")
    
    def exibir_pilha(self):
        """Exibe a pilha de chamadas"""
        if not self.pilha_frames:
            print("Pilha de chamadas vazia.")
            return
        
        print("\nPilha de Chamadas:")
        for i, frame in enumerate(reversed(self.pilha_frames)):
            print(f"  [{i}] {frame.nome_funcao} em {frame.arquivo}:{frame.linha}")
    
    def exibir_variaveis(self, escopo_idx: int = -1):
        """Exibe variáveis do escopo atual"""
        if not self.vm.escopos:
            print("Nenhum escopo ativo.")
            return
        
        escopo = self.vm.escopos[escopo_idx]
        if not escopo:
            print("Escopo vazio.")
            return
        
        print(f"\nVariáveis no escopo {escopo_idx}:")
        for nome, valor in escopo.items():
            tipo = type(valor).__name__
            if isinstance(valor, str):
                valor_repr = f'"{valor}"'
            elif isinstance(valor, list):
                valor_repr = f"<lista de {len(valor)} itens>"
            elif isinstance(valor, dict):
                valor_repr = f"<registro com {len(valor)} propriedades>"
            else:
                valor_repr = str(valor)
            print(f"  {nome}: {tipo} = {valor_repr}")
    
    def inspecionar_variavel(self, nome: str):
        """Inspeciona uma variável em detalhes"""
        for escopo in reversed(self.vm.escopos):
            if nome in escopo:
                valor = escopo[nome]
                print(f"\nInspecionando: {nome}")
                print(f"  Tipo: {type(valor).__name__}")
                print(f"  Valor: {repr(valor)}")
                
                if isinstance(valor, list):
                    print(f"  Tamanho: {len(valor)}")
                    if len(valor) <= 10:
                        for i, item in enumerate(valor):
                            print(f"    [{i}]: {repr(item)}")
                    else:
                        for i in range(5):
                            print(f"    [{i}]: {repr(valor[i])}")
                        print(f"    ... ({len(valor) - 10} itens restantes)")
                
                elif isinstance(valor, dict):
                    print(f"  Propriedades: {len(valor)}")
                    for chave, val in list(valor.items())[:10]:
                        print(f"    {chave}: {repr(val)}")
                
                return
        
        print(f"Variável '{nome}' não encontrada.")
    
    def executar_comando(self, comando: str) -> bool:
        """Executa um comando do depurador"""
        partes = comando.strip().split()
        if not partes:
            return True
        
        cmd = partes[0].lower()
        
        if cmd == "c" or cmd == "continuar":
            self.pausado = False
            self.modo_step = None
            return True
        
        elif cmd == "s" or cmd == "passo":
            self.modo_step = "passo"
            self.pausado = False
            return True
        
        elif cmd == "i" or cmd == "entrar":
            self.modo_step = "entrar"
            self.profundidade_step = len(self.vm.escopos)
            self.pausado = False
            return True
        
        elif cmd == "o" or cmd == "sair":
            self.modo_step = "sair"
            self.profundidade_step = len(self.vm.escopos)
            self.pausado = False
            return True
        
        elif cmd == "b" or cmd == "ponto_parada":
            if len(partes) >= 2:
                try:
                    linha = int(partes[1])
                    self.adicionar_breakpoint(self.arquivo_origem, linha)
                except ValueError:
                    print("Número de linha inválido.")
            else:
                self.listar_breakpoints()
            return True
        
        elif cmd == "l" or cmd == "listar":
            if len(partes) >= 2:
                try:
                    linha = int(partes[1])
                    self.exibir_contexto(linha)
                except ValueError:
                    print("Número de linha inválido.")
            else:
                self.exibir_contexto(self.linha_atual)
            return True
        
        elif cmd == "p" or cmd == "pilha":
            self.exibir_pilha()
            return True
        
        elif cmd == "v" or cmd == "variaveis":
            self.exibir_variaveis()
            return True
        
        elif cmd == "inspecionar":
            if len(partes) >= 2:
                self.inspecionar_variavel(partes[1])
            else:
                print("Uso: inspecionar <variavel>")
            return True
        
        elif cmd == "h" or cmd == "ajuda":
            self.ajuda()
            return True
        
        elif cmd == "q" or cmd == "sair":
            return False
        
        else:
            print(f"Comando desconhecido: {cmd}")
            self.ajuda()
            return True
    
    def ajuda(self):
        """Exibe ajuda de comandos"""
        print("""
Comandos do Depurador:
  c, continuar       Continua a execução até o próximo breakpoint
  s, passo          Executa a próxima linha (step over)
  i, entrar         Entra na função chamada (step into)
  o, sair           Sai da função atual (step out)
  b, ponto_parada [linha]  Adiciona breakpoint na linha
  l, listar [linha]        Lista código ao redor da linha
  p, pilha          Exibe pilha de chamadas
  v, variaveis      Exibe variáveis do escopo atual
  inspecionar <var> Inspeciona variável em detalhes
  h, ajuda          Exibe esta mensagem de ajuda
  q, sair           Sai do depurador
""")
    
    def loop_interativo(self):
        """Loop interativo do depurador"""
        print(f"\nDepurador BR - {self.arquivo_origem}")
        print("Digite 'ajuda' para comandos ou 'sair' para sair.\n")
        
        while True:
            try:
                comando = input("[depurar] ").strip()
                if not comando:
                    continue
                
                continuar = self.executar_comando(comando)
                if not continuar:
                    print("Saindo do depurador.")
                    break
                
                if not self.pausado:
                    break  # Retomar execução
            
            except KeyboardInterrupt:
                print("\nUse 'sair' para encerrar o depurador.")
            except EOFError:
                print("\nSaindo do depurador.")
                break
    
    def iniciar(self, breakpoint_inicial: Optional[int] = None):
        """Inicia a sessão de depuração"""
        if breakpoint_inicial:
            self.adicionar_breakpoint(self.arquivo_origem, breakpoint_inicial)
        
        print(f"Iniciando depuração de {self.arquivo_origem}")
        
        try:
            # Ativar debugger na VM
            self.vm.debugger_ativo = True
            self.vm.debugger_callback = self._callback_debugger
            
            # Executar com interceptação
            self.executar_com_depuracao()
        except Exception as e:
            print(f"\nErro durante execução: {e}")
            self.exibir_contexto(self.linha_atual)
            self.loop_interativo()
    
    def _callback_debugger(self, evento: str, linha: int):
        """Callback do debugger da VM"""
        if evento == "pausa":
            print(f"\nBreakpoint alcançado na linha {linha}")
            self.loop_interativo()
        elif evento == "aguardando":
            pass  # Aguardando comando do usuário
    
    def executar_com_depuracao(self):
        """Executa o bytecode com depuração ativa"""
        # Esta é uma implementação simplificada
        # Em produção, integraria com a VM para pausar em pontos específicos
        self.vm.executar()

def iniciar_depurador(caminho_bytecode: str, caminho_fonte: str, breakpoint: Optional[int] = None):
    """Função auxiliar para iniciar o depurador"""
    from maquina_virtual import ByteStream, deserializar
    
    with open(caminho_bytecode, "rb") as f:
        bytecode = deserializar(ByteStream(f.read()))
    
    vm = MaquinaVirtualBR(bytecode)
    debugger = DebuggerBR(vm, caminho_fonte)
    debugger.iniciar(breakpoint)
    
    if not os.path.exists(caminho_bytecode):
        print(f"Arquivo bytecode não encontrado: {caminho_bytecode}")
        return
    
    with open(caminho_bytecode, "rb") as f:
        header = f.read(4)
        if header != b"BRC1":
            print("Arquivo não está no formato BRC1")
            return
        data = f.read()
    
    stream = ByteStream(data)
    bytecode = deserializar(stream)
    
    debugger = DebuggerBR(bytecode, caminho_fonte)
    debugger.iniciar(breakpoint)
