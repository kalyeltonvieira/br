#!/usr/bin/env python3
"""
Script de build para Linguagem BR v2.0.0
Compila o runtime C++ de qualquer diretório
"""

import os
import sys
import subprocess
import platform

def main():
    print("=" * 50)
    print("  Linguagem BR v2.0.0 - Build Script")
    print("=" * 50)
    print()

    # Diretórios
    base_dir = os.path.dirname(os.path.abspath(__file__))
    src_dir = os.path.join(base_dir, "runtime", "src")
    inc_dir = os.path.join(base_dir, "runtime", "include")
    out_dir = os.path.join(base_dir, "bin")
    output = os.path.join(out_dir, "br.exe" if platform.system() == "Windows" else "br")

    # Criar diretório de saída
    os.makedirs(out_dir, exist_ok=True)

    # Arquivos fonte
    sources = [
        os.path.join(src_dir, "main.cpp"),
        os.path.join(src_dir, "servico_arquivo.cpp"),
        os.path.join(src_dir, "servico_rede.cpp"),
        os.path.join(src_dir, "servico_gui.cpp")
    ]

    # Verificar se arquivos existem
    for src in sources:
        if not os.path.exists(src):
            print(f"ERRO: Arquivo não encontrado: {src}")
            return 1

    # Compilar
    objects = []
    for src in sources:
        obj_name = os.path.splitext(os.path.basename(src))[0] + ".o"
        obj_path = os.path.join(out_dir, obj_name)
        objects.append(obj_path)

        print(f"Compilando: {os.path.basename(src)}...")
        cmd = [
            "g++",
            "-std=c++17",
            "-O2",
            "-I" + inc_dir,
            "-c",
            src,
            "-o",
            obj_path
        ]

        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"ERRO ao compilar {src}:")
            print(result.stderr)
            return 1

    # Linkar
    print("Linkando executável...")
    cmd = ["g++"] + objects + ["-o", output]
    
    # Adicionar bibliotecas do Windows
    if platform.system() == "Windows":
        cmd.extend(["-lws2_32", "-lwsock32"])
    else:
        cmd.extend(["-lpthread"])

    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("ERRO ao linkar:")
        print(result.stderr)
        return 1

    print()
    print("=" * 50)
    print("  Build concluído com sucesso!")
    print("=" * 50)
    print()
    print(f"Executável: {output}")
    print()
    print("Para execututar:")
    print(f"  {output} exemplos/demo_funcionalidades_avancadas.br")
    print()

    return 0

if __name__ == "__main__":
    sys.exit(main())