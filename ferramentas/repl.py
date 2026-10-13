# -*- coding: utf-8 -*-
"""
REPL (Read-Eval-Print Loop) para Linguagem BR
Console interativo para execução rápida de código e experimentação
"""
import sys
import os
try:
    import readline
except ImportError:
    # Windows não tem readline, usar pyreadline3 se disponível
    try:
        import pyreadline3 as readline
    except ImportError:
        readline = None
from pathlib import Path
from typing import Optional, List
from lexer import LexerBR
from parser import ParserBR
from semantic_analyzer import AnalisadorSemanticoBR, analisar_semantica
from optimizer import otimizar_codigo
from codegen import gerar_bytecode, salvar_bytecode_brc1
from maquina_virtual import MaquinaVirtualBR
from errors import SistemaErrosBR

class REPLBR:
    def __init__(self):
        self.historico_arquivo = os.path.expanduser("~/.brasil_historico")
        self.carregar_historico()
        self.escopo_global = {}
        self.versao = "1.0.0"
        self.sistema_erros = SistemaErrosBR()
        
        # Serviços nativos para autocomplete
        self.servicos_nativos = [
            "leia_arquivo", "escreva_arquivo", "texto_substituir", "dividir_por",
            "dividir_primeiro", "divida_texto", "adicione_item", "obter_item",
            "tamanho_lista", "definir_propriedade", "obter_propriedade", "tipo_de",
            "para_inteiro", "para_decimal", "texto_contem", "texto_contem_inicio",
            "acesse", "busque"
        ]
        
        # Palavras reservadas para autocomplete
        self.palavras_reservadas = [
            "função", "se", "senão", "enquanto", "para", "cada", "em",
            "tente", "capture", "execute", "paralelo", "devolva", "mostre",
            "espere", "salve", "remova", "busque", "acesse", "constante",
            "explicacao", "objetivo", "teste", "fim", "lista", "registro",
            "texto", "inteiro", "decimal", "booleano", "verdadeiro", "falso",
            "nulo", "é", "recebendo", "e", "então"
        ]
        
        # Configurar autocomplete
        self.configurar_autocomplete()
        
    def carregar_historico(self):
        """Carrega histórico de comandos anteriores"""
        try:
            if os.path.exists(self.historico_arquivo):
                readline.read_history_file(self.historico_arquivo)
        except:
            pass
    
    def salvar_historico(self):
        """Salva histórico de comandos"""
        try:
            readline.write_history_file(self.historico_arquivo)
        except:
            pass
    
    def configurar_autocomplete(self):
        """Configura autocomplete do readline"""
        if readline is None:
            return  # Windows sem readline, pular autocomplete
            
        def completar(texto, estado):
            """Função de autocomplete"""
            # Combinar palavras reservadas, serviços nativos e variáveis do escopo
            opcoes = self.palavras_reservadas + self.servicos_nativos + list(self.escopo_global.keys())
            
            # Filtrar opções que começam com o texto
            matches = [op for op in opcoes if op.startswith(texto)]
            
            if estado < len(matches):
                return matches[estado]
            return None
        
        readline.set_completer(completar)
        readline.parse_and_bind("tab: complete")
    
    def banner(self):
        """Exibe banner de boas-vindas"""
        print(f"""
╔══════════════════════════════════════════════════════════════╗
║           Linguagem BR (Versão {self.versao})                   ║
║              Console Interativo de Desenvolvimento            ║
╚══════════════════════════════════════════════════════════════╝

Digite 'ajuda' para comandos internos ou 'fim' para sair.
""")
    
    def ajuda(self):
        """Exibe ajuda de comandos"""
        print("""
Comandos Internos:
  ajuda              Exibe esta mensagem de ajuda
  fim                Sai do console interativo
  limpar             Limpa a tela do console
  historico          Exibe o histórico de comandos
  variaveis          Lista todas as variáveis no escopo atual
  limpar_variaveis   Remove todas as variáveis do escopo
  tipo_de <var>      Exibe o tipo de uma variável
  inspecione <var>   Exibe detalhes de uma variável/objeto

Exemplos de Uso:
  br> nome é "Carlos"
  br> mostre nome
  br> idade é 30
  br> soma é idade + 5
  br> mostre soma
  br> clientes é lista
  ...     "Carlos"
  ...     "Ana"
  ... fim
""")
    
    def limpar_tela(self):
        """Limpa a tela do console"""
        os.system('cls' if os.name == 'nt' else 'clear')
    
    def listar_variaveis(self):
        """Lista todas as variáveis no escopo atual"""
        if not self.escopo_global:
            print("Nenhuma variável definida no escopo atual.")
            return
        
        print("Variáveis no escopo atual:")
        for nome, valor in self.escopo_global.items():
            tipo = type(valor).__name__
            if isinstance(valor, str):
                valor_repr = f'"{valor}"'
            elif isinstance(valor, list):
                valor_repr = f"<lista de {len(valor)} itens>"
            elif isinstance(valor, dict):
                valor_repr = f"<registro com {len(valor)} propriedades>"
            else:
                valor_repr = str(valor)
            print(f"  {nome}: {tipo} = {valor_repr}")
    
    def limpar_variaveis(self):
        """Remove todas as variáveis do escopo"""
        self.escopo_global.clear()
        print("Todas as variáveis foram removidas do escopo.")
    
    def tipo_de(self, nome_var: str):
        """Exibe o tipo de uma variável"""
        if nome_var not in self.escopo_global:
            print(f"Variável '{nome_var}' não encontrada.")
            return
        
        valor = self.escopo_global[nome_var]
        tipo = type(valor).__name__
        
        tipo_map = {
            "str": "texto",
            "int": "inteiro",
            "float": "decimal",
            "bool": "booleano",
            "list": "lista",
            "dict": "registro",
            "NoneType": "nulo",
        }
        
        tipo_br = tipo_map.get(tipo, tipo)
        print(f"[Tipo: {tipo_br}]")
    
    def inspecionar(self, nome_var: str):
        """Exibe detalhes de uma variável/objeto"""
        if nome_var not in self.escopo_global:
            print(f"Variável '{nome_var}' não encontrada.")
            return
        
        valor = self.escopo_global[nome_var]
        tipo = type(valor).__name__
        
        print(f"Inspecionando: {nome_var}")
        print(f"  Tipo: {tipo}")
        
        if isinstance(valor, list):
            print(f"  Tamanho: {len(valor)}")
            print(f"  Elementos:")
            for i, elem in enumerate(valor):
                print(f"    [{i}]: {type(elem).__name__} = {repr(elem)[:50]}")
        elif isinstance(valor, dict):
            print(f"  Propriedades: {len(valor)}")
            print(f"  Chaves:")
            for chave in valor.keys():
                print(f"    - {chave}")
        elif isinstance(valor, str):
            print(f"  Comprimento: {len(valor)}")
            print(f"  Conteúdo: {repr(valor)}")
        else:
            print(f"  Valor: {repr(valor)}")
    
    def executar_linha(self, linha: str) -> Optional[str]:
        """Executa uma linha de código BR"""
        try:
            # Tokenizar
            lexer = LexerBR(linha)
            tokens = lexer.tokenizar()
            
            # Parsear
            parser = ParserBR(tokens)
            ast = parser.parse()
            
            # Análise semântica
            analisador = AnalisadorSemanticoBR()
            analisador.entrar_escopo()
            sucesso, erros, avisos = analisar_semantica(ast)
            
            if not sucesso:
                for erro in erros:
                    print(str(erro))
                return None
            
            # Otimizar
            ast_otimizado, relatorio = otimizar_codigo(ast)
            
            # Gerar bytecode
            bytecode = gerar_bytecode(ast_otimizado)
            
            # Executar
            vm = MaquinaVirtualBR(bytecode)
            vm.executar()
            
            # Capturar resultado se houver
            if vm.pilha:
                resultado = vm.pilha[-1]
                self.escopo_global['_ultimo_resultado'] = resultado
                return str(resultado)
            
            return None
            
        except Exception as e:
            print(f"Erro de execução: {e}")
            return None
    
    def executar_multilinha(self, linhas: List[str]) -> Optional[str]:
        """Executa um bloco de código multi-linha"""
        codigo = "\n".join(linhas)
        return self.executar_linha(codigo)
    
    def loop(self):
        """Loop principal do REPL"""
        self.banner()
        
        buffer = []
        em_bloco = False
        indentacao_esperada = 0
        
        while True:
            try:
                # Determinar prompt
                if em_bloco:
                    prompt = "... "
                else:
                    prompt = "br> "
                
                # Ler linha
                linha = input(prompt).strip()
                
                # Comandos internos
                if not em_bloco:
                    if linha == "fim":
                        print("Saindo do console BR. Até logo!")
                        self.salvar_historico()
                        break
                    
                    if linha == "ajuda":
                        self.ajuda()
                        continue
                    
                    if linha == "limpar":
                        self.limpar_tela()
                        continue
                    
                    if linha == "historico":
                        try:
                            for i in range(readline.get_current_history_length()):
                                print(f"  {i}: {readline.get_history_item(i)}")
                        except:
                            print("Histórico não disponível.")
                        continue
                    
                    if linha == "variaveis":
                        self.listar_variaveis()
                        continue
                    
                    if linha == "limpar_variaveis":
                        self.limpar_variaveis()
                        continue
                    
                    if linha.startswith("tipo_de "):
                        nome_var = linha[9:].strip()
                        self.tipo_de(nome_var)
                        continue
                    
                    if linha.startswith("inspecione "):
                        nome_var = linha[11:].strip()
                        self.inspecionar(nome_var)
                        continue
                
                # Linha vazia em bloco continua
                if not linha and em_bloco:
                    buffer.append("")
                    continue
                
                # Detectar início de bloco
                if linha in ["se", "enquanto", "para cada", "tente", "execute em paralelo", "função", "lista", "registro", "explicacao", "objetivo", "teste"]:
                    em_bloco = True
                    indentacao_esperada = 4
                    buffer.append(linha)
                    continue
                
                # Detectar fim de bloco
                if linha == "fim" and em_bloco:
                    buffer.append(linha)
                    resultado = self.executar_multilinha(buffer)
                    if resultado is not None:
                        print(resultado)
                    buffer = []
                    em_bloco = False
                    indentacao_esperada = 0
                    continue
                
                # Adicionar ao buffer se em bloco
                if em_bloco:
                    buffer.append(linha)
                    continue
                
                # Executar linha única
                resultado = self.executar_linha(linha)
                if resultado is not None:
                    print(resultado)
                
            except KeyboardInterrupt:
                print("\nInterrupção recebida. Use 'fim' para sair.")
                buffer = []
                em_bloco = False
            except EOFError:
                print("\nSaindo do console BR. Até logo!")
                self.salvar_historico()
                break

def main():
    """Ponto de entrada do REPL"""
    repl = REPLBR()
    repl.loop()

if __name__ == "__main__":
    main()
