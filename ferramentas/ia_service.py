# -*- coding: utf-8 -*-
"""
Serviço de IA funcional para BR.
Suporta múltiplos provedores: OpenAI, Anthropic, Ollama (local).
"""
import os
import json
import requests
from pathlib import Path
from typing import Optional, Dict, Any, List
from datetime import datetime


class IAProvider:
    """Base para provedores de IA"""
    
    def __init__(self, api_key: Optional[str] = None):
        self.api_key = api_key or os.environ.get("BR_IA_API_KEY")
    
    def perguntar(self, prompt: str, contexto: Optional[str] = None) -> str:
        """Envia pergunta à IA e retorna resposta"""
        raise NotImplementedError
    
    def chat(self, mensagens: List[Dict[str, str]]) -> str:
        """Conversa com contexto"""
        raise NotImplementedError


class OpenAIProvider(IAProvider):
    """Provedor OpenAI (GPT-4, GPT-3.5)"""
    
    def __init__(self, api_key: Optional[str] = None, modelo: str = "gpt-3.5-turbo"):
        super().__init__(api_key)
        self.modelo = modelo
        self.base_url = os.environ.get("BR_OPENAI_BASE_URL", "https://api.openai.com/v1")
    
    def perguntar(self, prompt: str, contexto: Optional[str] = None) -> str:
        if not self.api_key:
            return self._resposta_fallback(prompt)
        
        messages = []
        if contexto:
            messages.append({"role": "system", "content": contexto})
        messages.append({"role": "user", "content": prompt})
        
        try:
            response = requests.post(
                f"{self.base_url}/chat/completions",
                headers={
                    "Authorization": f"Bearer {self.api_key}",
                    "Content-Type": "application/json"
                },
                json={
                    "model": self.modelo,
                    "messages": messages,
                    "temperature": 0.7
                },
                timeout=30
            )
            response.raise_for_status()
            return response.json()["choices"][0]["message"]["content"]
        except Exception as e:
            return f"[Erro OpenAI] {str(e)}"
    
    def chat(self, mensagens: List[Dict[str, str]]) -> str:
        if not self.api_key:
            return self._resposta_fallback(str(mensagens))
        
        try:
            response = requests.post(
                f"{self.base_url}/chat/completions",
                headers={
                    "Authorization": f"Bearer {self.api_key}",
                    "Content-Type": "application/json"
                },
                json={
                    "model": self.modelo,
                    "messages": mensagens,
                    "temperature": 0.7
                },
                timeout=30
            )
            response.raise_for_status()
            return response.json()["choices"][0]["message"]["content"]
        except Exception as e:
            return f"[Erro OpenAI] {str(e)}"
    
    def _resposta_fallback(self, prompt: str) -> str:
        """Resposta simulada quando não há API key"""
        prompt_l = prompt.lower()
        if "buracos negros" in prompt_l or "buraco negro" in prompt_l:
            return "Um buraco negro é uma região do espaço-tempo de onde nada, nem mesmo a luz, pode escapar devido ao seu fortíssimo campo gravitacional."
        elif "quem descobriu o brasil" in prompt_l:
            return "O Brasil foi descoberto por Pedro Álvares Cabral em 22 de abril de 1500."
        elif "computação quântica" in prompt_l:
            return "Computação quântica é a área da ciência da computação baseada nos princípios da física quântica (superposição e emaranhamento) para resolver problemas complexos muito mais rápido."
        else:
            return f"[IA BR - Modo Simulado] Configure BR_IA_API_KEY para usar IA real. Pergunta: '{prompt}'"


class AnthropicProvider(IAProvider):
    """Provedor Anthropic (Claude)"""
    
    def __init__(self, api_key: Optional[str] = None, modelo: str = "claude-3-haiku-20240307"):
        super().__init__(api_key)
        self.modelo = modelo
        self.base_url = "https://api.anthropic.com/v1"
    
    def perguntar(self, prompt: str, contexto: Optional[str] = None) -> str:
        if not self.api_key:
            return self._resposta_fallback(prompt)
        
        try:
            messages = [{"role": "user", "content": prompt}]
            if contexto:
                messages.insert(0, {"role": "user", "content": f"Contexto: {contexto}"})
            
            response = requests.post(
                f"{self.base_url}/messages",
                headers={
                    "x-api-key": self.api_key,
                    "Content-Type": "application/json",
                    "anthropic-version": "2023-06-01"
                },
                json={
                    "model": self.modelo,
                    "max_tokens": 1024,
                    "messages": messages
                },
                timeout=30
            )
            response.raise_for_status()
            return response.json()["content"][0]["text"]
        except Exception as e:
            return f"[Erro Anthropic] {str(e)}"
    
    def chat(self, mensagens: List[Dict[str, str]]) -> str:
        if not self.api_key:
            return self._resposta_fallback(str(mensagens))
        
        try:
            # Converter formato OpenAI para Anthropic
            anthropic_messages = []
            for msg in mensagens:
                role = "user" if msg["role"] in ["user", "system"] else "assistant"
                anthropic_messages.append({"role": role, "content": msg["content"]})
            
            response = requests.post(
                f"{self.base_url}/messages",
                headers={
                    "x-api-key": self.api_key,
                    "Content-Type": "application/json",
                    "anthropic-version": "2023-06-01"
                },
                json={
                    "model": self.modelo,
                    "max_tokens": 1024,
                    "messages": anthropic_messages
                },
                timeout=30
            )
            response.raise_for_status()
            return response.json()["content"][0]["text"]
        except Exception as e:
            return f"[Erro Anthropic] {str(e)}"
    
    def _resposta_fallback(self, prompt: str) -> str:
        return f"[IA BR - Modo Simulado] Configure BR_IA_API_KEY para usar Anthropic. Pergunta: '{prompt}'"


