# -*- coding: utf-8 -*-
"""
Processador de F-strings para Linguagem BR
Converte F-strings em concatenação de strings no AST
"""
import json
from typing import List
from br_ast import *
from lexer import LexerBR, TipoToken

def processar_fstring(token_fstring) -> LiteralTexto:
    """
    Processa um token FSTRING e retorna um nó AST de concatenação
    Formato do token: "texto\x00[expressoes_json]"
    """
    from parser import ParserBR
    valor = token_fstring.valor
    partes = valor.split('\x00')
    texto_base = partes[0]
    
    if len(partes) < 2:
        return LiteralTexto(Posicao(0, 0), texto_base)
    
    expressoes = json.loads(partes[1])
    
    # Se não há expressões, retorna texto puro
    if not expressoes:
        return LiteralTexto(Posicao(0, 0), texto_base)
    
    # Reconstroi a expressão de concatenação
    resultado = None
    posicao_atual = 0
    
    for expr_info in expressoes:
        idx_expr = expr_info[1]
        expr_str = expr_info[2]
        
        # Adiciona texto antes da expressão
        if posicao_atual < len(texto_base):
            texto_antes = texto_base[posicao_atual:idx_expr]
            if texto_antes:
                if resultado is None:
                    resultado = LiteralTexto(Posicao(0, 0), texto_antes)
                else:
                    # Concatena com o resultado anterior
                    resultado = Binario(
                        Posicao(0, 0),
                        resultado,
                        "+",
                        LiteralTexto(Posicao(0, 0), texto_antes)
                    )
        
        # Parseia a expressão dentro de {}
        try:
            lexer = LexerBR(expr_str)
            tokens = lexer.tokenizar()
            parser = ParserBR(tokens)
            ast_expr = parser.parse()
            
            if ast_expr and len(ast_expr) > 0:
                expr_node = ast_expr[0]
                if resultado is None:
                    resultado = expr_node
                else:
                    resultado = Binario(
                        Posicao(0, 0),
                        resultado,
                        "+",
                        expr_node
                    )
        except:
            # Se falhar o parse, trata como texto literal
            texto_expr = "{" + expr_str + "}"
            if resultado is None:
                resultado = LiteralTexto(Posicao(0, 0), texto_expr)
            else:
                resultado = Binario(
                    Posicao(0, 0),
                    resultado,
                    "+",
                    LiteralTexto(Posicao(0, 0), texto_expr)
                )
        
        posicao_atual = idx_expr + 1
    
    # Adiciona texto restante
    if posicao_atual < len(texto_base):
        texto_restante = texto_base[posicao_atual:]
        if texto_restante:
            if resultado is None:
                resultado = LiteralTexto(Posicao(0, 0), texto_restante)
            else:
                resultado = Binario(
                    Posicao(0, 0),
                    resultado,
                    "+",
                    LiteralTexto(Posicao(0, 0), texto_restante)
                )
    
    return resultado if resultado else LiteralTexto(Posicao(0, 0), texto_base)