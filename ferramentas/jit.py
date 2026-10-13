# -*- coding: utf-8 -*-
"""
JIT experimental para trechos simples de bytecode BR.
"""
from typing import Any, Dict, List


class JITCompiladorBR:
    def __init__(self):
        self.cache: Dict[str, Any] = {}

    def chave(self, instrucoes: List) -> str:
        return repr(instrucoes)

    def pode_compilar(self, instrucoes: List) -> bool:
        ops_permitidas = {"EMPILHAR_CONST", "SOMAR", "SUBTRAIR", "MULTIPLICAR", "DIVIDIR", "RETORNAR"}
        return all(inst and inst[0] in ops_permitidas for inst in instrucoes)

    def compilar(self, bytecode: Dict[str, Any], instrucoes: List):
        if not self.pode_compilar(instrucoes):
            return None
        chave = self.chave(instrucoes)
        if chave in self.cache:
            return self.cache[chave]

        constantes = bytecode.get("constantes", [])

        def executavel():
            pilha = []
            for inst in instrucoes:
                op = inst[0]
                args = inst[1:]
                if op == "EMPILHAR_CONST":
                    pilha.append(constantes[args[0]])
                elif op == "SOMAR":
                    b, a = pilha.pop(), pilha.pop()
                    pilha.append(a + b)
                elif op == "SUBTRAIR":
                    b, a = pilha.pop(), pilha.pop()
                    pilha.append(a - b)
                elif op == "MULTIPLICAR":
                    b, a = pilha.pop(), pilha.pop()
                    pilha.append(a * b)
                elif op == "DIVIDIR":
                    b, a = pilha.pop(), pilha.pop()
                    pilha.append(a / b)
                elif op == "RETORNAR":
                    return pilha.pop() if pilha else None
            return pilha[-1] if pilha else None

        self.cache[chave] = executavel
        return executavel


jit_global = JITCompiladorBR()
