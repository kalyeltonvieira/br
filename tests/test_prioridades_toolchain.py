import sys
import os
import shutil
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FERRAMENTAS = ROOT / "ferramentas"
sys.path.insert(0, str(FERRAMENTAS))

from codegen import OPCODE_MAP as CODEGEN_OPCODE_MAP
from codegen import gerar_bytecode, salvar_bytecode_brc1
from lexer import LexerBR
from maquina_virtual import MaquinaVirtualBR
from maquina_virtual import OPCODE_MAP as VM_OPCODE_MAP
from parser import ParserBR
from packager import criar_pacote_projeto, LeitorPacoteBR
from modulos import resolver_arquivo_modulo


class PrioridadesToolchainTest(unittest.TestCase):
    def parsear(self, codigo):
        tokens = LexerBR(codigo).tokenizar()
        parser = ParserBR(tokens)
        ast = parser.parse()
        self.assertEqual(parser.erros, [])
        return ast

    def test_lexer_ignora_comentarios_e_acentos(self):
        ast = self.parsear(
            """# comentario inicial
// comentario de barra
teste "acentos"
    total é 0 # comentario inline
    conte de 1 até 2 como i
        total é total + i
    fim
    espere total igual a 3
fim
"""
        )
        bytecode = gerar_bytecode(ast)
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        for teste in bytecode["testes"]:
            vm.executar_instrucoes(teste["instrucoes"])

    def test_codegen_e_vm_tem_mesmos_opcodes(self):
        self.assertEqual(CODEGEN_OPCODE_MAP, VM_OPCODE_MAP)

    def test_quebra_de_linha_limita_chamada_natural(self):
        codigo = """
função primeiro recebendo valor
    devolva valor
fim

teste "linha limita chamada"
    resultado é primeiro 7
    outro é 99
    espere resultado igual a 7
    espere outro igual a 99
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        teste = bytecode["testes"][0]["instrucoes"]
        chamadas = [inst for inst in teste if inst[0] == "CHAMAR_METODO"]
        self.assertEqual(chamadas, [["CHAMAR_METODO", "primeiro", 1]])

    def test_chamada_natural_com_multiplos_argumentos(self):
        codigo = """
função somar recebendo a e b
    devolva a + b
fim

teste "natural"
    resultado é somar 4 e 6
    espere resultado igual a 10
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        for teste in bytecode["testes"]:
            vm.executar_instrucoes(teste["instrucoes"])

    def test_funcao_valida_quantidade_de_argumentos(self):
        codigo = """
função somar recebendo a e b
    devolva a + b
fim

teste "aridade"
    resultado é somar 1
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        with self.assertRaisesRegex(TypeError, "Função 'somar' esperava 2 argumento"):
            vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])

    def test_tarefa_valida_quantidade_de_argumentos(self):
        codigo = """
tarefa dobro recebendo valor
    devolva valor * 2
fim

teste "aridade tarefa"
    promessa é dobro()
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        with self.assertRaisesRegex(TypeError, "Tarefa 'dobro' esperava 1 argumento"):
            vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])

    def test_rastreamento_de_erro_em_funcoes(self):
        codigo = """
função falhar
    devolva 1 / 0
fim

função chamar
    devolva falhar()
fim

teste "erro"
    resultado é chamar()
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        with self.assertRaises(ZeroDivisionError) as contexto:
            vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])

        erro = vm.formatar_erro(contexto.exception)
        self.assertIn("Rastreamento BR:", erro)
        self.assertIn("em falhar", erro)
        self.assertIn("em chamar", erro)

    def test_rastreamento_de_erro_em_metodo_de_classe(self):
        codigo = """
classe Caixa
    metodo quebrar
        devolva 1 / 0
    fim
fim

