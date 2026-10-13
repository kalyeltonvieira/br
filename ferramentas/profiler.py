# -*- coding: utf-8 -*-
"""
Profiler simples para programas BR.
"""
import os
import sys
import time

from lexer import LexerBR
from parser import ParserBR
from semantic_analyzer import analisar_semantica
from optimizer import otimizar_codigo
from codegen import gerar_bytecode
from maquina_virtual import MaquinaVirtualBR


def profile_arquivo(caminho: str) -> int:
    inicio_total = time.perf_counter()
    with open(caminho, "r", encoding="utf-8") as f:
        codigo = f.read()

    t0 = time.perf_counter()
    tokens = LexerBR(codigo).tokenizar()
    t_lexer = time.perf_counter() - t0

    t0 = time.perf_counter()
    ast = ParserBR(tokens).parse()
    t_parser = time.perf_counter() - t0

    t0 = time.perf_counter()
    sucesso, erros, _avisos = analisar_semantica(ast)
    t_sem = time.perf_counter() - t0
    if not sucesso:
        for erro in erros:
            print(str(erro))
        return 1

    t0 = time.perf_counter()
    ast_otimizado, _relatorio = otimizar_codigo(ast)
    bytecode = gerar_bytecode(ast_otimizado)
    t_codegen = time.perf_counter() - t0

    t0 = time.perf_counter()
    vm = MaquinaVirtualBR(bytecode, diretorio_base=os.path.dirname(os.path.abspath(caminho)))
    vm.executar()
    t_exec = time.perf_counter() - t0

    total = time.perf_counter() - inicio_total
    print("\nPerfil BR")
    print(f"  tokens: {len(tokens)}")
    print(f"  instrucoes: {len(bytecode.get('instrucoes', []))}")
    print(f"  lexer: {t_lexer:.6f}s")
    print(f"  parser: {t_parser:.6f}s")
    print(f"  semantica: {t_sem:.6f}s")
    print(f"  codegen: {t_codegen:.6f}s")
    print(f"  execucao: {t_exec:.6f}s")
    print(f"  total: {total:.6f}s")
    return 0


def main():
    if len(sys.argv) < 2:
        print("Uso: python ferramentas/profiler.py <arquivo.br>")
        return 1
    return profile_arquivo(sys.argv[1])


if __name__ == "__main__":
    sys.exit(main())
