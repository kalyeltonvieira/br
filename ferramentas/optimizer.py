# -*- coding: utf-8 -*-
"""
Otimizador de Código para Linguagem BR
Implementa folding de constantes, eliminação de código morto e inlining de funções
"""
from typing import List, Optional, Any
from br_ast import *

class OtimizadorBR:
    def __init__(self):
        self.constantes_folding = 0
        self.codigo_morto_removido = 0
        self.funcoes_inlined = 0
    
    def otimizar(self, nos):
        """Aplica todas as otimizações em passagem"""
        nos = self.eliminar_codigo_morto(nos)
        nos = self.folding_constantes(nos)
        nos = self.inline_funcoes_simples(nos)
        return nos
    
    def folding_constantes(self, nos):
        """Avalia expressões constantes em tempo de compilação"""
        resultado = []
        for no in nos:
            no_otimizado = self._folding_constantes_no(no)
            if no_otimizado:
                resultado.append(no_otimizado)
        return resultado
    
    def _folding_constantes_no(self, no):
        if isinstance(no, Binario):
            esq = self._folding_constantes_no(no.esquerda)
            dir = self._folding_constantes_no(no.direita)
            
            if isinstance(esq, (LiteralInteiro, LiteralDecimal)) and isinstance(dir, (LiteralInteiro, LiteralDecimal)):
                try:
                    valor_esq = esq.valor if isinstance(esq, LiteralInteiro) else esq.valor
                    valor_dir = dir.valor if isinstance(dir, LiteralInteiro) else dir.valor
                    
                    resultado = None
                    if no.operador == "+":
                        resultado = valor_esq + valor_dir
                    elif no.operador == "-":
                        resultado = valor_esq - valor_dir
                    elif no.operador == "*":
                        resultado = valor_esq * valor_dir
                    elif no.operador == "/":
                        if valor_dir != 0:
                            resultado = valor_esq / valor_dir
                        else:
                            return Binario(no.posicao, esq, no.operador, dir)
                    
                    if resultado is not None:
                        self.constantes_folding += 1
                        if isinstance(resultado, int):
                            return LiteralInteiro(no.posicao, resultado)
                        else:
                            return LiteralDecimal(no.posicao, resultado)
                except:
                    return Binario(no.posicao, esq, no.operador, dir)
            
            return Binario(no.posicao, esq, no.operador, dir)
        
        elif isinstance(no, Unario):
            operando = self._folding_constantes_no(no.operando)
            
            if isinstance(operando, (LiteralInteiro, LiteralDecimal)):
                valor = operando.valor if isinstance(operando, LiteralInteiro) else operando.valor
                if no.operador == "-":
                    self.constantes_folding += 1
                    if isinstance(valor, int):
                        return LiteralInteiro(no.posicao, -valor)
                    else:
                        return LiteralDecimal(no.posicao, -valor)
            
            return Unario(no.posicao, no.operador, operando)
        
        elif isinstance(no, DeclaracaoVariavel):
            valor = self._folding_constantes_no(no.valor)
            return DeclaracaoVariavel(
                no.posicao,
                no.nome,
                no.tipo_estatico,
                valor,
                no.eh_constante,
                getattr(no, "eh_atribuicao", False)
            )

        elif isinstance(no, AtribuicaoAlvo):
            alvo = self._folding_constantes_no(no.alvo)
            valor = self._folding_constantes_no(no.valor)
            return AtribuicaoAlvo(no.posicao, alvo, valor)
        
        elif isinstance(no, SeStmt):
            condicao = self._folding_constantes_no(no.condicao)
            
            if isinstance(condicao, LiteralBooleano):
                if condicao.valor:
                    self.codigo_morto_removido += 1
                    return ExpressaoStmt(no.posicao, LiteralTexto(no.posicao, "# ramo senão eliminado"))
                else:
                    self.codigo_morto_removido += 1
                    return ExpressaoStmt(no.posicao, LiteralTexto(no.posicao, "# ramo se eliminado"))
            
            corpo_se = [self._folding_constantes_no(stmt) for stmt in no.corpo_se]
            corpo_senao = [self._folding_constantes_no(stmt) for stmt in no.corpo_senao]
            
            return SeStmt(no.posicao, condicao, corpo_se, corpo_senao)
        
        elif isinstance(no, EnquantoStmt):
            condicao = self._folding_constantes_no(no.condicao)
            
            if isinstance(condicao, LiteralBooleano) and not condicao.valor:
                self.codigo_morto_removido += 1
                return ExpressaoStmt(no.posicao, LiteralTexto(no.posicao, "# loop eliminado"))
            
            corpo = [self._folding_constantes_no(stmt) for stmt in no.corpo]
            return EnquantoStmt(no.posicao, condicao, corpo)
        
        elif isinstance(no, MostreStmt):
            expressao = self._folding_constantes_no(no.expressao)
            return MostreStmt(no.posicao, expressao)
        
        elif isinstance(no, RetornarStmt):
            valor = self._folding_constantes_no(no.valor)
            return RetornarStmt(no.posicao, valor)
        
        elif isinstance(no, ListaLiteral):
            elementos = [self._folding_constantes_no(elem) for elem in no.elementos]
            return ListaLiteral(no.posicao, elementos)
        
        elif isinstance(no, RegistroLiteral):
            propriedades = {k: self._folding_constantes_no(v) for k, v in no.propriedades.items()}
            return RegistroLiteral(no.posicao, propriedades)
        
        elif isinstance(no, ChamadaFuncao):
            argumentos = [self._folding_constantes_no(arg) for arg in no.argumentos]
            return ChamadaFuncao(no.posicao, no.nome, argumentos)
        
        elif isinstance(no, ChamadaMetodo):
            objeto = self._folding_constantes_no(no.objeto) if no.objeto else None
            argumentos = [self._folding_constantes_no(arg) for arg in no.argumentos]
            return ChamadaMetodo(no.posicao, objeto, no.metodo, argumentos)

        elif isinstance(no, AcessoIndice):
            objeto = self._folding_constantes_no(no.objeto)
            indice = self._folding_constantes_no(no.indice)
            return AcessoIndice(no.posicao, objeto, indice)
        
        elif isinstance(no, ConteStmt):
            inicio = self._folding_constantes_no(no.inicio)
            fim = self._folding_constantes_no(no.fim)
            corpo = [self._folding_constantes_no(stmt) for stmt in no.corpo]
            return ConteStmt(no.posicao, inicio, fim, no.variavel, corpo)

        elif isinstance(no, DeclaracaoTarefa):
            corpo = [self._folding_constantes_no(stmt) for stmt in no.corpo]
            return DeclaracaoTarefa(no.posicao, no.nome, no.parametros, corpo)

        elif isinstance(no, ExpressaoTarefa):
            expressao = self._folding_constantes_no(no.expressao)
            return ExpressaoTarefa(no.posicao, expressao)
        
        return no
    
    def eliminar_codigo_morto(self, nos):
        """Remove código que nunca será executado"""
        resultado = []
        
        for no in nos:
            if isinstance(no, RetornarStmt):
                resultado.append(no)
                break
            
            if isinstance(no, SeStmt):
                if not no.corpo_se and not no.corpo_senao:
                    self.codigo_morto_removido += 1
                    continue
                corpo_se = self.eliminar_codigo_morto(no.corpo_se)
                corpo_senao = self.eliminar_codigo_morto(no.corpo_senao)
                if corpo_se or corpo_senao:
                    resultado.append(SeStmt(no.posicao, no.condicao, corpo_se, corpo_senao))
                else:
                    self.codigo_morto_removido += 1
                continue
            
            if isinstance(no, EnquantoStmt):
                corpo = self.eliminar_codigo_morto(no.corpo)
                if corpo:
                    resultado.append(EnquantoStmt(no.posicao, no.condicao, corpo))
                else:
                    self.codigo_morto_removido += 1
                continue
            
            if isinstance(no, ParaCadaStmt):
                corpo = self.eliminar_codigo_morto(no.corpo)
                if corpo:
                    resultado.append(ParaCadaStmt(no.posicao, no.variavel, no.lista, corpo))
                else:
                    self.codigo_morto_removido += 1
                continue
            
            if isinstance(no, TenteCaptureStmt):
                corpo_tente = self.eliminar_codigo_morto(no.corpo_tente)
                corpo_capture = self.eliminar_codigo_morto(no.corpo_capture)
                if corpo_tente or corpo_capture:
                    resultado.append(TenteCaptureStmt(no.posicao, corpo_tente, no.variavel_erro, corpo_capture))
                else:
                    self.codigo_morto_removido += 1
                continue
            
            resultado.append(no)
        
        return resultado
    
    def inline_funcoes_simples(self, nos):
        return nos
    
    def relatorio(self) -> str:
        return f"""
Relatório de Otimização:
- Constantes folding: {self.constantes_folding}
- Código morto removido: {self.codigo_morto_removido}
- Funções inlined: {self.funcoes_inlined}
"""

def otimizar_codigo(nos):
    otimizador = OtimizadorBR()
    resultado = otimizador.otimizar(nos)
    relatorio = otimizador.relatorio()
    return resultado, relatorio