teste "erro metodo"
    caixa é Caixa()
    resultado é caixa.quebrar()
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        with self.assertRaises(ZeroDivisionError) as contexto:
            vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])

        erro = vm.formatar_erro(contexto.exception)
        self.assertIn("Rastreamento BR:", erro)
        self.assertIn("em Caixa.quebrar", erro)

    def test_pacote_compila_fonte_e_importa_brc_extraido(self):
        base = ROOT / "temp" / "pkg_toolchain"
        projeto = base / "projeto"
        instalacao = base / "modulos"
        pacote = base / "util_pkg.brp"
        shutil.rmtree(base, ignore_errors=True)
        try:
            (projeto / "fonte").mkdir(parents=True)
            projeto.joinpath("projeto.brm").write_text(
                'linguagem "BR 1.0"\n'
                'nome "util_pkg"\n'
                'versao "1.0.0"\n'
                'entrada "principal.brc"\n',
                encoding="utf-8",
            )
            projeto.joinpath("fonte", "principal.br").write_text(
                """
função dobro recebendo valor
    devolva valor * 2
fim
""",
                encoding="utf-8",
            )

            criar_pacote_projeto(str(projeto), str(pacote))
            leitor = LeitorPacoteBR(str(pacote))
            leitor.ler()
            self.assertEqual(leitor.manifesto.nome, "util_pkg")
            self.assertIn("principal.brc", leitor.arquivos_bytecode)

            leitor.extrair(str(instalacao / "util_pkg"))
            br_path_antigo = os.environ.get("BR_PATH")
            os.environ["BR_PATH"] = str(instalacao)
            try:
                caminho_modulo = resolver_arquivo_modulo("util_pkg")
                self.assertIsNotNone(caminho_modulo)
                self.assertEqual(caminho_modulo.name, "principal.brc")

                codigo = """
importar "util_pkg"

teste "importa pacote compilado"
    resultado é util_pkg.dobro(5)
    espere resultado igual a 10
fim
"""
                bytecode = gerar_bytecode(self.parsear(codigo))
                vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
                vm.executar()
                vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])
            finally:
                if br_path_antigo is None:
                    os.environ.pop("BR_PATH", None)
                else:
                    os.environ["BR_PATH"] = br_path_antigo
        finally:
            shutil.rmtree(base, ignore_errors=True)

    def test_brc_serializa_para_cada_conte_e_aguardar(self):
        codigo = """
tarefa dobro recebendo valor
    devolva valor * 2
fim

teste "opcodes"
    numeros é [1, 2]
    soma é 0
    para cada item em numeros
        soma é soma + item
    fim
    conte de 1 até 2 como i
        soma é soma + i
    fim
    promessa é dobro 3
    resultado é aguarde promessa
    espere soma + resultado igual a 12
fim
"""
        ast = self.parsear(codigo)
        bytecode = gerar_bytecode(ast)
        caminho = ROOT / "temp" / "prioridades_toolchain.brc"
        try:
            salvar_bytecode_brc1(bytecode, caminho)
            self.assertGreater(caminho.stat().st_size, 4)
        finally:
            caminho.unlink(missing_ok=True)

    def test_list_comprehension_gera_lista(self):
        codigo = """
teste "comprehension"
    numeros é [1, 2, 3, 4]
    dobros é [n * 2 para cada n em numeros]
    espere dobros[0] igual a 2
    espere dobros[3] igual a 8
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])

    def test_list_comprehension_com_filtro(self):
        codigo = """
teste "comprehension com filtro"
    numeros é [1, 2, 3, 4]
    filtrados é [n para cada n em numeros se n > 2]
    espere filtrados[0] igual a 3
    espere filtrados[1] igual a 4
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])

    def test_corresponda_com_guarda(self):
        codigo = """
teste "corresponda"
    valor é 3
    resultado é 0
    corresponda valor
        caso 3 se valor > 5
            resultado é 1
        fim
        caso 3
            resultado é 2
        fim
        padrao
            resultado é 9
        fim
    fim
    espere resultado igual a 2
fim
"""
        bytecode = gerar_bytecode(self.parsear(codigo))
        vm = MaquinaVirtualBR(bytecode, diretorio_base=str(ROOT))
        vm.executar()
        vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])

    def test_biblioteca_padrao_nativa_real(self):
        base = ROOT / "temp" / "stdlib_nativa"
        dados = base / "dados"
        arquivo = dados / "dados.txt"
        copia = dados / "copia.txt"
        movido = dados / "movido.txt"
        comando_ok = "cmd /c exit 0" if os.name == "nt" else "true"
        shutil.rmtree(base, ignore_errors=True)
        try:
            base.mkdir(parents=True)
            (base / "projeto.brm").write_text(
                "permissoes\n    arquivos\n    subprocesso\nfim\n",
                encoding="utf-8",
            )
            dados_br = str(dados).replace("\\", "/")
            arquivo_br = str(arquivo).replace("\\", "/")
            copia_br = str(copia).replace("\\", "/")
            movido_br = str(movido).replace("\\", "/")
            codigo = f"""
importar "arquivo"
importar "regex"
importar "sistema"

teste "arquivo regex e sistema reais"
    arquivo.criar_diretorio("{dados_br}")
    arquivo.escrever("{arquivo_br}", "abc123 xyz456")
    espere arquivo.existe("{arquivo_br}") igual a verdadeiro
    espere arquivo.arquivo("{arquivo_br}") igual a verdadeiro
    espere arquivo.tamanho("{arquivo_br}") igual a 13

    arquivo.copiar("{arquivo_br}", "{copia_br}")
    arquivo.mover("{copia_br}", "{movido_br}")
    espere arquivo.existe("{copia_br}") igual a falso
    espere arquivo.existe("{movido_br}") igual a verdadeiro
    itens é arquivo.listar("{dados_br}")
    espere itens.tamanho() igual a 2

    achados é regex.todos("[0-9]+", arquivo.ler("{arquivo_br}"))
    espere achados.tamanho() igual a 2
    espere regex.contar("[0-9]+", arquivo.ler("{arquivo_br}")) igual a 2
    partes é regex.dividir("[0-9]+", "a1b2")
    espere partes.tamanho() igual a 3

    espere sistema.executar("{comando_ok}") igual a 0
fim
"""
            bytecode = gerar_bytecode(self.parsear(codigo))
            vm = MaquinaVirtualBR(bytecode, diretorio_base=str(base))
            vm.executar()
            vm.executar_instrucoes(bytecode["testes"][0]["instrucoes"])
        finally:
            shutil.rmtree(base, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
