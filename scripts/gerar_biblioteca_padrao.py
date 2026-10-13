# -*- coding: utf-8 -*-
"""
Gera a estrutura inicial da biblioteca padrão BR em D:\br\lib.

O script é intencionalmente declarativo: cada módulo possui uma descrição,
funções públicas iniciais, contrato, documentação, exemplo e teste.
"""
from pathlib import Path
from textwrap import dedent


ROOT = Path(r"D:\br")
LIB = ROOT / "lib"
VERSION = "1.0.0"


MODULES = [
    ("core", "Núcleo da linguagem", "tipos, erros, memória, conversão e funções básicas", [
        ("identidade", ["valor"], "valor", "Devolve o próprio valor."),
        ("eh_nulo", ["valor"], "valor igual nulo", "Verifica se um valor é nulo."),
        ("para_texto", ["valor"], '"" + valor', "Converte um valor para texto."),
        ("verdade", [], "verdadeiro", "Constante lógica verdadeira."),
    ]),
    ("sistema", "Sistema operacional", "argumentos, variáveis de ambiente, processos e comandos", [
        ("plataforma", [], '"windows"', "Nome da plataforma padrão da instalação atual."),
        ("argumentos", [], '"[]"', "Representação textual dos argumentos do processo."),
        ("ambiente", ["chave"], '""', "Busca uma variável de ambiente."),
        ("executar", ["comando"], "0", "Executa um comando do sistema."),
    ]),
    ("arquivo", "Arquivos e pastas", "leitura, escrita e manipulação de arquivos e pastas", [
        ("ler", ["caminho"], '""', "Lê um arquivo de texto."),
        ("escrever", ["caminho", "conteudo"], "verdadeiro", "Escreve texto em arquivo."),
        ("existe", ["caminho"], "falso", "Verifica existência de caminho."),
        ("extensao", ["caminho"], '""', "Obtém a extensão do caminho."),
    ]),
    ("texto", "Texto e strings", "strings, regex, validações, formatação e acentos", [
        ("vazio", ["valor"], "valor igual \"\"", "Verifica texto vazio."),
        ("contem", ["valor", "trecho"], "falso", "Verifica presença de trecho."),
        ("slug", ["valor"], "valor", "Gera slug textual."),
        ("formatar", ["modelo", "valor"], "modelo + valor", "Formata texto simples."),
    ]),
    ("matematica", "Matemática", "operações básicas, avançadas, estatística e números aleatórios", [
        ("somar", ["a", "b"], "a + b", "Soma dois valores."),
        ("subtrair", ["a", "b"], "a - b", "Subtrai dois valores."),
        ("multiplicar", ["a", "b"], "a * b", "Multiplica dois valores."),
        ("dividir", ["a", "b"], "a / b", "Divide dois valores."),
    ]),
    ("colecoes", "Coleções", "listas, dicionários, conjuntos, filas, pilhas e funções funcionais", [
        ("tamanho", ["dados"], "0", "Retorna tamanho de coleção."),
        ("vazia", ["dados"], "verdadeiro", "Verifica se coleção está vazia."),
        ("primeiro", ["dados"], "nulo", "Retorna primeiro item."),
        ("ordenar", ["dados"], "dados", "Ordena uma coleção."),
    ]),
    ("json", "JSON", "parser, gerador e validador JSON", [
        ("validar", ["dados"], "verdadeiro", "Valida texto JSON."),
        ("gerar", ["dados"], '"{}"', "Gera texto JSON."),
        ("parsear", ["dados"], "dados", "Converte JSON em estrutura BR."),
        ("bonito", ["dados"], "dados", "Formata JSON para leitura."),
    ]),
    ("data_hora", "Datas e horários", "datas, horários, duração, calendário e fusos", [
        ("agora", [], '"2026-06-06T00:00:00"', "Retorna data/hora atual."),
        ("hoje", [], '"2026-06-06"', "Retorna data atual."),
        ("diferenca", ["inicio", "fim_valor"], "0", "Calcula duração entre datas."),
        ("fuso", [], '"America/Sao_Paulo"', "Retorna fuso padrão."),
    ]),
    ("rede", "Rede", "HTTP, URL, download, sockets, WebSocket e email", [
        ("get", ["url"], '""', "Executa requisição GET."),
        ("post", ["url", "dados"], '""', "Executa requisição POST."),
        ("baixar", ["url", "caminho"], "verdadeiro", "Baixa conteúdo para arquivo."),
        ("url_valida", ["url"], "verdadeiro", "Valida URL."),
    ]),
    ("banco", "Banco de dados", "SQLite, conexão, consultas, transações e migrações", [
        ("conectar", ["url"], "verdadeiro", "Abre conexão."),
        ("consulta", ["sql"], "sql", "Executa consulta."),
        ("transacao", [], "verdadeiro", "Inicia transação."),
        ("migrar", [], "verdadeiro", "Executa migrações."),
    ]),
    ("log", "Logs", "logger, níveis, arquivos de log, rotação e formatação", [
        ("info", ["mensagem"], "verdadeiro", "Registra mensagem INFO."),
        ("aviso", ["mensagem"], "verdadeiro", "Registra mensagem de aviso."),
        ("erro", ["mensagem"], "verdadeiro", "Registra erro."),
        ("nivel", ["valor"], "valor", "Define nível de log."),
    ]),
    ("testes", "Testes", "testes unitários, assertivas, mocks e cobertura", [
        ("igual", ["a", "b"], "a igual b", "Assertiva de igualdade."),
        ("eh_verdadeiro", ["valor"], "valor igual verdadeiro", "Assertiva verdadeira."),
        ("eh_falso", ["valor"], "valor igual falso", "Assertiva falsa."),
        ("mock", ["nome_item"], "nome_item", "Cria mock nomeado."),
    ]),
    ("pacote", "Pacotes", "instalação, remoção, atualização, manifesto, versões e dependências", [
        ("instalar", ["nome_item"], "verdadeiro", "Instala pacote."),
        ("remover", ["nome_item"], "verdadeiro", "Remove pacote."),
        ("atualizar", ["nome_item"], "verdadeiro", "Atualiza pacote."),
        ("versao", ["nome_item"], VERSION.join(['"', '"']), "Consulta versão."),
    ]),
    ("seguranca", "Segurança", "hash, Base64, bcrypt, JWT, permissões e sandbox", [
        ("hash", ["valor"], '"hash"', "Gera hash textual."),
        ("base64", ["valor"], "valor", "Codifica em Base64."),
        ("permitir", ["permissao"], "verdadeiro", "Verifica permissão."),
        ("jwt_validar", ["token"], "verdadeiro", "Valida token JWT."),
    ]),
    ("interface", "Interface de terminal", "terminal, cores, menus, perguntas, progresso e tabelas", [
        ("linha", ["mensagem"], "mensagem", "Monta linha de terminal."),
        ("perguntar", ["mensagem"], '""', "Solicita entrada do usuário."),
        ("progresso", ["valor"], "valor", "Exibe progresso."),
        ("tabela", ["dados"], "dados", "Renderiza tabela."),
    ]),
    ("xml", "XML", "ler, gerar, validar e consultar arquivos XML", [
        ("validar", ["dados"], "verdadeiro", "Valida XML."),
        ("ler", ["dados"], "dados", "Lê XML."),
        ("gerar", ["dados"], '"<xml></xml>"', "Gera XML."),
        ("consultar", ["dados", "caminho"], '""', "Consulta nó XML."),
    ]),
    ("csv", "CSV", "ler, escrever e transformar dados CSV", [
        ("ler", ["dados"], "dados", "Lê CSV."),
        ("escrever", ["dados"], "dados", "Gera CSV."),
        ("cabecalho", ["dados"], "dados", "Obtém cabeçalho."),
        ("linhas", ["dados"], "0", "Conta linhas."),
    ]),
    ("config", "Configuração", "carregar arquivos INI, ENV, YAML e TOML", [
        ("ini", ["dados"], "dados", "Carrega INI."),
        ("env", ["dados"], "dados", "Carrega ENV."),
        ("yaml", ["dados"], "dados", "Carrega YAML."),
        ("toml", ["dados"], "dados", "Carrega TOML."),
    ]),
    ("validacao", "Validação", "validar CPF, CNPJ, email, telefone, CEP e cartão", [
        ("cpf", ["valor"], "verdadeiro", "Valida CPF."),
        ("cnpj", ["valor"], "verdadeiro", "Valida CNPJ."),
        ("email", ["valor"], "verdadeiro", "Valida email."),
        ("cep", ["valor"], "verdadeiro", "Valida CEP."),
    ]),
    ("serializacao", "Serialização", "converter dados entre objeto, texto, binário e formatos externos", [
        ("para_texto", ["dados"], '"" + dados', "Serializa para texto."),
        ("de_texto", ["dados"], "dados", "Desserializa texto."),
        ("para_binario", ["dados"], "dados", "Serializa para binário."),
        ("clonar", ["dados"], "dados", "Clona estrutura."),
    ]),
    ("compressao", "Compressão", "compactar e descompactar ZIP, GZIP e TAR", [
        ("zip", ["caminho"], "caminho", "Compacta ZIP."),
        ("unzip", ["caminho"], "caminho", "Descompacta ZIP."),
        ("gzip", ["caminho"], "caminho", "Compacta GZIP."),
        ("tar", ["caminho"], "caminho", "Cria TAR."),
    ]),
    ("eventos", "Eventos", "criar emissores, ouvintes e filas de eventos", [
        ("emissor", ["nome_item"], "nome_item", "Cria emissor."),
        ("ouvir", ["nome_item"], "verdadeiro", "Registra ouvinte."),
        ("emitir", ["nome_item"], "verdadeiro", "Emite evento."),
        ("fila", [], '"fila"', "Cria fila de eventos."),
    ]),
    ("concorrencia", "Concorrência", "threads, tarefas assíncronas e promessas", [
        ("tarefa", ["nome_item"], "nome_item", "Cria tarefa."),
        ("aguardar", ["valor"], "valor", "Aguarda resultado."),
        ("promessa", ["valor"], "valor", "Cria promessa."),
        ("em_paralelo", ["valor"], "valor", "Executa em paralelo."),
    ]),
    ("memoria", "Memória", "monitorar uso de memória, referências e coleta de lixo", [
        ("uso", [], "0", "Mede uso de memória."),
        ("coletar", [], "verdadeiro", "Solicita coleta de lixo."),
        ("referencias", ["valor"], "0", "Conta referências."),
        ("limite", [], "0", "Consulta limite de memória."),
    ]),
    ("introspeccao", "Introspecção", "inspecionar objetos, módulos, tipos e funções", [
        ("tipo", ["valor"], '"desconhecido"', "Inspeciona tipo."),
        ("funcoes", ["valor"], '"[]"', "Lista funções."),
        ("campos", ["valor"], '"[]"', "Lista campos."),
        ("modulo", ["valor"], "valor", "Inspeciona módulo."),
    ]),
    ("depuracao", "Depuração", "breakpoints, pilha de chamadas, rastreamento e inspeção", [
        ("breakpoint", ["linha"], "linha", "Cria breakpoint."),
        ("pilha", [], '"[]"', "Obtém pilha."),
        ("rastrear", ["valor"], "valor", "Rastreia valor."),
        ("inspecionar", ["valor"], "valor", "Inspeciona variável."),
    ]),
    ("performance", "Performance", "profiler, benchmark, tempo de execução e memória", [
        ("tempo", ["nome_item"], "0", "Mede tempo."),
        ("benchmark", ["nome_item"], "0", "Executa benchmark."),
        ("perfil", ["nome_item"], "nome_item", "Executa profiling."),
        ("memoria", [], "0", "Mede memória."),
    ]),
    ("ffi", "FFI", "integração com C, C++, DLLs, Python, JavaScript e Shell", [
        ("dll", ["caminho"], "caminho", "Carrega DLL."),
        ("python", ["codigo"], "codigo", "Executa Python."),
        ("javascript", ["codigo"], "codigo", "Executa JavaScript."),
        ("shell", ["comando"], "comando", "Executa shell."),
    ]),
    ("internacionalizacao", "Internacionalização", "idiomas, tradução, moeda, números e localização", [
        ("idioma", [], '"pt-BR"', "Consulta idioma."),
        ("traduzir", ["valor"], "valor", "Traduz texto."),
        ("moeda", ["valor"], '"R$ " + valor', "Formata moeda."),
        ("localizacao", [], '"Brasil"', "Consulta localização."),
    ]),
    ("template", "Templates", "renderizar templates de texto, HTML e código", [
        ("renderizar", ["modelo", "dados"], "modelo", "Renderiza template."),
        ("html", ["modelo"], "modelo", "Renderiza HTML."),
        ("codigo", ["modelo"], "modelo", "Renderiza código."),
        ("variavel", ["nome_item"], "nome_item", "Cria variável de template."),
    ]),
    ("yaml", "YAML", "leitura, escrita e validação de arquivos YAML", [
        ("ler", ["dados"], "dados", "Lê conteúdo YAML."),
        ("escrever", ["dados"], "dados", "Gera texto YAML."),
        ("validar", ["dados"], "verdadeiro", "Valida sintaxe YAML."),
        ("chave", ["dados", "nome_item"], "nome_item", "Consulta chave YAML."),
    ]),
    ("toml", "TOML", "leitura, escrita e validação de arquivos TOML", [
        ("ler", ["dados"], "dados", "Lê conteúdo TOML."),
        ("escrever", ["dados"], "dados", "Gera texto TOML."),
        ("validar", ["dados"], "verdadeiro", "Valida sintaxe TOML."),
        ("secao", ["dados", "nome_item"], "nome_item", "Consulta seção TOML."),
    ]),
    ("ini", "INI", "leitura, escrita e edição de arquivos INI", [
        ("ler", ["dados"], "dados", "Lê conteúdo INI."),
        ("escrever", ["dados"], "dados", "Gera texto INI."),
        ("editar", ["dados", "chave", "valor"], "dados", "Edita uma chave INI."),
        ("secao", ["dados", "nome_item"], "nome_item", "Consulta seção INI."),
    ]),
    ("env", "ENV", "leitura e gerenciamento de variáveis de ambiente", [
        ("ler", ["chave"], '""', "Lê variável de ambiente."),
        ("definir", ["chave", "valor"], "verdadeiro", "Define variável de ambiente."),
        ("remover", ["chave"], "verdadeiro", "Remove variável de ambiente."),
        ("listar", [], '"[]"', "Lista variáveis de ambiente."),
    ]),
    ("path", "Path", "manipulação de caminhos, diretórios e extensões", [
        ("juntar", ["base", "nome_item"], "base + nome_item", "Junta partes de caminho."),
        ("diretorio", ["caminho"], "caminho", "Obtém diretório de um caminho."),
        ("extensao", ["caminho"], '""', "Obtém extensão de arquivo."),
        ("normalizar", ["caminho"], "caminho", "Normaliza separadores de caminho."),
    ]),
    ("uuid", "UUID", "geração e validação de identificadores únicos", [
        ("gerar", [], '"00000000-0000-4000-8000-000000000000"', "Gera UUID."),
        ("validar", ["valor"], "verdadeiro", "Valida UUID."),
        ("v4", [], '"00000000-0000-4000-8000-000000000000"', "Gera UUID v4."),
        ("curto", [], '"00000000"', "Gera identificador curto."),
    ]),
    ("regex", "Regex", "expressões regulares e busca avançada em texto", [
        ("buscar", ["padrao_texto", "valor"], "valor", "Busca padrão em texto."),
        ("substituir", ["padrao_texto", "novo", "valor"], "valor", "Substitui padrões em texto."),
        ("validar", ["padrao_texto", "valor"], "verdadeiro", "Valida texto por expressão regular."),
        ("grupos", ["padrao_texto", "valor"], '"[]"', "Retorna grupos encontrados."),
    ]),
    ("senha", "Senha", "geração, validação e força de senhas", [
        ("gerar", [], '"senha-segura"', "Gera senha."),
        ("validar", ["valor"], "verdadeiro", "Valida senha."),
        ("forca", ["valor"], "0", "Calcula força da senha."),
        ("hash", ["valor"], '"hash"', "Gera hash de senha."),
    ]),
    ("token", "Token", "geração e validação de tokens simples", [
        ("gerar", [], '"token"', "Gera token simples."),
        ("validar", ["valor"], "verdadeiro", "Valida token."),
        ("expirar", ["valor"], "verdadeiro", "Expira token."),
        ("renovar", ["valor"], "valor", "Renova token."),
    ]),
    ("permissao", "Permissão", "controle de permissões e acesso", [
        ("permitir", ["acao"], "verdadeiro", "Permite ação."),
        ("negar", ["acao"], "falso", "Nega ação."),
        ("verificar", ["acao"], "verdadeiro", "Verifica permissão."),
        ("papel", ["nome_item"], "nome_item", "Define papel de acesso."),
    ]),
    ("cli", "CLI", "criação de comandos, flags e argumentos de terminal", [
        ("comando", ["nome_item"], "nome_item", "Cria comando de terminal."),
        ("flag", ["nome_item"], "nome_item", "Define flag de comando."),
        ("argumento", ["nome_item"], "nome_item", "Define argumento de comando."),
        ("executar_cli", ["nome_item"], "0", "Executa comando CLI."),
    ]),
    ("repl", "REPL", "console interativo da linguagem", [
        ("iniciar", [], "verdadeiro", "Inicia console interativo."),
        ("avaliar", ["codigo"], "codigo", "Avalia código BR."),
        ("historico", [], '"[]"', "Retorna histórico do console."),
        ("sair", [], "verdadeiro", "Finaliza sessão REPL."),
    ]),
    ("lsp", "LSP", "suporte para editor, autocomplete e diagnóstico", [
        ("iniciar", [], "verdadeiro", "Inicia servidor LSP."),
        ("autocomplete", ["codigo"], '"[]"', "Retorna sugestões de autocomplete."),
        ("diagnostico", ["codigo"], '"[]"', "Retorna diagnósticos do código."),
        ("simbolo", ["codigo"], '"[]"', "Lista símbolos do documento."),
    ]),
    ("formatador", "Formatador", "formatação automática de código .br", [
        ("formatar", ["codigo"], "codigo", "Formata código BR."),
        ("arquivo", ["caminho"], "caminho", "Formata arquivo BR."),
        ("verificar", ["codigo"], "verdadeiro", "Verifica se o código está formatado."),
        ("configurar", ["opcoes"], "opcoes", "Configura regras de formatação."),
    ]),
    ("linter", "Linter", "análise estática e avisos de qualidade", [
        ("analisar", ["codigo"], '"[]"', "Analisa código BR."),
        ("avisos", ["codigo"], '"[]"', "Retorna avisos de qualidade."),
        ("regra", ["nome_item"], "nome_item", "Ativa regra de lint."),
        ("corrigir", ["codigo"], "codigo", "Aplica correções simples."),
    ]),
    ("docgen", "Docgen", "geração automática de documentação", [
        ("gerar", ["projeto"], "projeto", "Gera documentação do projeto."),
        ("modulo", ["nome_item"], "nome_item", "Documenta módulo."),
        ("html", ["projeto"], "projeto", "Gera documentação HTML."),
        ("markdown", ["projeto"], "projeto", "Gera documentação Markdown."),
    ]),
    ("build", "Build", "sistema de build e empacotamento", [
        ("construir", ["projeto"], "verdadeiro", "Executa build do projeto."),
        ("limpar", ["projeto"], "verdadeiro", "Limpa artefatos de build."),
        ("empacotar", ["projeto"], "projeto", "Empacota projeto."),
        ("artefato", ["nome_item"], "nome_item", "Define artefato de saída."),
    ]),
    ("instalador", "Instalador", "criação de instaladores para projetos .br", [
        ("criar", ["projeto"], "verdadeiro", "Cria instalador."),
        ("windows", ["projeto"], "projeto", "Cria instalador Windows."),
        ("linux", ["projeto"], "projeto", "Cria instalador Linux."),
        ("manifesto", ["projeto"], "projeto", "Gera manifesto de instalação."),
    ]),
    ("migracao", "Migração", "migração de versões, APIs e projetos", [
        ("migrar", ["projeto"], "verdadeiro", "Migra projeto."),
        ("versao", ["projeto"], '"1.0.0"', "Consulta versão de migração."),
        ("api", ["nome_item"], "nome_item", "Migra API."),
        ("relatorio", ["projeto"], "projeto", "Gera relatório de migração."),
    ]),
    ("scaffold", "Scaffold", "criação automática de projetos e templates", [
        ("projeto", ["nome_item"], "nome_item", "Cria projeto BR."),
        ("arquivo", ["nome_item"], "nome_item", "Cria arquivo BR."),
        ("template", ["nome_item"], "nome_item", "Aplica template."),
        ("estrutura", ["nome_item"], "nome_item", "Cria estrutura de diretórios."),
    ]),
]


