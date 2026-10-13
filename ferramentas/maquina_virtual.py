# -*- coding: utf-8 -*-
import sys
import os
import json
import re
import struct
import threading
import gc
from pathlib import Path
from encoding_utils import configurar_stdio, ler_texto, escrever_texto
from modulos import (
    ErroModuloBR,
    chamar_funcao_modulo,
    criar_handle_funcao,
    criar_handle_modulo,
    modulo_nativo,
    resolver_arquivo_modulo,
)
from agentes import mesclar_estado, salvar_agente
from sandbox import Permissao, SandboxBR
from ia_service import get_ia_service

configurar_stdio()

OPCODE_MAP = {
    "EMPILHAR_CONST": 1,
    "DEFINIR_VAR": 2,
    "OBTER_VAR": 3,
    "OBTER_PROPRIEDADE": 4,
    "SOMAR": 5,
    "SUBTRAIR": 6,
    "MULTIPLICAR": 7,
    "DIVIDIR": 8,
    "IGUAL_A": 9,
    "MAIOR_QUE": 10,
    "RETORNAR": 11,
    "ESPERE_IGUAL": 12,
    "CHAMAR_METODO": 13,
    "CHAMAR_SERVICO": 14,
    "CRIAR_LISTA_BLOCO": 15,
    "LISTA_ELEMENTO": 16,
    "CRIAR_REGISTRO_BLOCO": 17,
    "REGISTRO_PROPRIEDADE": 18,
    "IA_PERGUNTE_BLOCO": 19,
    "IA_AGENTE_BLOCO": 20,
    "TENTE_CAPTURE_BLOCO": 21,
    "PARALELO_EXECUTE": 22,
    "CONDICIONAL_SE": 23,
    "ENQUANTO_BLOCO": 24,
    "MENOR_QUE": 25,
    "IMPORTAR_MODULO": 26,
    "IMPORTAR_SIMBOLOS": 27,
    "CHAMAR_METODO_OBJETO": 28,
    "DECLARAR_CLASSE": 29,
    "REPITA_VEZES": 30,
    "ESCOLHA_CASO": 31,
    "DESCARTAR_TOPO": 32,
    "NAO_IGUAL": 33,
    "MAIOR_OU_IGUAL": 34,
    "MENOR_OU_IGUAL": 35,
    "OBTER_INDICE": 36,
    "ATRIBUIR_VAR": 37,
    "DEFINIR_PROPRIEDADE": 38,
    "DEFINIR_INDICE": 39,
    "CONTE_BLOCO": 40,
    "AGUARDAR_TAREFA": 41,
    "PARA_CADA_BLOCO": 42,
    "QUEBRAR_LOOP": 43,
}
REVERSE_OPCODE_MAP = {v: k for k, v in OPCODE_MAP.items()}

class BreakException(Exception):
    pass

def serializar(obj):
    if isinstance(obj, bool):
        return struct.pack('>BB', 4, 1 if obj else 0)
    elif obj is None:
        return struct.pack('>B', 5)
    elif isinstance(obj, int):
        return struct.pack('>Bi', 1, obj)
    elif isinstance(obj, float):
        return struct.pack('>Bd', 2, obj)
    elif isinstance(obj, str):
        utf = obj.encode('utf-8')
        return struct.pack('>BI', 3, len(utf)) + utf
    elif isinstance(obj, list):
        if len(obj) > 0 and isinstance(obj[0], str) and obj[0] in OPCODE_MAP:
            op_id = OPCODE_MAP[obj[0]]
            args = obj[1:]
            res = struct.pack('>BBB', 8, op_id, len(args))
            for arg in args:
                res += serializar(arg)
            return res
        else:
            res = struct.pack('>BI', 6, len(obj))
            for item in obj:
                res += serializar(item)
            return res
    elif isinstance(obj, dict):
        res = struct.pack('>BI', 7, len(obj))
        for k, v in obj.items():
            res += serializar(k)
            res += serializar(v)
        return res
    raise TypeError(f"Tipo não serializável: {type(obj)}")

class ByteStream:
    def __init__(self, data):
        self.data = data
        self.pos = 0
    def read(self, n):
        res = self.data[self.pos:self.pos+n]
        self.pos += n
        return res

def deserializar(stream):
    flag_bytes = stream.read(1)
    if not flag_bytes:
        return None
    flag = flag_bytes[0]
    
    if flag == 1:
        return struct.unpack('>i', stream.read(4))[0]
    elif flag == 2:
        return struct.unpack('>d', stream.read(8))[0]
    elif flag == 3:
        length = struct.unpack('>I', stream.read(4))[0]
        return stream.read(length).decode('utf-8')
    elif flag == 4:
        return stream.read(1)[0] == 1
    elif flag == 5:
        return None
    elif flag == 6:
        length = struct.unpack('>I', stream.read(4))[0]
        res = []
        for _ in range(length):
            res.append(deserializar(stream))
        return res
    elif flag == 7:
        length = struct.unpack('>I', stream.read(4))[0]
        res = {}
        for _ in range(length):
            k = deserializar(stream)
            v = deserializar(stream)
            res[k] = v
        return res
    elif flag == 8:
        op_id = stream.read(1)[0]
        arg_len = stream.read(1)[0]
        args = []
        for _ in range(arg_len):
            args.append(deserializar(stream))
        op_name = REVERSE_OPCODE_MAP[op_id]
        return [op_name] + args
    raise TypeError(f"Flag binário desconhecido: {flag}")

def verificar_permissao(permissao):
    caminho_proj = None
    for nome in ("projeto.brm", "projeto.br"):
        for path in (nome, os.path.join("..", nome), os.path.join("meu-projeto", nome)):
            if os.path.exists(path):
                caminho_proj = path
                break
        if caminho_proj:
            break
            
    if caminho_proj and os.path.exists(caminho_proj):
        with open(caminho_proj, "r", encoding="utf-8") as f:
            conteudo = f.read()
        match = re.search(r'permissoes\s+(.*?)\s+fim', conteudo, re.DOTALL)
        if match:
            perms = [p.strip() for p in match.group(1).splitlines() if p.strip()]
            if permissao in perms:
                return True
    return False

class ReturnException(Exception):
    def __init__(self, valor):
        super().__init__()
        self.valor = valor

