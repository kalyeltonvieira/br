# -*- coding: utf-8 -*-
"""
Máquina Virtual BR Enhanced com Green Threads e Canais CSP
Implementa concorrência de alta performance com modelo M:N
"""
import sys
import os
import json
import re
import struct
import threading
import queue
import time
from typing import List, Dict, Any, Optional, Callable
from enum import Enum

class EstadoTarefa(Enum):
    AGUARDANDO = "AGUARDANDO"
    EXECUTANDO = "EXECUTANDO"
    CONCLUIDO = "CONCLUIDO"
    BLOQUEADA = "BLOQUEADA"

class CanalCSP:
    """Canal CSP para comunicação entre tarefas sem compartilhamento de memória"""
    def __init__(self, capacidade: int = 0):
        self.fila = queue.Queue(maxsize=capacidade)
        self.fechado = False
    
    def escrever(self, valor: Any) -> bool:
        """Escreve valor no canal (bloqueia se cheio)"""
        if self.fechado:
            return False
        try:
            self.fila.put(valor, block=True, timeout=None)
            return True
        except:
            return False
    
    def ler(self) -> Optional[Any]:
        """Lê valor do canal (bloqueia se vazio)"""
        if self.fechado and self.fila.empty():
            return None
        try:
            return self.fila.get(block=True, timeout=None)
        except:
            return None
    
    def fechar(self):
        """Fecha o canal"""
        self.fechado = True
    
    def tamanho(self) -> int:
        """Retorna número de itens no canal"""
        return self.fila.qsize()

class GreenThread:
    """Tarefa leve (Green Thread) para execução concorrente"""
    def __init__(self, id: int, instrucoes: List, vm_parent, argumentos: Optional[List[Any]] = None):
        self.id = id
        self.instrucoes = instrucoes
        self.vm_parent = vm_parent
        self.estado = EstadoTarefa.AGUARDANDO
        self.pilha_local = list(argumentos or [])
        self.escopos_locais = [{}]
        self.resultado = None
        self.erro = None
        self.canais_criados = []
    
    def executar(self):
        """Executa a tarefa"""
        self.estado = EstadoTarefa.EXECUTANDO
        try:
            # Criar sub-VM isolada para esta tarefa
            from maquina_virtual import MaquinaVirtualBR, ReturnException
            sub_vm = MaquinaVirtualBR({
                "constantes": self.vm_parent.constantes,
                "instrucoes": [],
                "funcoes": self.vm_parent.funcoes,
                "classes": getattr(self.vm_parent, "classes", {}),
                "testes": [],
                "documentacoes": [],
            }, diretorio_base=getattr(self.vm_parent, "diretorio_base", None))
            sub_vm.modulos_carregados = getattr(self.vm_parent, "modulos_carregados", {})
            sub_vm._module_lock = getattr(self.vm_parent, "_module_lock", threading.RLock())
            sub_vm.pilha = self.pilha_local
            sub_vm.escopos = self.escopos_locais
            sub_vm.banco = self.vm_parent.banco
            sub_vm.canais = self.vm_parent.canais
            
            try:
                sub_vm.executar_instrucoes(self.instrucoes)
                self.resultado = sub_vm.pilha[-1] if sub_vm.pilha else None
            except ReturnException as rx:
                self.resultado = rx.valor
            self.estado = EstadoTarefa.CONCLUIDO
            
        except Exception as e:
            self.erro = e
            self.estado = EstadoTarefa.BLOQUEADA
            raise

class SchedulerGreenThreads:
    """Escalonador de Green Threads (modelo M:N)"""
    def __init__(self, num_threads_sistema: int = 4):
        self.num_threads_sistema = num_threads_sistema
        self.fila_tarefas = queue.Queue()
        self.tarefas_ativas: Dict[int, GreenThread] = {}
        self.threads_sistema = []
        self.proximo_id = 0
        self.rodando = False
    
    def iniciar(self):
        """Inicia o escalonador"""
        self.rodando = True
        for i in range(self.num_threads_sistema):
            thread = threading.Thread(target=self._loop_thread, daemon=True)
            thread.start()
            self.threads_sistema.append(thread)
    
    def parar(self):
        """Para o escalonador"""
        self.rodando = False
        for thread in self.threads_sistema:
            thread.join(timeout=1)
    
    def _loop_thread(self):
        """Loop de thread do sistema"""
        while self.rodando:
            try:
                tarefa = self.fila_tarefas.get(timeout=0.1)
                if tarefa:
                    tarefa.executar()
                    self.tarefas_ativas[tarefa.id] = tarefa
            except queue.Empty:
                continue
            except Exception as e:
                print(f"Erro na thread do sistema: {e}")
    
    def criar_tarefa(self, instrucoes: List, vm_parent, argumentos: Optional[List[Any]] = None) -> int:
        """Cria uma nova tarefa"""
        tarefa_id = self.proximo_id
        self.proximo_id += 1
        
        tarefa = GreenThread(tarefa_id, instrucoes, vm_parent, argumentos)
        self.fila_tarefas.put(tarefa)
        self.tarefas_ativas[tarefa_id] = tarefa
        
        return tarefa_id
    
    def aguardar_tarefa(self, tarefa_id: int) -> Optional[Any]:
        """Aguarda conclusão de uma tarefa"""
        while tarefa_id in self.tarefas_ativas:
            tarefa = self.tarefas_ativas[tarefa_id]
            if tarefa.estado == EstadoTarefa.CONCLUIDO:
                return tarefa.resultado
            elif tarefa.estado == EstadoTarefa.BLOQUEADA:
                raise tarefa.erro
            time.sleep(0.01)
        return None
    
    def status_tarefas(self) -> List[Dict]:
        """Retorna status de todas as tarefas"""
        return [
            {
                "id": t.id,
                "estado": t.estado.value,
                "erro": str(t.erro) if t.erro else None
            }
            for t in self.tarefas_ativas.values()
        ]

