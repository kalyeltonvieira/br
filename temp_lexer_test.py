import sys
sys.path.append(r'd:/br/ferramentas')
from lexer import LexerBR
code = """função teste
    a é 1
    b é 2
fim"""
lexer = LexerBR(code)
tokens = lexer.tokenizar()
for t in tokens:
    print(t)