RESERVED_IDENTIFIERS = {
    "booleano",
    "caso",
    "classe",
    "como",
    "constante",
    "cada",
    "capture",
    "de",
    "decimal",
    "devolva",
    "em",
    "enquanto",
    "entao",
    "execute",
    "explicacao",
    "falso",
    "fim",
    "funcao",
    "import",
    "importar",
    "importe",
    "inteiro",
    "lista",
    "mostre",
    "nulo",
    "objetivo",
    "padrao",
    "para",
    "paralelo",
    "recebendo",
    "registro",
    "remova",
    "repita",
    "salve",
    "se",
    "senao",
    "texto",
    "teste",
    "tente",
    "verdadeiro",
    "vezes",
}


def br_params(params):
    return "" if not params else " recebendo " + " e ".join(params)


def write(path: Path, content: str):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content.strip() + "\n", encoding="utf-8")


def module_alias(name):
    return f"modulo_{name}" if name in RESERVED_IDENTIFIERS else name


def import_source(name):
    alias = module_alias(name)
    if alias == name:
        return f'importar "{name}"'
    return f'importar "{name}" como {alias}'


def module_source(name, title, description, apis):
    lines = [
        "explicacao",
        f"    {title}: {description}.",
        "    Este arquivo faz parte da biblioteca padrao BR.",
        "fim",
        "",
        "função nome",
        f"    devolva \"{name}\"",
        "fim",
        "",
        "função descricao",
        f"    devolva \"{title}: {description}\"",
        "fim",
        "",
        "função versao",
        f"    devolva \"{VERSION}\"",
        "fim",
    ]
    for func, params, body, _doc in apis:
        lines.extend([
            "",
            f"função {func}{br_params(params)}",
            f"    devolva {body}",
            "fim",
        ])
    return "\n".join(lines)


