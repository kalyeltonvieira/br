# -*- coding: utf-8 -*-
import re
import json
from enum import Enum

class TipoToken(Enum):
    FUNCAO = 1; TAREFA = 2; AGUARDE = 3; CONTE = 4; ATE = 5
    METODO = 6; HERDA = 7; CONTRATO = 8
    SE = 10; SENAO = 11; ENTAO = 12; ENQUANTO = 13
    PARA = 14; CADA = 15; EM = 16
    TENTE = 17; CAPTURE = 18
    EXECUTE = 19; PARALELO = 20
    DEVOLVA = 21; MOSTRE = 22; ESPERE = 23
    SALVE = 24; REMOVA = 25; BUSQUE = 26; ACESSE = 27
    CONSTANTE = 28; VARIAVEL = 29
    IMPORT = 30; IMPORTE = 31; COMO = 32
    CLASSE = 33; DE = 34; ESCOLHA = 35; CASO = 36; PADRAO = 37
    REPITA = 38; VEZES = 39
    FALHOU = 40; NAO = 41; ISTO = 42; ONDE = 43
    SITE = 44; ROTA = 45; PORTA = 46
    JANELA = 47; CAMPO = 48; BOTAO = 49
    QUANDO = 50; CLICAR = 51
    TITULO = 52; LARGURA = 53; ALTURA = 54; ABRA = 55
    EXPLICACAO = 60; OBJETIVO = 61; TESTE = 62; FIM = 63
    LISTA = 70; REGISTRO = 71; TEXTO = 72; INTEIRO = 73; DECIMAL = 74; BOOLEANO = 75
    IGUAL = 80; IGUAL_COMPARACAO = 81; DIFERENTE = 82
    MAIOR_QUE = 83; MAIOR_OU_IGUAL = 84; MENOR_QUE = 85; MENOR_OU_IGUAL = 86
    SOMA = 87; SUBTRACAO = 88; MULTIPLICACAO = 89; DIVISAO = 90
    DOIS_PONTOS = 100; PONTO = 101; VIRGULA = 102
    PARENTESE_ESQ = 103; PARENTESE_DIR = 104
    COLCHETE_ESQ = 105; COLCHETE_DIR = 106
    CHAVE_ESQ = 107; CHAVE_DIR = 108
    LITERAL_TEXTO = 200; LITERAL_INTEIRO = 201; LITERAL_DECIMAL = 202
    LITERAL_VERDADEIRO = 203; LITERAL_FALSO = 204; LITERAL_NULO = 205
    FSTRING = 206
    IDENTIFICADOR = 300; E = 301; RECEBENDO = 302; PARE = 303
    ARROBA = 500; SETA = 501; YIELD = 502; COM = 503; CORRESPONDA = 504
    DOCSTRING = 505; ASSINCRONO = 506
    INDENTAR = 600; DESINDENTAR = 601; NOVA_LINHA = 602; EOF = 999


def _tt(nome: str) -> TipoToken:
    """Converte string nome para TipoToken enum"""
    return TipoToken[nome]


class Token:
    def __init__(self, tipo: TipoToken, valor: str, linha: int, coluna: int):
        self.tipo = tipo
        self.valor = valor
        self.linha = linha
        self.coluna = coluna

    def __repr__(self):
        return f"Token({self.tipo.name}, '{self.valor}', {self.linha}:{self.coluna})"


