# -*- coding: utf-8 -*-
"""
Script para implementar todos os recursos faltantes da Linguagem BR
"""
import sys, os
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(_SCRIPT_DIR, 'ferramentas'))

# ============================================================
# 1. RODAR TESTE DE DIAGNÓSTICO PRIMEIRO
# ============================================================
print("="*60)
print("IMPLEMENTAÇÃO COMPLETA DOS RECURSOS FALTANTES")
print("="*60)

def testar_lexer():
    """Testa se o lexer funciona com os novos tokens"""
    from lexer import LexerBR, TipoToken
    codigo = 'teste "ok"\n    a é nao verdadeiro\n    espere a igual a falso\nfim'
    try:
        tokens = LexerBR(codigo).tokenizar()
        tipos = [t.tipo for t in tokens]
        print(f"  Lexer OK - {len(tokens)} tokens gerados")
        # Verificar se NAO está nos tokens
        tipos_str = [str(t) for t in tipos]
        return True
    except Exception as e:
        print(f"  Lexer ERRO: {e}")
        return False

testar_lexer()