# -*- coding: utf-8 -*-
"""
Sistema de Segurança e Sandbox para Linguagem BR
Implementa validação de permissões, interceptação de chamadas de sistema e assinatura digital
"""
import os
import re
import hashlib
import json
from pathlib import Path
from typing import Set, Dict, List, Optional, Tuple
from dataclasses import dataclass
from enum import Enum
from encoding_utils import ler_texto

class Permissao(Enum):
    ARQUIVOS = "arquivos"
    REDE = "rede"
    BANCO = "banco"
    SUBPROCESSO = "subprocesso"
    HARDWARE = "hardware"
    IA = "ia"
    CAMERA = "camera"
    MICROFONE = "microfone"
    LOCALIZACAO = "localizacao"

@dataclass
class PoliticaSeguranca:
    permissoes_concedidas: Set[Permissao]
    assinatura_digital: Optional[str] = None
    autor_verificado: bool = False
    pacote_assinado: bool = False

class ErroSeguranca(Exception):
    """Exceção lançada quando uma operação viola o sandbox"""
    def __init__(self, mensagem: str, permissao_necessaria: Permissao):
        self.mensagem = mensagem
        self.permissao_necessaria = permissao_necessaria
        super().__init__(f"[ERRO DE SEGURANÇA] {mensagem} (Permissão necessária: {permissao_necessaria.value})")