def api_source(name, title, apis):
    lines = [
        "explicacao",
        f"    API publica complementar do modulo {name}.",
        "fim",
        "",
        "função status",
        f"    devolva \"{name}: pronto\"",
        "fim",
        "",
        "função titulo",
        f"    devolva \"{title}\"",
        "fim",
    ]
    if apis:
        func, _params, _body, _doc = apis[0]
        lines.extend([
            "",
            "função primeira_funcao",
            f"    devolva \"{func}\"",
            "fim",
        ])
    return "\n".join(lines)


def contract_source(name, apis):
    lines = [f"contrato {name}", "    função nome", "    função descricao", "    função versao"]
    for func, params, _body, _doc in apis:
        suffix = br_params(params)
        lines.append(f"    função {func}{suffix}")
    lines.append("fim")
    return "\n".join(lines)


def readme_source(name, title, description, apis):
    api_lines = "\n".join(
        f"- `{func}({', '.join(params)})`: {doc}" for func, params, _body, doc in apis
    )
    return f"""
# {title}

Módulo `{name}` da biblioteca padrão BR.

## Escopo

Este módulo cobre {description}.

## Arquivos

- `{name}.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `{name}.bri`: contrato público.
- `{name}.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/{name}_teste.brt`: testes do módulo.

## API Inicial

{api_lines}
"""


