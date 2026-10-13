#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BR - Interpretador da Linguagem BR
Ponto de entrada principal para execução de programas BR
"""
import sys
import os
import argparse
import faulthandler
from pathlib import Path
from typing import Optional, List

# Adicionar ferramentas ao path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'ferramentas'))

from encoding_utils import configurar_stdio, ler_texto
from lexer import LexerBR
from parser import ParserBR
from semantic_analyzer import AnalisadorSemanticoBR, analisar_semantica
from optimizer import otimizar_codigo
from codegen import gerar_bytecode, salvar_bytecode_brc1
from maquina_virtual import MaquinaVirtualBR
from repl import REPLBR

configurar_stdio()
try:
    faulthandler.enable()
except (OSError, RuntimeError):
    pass

def validar_contrato_se_existir(caminho_arquivo: str) -> int:
    try:
        from contratos import contrato_adjacente, validar_contrato_fonte
        caminho_bri = contrato_adjacente(caminho_arquivo)
        if not caminho_bri:
            return 0
        erros = validar_contrato_fonte(caminho_arquivo, caminho_bri)
        for erro in erros:
            print(f"Erro de contrato: {erro}")
        return 1 if erros else 0
    except Exception as e:
        print(f"Erro ao validar contrato: {e}")
        return 1

def buscar_arquivo(nome_arquivo: str) -> Optional[str]:
    """Busca inteligente de arquivo em múltiplos locais"""
    # Verificar se é caminho absoluto
    caminho = Path(nome_arquivo)
    if caminho.is_absolute():
        if caminho.exists():
            return str(caminho)
        return None
    
    # Verificar no diretório atual
    caminho_atual = Path(nome_arquivo)
    if caminho_atual.exists():
        return str(caminho_atual)
    
    # Adicionar extensão .br se não tiver
    if not nome_arquivo.endswith('.br'):
        caminho_br = Path(nome_arquivo + '.br')
        if caminho_br.exists():
            return str(caminho_br)
    
    # Verificar em bibliotecas padrão BR
    bibliotecas_br = Path.home() / '.br' / 'lib'
    if bibliotecas_br.exists():
        caminho_lib = bibliotecas_br / nome_arquivo
        if caminho_lib.exists():
            return str(caminho_lib)
        
        if not nome_arquivo.endswith('.br'):
            caminho_lib_br = bibliotecas_br / (nome_arquivo + '.br')
            if caminho_lib_br.exists():
                return str(caminho_lib_br)
    
    return None

def buscar_arquivo(nome_arquivo: str) -> Optional[str]:
    """Busca arquivos BR em projeto, usuario, instalacao e BR_PATH."""
    extensoes = ["", ".br", ".brt", ".bri", ".bra", ".brc", ".brp", ".brm", ".brd"]

    def candidatos_com_extensao(base: Path):
        if base.suffix:
            yield base
        else:
            for ext in extensoes:
                yield Path(str(base) + ext)

    caminho = Path(nome_arquivo)
    if caminho.is_absolute():
        for candidato in candidatos_com_extensao(caminho):
            if candidato.exists():
                return str(candidato)
        return None

    bases = [
        Path.cwd(),
        Path.cwd() / "modulos",
        Path.home() / ".br" / "modulos",
        Path.home() / ".br" / "lib",
    ]

    if os.name == "nt":
        program_files = Path(os.environ.get("ProgramFiles", r"C:\Program Files"))
        bases.extend([
            program_files / "BRlang" / "lib",
            program_files / "BRlang" / "modulos",
        ])
    else:
        bases.extend([
            Path("/usr/local/brlang/lib"),
            Path("/usr/local/brlang/modulos"),
        ])

    br_path = os.environ.get("BR_PATH", "")
    if br_path:
        bases.extend(Path(p) for p in br_path.split(os.pathsep) if p.strip())

    for base in bases:
        for candidato in candidatos_com_extensao(base / nome_arquivo):
            if candidato.exists():
                return str(candidato)

    return None

def executar_arquivo(caminho_arquivo: str, argumentos: List[str] = None):
    """Executa um arquivo BR"""
    if argumentos is None:
        argumentos = []
    
    # Ler código fonte
    try:
        codigo_fonte = ler_texto(caminho_arquivo)
    except Exception as e:
        print(f"Erro ao ler arquivo: {e}")
        return 1

    contrato_status = validar_contrato_se_existir(caminho_arquivo)
    if contrato_status:
        return contrato_status
    
    # Tokenizar
    lexer = LexerBR(codigo_fonte)
    tokens = lexer.tokenizar()
    
    # Parsear
    parser = ParserBR(tokens)
    ast = parser.parse()
    if parser.erros:
        return 1
    
    # Análise semântica
    analisador = AnalisadorSemanticoBR()
    analisador.entrar_escopo()
    sucesso, erros, avisos = analisar_semantica(ast)
    
    if not sucesso:
        for erro in erros:
            print(str(erro))
        return 1
    
    # Otimizar
    ast_otimizado, relatorio = otimizar_codigo(ast)
    
    # Gerar bytecode
    bytecode = gerar_bytecode(ast_otimizado)
    
    # Executar
    vm = MaquinaVirtualBR(bytecode, diretorio_base=str(Path(caminho_arquivo).parent))
    
    # Adicionar argumentos ao escopo global
    vm.definir_var('argumentos', argumentos)
    
    try:
        vm.executar()
        sys.stdout.flush()
    except Exception as e:
        print(f"Erro de execução: {vm.formatar_erro(e)}", flush=True)
        sys.stdout.flush()
        return 1
    
    return 0

def compilar_arquivo(caminho_arquivo: str, caminho_saida: Optional[str] = None):
    """Compila um arquivo BR para bytecode"""
    if caminho_saida is None:
        caminho_saida = caminho_arquivo.replace('.br', '.brc')
    
    # Ler código fonte
    try:
        codigo_fonte = ler_texto(caminho_arquivo)
    except Exception as e:
        print(f"Erro ao ler arquivo: {e}")
        return 1

    contrato_status = validar_contrato_se_existir(caminho_arquivo)
    if contrato_status:
        return contrato_status
    
    # Tokenizar
    lexer = LexerBR(codigo_fonte)
    tokens = lexer.tokenizar()
    
    # Parsear
    parser = ParserBR(tokens)
    ast = parser.parse()
    if parser.erros:
        return 1
    
    # Análise semântica
    analisador = AnalisadorSemanticoBR()
    analisador.entrar_escopo()
    sucesso, erros, avisos = analisar_semantica(ast)
    
    if not sucesso:
        for erro in erros:
            print(str(erro))
        return 1
    
    # Otimizar
    ast_otimizado, relatorio = otimizar_codigo(ast)
    
    # Gerar bytecode
    bytecode = gerar_bytecode(ast_otimizado)
    
    # Salvar bytecode
    try:
        salvar_bytecode_brc1(bytecode, caminho_saida)
        print(f"Compilação concluída: {caminho_saida}")
        return 0
    except Exception as e:
        print(f"Erro ao salvar bytecode: {e}")
        return 1

def executar_bytecode(caminho_brc: str, argumentos: List[str] = None):
    """Executa um arquivo bytecode .brc"""
    if argumentos is None:
        argumentos = []
    
    # Carregar bytecode
    try:
        from maquina_virtual import ByteStream, deserializar
        with open(caminho_brc, 'rb') as f:
            conteudo = f.read()
        if conteudo[:4] == b"BRC1":
            conteudo = conteudo[4:]
        bytecode = deserializar(ByteStream(conteudo))
    except Exception as e:
        print(f"Erro ao ler bytecode: {e}")
        return 1
    
    # Executar
    vm = MaquinaVirtualBR(bytecode, diretorio_base=str(Path(caminho_brc).parent))
    vm.definir_var('argumentos', argumentos)
    
    try:
        vm.executar()
    except Exception as e:
        print(f"Erro de execução: {vm.formatar_erro(e)}")
        return 1
    
    return 0

def executar_pacote(caminho_brp: str, argumentos: List[str] = None):
    """Executa pacote BR .brp pelo ponto de entrada do manifesto."""
    if argumentos is None:
        argumentos = []

    vm = None
    try:
        from packager import LeitorPacoteBR
        from maquina_virtual import ByteStream, deserializar

        leitor = LeitorPacoteBR(caminho_brp)
        leitor.ler()
        entrada = leitor.manifesto.ponto_entrada if leitor.manifesto else "principal.brc"
        conteudo = leitor.obter_bytecode(entrada)
        if conteudo is None and leitor.arquivos_bytecode:
            entrada, conteudo = next(iter(leitor.arquivos_bytecode.items()))
        if conteudo is None:
            print(f"Erro: pacote '{caminho_brp}' nao contem bytecode executavel")
            return 1
        if conteudo[:4] != b"BRC1":
            print(f"Erro: entrada '{entrada}' nao esta no formato BRC1")
            return 1

        bytecode = deserializar(ByteStream(conteudo[4:]))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(Path(caminho_brp).parent))
        vm.definir_var('argumentos', argumentos)
        vm.executar()
        return 0
    except Exception as e:
        erro = vm.formatar_erro(e) if vm is not None else str(e)
        print(f"Erro ao executar pacote: {erro}")
        return 1

def executar_one_liner(codigo: str):
    """Executa um comando de uma linha"""
    # Tokenizar
    lexer = LexerBR(codigo)
    tokens = lexer.tokenizar()
    
    # Parsear
    parser = ParserBR(tokens)
    ast = parser.parse()
    if parser.erros:
        return 1
    
    # Análise semântica
    analisador = AnalisadorSemanticoBR()
    analisador.entrar_escopo()
    sucesso, erros, avisos = analisar_semantica(ast)
    
    if not sucesso:
        for erro in erros:
            print(str(erro))
        return 1
    
    # Otimizar
    ast_otimizado, relatorio = otimizar_codigo(ast)
    
    # Gerar bytecode
    bytecode = gerar_bytecode(ast_otimizado)
    
    # Executar
    vm = MaquinaVirtualBR(bytecode, diretorio_base=os.getcwd())
    
    try:
        vm.executar()
    except Exception as e:
        print(f"Erro de execução: {vm.formatar_erro(e)}")
        return 1
    
    return 0

def exibir_contrato(caminho_bri: str):
    """Exibe resumo de um contrato .bri."""
    try:
        from contratos import ler_contrato
        assinaturas = ler_contrato(caminho_bri)
        print(f"Contrato BR: {caminho_bri}")
        if not assinaturas:
            print("Nenhuma assinatura de função encontrada.")
            return 0
        for assinatura in assinaturas:
            params = " e ".join(assinatura.parametros)
            sufixo = f" recebendo {params}" if params else ""
            print(f"- função {assinatura.nome}{sufixo}")
        return 0
    except Exception as e:
        print(f"Erro ao ler contrato: {e}")
        return 1

def exibir_texto(caminho: str, titulo: str):
    """Exibe arquivo textual BR associado a duplo clique."""
    try:
        print(f"{titulo}: {caminho}")
        print()
        print(ler_texto(caminho))
        return 0
    except Exception as e:
        print(f"Erro ao ler arquivo: {e}")
        return 1

def mostrar_ajuda():
    """Exibe mensagem de ajuda em português"""
    print("""