class LexerBR:
    def __init__(self, codigo):
        self.codigo = codigo.lstrip("\ufeff")
        self.pos = 0; self.linha = 0; self.col = 0
        self.tokens = []; self.niveis = [0]; self.inicio = True
        self.MAPA = {
            "tarefa": _tt("TAREFA"), "aguarde": _tt("AGUARDE"),
            "conte": _tt("CONTE"), "ate": _tt("ATE"), "até": _tt("ATE"),
            "metodo": _tt("METODO"), "método": _tt("METODO"),
            "herda": _tt("HERDA"), "contrato": _tt("CONTRATO"),
            "falhou": _tt("FALHOU"), "nao": _tt("NAO"), "não": _tt("NAO"),
            "isto": _tt("ISTO"), "self": _tt("ISTO"),
            "onde": _tt("ONDE"), "site": _tt("SITE"), "rota": _tt("ROTA"),
            "porta": _tt("PORTA"),
            "janela": _tt("JANELA"), "campo": _tt("CAMPO"),
            "botao": _tt("BOTAO"), "botão": _tt("BOTAO"),
            "quando": _tt("QUANDO"), "clicar": _tt("CLICAR"),
            "titulo": _tt("TITULO"), "título": _tt("TITULO"),
            "largura": _tt("LARGURA"), "altura": _tt("ALTURA"),
            "abra": _tt("ABRA"),
            "função": _tt("FUNCAO"), "funcao": _tt("FUNCAO"),
            "se": _tt("SE"), "senão": _tt("SENAO"), "então": _tt("ENTAO"),
            "enquanto": _tt("ENQUANTO"),
            "para": _tt("PARA"), "cada": _tt("CADA"), "em": _tt("EM"),
            "tente": _tt("TENTE"), "capture": _tt("CAPTURE"),
            "execute": _tt("EXECUTE"), "paralelo": _tt("PARALELO"),
            "devolva": _tt("DEVOLVA"), "mostre": _tt("MOSTRE"),
            "espere": _tt("ESPERE"),
            "salve": _tt("SALVE"), "remova": _tt("REMOVA"),
            "busque": _tt("BUSQUE"), "acesse": _tt("ACESSE"),
            "constante": _tt("CONSTANTE"), "variavel": _tt("VARIAVEL"),
            "importar": _tt("IMPORT"), "import": _tt("IMPORT"),
            "de": _tt("DE"), "importe": _tt("IMPORTE"),
            "como": _tt("COMO"), "classe": _tt("CLASSE"),
            "escolha": _tt("ESCOLHA"), "caso": _tt("CASO"),
            "padrao": _tt("PADRAO"), "padrão": _tt("PADRAO"),
            "repita": _tt("REPITA"), "vezes": _tt("VEZES"),
            "pare": _tt("PARE"),
            "explicacao": _tt("EXPLICACAO"), "objetivo": _tt("OBJETIVO"),
            "teste": _tt("TESTE"), "fim": _tt("FIM"),
            "lista": _tt("LISTA"), "registro": _tt("REGISTRO"),
            "texto": _tt("TEXTO"), "inteiro": _tt("INTEIRO"),
            "decimal": _tt("DECIMAL"), "booleano": _tt("BOOLEANO"),
            "verdadeiro": _tt("LITERAL_VERDADEIRO"),
            "falso": _tt("LITERAL_FALSO"),
            "nulo": _tt("LITERAL_NULO"),
            "é": _tt("IGUAL"), "recebendo": _tt("RECEBENDO"),
            "e": _tt("E"),
            "yield": _tt("YIELD"), "com": _tt("COM"),
            "corresponda": _tt("CORRESPONDA"), "corresponder": _tt("CORRESPONDA"),
            "assincrono": _tt("ASSINCRONO"), "assíncrono": _tt("ASSINCRONO"),
            "async": _tt("ASSINCRONO"),
        }
        self.SIMPLES = {
            ':': _tt("DOIS_PONTOS"), '.': _tt("PONTO"), ',': _tt("VIRGULA"),
            '(': _tt("PARENTESE_ESQ"), ')': _tt("PARENTESE_DIR"),
            '[': _tt("COLCHETE_ESQ"), ']': _tt("COLCHETE_DIR"),
            '{': _tt("CHAVE_ESQ"), '}': _tt("CHAVE_DIR"),
            '+': _tt("SOMA"), '-': _tt("SUBTRACAO"),
            '*': _tt("MULTIPLICACAO"), '/': _tt("DIVISAO"),
        }
        
    def char(self): return self.codigo[self.pos] if self.pos < len(self.codigo) else None
    def next_char(self): return self.codigo[self.pos+1] if self.pos+1 < len(self.codigo) else None
    
    def avancar(self):
        c = self.char()
        if c: self.pos += 1; self.col += 1
        if c == '\n': self.linha += 1; self.col = 0
        return c
    
    def add(self, tipo: TipoToken, valor: str):
        self.tokens.append(Token(tipo, valor, self.linha+1, self.col))
    
    def tokenizar(self):
        while self.pos < len(self.codigo):
            if self.inicio:
                esp = 0
                while self.char() == ' ': esp += 1; self.avancar()
                c_inicio = self.char()
                eh_comentario_barra = c_inicio == '/' and self.next_char() == '/'
                if c_inicio not in ('\n', None, '#') and not eh_comentario_barra:
                    nivel = self.niveis[-1]
                    if esp > nivel: self.niveis.append(esp); self.add(_tt("INDENTAR"), "INDENTAR")
                    elif esp < nivel:
                        while self.niveis and self.niveis[-1] > esp:
                            self.niveis.pop(); self.add(_tt("DESINDENTAR"), "DESINDENTAR")
                self.inicio = False
            
            # pular espaços no meio
            while self.char() and self.char().isspace() and self.char() != '\n': self.avancar()
            
            c = self.char()
            if c is None: break
            if c == '#':
                while self.char() and self.char() != '\n':
                    self.avancar()
                continue
            if c == '/':
                if self.next_char() == '/':
                    while self.char() and self.char() != '\n':
                        self.avancar()
                    continue
                else:
                    self.avancar()
                    self.add(_tt("DIVISAO"), '/')
                    continue
            if c == '\n':
                self.add(_tt("NOVA_LINHA"), "\n")
                self.avancar()
                self.inicio = True
                continue
            if c in ('"', "'"):
                aspas = self.avancar(); val = ""
                while self.char() and self.char() != aspas:
                    ch = self.avancar()
                    if ch == '\\' and self.char():
                        ch2 = self.avancar()
                        val += {'n':'\n','t':'\t','\\':'\\'}.get(ch2, ch2)
                    else: val += ch
                self.avancar(); self.add(_tt("LITERAL_TEXTO"), val); continue
            if c == 'f' and (self.next_char() == '"' or self.next_char() == "'"):
                self.avancar()  # consume 'f'
                aspas = self.avancar()
                val = ""
                expressoes = []
                idx_expr = 0
                while self.char() and self.char() != aspas:
                    if self.char() == '{':
                        self.avancar()
                        inicio_expr = self.pos
                        profundidade = 1
                        while self.char() and profundidade > 0:
                            if self.char() == '{': profundidade += 1
                            elif self.char() == '}': profundidade -= 1
                            if profundidade > 0: self.avancar()
                        if self.char() == '}': self.avancar()
                        expr_str = self.codigo[inicio_expr:self.pos-1]
                        expressoes.append(("EXPR", idx_expr, expr_str))
                        idx_expr += 1
                    else:
                        ch = self.avancar()
                        if ch == '\\' and self.char():
                            ch2 = self.avancar()
                            val += {'n':'\n','t':'\t','\\':'\\'}.get(ch2, ch2)
                        else:
                            val += ch
                self.avancar()
                token_val = val + "\x00" + json.dumps(expressoes)
                self.add(_tt("FSTRING"), token_val)
                continue
            if c.isdigit():
                start = self.pos
                while self.char() and self.char().isdigit(): self.avancar()
                if self.char() == '.':
                    self.avancar()
                    while self.char() and self.char().isdigit(): self.avancar()
                    self.add(_tt("LITERAL_DECIMAL"), self.codigo[start:self.pos])
                else: self.add(_tt("LITERAL_INTEIRO"), self.codigo[start:self.pos])
                continue
            
            # operadores multi-char
            if c == '<':
                self.avancar()
                if self.char() == '=': self.avancar(); self.add(_tt("MENOR_OU_IGUAL"), '<=')
                else: self.add(_tt("MENOR_QUE"), '<')
                continue
            if c == '>':
                self.avancar()
                if self.char() == '=': self.avancar(); self.add(_tt("MAIOR_OU_IGUAL"), '>=')
                else: self.add(_tt("MAIOR_QUE"), '>')
                continue
            if c == '=':
                self.avancar()
                if self.char() == '=': self.avancar(); self.add(_tt("IGUAL_COMPARACAO"), '==')
                else: self.add(_tt("IGUAL"), '=')
                continue
            if c == '!':
                self.avancar()
                if self.char() == '=': self.avancar(); self.add(_tt("DIFERENTE"), '!=')
                else: raise ValueError(f"Caractere invalido '!' linha {self.linha+1}")
                continue
            
            if c == '@':
                self.avancar(); self.add(_tt("ARROBA"), "@"); continue
            if c == '-':
                if self.next_char() == '>':
                    self.avancar(); self.avancar(); self.add(_tt("SETA"), "->"); continue
                else:
                    self.avancar(); self.add(_tt("SUBTRACAO"), "-"); continue
            if c in self.SIMPLES:
                self.avancar(); self.add(self.SIMPLES[c], c); continue
            
            if c.isalpha() or c == '_':
                start = self.pos
                while self.char() and (self.char().isalnum() or self.char() == '_'): self.avancar()
                palavra = self.codigo[start:self.pos]
                tipo = self.MAPA.get(palavra, _tt("IDENTIFICADOR"))
                self.add(tipo, palavra)
                continue
            
            if c == '`':
                if self.next_char() == '`':
                    self.avancar(); self.avancar(); self.avancar()
                    while self.char() and self.char() != '\n': self.avancar()
                    if self.char() == '\n': self.avancar()
                    while self.pos < len(self.codigo):
                        if self.char() == '`' and self.next_char() == '`':
                            p2 = self.pos+1
                            if p2+1 < len(self.codigo) and self.codigo[p2+1] == '`':
                                self.avancar(); self.avancar(); self.avancar()
                                while self.char() and self.char() != '\n': self.avancar()
                                break
                        if self.char() == '\n': self.linha += 1; self.col = 0
                        self.avancar()
                    continue
                self.avancar(); val = ""
                while self.char() and self.char() != '`':
                    if self.char() == '\n': raise ValueError(f"Backtick nao fechado linha {self.linha+1}")
                    val += self.avancar()
                if self.char() == '`': self.avancar()
                self.add(_tt("LITERAL_TEXTO"), val); continue
            
            raise ValueError(f"Caractere invalido '{c}' linha {self.linha+1}")
        
        while len(self.niveis) > 1:
            self.niveis.pop(); self.add(_tt("DESINDENTAR"), "DESINDENTAR")
        self.add(_tt("EOF"), "EOF")
        return self.tokens
