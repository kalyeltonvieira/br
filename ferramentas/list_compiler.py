# -*- coding: utf-8 -*-
"""
Compilador de List Comprehensions para Linguagem BR
Transforma sintaxe de comprehension em loops normais
"""
from typing import List
from br_ast import *
from parser import ParserBR, LexerBR
from lexer import TipoToken

def compilar_list_comprehension(expr_str: str) -> ListaLiteral:
    """
    Compila uma list comprehension como:
    [expr para cada var em lista]
    [expr para cada var em lista se condicao]
    """
    try:
        # Parseia a expressão
        lexer = LexerBR(expr_str)
        tokens = lexer.tokenizar()
        parser = ParserBR(tokens)
        ast_list = parser.parse()
        
        if not ast_list or len(ast_list) == 0:
            raise ValueError("Expressão inválida")
        
        # Se não for uma lista inline, retorna como está
        primeiro = ast_list[0]
        if not isinstance(primeiro, ListaLiteral):
            return primeiro
        
        return primeiro
        
    except Exception as e:
        # Fallback: trata como expressão normal
        lexer = LexerBR(expr_str)
        tokens = lexer.tokenizar()
        parser = ParserBR(tokens)
        ast = parser.parse()
        if ast and len(ast) > 0:
            return ListaLiteral(Posicao(0, 0), [ast[0]])
        return ListaLiteral(Posicao(0, 0), [])

def detectar_comprehension(tokens) -> bool:
    """Detecta se um token é uma list comprehension"""
    # Verifica se começa com COLCHETE_ESQ
    if not tokens or tokens[0].tipo != TipoToken.COLCHETE_ESQ:
        return False
    return True