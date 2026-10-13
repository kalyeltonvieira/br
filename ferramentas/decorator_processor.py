# -*- coding: utf-8 -*-
"""
Processador de Decorators para Linguagem BR
Transforma decorators em wrappers de função
"""
from typing import List, Dict, Any
from br_ast import *
from parser import ParserBR, LexerBR
from lexer import TipoToken

class DecoradorBR:
    """Representa um decorator aplicado a uma função"""
    def __init__(self, nome: str, argumentos: List[NoAST] = None):
        self.nome = nome
        self.argumentos = argumentos or []
    
    def __repr__(self):
        return f"Decorador({self.nome}, args={len(self.argumentos)})"

def processar_decorators(declaracao_funcao: DeclaracaoFuncao, decorators: List[DecoradorBR]) -> DeclaracaoFuncao:
    """
    Aplica decorators a uma função, transformando-a em uma versão decorada
    """
    if not decorators:
        return declaracao_funcao
    
    # Por enquanto, apenas marca a função como decorada
    # A aplicação real dos decorators seria feita em tempo de execução
    declaracao_funcao.decorators = decorators
    return declaracao_funcao

def detectar_decorator(token) -> bool:
    """Detecta se um token é um decorator (começa com @)"""
    if not token:
        return False
    return token.valor.startswith('@') if hasattr(token, 'valor') else False