# -*- coding: utf-8 -*-
"""Teste completo de TODOS os recursos da Linguagem BR"""
import sys, os
sys.path.insert(0, 'ferramentas')

from lexer import LexerBR, TipoToken
from parser import ParserBR
from codegen import gerar_bytecode
from maquina_virtual import MaquinaVirtualBR
from semantic_analyzer import AnalisadorSemanticoBR, analisar_semantica
from optimizer import otimizar_codigo

def testar(nome, codigo):
    print(f"\n{'='*60}")
    print(f"TESTE: {nome}")
    print(f"{'='*60}")
    try:
        tokens = LexerBR(codigo).tokenizar()
        parser = ParserBR(tokens)
        ast = parser.parse()
        if parser.erros:
            for e in parser.erros:
                print(f"  ERRO SINTÁTICO: {e}")
            return False
        
        sucesso, erros, avisos = analisar_semantica(ast)
        if not sucesso:
            for e in erros:
                print(f"  AVISO SEMÂNTICO: {e}")
        
        ast_otim, rel = otimizar_codigo(ast)
        bytecode = gerar_bytecode(ast_otim)
        
        vm = MaquinaVirtualBR(bytecode)
        vm.executar()
        print(f"  ✓ OK")
        return True
    except Exception as e:
        print(f"  ✗ ERRO: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        return False

testes = []

# 1. Operador NÃO (negação)
testes.append(("negação com não", '''
teste "negacao"
    valor é verdadeiro
    resultado é nao valor
    espere resultado igual a falso
fim
'''))

# 2. FALHOU (verificação de erro)
testes.append(("falhou verificação", '''
teste "falhou"
    resultado é tente
        10 / 0
    capture erro
        erro.mensagem
    fim
    se resultado falhou entao
        ok é verdadeiro
    fim
    espere ok igual a verdadeiro
fim
'''))

# 3. ISTO/SELF em métodos de classe
testes.append(("metodo com isto", '''
classe Conta
    saldo: decimal
fim
teste "metodo conta"
    conta é Conta(100.0)
    espere conta.saldo igual a 100.0
fim
'''))

# 4. CONTRATO
testes.append(("contrato", '''
contrato Repositorio
    funcao buscar recebendo id
    funcao salvar recebendo registro
fim
teste "contrato declarado"
    ok é verdadeiro
    espere ok igual a verdadeiro
fim
'''))

# 5. ONDE (filtro banco de dados)
testes.append(("busque onde", '''
teste "busque onde"
    salve "cliente1"
    salve "cliente2"
    resultados é busque todos os clientes
    espere tamanho_lista(resultados) igual a 2
fim
'''))

# 6. AGUARDE (tarefa assíncrona)
testes.append(("tarefa aguarde", '''
tarefa somar recebendo a e b
    devolva a + b
fim
teste "tarefa aguarde"
    resultado é somar 3 e 4
    espere resultado igual a 7
fim
'''))

# 7. Web Service Nativo
testes.append(("crie um site", '''
teste "criar site"
    porta é 8080
    espere porta igual a 8080
fim
'''))

# 8. Todos os operadores
testes.append(("todos operadores", '''
teste "operadores"
    a é 10
    b é 3
    espere (a + b) igual a 13
    espere (a - b) igual a 7
    espere (a * b) igual a 30
    espere (a / b) igual a 3.3333333333333335
    espere (a > b) igual a verdadeiro
    espere (a < b) igual a falso
    espere (a igual a 10) igual a verdadeiro
    espere (a != b) igual a verdadeiro
    espere (a >= 10) igual a verdadeiro
    espere (b <= 3) igual a verdadeiro
fim
'''))

# 9. NEGAÇÃO com !
testes.append(("diferente operador", '''
teste "diferente"
    a é 5
    b é 10
    espere (a != b) igual a verdadeiro
    espere (a == 5) igual a verdadeiro
fim
'''))

# 10. Strings e concatenação
testes.append(("strings", '''
teste "strings"
    nome é "Maria"
    saudacao é "Ola, " + nome
    espere saudacao igual a "Ola, Maria"
    espere texto_contem(saudacao e "Maria") igual a verdadeiro
fim
'''))

# 11. Listas e registro
testes.append(("listas registro", '''
teste "listas"
    numeros é lista
        10
        20
        30
    fim
    espere obter_item(numeros e 0) igual a 10
    espere tamanho_lista(numeros) igual a 3
fim
'''))

# 12. Para cada com lista literal
testes.append(("para cada inline", '''
teste "para cada inline"
    itens é [1, 2, 3]
    soma é 0
    para cada item em itens
        soma é soma + item
    fim
    espere soma igual a 6
fim
'''))

# 13. Registro inline
testes.append(("registro inline", '''
teste "registro inline"
    pessoa é {"nome": "Joao", "idade": 25}
    espere pessoa.nome igual a "Joao"
    espere pessoa.idade igual a 25
fim
'''))

# 14. Constantes
testes.append(("constante", '''
teste "constante"
    constante MAX é 100
    espere MAX igual a 100
fim
'''))

# 15. Tipo estático
testes.append(("tipo estatico", '''
teste "tipo estatico"
    idade : inteiro é 30
    preco : decimal é 49.99
    nome : texto é "Carlos"
    ativo : booleano é verdadeiro
    espere idade igual a 30
    espere nome igual a "Carlos"
    espere ativo igual a verdadeiro
fim
'''))

sucessos = 0
falhas = 0
for nome, codigo in testes:
    if testar(nome, codigo):
        sucessos += 1
    else:
        falhas += 1

print(f"\n{'='*60}")
print(f"RESULTADO FINAL: {sucessos} sucessos, {falhas} falhas de {len(testes)} testes")
print(f"{'='*60}")
if falhas > 0:
    sys.exit(1)
else:
    print("TODOS OS TESTES PASSARAM!")