def brd_source(name, title, description, apis):
    api_lines = "\n".join(
        f"* {func}: {doc}" for func, _params, _body, doc in apis
    )
    return f"""
# Documentação BRD: {name}

## Nome

{title}

## Descrição

{description}.

## Funções

* nome: retorna o nome canônico do módulo.
* descricao: retorna descrição curta.
* versao: retorna versão da biblioteca.
{api_lines}
"""


def example_source(name):
    alias = module_alias(name)
    return f"""
{import_source(name)}

mostre {alias}.nome()
mostre {alias}.descricao()
mostre {alias}.versao()
"""


def test_source(name):
    alias = module_alias(name)
    return f"""
{import_source(name)}

teste "nome do modulo {name}"
    espere {alias}.nome() igual a "{name}"
fim
"""


def main():
    LIB.mkdir(parents=True, exist_ok=True)
    for name, title, description, apis in MODULES:
        base = LIB / name
        write(base / f"{name}.br", module_source(name, title, description, apis))
        write(base / "api.br", api_source(name, title, apis))
        write(base / f"{name}.bri", contract_source(name, apis))
        write(base / f"{name}.brd", brd_source(name, title, description, apis))
        write(base / "README.md", readme_source(name, title, description, apis))
        write(base / "exemplos" / "basico.br", example_source(name))
        write(base / "testes" / f"{name}_teste.brt", test_source(name))

    indice = "\n".join(f"- [{title}]({name}/README.md) (`{name}`)" for name, title, _desc, _apis in MODULES)
    write(LIB / "README.md", f"""
# Biblioteca Padrão BR

Esta pasta contém os módulos oficiais iniciais da linguagem BR.

## Módulos

{indice}
""")

    print(f"Biblioteca padrao gerada em {LIB}")
    print(f"Modulos: {len(MODULES)}")


if __name__ == "__main__":
    main()