Linguagem BR - Interpretador

Uso:
  br [arquivo.br]              Executa um arquivo BR
  br -i                        Inicia o modo interativo (REPL)
  br -c "código"               Executa um comando de uma linha
  br --compile arquivo.br      Compila para bytecode (.brc)
  br --run arquivo.brc         Executa bytecode compilado
  br arquivo.brc               Executa bytecode compilado
  br arquivo.brp               Executa pacote BR
  br arquivo.brt               Executa testes BR
  br --version                 Exibe a versão
  br --help                    Exibe esta mensagem de ajuda

Exemplos:
  br meu_programa.br           Executa o arquivo
  br -i                        Inicia o REPL
  br -c "mostre 'Olá'"         Executa comando rápido
  br --compile prog.br         Compila para bytecode
  br --run prog.brc            Executa bytecode

Para mais informações, visite: https://br-lang.org
""")

def main():
    parser = argparse.ArgumentParser(
        description='Interpretador da Linguagem BR',
        add_help=False
    )
    
    parser.add_argument('arquivo', nargs='?', help='Arquivo BR para executar')
    parser.add_argument('-i', '--interativo', action='store_true', help='Modo interativo (REPL)')
    parser.add_argument('-c', '--codigo', help='Executar comando de uma linha')
    parser.add_argument('--compile', action='store_true', help='Compilar para bytecode')
    parser.add_argument('--run', action='store_true', help='Executar bytecode')
    parser.add_argument('--version', action='store_true', help='Exibir versão')
    parser.add_argument('--help', '-h', action='store_true', help='Exibir ajuda')
    parser.add_argument('argumentos', nargs='*', help='Argumentos para o programa')
    
    args = parser.parse_args()
    
    # Exibir ajuda
    if args.help:
        mostrar_ajuda()
        return 0

    if args.version:
        print("BR 1.0.0")
        return 0
    
    # Modo one-liner
    if args.codigo:
        return executar_one_liner(args.codigo)
    
    # Modo interativo
    if args.interativo or not args.arquivo:
        repl = REPLBR()
        repl.loop()
        return 0
    
    # Buscar arquivo
    caminho_arquivo = buscar_arquivo(args.arquivo)
    if not caminho_arquivo:
        print(f"Erro: Arquivo '{args.arquivo}' não encontrado")
        return 1
    
    # Modo compilação
    sufixo = Path(caminho_arquivo).suffix.lower()

    if sufixo == ".brp":
        return executar_pacote(caminho_arquivo, args.argumentos)

    if sufixo == ".brc":
        return executar_bytecode(caminho_arquivo, args.argumentos)

    if sufixo == ".brt":
        from tester import FrameworkTestesBR
        framework = FrameworkTestesBR()
        suite = framework.executar_arquivo(caminho_arquivo)
        print(framework.relatorio_suite(suite))
        return 1 if any(not teste.sucesso for teste in suite.testes) else 0

    if sufixo == ".bri":
        return exibir_contrato(caminho_arquivo)

    if sufixo == ".brd":
        return exibir_texto(caminho_arquivo, "Documentação BR")

    if sufixo == ".brm":
        return exibir_texto(caminho_arquivo, "Manifesto BR")

    if sufixo == ".bra":
        print("Executando agente BR...")
        return executar_arquivo(caminho_arquivo, args.argumentos)

    if args.compile:
        return compilar_arquivo(caminho_arquivo)
    
    # Modo execução bytecode
    if args.run:
        return executar_bytecode(caminho_arquivo, args.argumentos)
    
    # Modo execução normal
    return executar_arquivo(caminho_arquivo, args.argumentos)

if __name__ == '__main__':
    sys.exit(main())
