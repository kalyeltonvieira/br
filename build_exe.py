#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para criar executável br.exe usando PyInstaller
"""
import subprocess
import sys
import os

def criar_executavel():
    """Cria o executável br.exe usando PyInstaller"""
    print("Criando executável br.exe...")
    
    # Verificar se PyInstaller está instalado
    try:
        import PyInstaller
    except ImportError:
        print("Instalando PyInstaller...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "pyinstaller"])
    
    add_data_sep = ";" if os.name == "nt" else ":"

    # Comando PyInstaller
    comando = [
        "pyinstaller",
        "--onefile",
        "--name", "br",
        "--noupx",
        "--add-data", f"ferramentas{add_data_sep}ferramentas",
        "--hidden-import", "json",
        "--hidden-import", "base64",
        "--hidden-import", "csv",
        "--hidden-import", "datetime",
        "--hidden-import", "hashlib",
        "--hidden-import", "io",
        "--hidden-import", "platform",
        "--hidden-import", "re",
        "--hidden-import", "secrets",
        "--hidden-import", "sqlite3",
        "--hidden-import", "_sqlite3",
        "--hidden-import", "unicodedata",
        "--hidden-import", "urllib.parse",
        "--hidden-import", "urllib.request",
        "--hidden-import", "uuid",
        "--hidden-import", "pygame",
        "br.py"
    ]
    
    try:
        subprocess.check_call(comando)
        print("\nExecutável br.exe criado com sucesso!")
        print("Localização: dist/br.exe")
        return True
    except subprocess.CalledProcessError as e:
        print(f"\nErro ao criar executável: {e}")
        return False

if __name__ == "__main__":
    criar_executavel()