class MaquinaVirtualBREnhanced:
    """Máquina Virtual BR Enhanced com Green Threads e Canais CSP"""
    def __init__(self, bytecode: Dict[str, Any]):
        self.constantes = bytecode.get("constantes", [])
        self.instrucoes = bytecode.get("instrucoes", [])
        self.funcoes = bytecode.get("funcoes", {})
        self.testes = bytecode.get("testes", [])
        
        self.pilha = []
        self.escopos = [{}]
        self.banco = []
        
        # Enhanced features
        self.scheduler = SchedulerGreenThreads(num_threads_sistema=4)
        self.canais: Dict[str, CanalCSP] = {}
        self.scheduler.iniciar()
        
        self.temp_lista = []
        self.temp_registro = {}
        self.temp_agente = {}
        
        # Sandbox de segurança
        self.permissoes_ativas = set()
        self.carregar_permissoes()
    
    def carregar_permissoes(self):
        """Carrega permissões do arquivo projeto.br"""
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
                self.permissoes_ativas = set(perms)
    
    def verificar_permissao(self, permissao: str) -> bool:
        """Verifica se uma permissão está ativa"""
        return permissao in self.permissoes_ativas
    
    def criar_canal(self, nome: str, capacidade: int = 0) -> CanalCSP:
        """Cria um novo canal CSP"""
        canal = CanalCSP(capacidade)
        self.canais[nome] = canal
        return canal
    
    def obter_var(self, nome: str) -> Any:
        """Obtém valor de variável"""
        for escopo in reversed(self.escopos):
            if nome in escopo:
                return escopo[nome]
        return None
    
    def definir_var(self, nome: str, valor: Any):
        """Define valor de variável"""
        self.escopos[-1][nome] = valor
    
    def executar(self):
        """Executa bytecode"""
        try:
            self.scheduler.iniciar()
            self.executar_instrucoes(self.instrucoes)
        finally:
            self.scheduler.parar()
    
    def executar_instrucoes(self, instrucoes: List):
        """Executa lista de instruções"""
        from maquina_virtual import OPCODE_MAP, REVERSE_OPCODE_MAP, ReturnException
        
        idx = 0
        total = len(instrucoes)
        
        while idx < total:
            inst = instrucoes[idx]
            op = inst[0]
            args = inst[1:]
            idx += 1
            
            # Operações básicas (delegar para VM original)
            if op == "EMPILHAR_CONST":
                c_idx = args[0]
                self.pilha.append(self.constantes[c_idx])
            
            elif op == "DEFINIR_VAR":
                var_nome = args[0]
                val = self.pilha.pop()
                self.definir_var(var_nome, val)
            
            elif op == "OBTER_VAR":
                var_nome = args[0]
                val = self.obter_var(var_nome)
                self.pilha.append(val)
            
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
            
            elif op == "MAIOR_QUE":
                b = self.pilha.pop()
                a = self.pilha.pop()
                if isinstance(a, (int, float)) and isinstance(b, (int, float)):
                    self.pilha.append(a > b)
                else:
                    self.pilha.append(str(a) > str(b))
            
            elif op == "MENOR_QUE":
                b = self.pilha.pop()
                a = self.pilha.pop()
                self.pilha.append(a < b)
            
            elif op == "RETORNAR":
                val = self.pilha.pop()
                raise ReturnException(val)
            
            elif op == "ESPERE_IGUAL":
                esperado = self.pilha.pop()
                obtido = self.pilha.pop()
                if obtido != esperado:
                    raise AssertionError(f"Esperava '{esperado}', mas obteve '{obtido}'")
            
            # Enhanced: Operações de canais CSP
            elif op == "CRIAR_CANAL":
                nome_canal = args[0]
                capacidade = args[1] if len(args) > 1 else 0
                canal = self.criar_canal(nome_canal, capacidade)
                self.pilha.append(canal)
            
            elif op == "ESCREVER_CANAL":
                nome_canal = args[0]
                valor = self.pilha.pop()
                if nome_canal in self.canais:
                    sucesso = self.canais[nome_canal].escrever(valor)
                    self.pilha.append(sucesso)
                else:
                    self.pilha.append(False)
            
            elif op == "LER_CANAL":
                nome_canal = args[0]
                if nome_canal in self.canais:
                    valor = self.canais[nome_canal].ler()
                    self.pilha.append(valor)
                else:
                    self.pilha.append(None)
            
            elif op == "FECHAR_CANAL":
                nome_canal = args[0]
                if nome_canal in self.canais:
                    self.canais[nome_canal].fechar()
            
            # Enhanced: Operações de green threads
            elif op == "CRIAR_TAREFA":
                instrucoes_tarefa = args[0]
                tarefa_id = self.scheduler.criar_tarefa(instrucoes_tarefa, self)
                self.pilha.append(tarefa_id)
            
            elif op == "AGUARDAR_TAREFA":
                tarefa_id = args[0]
                resultado = self.scheduler.aguardar_tarefa(tarefa_id)
                self.pilha.append(resultado)
            
            elif op == "STATUS_TAREFAS":
                status = self.scheduler.status_tarefas()
                self.pilha.append(status)
            
            # Operações existentes (delegar)
            elif op == "CHAMAR_METODO":
                func_nome = args[0]
                arg_count = args[1]
                func_args = []
                for _ in range(arg_count):
                    func_args.append(self.pilha.pop())
                func_args.reverse()
                
                if func_nome not in self.funcoes:
                    raise NameError(f"Função '{func_nome}' não declarada.")
                
                func_insts = self.funcoes[func_nome]
                self.escopos.append({})
                for arg in func_args:
                    self.pilha.append(arg)
                
                retorno = None
                try:
                    self.executar_instrucoes(func_insts)
                except ReturnException as rx:
                    retorno = rx.valor
                finally:
                    self.escopos.pop()
                
                self.pilha.append(retorno)
            
            elif op == "CHAMAR_SERVICO":
                servico = args[0]
                metodo = args[1]
                arg_count = args[2]
                func_args = []
                for _ in range(arg_count):
                    func_args.append(self.pilha.pop())
                func_args.reverse()
                
                res = self.chamar_servico_nativo(servico, metodo, func_args)
                comandos_sem_retorno = {
                    ("SISTEMA", "MOSTRAR"),
                    ("BANCO", "SALVAR"),
                    ("BANCO", "REMOVER"),
                }
                if (servico, metodo) not in comandos_sem_retorno:
                    self.pilha.append(res)
            
            # Outras operações existentes (simplificadas)
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
            
            elif op == "CONDICIONAL_SE":
                bloco_dict = args[0]
                cond_insts = bloco_dict["condicao"]
                se_insts = bloco_dict["se"]
                senao_insts = bloco_dict["senao"]
                
                self.executar_instrucoes(cond_insts)
                cond_res = self.pilha.pop()
                if cond_res:
                    self.executar_instrucoes(se_insts)
                else:
                    self.executar_instrucoes(senao_insts)
            
            elif op == "ENQUANTO_BLOCO":
                bloco_dict = args[0]
                cond_insts = bloco_dict["condicao"]
                corpo = bloco_dict["corpo"]
                
                while True:
                    self.executar_instrucoes(cond_insts)
                    cond_res = self.pilha.pop()
                    if not cond_res:
                        break
                    try:
                        self.executar_instrucoes(corpo)
                    except ReturnException as rx:
                        raise rx
            
            elif op == "PARA_CADA_BLOCO":
                block_dict = args[0]
                var_item = block_dict["var_item"]
                lista_insts = block_dict["lista_insts"]
                corpo = block_dict["corpo"]
                
                self.executar_instrucoes(lista_insts)
                lst = self.pilha.pop()
                
                if isinstance(lst, list):
                    for item in lst:
                        self.escopos.append({})
                        self.definir_var(var_item, item)
                        try:
                            self.executar_instrucoes(corpo)
                        except ReturnException as rx:
                            self.escopos.pop()
                            raise rx
                        finally:
                            self.escopos.pop()
            
            elif op == "PARALELO_EXECUTE":
                blocos = args[0]
                # Usar green threads em vez de threads nativas
                tarefa_ids = []
                for bloco in blocos:
                    tarefa_id = self.scheduler.criar_tarefa(bloco, self)
                    tarefa_ids.append(tarefa_id)
                
                # Aguardar todas as tarefas
                for tarefa_id in tarefa_ids:
                    self.scheduler.aguardar_tarefa(tarefa_id)
    
    def chamar_servico_nativo(self, servico: str, metodo: str, args: List[Any]) -> Any:
        """Chama serviço nativo com verificação de permissões"""
        # Verificar permissões para serviços sensíveis
        if servico == "ARQUIVOS" and not self.verificar_permissao("arquivos"):
            raise PermissionError("Permissão 'arquivos' não concedida no projeto.br")
        
        if servico == "WEB" and not self.verificar_permissao("rede"):
            raise PermissionError("Permissão 'rede' não concedida no projeto.br")
        
        # Delegar para implementação original
        from maquina_virtual import MaquinaVirtualBR
        vm_original = MaquinaVirtualBR({
            "constantes": self.constantes,
            "funcoes": self.funcoes
        })
        vm_original.pilha = self.pilha
        vm_original.escopos = self.escopos
        vm_original.banco = self.banco
        
        return vm_original.chamar_servico_nativo(servico, metodo, args)