class MaquinaVirtualBR:
    def __init__(self, bytecode, diretorio_base=None):
        self.constantes = bytecode.get("constantes", [])
        self.instrucoes = bytecode.get("instrucoes", [])
        self.funcoes = bytecode.get("funcoes", {})
        self.classes = bytecode.get("classes", {})
        self.testes = bytecode.get("testes", [])
        self.diretorio_base = str(diretorio_base or os.getcwd())
        self.modulos_carregados = {}
        self.sandbox = SandboxBR(self._resolver_manifesto_projeto())
        self._escopo_lock = threading.RLock()
        self._module_lock = threading.RLock()
        self.max_call_depth = int(os.environ.get("BR_MAX_CALL_DEPTH", "1000"))
        self.call_depth = 0
        self.instrucoes_executadas = 0
        
        self.pilha = []
        self.escopos = [{}]
        self.banco = []
        
        self.temp_lista = []
        self.temp_registro = {}
        self.temp_agente = {}
        
        # Suporte a debugger
        self.debugger_ativo = False
        self.breakpoints = set()
        self.pausado = False
        self.debugger_callback = None
        self.linha_atual = 0
        self.pilha_chamadas = []
        self.rastreamento_chamadas = []
        
        # Garbage Collector
        self.heap = {}
        self.heap_contador = 0
        self.gc_ativo = True
        self.gc_limiar = 1024
        self.gc_estatisticas = {"ciclos": 0, "objetos_liberados": 0}
        
        # Canais CSP para concorrência
        self.canais = {}
        self.canal_contador = 0
        
        # Scheduler de Green Threads
        self.scheduler_ativo = False
        self.tarefas_pendentes = []
        self.tarefas_executando = []

    def _resolver_manifesto_projeto(self):
        base = Path(self.diretorio_base).resolve()
        for pasta in (base, *base.parents):
            for nome in ("projeto.brm", "projeto.br"):
                candidato = pasta / nome
                if candidato.exists():
                    return str(candidato)
        return None

    def anexar_rastreamento(self, exc, stack=None):
        if isinstance(exc, ReturnException):
            return exc
        if not hasattr(exc, "_br_stack"):
            exc._br_stack = list(self.rastreamento_chamadas if stack is None else stack)
        if not hasattr(exc, "_br_instrucao") and hasattr(self, "ultima_instrucao"):
            exc._br_instrucao = self.ultima_instrucao
        return exc

    def formatar_erro(self, exc):
        mensagem = str(exc) or exc.__class__.__name__
        stack = getattr(exc, "_br_stack", None)
        if not stack:
            return mensagem

        linhas = [mensagem, "Rastreamento BR:"]
        for frame in reversed(stack):
            linhas.append(f"  em {frame}")
        return "\n".join(linhas)

    def _mtime_modulo(self, caminho):
        try:
            return Path(caminho).stat().st_mtime_ns
        except OSError:
            return None

    def obter_var(self, nome):
        with self._escopo_lock:
            for escopo in reversed(self.escopos):
                if nome in escopo:
                    return escopo[nome]
        return None

    def definir_var(self, nome, valor):
        with self._escopo_lock:
            self.escopos[-1][nome] = valor

    def atribuir_var(self, nome, valor):
        with self._escopo_lock:
            for escopo in reversed(self.escopos):
                if nome in escopo:
                    escopo[nome] = valor
                    return
            self.escopos[-1][nome] = valor

    def carregar_modulo(self, nome):
        nome_normalizado = nome.strip().strip('"').strip("'").replace("\\", "/")

        with self._module_lock:
            if modulo_nativo(nome_normalizado) is not None:
                entrada = self.modulos_carregados.get(nome_normalizado)
                if entrada:
                    return entrada["handle"]
                handle = criar_handle_modulo(nome_normalizado)
                self.modulos_carregados[nome_normalizado] = {
                    "handle": handle,
                    "bytecode": None,
                    "globals": {},
                    "status": "carregado",
                    "mtime": None,
                    "caminho": None,
                }
                return handle

            caminho = resolver_arquivo_modulo(nome_normalizado, self.diretorio_base)
            if caminho is None:
                raise ErroModuloBR(f"Modulo '{nome_normalizado}' nao encontrado")

            caminho_str = str(caminho)
            mtime = self._mtime_modulo(caminho_str)
            entrada = self.modulos_carregados.get(nome_normalizado)
            if entrada:
                if entrada.get("status") == "carregando":
                    return entrada["handle"]
                if entrada.get("caminho") == caminho_str and entrada.get("mtime") == mtime:
                    return entrada["handle"]

            handle = criar_handle_modulo(nome_normalizado, caminho_str)
            self.modulos_carregados[nome_normalizado] = {
                "handle": handle,
                "bytecode": None,
                "globals": {},
                "status": "carregando",
                "mtime": mtime,
                "caminho": caminho_str,
            }

        try:
            bytecode = self.compilar_modulo_br(caminho_str)
            init_vm = MaquinaVirtualBR(bytecode, diretorio_base=str(Path(caminho_str).parent))
            init_vm.modulos_carregados = self.modulos_carregados
            init_vm._module_lock = self._module_lock
            init_vm.executar_instrucoes(bytecode.get("instrucoes", []))
            globals_modulo = init_vm.escopos[-1].copy()

            with self._module_lock:
                self.modulos_carregados[nome_normalizado].update({
                    "bytecode": bytecode,
                    "globals": globals_modulo,
                    "status": "carregado",
                    "mtime": mtime,
                })
            return handle
        except Exception:
            with self._module_lock:
                entrada = self.modulos_carregados.get(nome_normalizado)
                if entrada and entrada.get("status") == "carregando":
                    del self.modulos_carregados[nome_normalizado]
            raise

    def compilar_modulo_br(self, caminho):
        from lexer import LexerBR
        from parser import ParserBR
        from codegen import gerar_bytecode

        if str(caminho).lower().endswith(".brc"):
            with open(caminho, "rb") as f:
                conteudo = f.read()
            if conteudo[:4] == b"BRC1":
                conteudo = conteudo[4:]
            return deserializar(ByteStream(conteudo))

        codigo = ler_texto(caminho)
        tokens = LexerBR(codigo).tokenizar()
        parser = ParserBR(tokens)
        ast = parser.parse()
        if parser.erros:
            raise ErroModuloBR(f"Modulo '{caminho}' contem erro de sintaxe")
        return gerar_bytecode(ast)

    def eh_funcao_modulo(self, valor):
        return isinstance(valor, dict) and valor.get("__tipo_br__") == "funcao_modulo"

    def eh_modulo(self, valor):
        return isinstance(valor, dict) and valor.get("__tipo_br__") == "modulo"

    def chamar_handle_funcao(self, handle, args):
        modulo = handle["modulo"]
        simbolo = handle["simbolo"]
        if modulo_nativo(modulo) is not None:
            self.validar_chamada_modulo(modulo, simbolo, args)
            return chamar_funcao_modulo(modulo, simbolo, args)

        self.carregar_modulo(modulo)
        entrada = self.modulos_carregados[modulo]
        bytecode = entrada["bytecode"]
        if simbolo not in bytecode.get("funcoes", {}):
            raise ErroModuloBR(f"Modulo '{modulo}' nao exporta funcao '{simbolo}'")
        return self.executar_funcao_bytecode(bytecode, simbolo, args, entrada.get("globals", {}), modulo)

    def executar_funcao_bytecode(self, bytecode, nome_funcao, args, globals_modulo=None, nome_modulo=None):
        sub_vm = MaquinaVirtualBR(bytecode, diretorio_base=self.diretorio_base)
        sub_vm.modulos_carregados = self.modulos_carregados
        sub_vm._module_lock = self._module_lock
        sub_vm.escopos = [dict(globals_modulo or {})]
        frame = f"{nome_modulo}.{nome_funcao}" if nome_modulo else nome_funcao
        sub_vm.rastreamento_chamadas = list(self.rastreamento_chamadas) + [frame]
        for arg in args:
            sub_vm.pilha.append(arg)
        retorno = None
        try:
            sub_vm.executar_instrucoes(bytecode["funcoes"][nome_funcao])
        except ReturnException as rx:
            retorno = rx.valor
        except Exception as exc:
            self.anexar_rastreamento(exc, sub_vm.rastreamento_chamadas)
            raise
        return retorno

    def chamar_metodo_objeto(self, objeto, metodo_nome, args):
        if self.eh_modulo(objeto):
            modulo = objeto["nome"]
            if modulo_nativo(modulo) is not None:
                self.validar_chamada_modulo(modulo, metodo_nome, args)
                return chamar_funcao_modulo(modulo, metodo_nome, args)
            handle = criar_handle_funcao(modulo, metodo_nome)
            return self.chamar_handle_funcao(handle, args)

        if isinstance(objeto, dict) and metodo_nome == "pergunte":
            return self.chamar_servico_nativo("IA", "PERGUNTAR_AGENTE", [objeto] + args)

        if isinstance(objeto, dict) and "__classe_br__" in objeto:
            classe = self.resolver_classe_completa(objeto["__classe_br__"])
            if metodo_nome in classe.get("metodos", {}):
                return self.executar_metodo_classe(objeto, metodo_nome, args)

        if isinstance(objeto, dict) and metodo_nome in objeto:
            valor = objeto[metodo_nome]
            if self.eh_funcao_modulo(valor):
                return self.chamar_handle_funcao(valor, args)
            return valor

        if isinstance(objeto, list):
            if metodo_nome in ("tamanho", "comprimento"):
                return len(objeto)
            if metodo_nome in ("adicionar", "adicione"):
                if len(args) != 1:
                    raise TypeError(f"Lista.{metodo_nome} esperava 1 argumento(s), mas recebeu {len(args)}.")
                objeto.append(args[0])
                return objeto
            if metodo_nome == "obter":
                if len(args) != 1:
                    raise TypeError(f"Lista.obter esperava 1 argumento(s), mas recebeu {len(args)}.")
                return objeto[int(args[0])]
            if metodo_nome == "primeiro":
                return objeto[0] if objeto else None
            if metodo_nome == "ultimo":
                return objeto[-1] if objeto else None

        if isinstance(objeto, str):
            if metodo_nome in ("tamanho", "comprimento"):
                return len(objeto)
            if metodo_nome == "contem":
                if len(args) != 1:
                    raise TypeError(f"Texto.contem esperava 1 argumento(s), mas recebeu {len(args)}.")
                return str(args[0]) in objeto
            if metodo_nome == "substituir":
                if len(args) != 2:
                    raise TypeError(f"Texto.substituir esperava 2 argumento(s), mas recebeu {len(args)}.")
                return objeto.replace(str(args[0]), str(args[1]))
            if metodo_nome == "dividir":
                separador = str(args[0]) if args else " "
                return objeto.split(separador)

        if isinstance(objeto, dict):
            if metodo_nome == "obter":
                if len(args) != 1:
                    raise TypeError(f"Registro.obter esperava 1 argumento(s), mas recebeu {len(args)}.")
                return objeto.get(args[0])
            if metodo_nome == "definir":
                if len(args) != 2:
                    raise TypeError(f"Registro.definir esperava 2 argumento(s), mas recebeu {len(args)}.")
                objeto[args[0]] = args[1]
                return objeto
            if metodo_nome in ("tamanho", "comprimento"):
                return len(objeto)
            if metodo_nome == "chaves":
                return list(objeto.keys())
            if metodo_nome == "valores":
                return list(objeto.values())

        raise AttributeError(f"Objeto nao possui metodo '{metodo_nome}'")

    def instanciar_classe(self, nome_classe, args):
        classe = self.resolver_classe_completa(nome_classe)
        campos = classe.get("campos", [])
        instancia = {"__classe_br__": nome_classe}
        for campo in campos:
            instancia[campo] = None
        metodos = classe.get("metodos", {})
        if "iniciar" in metodos:
            self.executar_metodo_classe(instancia, "iniciar", args)
        else:
            if len(args) > len(campos):
                raise TypeError(
                    f"Classe '{nome_classe}' esperava no máximo {len(campos)} argumento(s), mas recebeu {len(args)}."
                )
            for idx, campo in enumerate(campos):
                instancia[campo] = args[idx] if idx < len(args) else None
        return instancia

    def resolver_classe_completa(self, nome_classe):
        classe = self.classes.get(nome_classe, {})
        classe_mae = classe.get("classe_mae")
        campos = []
        metodos = {}
        if classe_mae:
            base = self.resolver_classe_completa(classe_mae)
            campos.extend(base.get("campos", []))
            metodos.update(base.get("metodos", {}))
        for campo in classe.get("campos", []):
            if campo not in campos:
                campos.append(campo)
        metodos.update(classe.get("metodos", {}))
        return {"campos": campos, "metodos": metodos, "classe_mae": classe_mae}

    def executar_metodo_classe(self, objeto, metodo_nome, args):
        nome_classe = objeto.get("__classe_br__")
        classe = self.resolver_classe_completa(nome_classe)
        metodo = classe.get("metodos", {}).get(metodo_nome)
        if metodo is None:
            raise AttributeError(f"Classe '{nome_classe}' nao possui metodo '{metodo_nome}'")

        instrucoes = metodo.get("instrucoes", metodo)
        frame = f"{nome_classe}.{metodo_nome}"
        retorno = None
        escopo_aberto = False
        self.rastreamento_chamadas.append(frame)
        try:
            self.validar_aridade_funcao(f"Metodo {nome_classe}", metodo_nome, instrucoes, args)
            self.escopos.append({"isto": objeto})
            escopo_aberto = True
            for arg in args:
                self.pilha.append(arg)
            self.executar_instrucoes(instrucoes)
        except ReturnException as rx:
            retorno = rx.valor
        except Exception as exc:
            self.anexar_rastreamento(exc)
            raise
        finally:
            if escopo_aberto:
                self.escopos.pop()
            if self.rastreamento_chamadas and self.rastreamento_chamadas[-1] == frame:
                self.rastreamento_chamadas.pop()
        return retorno

    def validar_chamada_modulo(self, modulo, simbolo, args):
        if modulo == "arquivo":
            leitura = {
                "ler",
                "linhas",
                "existe",
                "arquivo",
                "diretorio_existe",
                "listar",
                "tamanho",
            }
            escrita = {
                "escrever",
                "anexar",
                "criar_diretorio",
                "remover",
                "apagar",
            }
            if simbolo == "copiar" and len(args) >= 2:
                args[0] = self.sandbox.validar_arquivo(str(args[0]), "leitura")
                args[1] = self.sandbox.validar_arquivo(str(args[1]), "escrita")
            elif simbolo == "mover" and len(args) >= 2:
                args[0] = self.sandbox.validar_arquivo(str(args[0]), "escrita")
                args[1] = self.sandbox.validar_arquivo(str(args[1]), "escrita")
            elif simbolo in leitura or simbolo in escrita:
                modo = "escrita" if simbolo in escrita else "leitura"
                caminho = args[0] if args else ""
                if args:
                    args[0] = self.sandbox.validar_arquivo(str(caminho), modo)
        if modulo == "rede" and simbolo in ("get", "post", "baixar"):
            url = args[0] if args else ""
            self.sandbox.validar_rede(str(url))
        if modulo == "sistema" and simbolo in ("executar", "executar_detalhado"):
            comando = args[0] if args else ""
            if str(comando).strip():
                self.sandbox.validar_subprocesso(str(comando))

    def aridade_funcao(self, instrucoes):
        aridade = 0
        for inst in instrucoes:
            if inst and inst[0] == "DEFINIR_VAR":
                aridade += 1
                continue
            break
        return aridade

    def validar_aridade_funcao(self, tipo, nome, instrucoes, args):
        esperado = self.aridade_funcao(instrucoes)
        recebido = len(args)
        if recebido != esperado:
            raise TypeError(
                f"{tipo} '{nome}' esperava {esperado} argumento(s), mas recebeu {recebido}."
            )
    
    def adicionar_breakpoint(self, linha: int):
        """Adiciona um breakpoint"""
        self.breakpoints.add(linha)
    
    def remover_breakpoint(self, linha: int):
        """Remove um breakpoint"""
        self.breakpoints.discard(linha)
    
    def verificar_breakpoint(self, linha: int) -> bool:
        """Verifica se há breakpoint na linha"""
        return linha in self.breakpoints
    
    def pausar_debugger(self):
        """Pausa a execução no debugger"""
        self.pausado = True
        if self.debugger_callback:
            self.debugger_callback("pausa", self.linha_atual)
    
    def continuar_debugger(self):
        """Continua a execução no debugger"""
        self.pausado = False
    
    def entrar_funcao_debugger(self, nome_funcao: str):
        """Registra entrada em função para pilha de chamadas"""
        self.pilha_chamadas.append({
            "funcao": nome_funcao,
            "linha": self.linha_atual,
            "escopo": self.escopos[-1].copy()
        })
    
    def sair_funcao_debugger(self):
        """Registra saída de função da pilha de chamadas"""
        if self.pilha_chamadas:
            self.pilha_chamadas.pop()
    
    def step_over_debugger(self):
        """Executa uma linha sem entrar em funções"""
        self.pausado = False
        self.modo_step = "over"
    
    def step_into_debugger(self):
        """Executa uma linha entrando em funções"""
        self.pausado = False
        self.modo_step = "into"
        self.profundidade_step = len(self.pilha_chamadas)
    
    def step_out_debugger(self):
        """Executa até sair da função atual"""
        self.pausado = False
        self.modo_step = "out"
        self.profundidade_step = len(self.pilha_chamadas)
    
    def alocar_heap(self, valor):
        """Aloca objeto no heap e retorna referência"""
        ref_id = self.heap_contador
        self.heap[ref_id] = valor
        self.heap_contador += 1
        if self.gc_ativo and len(self.heap) >= self.gc_limiar:
            self.coletar_lixo()
        return ref_id
    
    def liberar_heap(self, ref_id):
        """Libera objeto do heap"""
        if ref_id in self.heap:
            del self.heap[ref_id]
    
    def coletar_lixo(self):
        """Garbage Collector Mark-and-Sweep"""
        if not self.gc_ativo:
            return
        
        # Mark: marcar objetos alcançáveis
        marcados = set()
        
        # Marcar objetos na pilha
        for item in self.pilha:
            if isinstance(item, int) and item in self.heap:
                self._marcar_objeto(item, marcados)
        
        # Marcar objetos nos escopos
        for escopo in self.escopos:
            for valor in escopo.values():
                if isinstance(valor, int) and valor in self.heap:
                    self._marcar_objeto(valor, marcados)
        
        # Sweep: remover objetos não marcados
        para_remover = [ref_id for ref_id in self.heap if ref_id not in marcados]
        for ref_id in para_remover:
            self.liberar_heap(ref_id)
        
        self.gc_estatisticas["ciclos"] += 1
        self.gc_estatisticas["objetos_liberados"] += len(para_remover)
        return len(para_remover)

    def estatisticas_gc(self):
        return {
            "objetos_heap": len(self.heap),
            "proximo_id": self.heap_contador,
            **self.gc_estatisticas,
        }
    
    def _marcar_objeto(self, ref_id, marcados):
        """Marca objetos alcançáveis sem recursão Python."""
        pendentes = [ref_id]

        while pendentes:
            atual = pendentes.pop()
            if atual in marcados:
                continue
            marcados.add(atual)

            if atual not in self.heap:
                continue

            obj = self.heap[atual]
            if isinstance(obj, dict):
                for valor in obj.values():
                    if isinstance(valor, int) and valor in self.heap:
                        pendentes.append(valor)
            elif isinstance(obj, list):
                for item in obj:
                    if isinstance(item, int) and item in self.heap:
                        pendentes.append(item)
    
    def criar_canal(self, capacidade: int = 0) -> int:
        """Cria um novo canal CSP e retorna seu ID"""
        from vm_enhanced import CanalCSP
        canal_id = self.canal_contador
        self.canais[canal_id] = CanalCSP(capacidade)
        self.canal_contador += 1
        return canal_id
    
    def escrever_canal(self, canal_id: int, valor):
        """Escreve valor em um canal"""
        if canal_id in self.canais:
            return self.canais[canal_id].escrever(valor)
        return False
    
    def ler_canal(self, canal_id):
        """Lê valor de um canal"""
        if canal_id in self.canais:
            return self.canais[canal_id].ler()
        return None
    
    def fechar_canal(self, canal_id: int):
        """Fecha um canal"""
        if canal_id in self.canais:
            self.canais[canal_id].fechar()
    
    def ativar_scheduler(self):
        """Ativa o scheduler de green threads"""
        from vm_enhanced import SchedulerGreenThreads
        self.scheduler = SchedulerGreenThreads(num_threads_sistema=4)
        self.scheduler.iniciar()
        self.scheduler_ativo = True
    
    def adicionar_tarefa_paralela(self, instrucoes):
        """Adiciona uma tarefa para execução paralela"""
        if not self.scheduler_ativo:
            self.ativar_scheduler()
        
        from vm_enhanced import GreenThread
        tarefa_id = len(self.tarefas_pendentes) + len(self.tarefas_executando)
        tarefa = GreenThread(tarefa_id, instrucoes, self)
        self.tarefas_pendentes.append(tarefa)
        return tarefa_id

    def executar(self):
        try:
            self.executar_instrucoes(self.instrucoes)
        except ReturnException as rx:
            raise rx
        except Exception as e:
            self.anexar_rastreamento(e)
            raise
            if hasattr(self, "ultima_instrucao"):
                print(f"Erro ao executar instrução: {self.ultima_instrucao}")
            print(f"Pilha da VM: {self.pilha}")
            print("Escopos da VM (do mais recente ao mais antigo):")
            for i, escopo in enumerate(reversed(self.escopos)):
                truncated_escopo = {}
                for k, v in escopo.items():
                    if k == "linhas":
                        truncated_escopo[k] = f"<lista de {len(v)} linhas>"
                    elif k == "constantes":
                        truncated_escopo[k] = f"<lista de {len(v)} constantes>"
                    elif isinstance(v, str) and len(v) > 200:
                        truncated_escopo[k] = v[:200] + "..."
                    else:
                        truncated_escopo[k] = v
                print(f"  Escopo {i}: {truncated_escopo}")
            raise e

    def executar_instrucoes(self, instrucoes):
        idx = 0
        total = len(instrucoes)
        
        while idx < total:
            # Verificar breakpoint se debugger ativo
            if self.debugger_ativo and self.linha_atual in self.breakpoints:
                self.pausar_debugger()
            
            # Aguardar se pausado
            while self.pausado:
                if self.debugger_callback:
                    self.debugger_callback("aguardando", self.linha_atual)
                import time
                time.sleep(0.1)
            
            inst = instrucoes[idx]
            self.ultima_instrucao = inst
            op = inst[0]
            args = inst[1:]
            idx += 1
            self.instrucoes_executadas += 1
            if self.instrucoes_executadas % self.gc_limiar == 0:
                self.coletar_lixo()
                gc.collect()
            
            if op == "EMPILHAR_CONST":
                c_idx = args[0]
                self.pilha.append(self.constantes[c_idx])
                
            elif op == "DEFINIR_VAR":
                var_nome = args[0]
                val = self.pilha.pop()
                self.definir_var(var_nome, val)

            elif op == "ATRIBUIR_VAR":
                var_nome = args[0]
                val = self.pilha.pop()
                self.atribuir_var(var_nome, val)
                
            elif op == "OBTER_VAR":
                var_nome = args[0]
                val = self.obter_var(var_nome)
                self.pilha.append(val)

            elif op == "IMPORTAR_MODULO":
                modulo_nome = args[0]
                nome_local = args[1]
                self.definir_var(nome_local, self.carregar_modulo(modulo_nome))

            elif op == "IMPORTAR_SIMBOLOS":
                modulo_nome = args[0]
                simbolos = args[1]
                self.carregar_modulo(modulo_nome)
                for simbolo in simbolos:
                    self.definir_var(simbolo, criar_handle_funcao(modulo_nome, simbolo))

            elif op == "DECLARAR_CLASSE":
                nome_classe = args[0]
                campos = args[1]
                metodos = args[2] if len(args) > 2 else {}
                classe_mae = args[3] if len(args) > 3 else None
                self.classes[nome_classe] = {"campos": campos, "metodos": metodos, "classe_mae": classe_mae}
                self.definir_var(nome_classe, {
                    "__tipo_br__": "classe",
                    "nome": nome_classe,
                    "campos": campos,
                    "metodos": metodos,
                    "classe_mae": classe_mae,
                })
                
            elif op == "OBTER_PROPRIEDADE":
                prop_nome = args[0]
                obj = self.pilha.pop()
                if isinstance(obj, dict):
                    self.pilha.append(obj.get(prop_nome, None))
                elif isinstance(obj, list):
                    try:
                        self.pilha.append(obj[int(prop_nome)])
                    except:
                        self.pilha.append(None)
                else:
                    self.pilha.append(None)

            elif op == "OBTER_INDICE":
                indice = self.pilha.pop()
                obj = self.pilha.pop()
                if isinstance(obj, dict):
                    self.pilha.append(obj.get(indice, None))
                elif isinstance(obj, (list, tuple, str)):
                    try:
                        if isinstance(indice, slice):
                            self.pilha.append(obj[indice])
                        else:
                            self.pilha.append(obj[int(indice)])
                    except:
                        self.pilha.append(None)
                else:
                    self.pilha.append(None)

            elif op == "DEFINIR_PROPRIEDADE":
                prop_nome = args[0]
                valor = self.pilha.pop()
                obj = self.pilha.pop()
                if isinstance(obj, dict):
                    obj[prop_nome] = valor
                else:
                    raise TypeError(f"Objeto nao aceita propriedade '{prop_nome}'")

            elif op == "DEFINIR_INDICE":
                valor = self.pilha.pop()
                indice = self.pilha.pop()
                obj = self.pilha.pop()
                if isinstance(obj, dict):
                    obj[indice] = valor
                elif isinstance(obj, list):
                    if isinstance(indice, slice):
                        obj[indice] = valor
                    else:
                        indice_lista = int(indice)
                        if -len(obj) <= indice_lista < len(obj):
                            obj[indice_lista] = valor
                        else:
                            raise IndexError(f"Indice fora da lista: {indice}")
                else:
                    raise TypeError("Objeto nao aceita atribuicao por indice")
                
            elif op == "SOMAR":
                b = self.pilha.pop()
                a = self.pilha.pop()
                self.pilha.append(str(a) + str(b) if (isinstance(a, str) or isinstance(b, str)) else a + b)
                
            elif op == "SUBTRAIR":
                b = self.pilha.pop()
                a = self.pilha.pop()
                self.pilha.append(a - b)
                
            elif op == "MULTIPLICAR":
                b = self.pilha.pop()
                a = self.pilha.pop()
                self.pilha.append(a * b)
                
            elif op == "DIVIDIR":
                b = self.pilha.pop()
                a = self.pilha.pop()
                self.pilha.append(a / b)
                
            elif op == "IGUAL_A":
                b = self.pilha.pop()
                a = self.pilha.pop()
                self.pilha.append(a == b)

            elif op == "NAO_IGUAL":
                b = self.pilha.pop()
                a = self.pilha.pop()
                self.pilha.append(a != b)
                
            elif op == "MAIOR_QUE":
                b = self.pilha.pop()
                a = self.pilha.pop()
                # Flexible comparison: numbers, booleans, strings; otherwise compare string representations
                if isinstance(a, (int, float)) and isinstance(b, (int, float)):
                    self.pilha.append(a > b)
                elif isinstance(a, bool) and isinstance(b, bool):
                    self.pilha.append(a > b)
                elif isinstance(a, str) and isinstance(b, str):
                    self.pilha.append(a > b)
                else:
                    # Fallback: compare string representations to avoid TypeError
                    self.pilha.append(str(a) > str(b))

            elif op == "MAIOR_OU_IGUAL":
                b = self.pilha.pop()
                a = self.pilha.pop()
                if isinstance(a, (int, float)) and isinstance(b, (int, float)):
                    self.pilha.append(a >= b)
                elif isinstance(a, bool) and isinstance(b, bool):
                    self.pilha.append(a >= b)
                elif isinstance(a, str) and isinstance(b, str):
                    self.pilha.append(a >= b)
                else:
                    self.pilha.append(str(a) >= str(b))
                
            elif op == "MENOR_QUE":
                b = self.pilha.pop()
                a = self.pilha.pop()
                if isinstance(a, (int, float)) and isinstance(b, (int, float)):
                    self.pilha.append(a < b)
                elif isinstance(a, bool) and isinstance(b, bool):
                    self.pilha.append(a < b)
                elif isinstance(a, str) and isinstance(b, str):
                    self.pilha.append(a < b)
                else:
                    self.pilha.append(str(a) < str(b))

            elif op == "MENOR_OU_IGUAL":
                b = self.pilha.pop()
                a = self.pilha.pop()
                if isinstance(a, (int, float)) and isinstance(b, (int, float)):
                    self.pilha.append(a <= b)
                elif isinstance(a, bool) and isinstance(b, bool):
                    self.pilha.append(a <= b)
                elif isinstance(a, str) and isinstance(b, str):
                    self.pilha.append(a <= b)
                else:
                    self.pilha.append(str(a) <= str(b))
                
            elif op == "RETORNAR":
                val = self.pilha.pop() if self.pilha else None
                raise ReturnException(val)

            elif op == "QUEBRAR_LOOP":
                raise BreakException()

            elif op == "DESCARTAR_TOPO":
                if self.pilha:
                    self.pilha.pop()
                
            elif op == "ESPERE_IGUAL":
                esperado = self.pilha.pop()
                obtido = self.pilha.pop()
                if obtido != esperado:
                    raise AssertionError(f"Esperava '{esperado}' ({type(esperado).__name__}), mas obteve '{obtido}' ({type(obtido).__name__})")
                    
            elif op == "CHAMAR_METODO":
                func_nome = args[0]
                arg_count = args[1]
                
                func_args = []
                for _ in range(arg_count):
                    func_args.append(self.pilha.pop())
                func_args.reverse()
                
                if func_nome in ("len", "tamanho") and arg_count == 1:
                    try:
                        self.pilha.append(len(func_args[0]))
                    except:
                        self.pilha.append(0)
                    continue
                    
                if func_nome == "inteiro" and arg_count == 1:
                    try:
                        self.pilha.append(int(func_args[0]))
                    except:
                        self.pilha.append(0)
                    continue

                if func_nome in ("str", "texto") and arg_count == 1:
                    try:
                        self.pilha.append(str(func_args[0]))
                    except:
                        self.pilha.append("")
                    continue
                
                importado = self.obter_var(func_nome)
                if self.eh_funcao_modulo(importado):
                    try:
                        self.pilha.append(self.chamar_handle_funcao(importado, func_args))
                    except Exception as exc:
                        self.anexar_rastreamento(exc, self.rastreamento_chamadas + [func_nome])
                        raise
                    continue

                if func_nome in self.classes:
                    try:
                        self.pilha.append(self.instanciar_classe(func_nome, func_args))
                    except Exception as exc:
                        self.anexar_rastreamento(exc, self.rastreamento_chamadas + [func_nome])
                        raise
                    continue

                # Verificar se é uma tarefa
                nome_tarefa = "__tarefa_" + func_nome
                if nome_tarefa in self.funcoes:
                    self.validar_aridade_funcao("Tarefa", func_nome, self.funcoes[nome_tarefa], func_args)
                    if not self.scheduler_ativo:
                        self.ativar_scheduler()
                    tarefa_id = self.scheduler.criar_tarefa(self.funcoes[nome_tarefa], self, func_args)
                    self.pilha.append(tarefa_id)
                    continue

                if func_nome not in self.funcoes:
                    raise NameError(f"Função '{func_nome}' não declarada na linguagem BR.")
                if self.call_depth >= self.max_call_depth:
                    raise RecursionError(
                        f"Recursão máxima excedida em '{func_nome}' "
                        f"({self.max_call_depth} chamadas). Ajuste BR_MAX_CALL_DEPTH se necessário."
                    )
                    
                func_insts = self.funcoes[func_nome]
                self.validar_aridade_funcao("Função", func_nome, func_insts, func_args)
                self.rastreamento_chamadas.append(func_nome)
                self.escopos.append({})
                for arg in func_args:
                    self.pilha.append(arg)
                    
                retorno = None
                try:
                    self.call_depth += 1
                    self.executar_instrucoes(func_insts)
                except ReturnException as rx:
                    retorno = rx.valor
                except Exception as e:
                    self.anexar_rastreamento(e)
                    raise
                    if not getattr(e, "_escopos_printed", False):
                        e._escopos_printed = True
                        print(f"Erro ao executar método '{func_nome}'")
                        print("Escopos da VM no momento do erro:")
                        for i, escopo in enumerate(reversed(self.escopos)):
                            truncated_escopo = {}
                            for k, v in escopo.items():
                                if k == "linhas":
                                    truncated_escopo[k] = f"<lista de {len(v)} linhas>"
                                elif k == "constantes":
                                    truncated_escopo[k] = f"<lista de {len(v)} constantes>"
                                elif isinstance(v, str) and len(v) > 200:
                                    truncated_escopo[k] = v[:200] + "..."
                                else:
                                    truncated_escopo[k] = v
                            print(f"  Escopo {i}: {truncated_escopo}")
                    raise e
                finally:
                    self.call_depth -= 1
                    self.escopos.pop()
                    if self.rastreamento_chamadas and self.rastreamento_chamadas[-1] == func_nome:
                        self.rastreamento_chamadas.pop()
                    
                self.pilha.append(retorno)

            elif op == "CHAMAR_METODO_OBJETO":
                metodo_nome = args[0]
                arg_count = args[1]

                func_args = []
                for _ in range(arg_count):
                    func_args.append(self.pilha.pop())
                func_args.reverse()

                objeto = self.pilha.pop()
                try:
                    self.pilha.append(self.chamar_metodo_objeto(objeto, metodo_nome, func_args))
                except Exception as exc:
                    self.anexar_rastreamento(exc)
                    raise
                
            elif op == "CHAMAR_SERVICO":
                servico = args[0]
                metodo = args[1]
                arg_count = args[2]
                
                func_args = []
                for _ in range(arg_count):
                    func_args.append(self.pilha.pop())
                func_args.reverse()
                
                res = self.chamar_servico_nativo(servico, metodo, func_args)
                comandos = {
                    ("SISTEMA", "MOSTRAR"),
                    ("BANCO", "SALVAR"),
                    ("BANCO", "REMOVER"),
                    ("ARQUIVOS", "ESCREVER"),
                    ("ARQUIVOS", "ESCREVER_BINARIO"),
                    ("LISTA", "ADICIONAR"),
                    ("REGISTRO", "DEFINIR")
                }
                if (servico, metodo) not in comandos:
                    self.pilha.append(res)

            elif op == "CRIAR_LISTA_BLOCO":
                var_nome = args[0]
                corpo = args[1]
                
                antiga_lista = self.temp_lista
                self.temp_lista = []
                self.executar_instrucoes(corpo)
                
                resultado = self.temp_lista
                self.temp_lista = antiga_lista
                self.definir_var(var_nome, resultado)

            elif op == "LISTA_ELEMENTO":
                val = self.pilha.pop()
                self.temp_lista.append(val)

            elif op == "CRIAR_REGISTRO_BLOCO":
                var_nome = args[0]
                corpo = args[1]
                
                antigo_reg = self.temp_registro
                self.temp_registro = {}
                self.executar_instrucoes(corpo)
                
                resultado = self.temp_registro
                self.temp_registro = antigo_reg
                self.definir_var(var_nome, resultado)

            elif op == "REGISTRO_PROPRIEDADE":
                prop_nome = args[0]
                val = self.pilha.pop()
                if self.temp_agente is not None and len(self.temp_agente) > 0:
                    self.temp_agente[prop_nome] = val
                else:
                    self.temp_registro[prop_nome] = val

            elif op == "IA_PERGUNTE_BLOCO":
                var_nome = args[0]
                corpo = args[1]
                self.executar_instrucoes(corpo)
                prompt = self.pilha.pop()
                res = self.chamar_servico_nativo("IA", "PERGUNTAR", [prompt])
                self.definir_var(var_nome, res)

            elif op == "IA_AGENTE_BLOCO":
                var_nome = args[0]
                corpo = args[1]
                
                antigo_agente = self.temp_agente
                self.temp_agente = {"tipo": "agente_ia"}
                self.executar_instrucoes(corpo)
                
                resultado = mesclar_estado(var_nome, self.temp_agente)
                salvar_agente(var_nome, resultado)
                self.temp_agente = antigo_agente
                self.definir_var(var_nome, resultado)

            elif op == "TENTE_CAPTURE_BLOCO":
                bloco_dict = args[0]
                tente_corpo = bloco_dict["tente"]
                var_erro = bloco_dict["var_erro"]
                capture_corpo = bloco_dict["capture"]
                
                try:
                    self.executar_instrucoes(tente_corpo)
                except Exception as e:
                    self.escopos.append({})
                    erro_reg = {
                        "mensagem": str(e),
                        "tipo": type(e).__name__
                    }
                    self.definir_var(var_erro, erro_reg)
                    try:
                        self.executar_instrucoes(capture_corpo)
                    finally:
                        self.escopos.pop()

            elif op == "PARA_CADA_BLOCO":
                block_dict = args[0]
                var_item = block_dict["var_item"]
                lista_insts = block_dict["lista_insts"]
                corpo = block_dict["corpo"]
                
                if lista_insts:
                    self.executar_instrucoes(lista_insts)
                lst = self.pilha.pop()
                
                if isinstance(lst, list):
                    for item in lst:
                        self.escopos.append({})
                        self.definir_var(var_item, item)
                        try:
                            self.executar_instrucoes(corpo)
                        except BreakException:
                            self.escopos.pop()
                            break
                        except ReturnException as rx:
                            raise rx
                        finally:
                            if self.escopos and len(self.escopos) > 1:
                                self.escopos.pop()

            elif op == "ENQUANTO_BLOCO":
                bloco_dict = args[0]
                cond_insts = bloco_dict["condicao"]
                corpo = bloco_dict["corpo"]
                
                while True:
                    if cond_insts:
                        self.executar_instrucoes(cond_insts)
                    cond_res = self.pilha.pop()
                    if not cond_res:
                        break
                    try:
                        self.executar_instrucoes(corpo)
                    except BreakException:
                        break
                    except ReturnException as rx:
                        raise rx

            elif op == "PARALELO_EXECUTE":
                blocos = args[0]
                threads = []
                erros_threads = []
                timeout = float(os.environ.get("BR_THREAD_TIMEOUT", "30"))
                
                def thread_func(bloco_insts):
                    try:
                        sub_vm = MaquinaVirtualBR({
                            "constantes": self.constantes,
                            "funcoes": self.funcoes,
                            "classes": self.classes,
                        }, diretorio_base=self.diretorio_base)
                        sub_vm.escopos = self.escopos
                        sub_vm._escopo_lock = self._escopo_lock
                        sub_vm.modulos_carregados = self.modulos_carregados
                        sub_vm._module_lock = self._module_lock
                        sub_vm.banco = self.banco
                        sub_vm.executar_instrucoes(bloco_insts)
                    except Exception as exc:
                        erros_threads.append(exc)
                
                for bloco in blocos:
                    t = threading.Thread(target=thread_func, args=(bloco,), daemon=True)
                    threads.append(t)
                    t.start()
                for t in threads:
                    t.join(timeout=timeout)
                    if t.is_alive():
                        raise TimeoutError(
                            f"Deadlock ou tarefa paralela bloqueada por mais de {timeout:.1f}s"
                        )
                if erros_threads:
                    raise erros_threads[0]

            elif op == "REPITA_VEZES":
                corpo = args[0]
                vezes = self.pilha.pop()
                for _ in range(int(vezes)):
                    try:
                        self.executar_instrucoes(corpo)
                    except BreakException:
                        break

            elif op == "CONTE_BLOCO":
                bloco_dict = args[0]
                var_nome = bloco_dict["variavel"]
                corpo = bloco_dict["corpo"]
                
                fim = self.pilha.pop()
                inicio = self.pilha.pop()
                
                for i in range(int(inicio), int(fim) + 1):
                    self.escopos.append({})
                    self.definir_var(var_nome, i)
                    try:
                        self.executar_instrucoes(corpo)
                    except BreakException:
                        self.escopos.pop()
                        break
                    except ReturnException as rx:
                        raise rx
                    finally:
                        if self.escopos and len(self.escopos) > 1:
                            self.escopos.pop()

            elif op == "AGUARDAR_TAREFA":
                tarefa_id = self.pilha.pop()
                if self.scheduler_ativo:
                    resultado = self.scheduler.aguardar_tarefa(tarefa_id)
                    self.pilha.append(resultado)
                else:
                    self.pilha.append(None)

            elif op == "ESCOLHA_CASO":
                bloco_dict = args[0]
                alvo = self.pilha.pop()
                executou = False
                for caso in bloco_dict["casos"]:
                    self.executar_instrucoes(caso["valor"])
                    valor = self.pilha.pop()
                    if alvo == valor:
                        self.executar_instrucoes(caso["corpo"])
                        executou = True
                        break
                if not executou:
                    self.executar_instrucoes(bloco_dict["padrao"])

            elif op == "CONDICIONAL_SE":
                bloco_dict = args[0]
                cond_insts = bloco_dict["condicao"]
                se_insts = bloco_dict["se"]
                senao_insts = bloco_dict["senao"]
                
                if cond_insts:
                    self.executar_instrucoes(cond_insts)
                cond_res = self.pilha.pop()
                if cond_res:
                    self.executar_instrucoes(se_insts)
                else:
                    self.executar_instrucoes(senao_insts)

    def chamar_servico_nativo(self, servico, metodo, args):
        if servico == "SISTEMA":
            if metodo == "MOSTRAR":
                val = args[0]
                sys.stdout.flush()
                print(val, flush=True)
                sys.stdout.flush()
                return None
            elif metodo == "TIPO":
                val = args[0]
                if isinstance(val, bool):
                    return "booleano"
                elif val is None:
                    return "nulo"
                elif isinstance(val, int):
                    return "inteiro"
                elif isinstance(val, float):
                    return "decimal"
                elif isinstance(val, str):
                    return "texto"
                elif isinstance(val, list):
                    return "lista"
                elif isinstance(val, dict):
                    return "registro"
                return "desconhecido"
            elif metodo == "INTEIRO":
                try:
                    return int(args[0])
                except:
                    return None
            elif metodo == "DECIMAL":
                try:
                    return float(args[0])
                except:
                    return None
            elif metodo == "FATIA":
                return slice(args[0], args[1])
            elif metodo == "ARGUMENTOS":
                return sys.argv[3:]
                
        elif servico == "IA":
            ia_service = get_ia_service()
            
            if metodo == "PERGUNTAR":
                prompt = args[0]
                contexto = args[1] if len(args) > 1 else None
                try:
                    return ia_service.perguntar(prompt, contexto)
                except Exception as e:
                    return f"[Erro IA] {str(e)}"
                    
            elif metodo == "PERGUNTAR_AGENTE":
                agente_dict = args[0]
                prompt = args[1]
                nome_agente = agente_dict.get("nome", "agente_" + str(hash(str(agente_dict))))
                especialidade = agente_dict.get("especialidade", "geral")
                
                try:
                    agente = ia_service.criar_agente(nome_agente, especialidade)
                    return agente.perguntar(prompt)
                except Exception as e:
                    return f"[Erro Agente IA] {str(e)}"
            
            elif metodo == "CRIAR_AGENTE":
                nome = args[0]
                especialidade = args[1] if len(args) > 1 else "geral"
                try:
                    agente = ia_service.criar_agente(nome, especialidade)
                    return {"nome": agente.nome, "especialidade": agente.especialidade}
                except Exception as e:
                    return f"[Erro Criar Agente] {str(e)}"
                
        elif servico == "BANCO":
            if metodo == "SALVAR":
                reg = args[0]
                self.banco.append(reg)
                print(f"[Banco de Dados BR] Registro salvo com sucesso! ({reg})")
                return None
            elif metodo == "REMOVER":
                reg = args[0]
                if reg in self.banco:
                    self.banco.remove(reg)
                print(f"[Banco de Dados BR] Registro removido do banco.")
                return None
            elif metodo == "BUSQUE_TODOS_CLIENTES":
                return self.banco
                
        elif servico == "WEB":
            if metodo == "ACESSE":
                url = args[0]
                print(f"[Internet Nativa BR] Acessando URL '{url}'...")
                return f"Conteúdo Simulado da Página '{url}': Loja de Vendas BR - Produtos de Informática"
                
        elif servico == "ARQUIVOS":
            if metodo == "LER":
                caminho = args[0]
                caminho_seguro = self.sandbox.validar_arquivo(str(caminho), "leitura")
                return ler_texto(caminho_seguro)
            elif metodo == "ESCREVER":
                caminho = args[0]
                conteudo = args[1]
                caminho_seguro = self.sandbox.validar_arquivo(str(caminho), "escrita")
                parent = Path(caminho_seguro).parent
                if str(parent) not in ("", "."):
                    parent.mkdir(parents=True, exist_ok=True)
                escrever_texto(caminho_seguro, conteudo)
                return None
            elif metodo == "ESCREVER_BINARIO":
                caminho = args[0]
                conteudo = args[1]
                caminho_seguro = self.sandbox.validar_arquivo(str(caminho), "escrita")
                parent = Path(caminho_seguro).parent
                if str(parent) not in ("", "."):
                    parent.mkdir(parents=True, exist_ok=True)
                # Serializar dicionário/lista de bytecode em formato BRC1
                serialized = b"BRC1" + serializar(conteudo)
                with open(caminho_seguro, "wb") as f:
                    f.write(serialized)
                return None
                
        elif servico == "TEXTO":
            if metodo == "DIVIDIR":
                val = args[0]
                return [line.strip() for line in val.splitlines() if line.strip()]
            elif metodo == "CONTEM":
                texto = args[0]
                sub = args[1]
                if texto is None or sub is None:
                    return False
                return sub in texto
            elif metodo == "DIVIDIR_POR":
                texto = args[0]
                delim = args[1]
                if texto is None:
                    return []
                if delim == "" or delim is None:
                    return [p for p in texto.split() if p]
                return [p.strip() for p in texto.split(delim) if p.strip()]
            elif metodo == "SUBSTITUIR":
                texto = args[0]
                antigo = args[1]
                novo = args[2]
                if texto is None or antigo is None or novo is None:
                    return None
                return texto.replace(antigo, novo)
            elif metodo == "CONTEM_INICIO":
                texto = args[0]
                pref = args[1]
                if texto is None or pref is None:
                    return False
                return texto.startswith(pref)
            elif metodo == "DIVIDIR_PRIMEIRO":
                texto = args[0]
                delim = args[1]
                if texto is None:
                    return []
                if delim == "" or delim is None:
                    return [p for p in texto.split() if p]
                parts = texto.split(delim, 1)
                return [p.strip() for p in parts if p.strip()]
                
        elif servico == "LISTA":
            if metodo == "ADICIONAR":
                lst = args[0]
                item = args[1]
                if isinstance(lst, list):
                    lst.append(item)
                return None
            elif metodo == "OBTER":
                lst = args[0]
                idx = args[1]
                if isinstance(lst, list) and 0 <= idx < len(lst):
                    return lst[idx]
                return None
            elif metodo == "TAMANHO":
                lst = args[0]
                if isinstance(lst, list):
                    return len(lst)
                return 0
                
        elif servico == "REGISTRO":
            if metodo == "DEFINIR":
                reg = args[0]
                chave = args[1]
                valor = args[2]
                if isinstance(reg, dict):
                    reg[chave] = valor
                return None
            elif metodo == "OBTER":
                reg = args[0]
                chave = args[1]
                if isinstance(reg, dict):
                    return reg.get(chave, None)
                return None
                
        raise NotImplementedError(f"Serviço nativo '{servico}' com método '{metodo}' não está implementado no runtime desta versão da VM.")

def main():
    if len(sys.argv) < 3:
        print("Uso da Máquina Virtual BR:")
        print("  brasil executar <arquivo.brc>")
        sys.exit(1)
        
    acao = sys.argv[1]
    caminho = sys.argv[2]
    
    if acao == "executar":
        if not os.path.exists(caminho):
            print(f"Erro: Arquivo compilado '{caminho}' não encontrado.")
            sys.exit(1)
            
        with open(caminho, "rb") as f:
            header = f.read(4)
            if header != b"BRC1":
                print(f"Erro: Arquivo '{caminho}' não está no formato de bytecode binário BRC1 válido.")
                sys.exit(1)
            data = f.read()
            
        stream = ByteStream(data)
        bytecode = deserializar(stream)
        
        vm = MaquinaVirtualBR(bytecode)
        vm.executar()

if __name__ == "__main__":
    main()