class OllamaProvider(IAProvider):
    """Provedor Ollama (LLMs locais gratuitos)"""
    
    def __init__(self, base_url: str = "http://localhost:11434", modelo: str = "llama2"):
        self.base_url = base_url
        self.modelo = modelo
    
    def perguntar(self, prompt: str, contexto: Optional[str] = None) -> str:
        try:
            full_prompt = prompt
            if contexto:
                full_prompt = f"Contexto: {contexto}\n\nPergunta: {prompt}"
            
            response = requests.post(
                f"{self.base_url}/api/generate",
                json={
                    "model": self.modelo,
                    "prompt": full_prompt,
                    "stream": False
                },
                timeout=60
            )
            response.raise_for_status()
            return response.json()["response"]
        except Exception as e:
            return f"[Erro Ollama] Certifique-se que Ollama está rodando em {self.base_url}. Erro: {str(e)}"
    
    def chat(self, mensagens: List[Dict[str, str]]) -> str:
        try:
            # Construir prompt de conversa
            prompt_parts = []
            for msg in mensagens:
                role = msg["role"].upper()
                prompt_parts.append(f"{role}: {msg['content']}")
            full_prompt = "\n\n".join(prompt_parts)
            
            response = requests.post(
                f"{self.base_url}/api/generate",
                json={
                    "model": self.modelo,
                    "prompt": full_prompt,
                    "stream": False
                },
                timeout=60
            )
            response.raise_for_status()
            return response.json()["response"]
        except Exception as e:
            return f"[Erro Ollama] {str(e)}"


class AgenteBR:
    """Agente de IA persistente com contexto"""
    
    def __init__(self, nome: str, especialidade: str = "geral", provider: Optional[IAProvider] = None):
        self.nome = nome
        self.especialidade = especialidade
        self.provider = provider or self._criar_provider_default()
        self.historico: List[Dict[str, str]] = []
        self.metadata: Dict[str, Any] = {
            "criado_em": datetime.now().isoformat(),
            "especialidade": especialidade
        }
        self._carregar_estado()
    
    def _criar_provider_default(self) -> IAProvider:
        """Cria provider baseado em variáveis de ambiente"""
        provider_type = os.environ.get("BR_IA_PROVIDER", "openai").lower()
        
        if provider_type == "anthropic":
            return AnthropicProvider()
        elif provider_type == "ollama":
            return OllamaProvider(
                base_url=os.environ.get("BR_OLLAMA_URL", "http://localhost:11434"),
                modelo=os.environ.get("BR_OLLAMA_MODEL", "llama2")
            )
        else:
            return OpenAIProvider()
    
    def _carregar_estado(self):
        """Carrega estado persistente do agente"""
        from agentes import carregar_agente
        estado = carregar_agente(self.nome)
        if estado:
            self.historico = estado.get("historico", [])
            self.metadata.update(estado.get("metadata", {}))
    
    def _salvar_estado(self):
        """Salva estado persistente do agente"""
        from agentes import salvar_agente
        estado = {
            "nome": self.nome,
            "especialidade": self.especialidade,
            "historico": self.historico[-50:],  # Manter últimas 50 mensagens
            "metadata": self.metadata
        }
        salvar_agente(self.nome, estado)
    
    def perguntar(self, prompt: str) -> str:
        """Faz pergunta ao agente com contexto"""
        # Adicionar contexto de especialidade
        contexto_sistema = f"Você é um agente de IA com especialidade em {self.especialidade}. Responda em português brasileiro."
        
        # Construir histórico
        mensagens = [{"role": "system", "content": contexto_sistema}]
        mensagens.extend(self.historico[-10:])  # Últimas 10 mensagens
        mensagens.append({"role": "user", "content": prompt})
        
        # Obter resposta
        resposta = self.provider.chat(mensagens)
        
        # Salvar no histórico
        self.historico.append({"role": "user", "content": prompt})
        self.historico.append({"role": "assistant", "content": resposta})
        
        # Persistir estado
        self._salvar_estado()
        
        return resposta
    
    def limpar_historico(self):
        """Limpa histórico de conversa"""
        self.historico = []
        self._salvar_estado()


class IAServiceBR:
    """Serviço principal de IA para BR"""
    
    def __init__(self):
        self.provider = self._criar_provider()
        self.agentes: Dict[str, AgenteBR] = {}
    
    def _criar_provider(self) -> IAProvider:
        """Cria provider baseado em configuração"""
        provider_type = os.environ.get("BR_IA_PROVIDER", "openai").lower()
        
        if provider_type == "anthropic":
            return AnthropicProvider()
        elif provider_type == "ollama":
            return OllamaProvider()
        else:
            return OpenAIProvider()
    
    def perguntar(self, prompt: str, contexto: Optional[str] = None) -> str:
        """Pergunta simples à IA sem contexto persistente"""
        return self.provider.perguntar(prompt, contexto)
    
    def criar_agente(self, nome: str, especialidade: str = "geral") -> AgenteBR:
        """Cria ou recupera agente persistente"""
        if nome not in self.agentes:
            self.agentes[nome] = AgenteBR(nome, especialidade, self.provider)
        return self.agentes[nome]
    
    def obter_agente(self, nome: str) -> Optional[AgenteBR]:
        """Obtém agente existente"""
        return self.agentes.get(nome)


# Instância global do serviço
_ia_service: Optional[IAServiceBR] = None


def get_ia_service() -> IAServiceBR:
    """Obtém instância singleton do serviço de IA"""
    global _ia_service
    if _ia_service is None:
        _ia_service = IAServiceBR()
    return _ia_service
