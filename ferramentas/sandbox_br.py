# -*- coding: utf-8 -*-
"""
Sandbox e Permissões para Linguagem BR
Controla acesso a recursos do sistema baseado em manifesto
"""
import os
import sys
import json
from pathlib import Path
from typing import List, Set

class ErroPermissao(Exception):
    def __init__(self, recurso, acao):
        self.recurso = recurso
        self.acao = acao
        super().__init__(f"Permissão negada: {acao} em {recurso}")

class SandboxBR:
    def __init__(self, manifesto_path=None):
        self.permissoes: Set[str] = set()
        self.diretorios_permitidos: List[str] = []
        self.urls_permitidas: List[str] = []
        self.carregar_manifesto(manifesto_path)
    
    def carregar_manifesto(self, path=None):
        """Carrega permissões do arquivo .brm"""
        if path and Path(path).exists():
            with open(path, 'r', encoding='utf-8') as f:
                conteudo = f.read()
            self._parse_manifesto(conteudo)
            return
        
        # Procura .brm no diretório atual e superiores
        for p in [Path.cwd(), *Path.cwd().parents]:
            brm = p / "projeto.brm"
            if brm.exists():
                with open(brm, 'r', encoding='utf-8') as f:
                    self._parse_manifesto(f.read())
                return
    
    def _parse_manifesto(self, conteudo):
        """Parseia manifesto .brm"""
        linhas = conteudo.split('\n')
        em_permissoes = False
        
        for linha in linhas:
            linha = linha.strip()
            if linha == "permissoes":
                em_permissoes = True
                continue
            if linha == "fim" and em_permissoes:
                em_permissoes = False
                continue
            if em_permissoes and linha:
                self.permissoes.add(linha.lower())
    
    def verificar(self, recurso: str, acao: str = "acessar"):
        """Verifica se uma permissão existe"""
        if recurso.lower() not in self.permissoes:
            raise ErroPermissao(recurso, acao)
        return True
    
    def verificar_arquivo(self, caminho: str):
        """Verifica se pode acessar arquivo"""
        self.verificar("arquivos", "ler/escrever")
        caminho_abs = os.path.abspath(caminho)
        # Verifica se está dentro do diretório do projeto
        projeto_dir = os.path.abspath(".")
        if not caminho_abs.startswith(projeto_dir):
            raise ErroPermissao(caminho, "acessar fora do diretório do projeto")
        return True
    
    def verificar_rede(self, url: str = ""):
        """Verifica se pode acessar rede"""
        self.verificar("internet", "acessar")
        return True
    
    def verificar_banco(self):
        """Verifica se pode acessar banco de dados"""
        self.verificar("banco", "acessar")
        return True
    
    def verificar_ia(self):
        """Verifica se pode usar IA"""
        self.verificar("ia", "usar")
        return True
    
    def verificar_gui(self):
        """Verifica se pode criar interface gráfica"""
        self.verificar("interface", "criar")
        return True
    
    def listar_permissoes(self):
        """Lista permissões atuais"""
        print("Permissões do projeto:")
        for p in sorted(self.permissoes):
            print(f"  ✅ {p}")
        if not self.permissoes:
            print("  (nenhuma permissão declarada)")

def criar_manifesto_padrao():
    """Cria um manifesto .brm padrão"""
    manifesto = """nome "meu-projeto"
versao "1.0.0"

dependencias
    web "1.0"
    banco "1.0"
fim

permissoes
    internet
    arquivos
    banco
fim
"""
    with open("projeto.brm", "w", encoding="utf-8") as f:
        f.write(manifesto)
    print("[Sandbox] Manifesto criado: projeto.brm")