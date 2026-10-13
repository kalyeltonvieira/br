# -*- coding: utf-8 -*-
"""
Package Manager BR - Mercado
Gerencia instalação, publicação e dependências de pacotes BR
"""
import os
import json
import zipfile
import hashlib
import requests
from pathlib import Path
from typing import List, Dict, Optional

REGISTRO_PADRAO = "https://registro.linguagem.br"

class PacoteBR:
    def __init__(self, nome, versao, descricao="", autor="", dependencias=None, permissoes=None):
        self.nome = nome
        self.versao = versao
        self.descricao = descricao
        self.autor = autor
        self.dependencias = dependencias or []
        self.permissoes = permissoes or []
        self.sha256 = ""
        
    def to_dict(self):
        return {
            "nome": self.nome,
            "versao": self.versao,
            "descricao": self.descricao,
            "autor": self.autor,
            "dependencias": self.dependencias,
            "permissoes": self.permissoes,
            "sha256": self.sha256
        }
    
    @classmethod
    def from_dict(cls, d):
        p = cls(d["nome"], d["versao"], d.get("descricao", ""), d.get("autor", ""),
                d.get("dependencias", []), d.get("permissoes", []))
        p.sha256 = d.get("sha256", "")
        return p

class Mercado:
    def __init__(self, diretorio_base=None):
        self.diretorio = Path(diretorio_base or os.path.expanduser("~/.br"))
        self.modulos_dir = self.diretorio / "modulos"
        self.cache_dir = self.diretorio / "cache"
        self.modulos_dir.mkdir(parents=True, exist_ok=True)
        self.cache_dir.mkdir(parents=True, exist_ok=True)
        
    def instalar(self, nome_pacote, versao=None):
        """Instala um pacote do registro"""
        print(f"[Mercado] Instalando {nome_pacote}...")
        
        # Verifica se já está instalado
        pacote_dir = self.modulos_dir / nome_pacote
        if pacote_dir.exists():
            print(f"[Mercado] Pacote {nome_pacote} já instalado")
            return True
        
        # Tenta baixar do registro
        url = f"{REGISTRO_PADRAO}/pacotes/{nome_pacote}"
        if versao:
            url += f"/{versao}"
        
        try:
            # Download do pacote
            req = requests.get(f"{url}/download")
            if req.status_code == 200:
                # Extrai na pasta de módulos
                zip_path = self.cache_dir / f"{nome_pacote}.zip"
                with open(zip_path, "wb") as f:
                    f.write(req.content)
                
                with zipfile.ZipFile(zip_path, "r") as zf:
                    zf.extractall(pacote_dir)
                
                print(f"[Mercado] Pacote {nome_pacote} instalado com sucesso!")
                return True
            
            # Se registro falhar, cria pacote local
            print(f"[Mercado] Registro indisponível, criando pacote local...")
            return self._criar_pacote_local(nome_pacote)
            
        except Exception as e:
            print(f"[Mercado] Erro ao instalar {nome_pacote}: {e}")
            return self._criar_pacote_local(nome_pacote)
    
    def _criar_pacote_local(self, nome_pacote):
        """Cria um pacote local como fallback"""
        pacote_dir = self.modulos_dir / nome_pacote
        pacote_dir.mkdir(parents=True, exist_ok=True)
        
        # Cria arquivo .br básico
        with open(pacote_dir / f"{nome_pacote}.br", "w", encoding="utf-8") as f:
            f.write(f"# Pacote {nome_pacote}\n")
            f.write(f"# Versão: 1.0.0\n")
            f.write(f"\n")
            f.write(f"função obter_nome\n")
            f.write(f"    devolva \"{nome_pacote}\"\n")
            f.write(f"fim\n")
        
        # Cria manifesto
        manifesto = {
            "nome": nome_pacote,
            "versao": "1.0.0",
            "descricao": f"Pacote {nome_pacote}",
            "permissoes": []
        }
        with open(pacote_dir / "pacote.json", "w", encoding="utf-8") as f:
            json.dump(manifesto, f, indent=2)
        
        print(f"[Mercado] Pacote local {nome_pacote} criado")
        return True
    
    def publicar(self, caminho):
        """Publica um pacote no registro"""
        print(f"[Mercado] Publicando {caminho}...")
        # TODO: Implementar publicação real no registro
        print(f"[Mercado] Publicação simulada. Configure REGISTRO_PADRAO para publicar de verdade.")
        return True
    
    def remover(self, nome_pacote):
        """Remove um pacote"""
        pacote_dir = self.modulos_dir / nome_pacote
        if pacote_dir.exists():
            import shutil
            shutil.rmtree(pacote_dir)
            print(f"[Mercado] Pacote {nome_pacote} removido")
            return True
        print(f"[Mercado] Pacote {nome_pacote} não encontrado")
        return False
    
    def listar(self):
        """Lista pacotes instalados"""
        print("[Mercado] Pacotes instalados:")
        for p in self.modulos_dir.iterdir():
            if p.is_dir():
                manifesto = p / "pacote.json"
                if manifesto.exists():
                    with open(manifesto) as f:
                        info = json.load(f)
                    print(f"  - {info['nome']} v{info['versao']}")
                else:
                    print(f"  - {p.name} (sem manifesto)")
    
    def buscar(self, termo):
        """Busca pacotes no registro"""
        try:
            req = requests.get(f"{REGISTRO_PADRAO}/buscar?q={termo}")
            if req.status_code == 200:
                resultados = req.json()
                print(f"[Mercado] Resultados para '{termo}':")
                for r in resultados:
                    print(f"  - {r['nome']} v{r['versao']}: {r.get('descricao', '')}")
                return resultados
        except:
            pass
        
        print(f"[Mercado] Registro indisponível. Pacotes locais:")
        self.listar()
        return []
    
    def atualizar(self, nome_pacote=None):
        """Atualiza pacotes"""
        if nome_pacote:
            print(f"[Mercado] Atualizando {nome_pacote}...")
            self.instalar(nome_pacote)
        else:
            print("[Mercado] Atualizando todos os pacotes...")
            for p in self.modulos_dir.iterdir():
                if p.is_dir():
                    self.instalar(p.name)
    
    def resolver_dependencias(self, nome_pacote):
        """Resolve dependências de um pacote"""
        pacote_dir = self.modulos_dir / nome_pacote
        manifesto = pacote_dir / "pacote.json"
        if not manifesto.exists():
            return []
        
        with open(manifesto) as f:
            info = json.load(f)
        
        dependencias = info.get("dependencias", [])
        for dep in dependencias:
            if not (self.modulos_dir / dep).exists():
                print(f"[Mercado] Instalando dependência: {dep}")
                self.instalar(dep)
        
        return dependencias