class SandboxBR:
    """Sandbox de segurança para execução de código BR"""
    
    def __init__(self, caminho_projeto: Optional[str] = None):
        self.politica = PoliticaSeguranca(set())
        self.caminho_projeto = caminho_projeto or self._encontrar_projeto()
        self.carregar_politica()
        self.chamadas_interceptadas: List[Dict] = []
    
    def _encontrar_projeto(self) -> Optional[str]:
        """Encontra o arquivo de manifesto do projeto"""
        cwd = Path.cwd().resolve()
        for pasta in (cwd, *cwd.parents):
            for nome in ("projeto.brm", "projeto.br"):
                candidato = pasta / nome
                if candidato.exists():
                    return str(candidato)
        return None
    
    def carregar_politica(self):
        """Carrega política de segurança do arquivo projeto.br"""
        if not self.caminho_projeto or not os.path.exists(self.caminho_projeto):
            return
        
        conteudo = ler_texto(self.caminho_projeto)
        
        # Extrair permissões
        match = re.search(r'permissoes\s+(.*?)\s+fim', conteudo, re.DOTALL)
        if match:
            perms_texto = [p.strip() for p in match.group(1).splitlines() if p.strip()]
            for perm_texto in perms_texto:
                try:
                    perm = Permissao(perm_texto)
                    self.politica.permissoes_concedidas.add(perm)
                except ValueError:
                    pass  # Permissão desconhecida, ignorar
        
        # Verificar assinatura digital
        match_assinatura = re.search(r'assinatura\s+"([^"]+)"', conteudo)
        if match_assinatura:
            self.politica.assinatura_digital = match_assinatura.group(1)
            self.politica.pacote_assinado = True
    
    def verificar_permissao(self, permissao: Permissao) -> bool:
        """Verifica se uma permissão está concedida"""
        return permissao in self.politica.permissoes_concedidas
    
    def exigir_permissao(self, permissao: Permissao, operacao: str = ""):
        """Exige uma permissão, lançando erro se não concedida"""
        if not self.verificar_permissao(permissao):
            mensagem = f"Operação '{operacao}' bloqueada pelo sandbox"
            raise ErroSeguranca(mensagem, permissao)
        
        # Registrar chamada permitida
        self.chamadas_interceptadas.append({
            "operacao": operacao,
            "permissao": permissao.value,
            "permitida": True
        })
    
    def resolver_caminho_arquivo(self, caminho: str) -> str:
        caminho_path = Path(caminho).expanduser()
        if caminho_path.is_absolute():
            return str(caminho_path.resolve())
        if self.caminho_projeto:
            base = Path(self.caminho_projeto).resolve().parent
        else:
            base = Path.cwd().resolve()
        return str((base / caminho_path).resolve())

    def validar_arquivo(self, caminho: str, modo: str = "leitura"):
        """Valida acesso a arquivo"""
        self.exigir_permissao(Permissao.ARQUIVOS, f"acesso_arquivo({caminho}, {modo})")

        caminho_absoluto = Path(self.resolver_caminho_arquivo(caminho))

        # Verificar se está dentro do diretório do projeto
        if self.caminho_projeto:
            dir_projeto = Path(self.caminho_projeto).resolve().parent
            try:
                caminho_absoluto.relative_to(dir_projeto)
            except ValueError:
                raise ErroSeguranca("Acesso fora do diretório do projeto", Permissao.ARQUIVOS)

        return str(caminho_absoluto)
    
    def validar_rede(self, url: str):
        """Valida acesso à rede"""
        self.exigir_permissao(Permissao.REDE, f"acesso_rede({url})")
        
        # Verificar se é HTTPS
        if not url.startswith("https://") and not url.startswith("http://"):
            raise ErroSeguranca("Apenas protocolos HTTP/HTTPS são permitidos", Permissao.REDE)
        
        # Verificar domínios bloqueados
        dominios_bloqueados = ["localhost", "127.0.0.1", "0.0.0.0"]
        for dominio in dominios_bloqueados:
            if dominio in url:
                raise ErroSeguranca(f"Domínio {dominio} bloqueado", Permissao.REDE)
    
    def validar_subprocesso(self, comando: str):
        """Valida execução de subprocesso"""
        self.exigir_permissao(Permissao.SUBPROCESSO, f"executar_subprocesso({comando})")
        
        # Comandos perigosos bloqueados
        comandos_perigosos = ["rm", "del", "format", "shutdown", "reboot"]
        for cmd_perigoso in comandos_perigosos:
            if cmd_perigoso in comando.lower():
                raise ErroSeguranca(f"Comando perigoso detectado: {cmd_perigoso}", Permissao.SUBPROCESSO)
    
    def validar_hardware(self, dispositivo: str):
        """Valida acesso a hardware"""
        self.exigir_permissao(Permissao.HARDWARE, f"acesso_hardware({dispositivo})")
    
    def validar_ia(self, operacao: str):
        """Valida operações de IA"""
        self.exigir_permissao(Permissao.IA, f"operacao_ia({operacao})")
    
    def relatorio_chamadas(self) -> str:
        """Gera relatório de chamadas interceptadas"""
        if not self.chamadas_interceptadas:
            return "Nenhuma chamada de sistema interceptada."
        
        relatorio = "Relatório de Chamadas de Sistema:\n"
        for chamada in self.chamadas_interceptadas:
            status = "✓ PERMITIDA" if chamada["permitida"] else "✗ BLOQUEADA"
            relatorio += f"  {status}: {chamada['operacao']} (Permissão: {chamada['permissao']})\n"
        
        return relatorio
    
    def politica_atual(self) -> str:
        """Retorna representação textual da política atual"""
        if not self.politica.permissoes_concedidas:
            return "Nenhuma permissão concedida (modo sandbox restrito)."
        
        perms = ", ".join([p.value for p in self.politica.permissoes_concedidas])
        return f"Permissões concedidas: {perms}"

