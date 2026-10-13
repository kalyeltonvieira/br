# -*- coding: utf-8 -*-
"""
Hot-reload para programas BR: reexecuta ao detectar alteracao no arquivo.
"""
import os
import subprocess
import sys
import time


def observar(caminho: str, intervalo: float = 0.5) -> int:
    if not os.path.exists(caminho):
        print(f"Arquivo nao encontrado: {caminho}")
        return 1

    ultimo_mtime = None
    print(f"Hot-reload BR observando {caminho}")
    while True:
        mtime = os.path.getmtime(caminho)
        if ultimo_mtime is None or mtime != ultimo_mtime:
            ultimo_mtime = mtime
            print("\n[hot-reload] executando...")
            resultado = subprocess.run([sys.executable, "br.py", caminho], cwd=os.path.dirname(os.path.dirname(__file__)))
            print(f"[hot-reload] saida: {resultado.returncode}")
        time.sleep(intervalo)


def main():
    if len(sys.argv) < 2:
        print("Uso: python ferramentas/hot_reload.py <arquivo.br>")
        return 1
    return observar(sys.argv[1])


if __name__ == "__main__":
    sys.exit(main())