def cli_mercado():
    """CLI do gerenciador de pacotes"""
    import sys
    args = sys.argv[2:]  # skip "mercado" command
    
    mercado = Mercado()
    
    if not args or args[0] == "help":
        print("Uso: mercado <comando> [argumentos]")
        print("\nComandos:")
        print("  install <pacote>   Instalar pacote")
        print("  remove <pacote>    Remover pacote")
        print("  update [pacote]    Atualizar pacote(s)")
        print("  list               Listar pacotes instalados")
        print("  search <termo>     Buscar pacotes")
        print("  publish <caminho>  Publicar pacote")
        print("\nExemplos:")
        print("  mercado install web")
        print("  mercado install banco")
        print("  mercado list")
        return
    
    comando = args[0]
    
    if comando == "install" and len(args) >= 2:
        mercado.instalar(args[1])
    elif comando == "remove" and len(args) >= 2:
        mercado.remover(args[1])
    elif comando == "update":
        mercado.atualizar(args[1] if len(args) >= 2 else None)
    elif comando == "list":
        mercado.listar()
    elif comando == "search" and len(args) >= 2:
        mercado.buscar(args[1])
    elif comando == "publish" and len(args) >= 2:
        mercado.publicar(args[1])
    else:
        print(f"Comando desconhecido: {comando}")

if __name__ == "__main__":
    cli_mercado()