class ValidadorAssinaturaDigital:
    """Validador de assinaturas digitais para pacotes BR"""
    
    @staticmethod
    def gerar_hash_arquivo(caminho: str) -> str:
        """Gera hash SHA-256 de um arquivo"""
        sha256_hash = hashlib.sha256()
        with open(caminho, "rb") as f:
            for byte_block in iter(lambda: f.read(4096), b""):
                sha256_hash.update(byte_block)
        return sha256_hash.hexdigest()
    
    @staticmethod
    def verificar_assinatura(caminho_pacote: str, assinatura_esperada: str) -> bool:
        """Verifica se a assinatura do pacote corresponde ao esperado"""
        hash_calculado = ValidadorAssinaturaDigital.gerar_hash_arquivo(caminho_pacote)
        return hash_calculado == assinatura_esperada
    
    @staticmethod
    def assinar_pacote(caminho_pacote: str, chave_privada: str) -> str:
        """Gera assinatura digital para um pacote (simplificado)"""
        hash_arquivo = ValidadorAssinaturaDigital.gerar_hash_arquivo(caminho_pacote)
        # Em produção, usaria criptografia assimétrica real
        assinatura = hashlib.sha256((hash_arquivo + chave_privada).encode()).hexdigest()
        return assinatura

class ValidadorSegurancaIA:
    """Validador de segurança para operações de IA"""
    
    PATTERNS_PERIGOSOS = [
        r"senha\s*=",
        r"password\s*=",
        r"token\s*=",
        r"api_key\s*=",
        r"chave\s*privada",
        r"ignore\s*previous",
        r"desconsidere\s*instruções",
    ]
    
    @staticmethod
    def validar_prompt(prompt: str) -> Tuple[bool, Optional[str]]:
        """Valida prompt de IA contra injeção"""
        prompt_lower = prompt.lower()
        
        for pattern in ValidadorSegurancaIA.PATTERNS_PERIGOSOS:
            if re.search(pattern, prompt_lower):
                return False, f"Pattern perigoso detectado no prompt: {pattern}"
        
        # Verificar comprimento excessivo
        if len(prompt) > 10000:
            return False, "Prompt excede comprimento máximo (10000 caracteres)"
        
        return True, None
    
    @staticmethod
    def sanitizar_prompt(prompt: str) -> str:
        """Sanitiza prompt removendo conteúdo potencialmente perigoso"""
        # Remover instruções de sistema
        prompt = re.sub(r"<\|.*?\|>", "", prompt, flags=re.DOTALL)
        prompt = re.sub(r"\[SYSTEM\].*?\[/SYSTEM\]", "", prompt, flags=re.DOTALL)
        
        return prompt.strip()

class GerenciadorSessaoSegura:
    """Gerenciador de sessões de execução segura"""
    
    def __init__(self):
        self.sandbox_ativo: Optional[SandboxBR] = None
        self.sessoes_ativas: Dict[str, SandboxBR] = {}
    
    def criar_sessao(self, id_sessao: str, caminho_projeto: Optional[str] = None) -> SandboxBR:
        """Cria uma nova sessão segura"""
        sandbox = SandboxBR(caminho_projeto)
        self.sessoes_ativas[id_sessao] = sandbox
        return sandbox
    
    def obter_sessao(self, id_sessao: str) -> Optional[SandboxBR]:
        """Obtém uma sessão existente"""
        return self.sessoes_ativas.get(id_sessao)
    
    def encerrar_sessao(self, id_sessao: str):
        """Encerra uma sessão e limpa recursos"""
        if id_sessao in self.sessoes_ativas:
            del self.sessoes_ativas[id_sessao]
    
    def listar_sessoes(self) -> List[str]:
        """Lista todas as sessões ativas"""
        return list(self.sessoes_ativas.keys())

# Singleton global
gerenciador_sessao = GerenciadorSessaoSegura()

def criar_sandbox(caminho_projeto: Optional[str] = None) -> SandboxBR:
    """Função auxiliar para criar um sandbox"""
    return SandboxBR(caminho_projeto)

def validar_operacao_arquivo(caminho: str, modo: str = "leitura"):
    """Valida operação de arquivo usando sandbox global"""
    sandbox = criar_sandbox()
    sandbox.validar_arquivo(caminho, modo)

def validar_operacao_rede(url: str):
    """Valida operação de rede usando sandbox global"""
    sandbox = criar_sandbox()
    sandbox.validar_rede(url)
