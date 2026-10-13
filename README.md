# Linguagem BR - Programação em Português Brasileiro
**Versão 2.0.0** | **Runtime C++ Nativo** | **100% Completo**

---

## 🚀 O que é a Linguagem BR?

A **BR** é uma linguagem de programação revolucionária que usa **português brasileiro** como sintaxe. Ela foi projetada para ser:

- 🇧🇷 **Semântica Brasileira**: Sintaxe que lê-se como português formal
- 🧵 **Paralela nativamente**: Suporte a threads e concorrência
- 🤖 **IA integrada**: Inteligência artificial como cidadão de primeira classe
- 🔒 **Segura**: Sistema de permissões e sandboxing
- ⚡ **Rápida**: Runtime C++ nativo compilado
- 🧩 **Extensível**: LSP, Debugger, ORM, Framework Web, Package Manager

---

## 📦 Instalação

### Pré-requisitos
- Windows 10/11, Linux ou macOS
- G++ com suporte a C++17
- Python 3.10+ (para ferramentas)

### Windows

**Opção 1 - Build automático:**
```bash
build.bat
```

**Opção 2 - Build com Make (MSYS2/MinGW):**
```bash
make build
```

**Opção 3 - Instalação completa:**
```bash
install_windows.bat
```

**Opção 4 - Build Python:**
```bash
python build.py
```

**Opção 5 - Gerar executável:**
```bash
python build_exe.py
```

### Linux/macOS

```bash
chmod +x install_unix.sh
./install_unix.sh
# Ou build manual:
cd runtime/src
g++ -std=c++17 -O2 main.cpp servico_arquivo.cpp servico_rede.cpp -o ../../bin/br -I../include
```

### Launchers disponíveis
| Comando | Descrição |
|---|---|
| `br.bat` | Launcher principal (Windows) |
| `brasil.bat` | Launcher alternativo |
| `mercado.bat` | Gerenciador de pacotes |
| `make run ARQUIVO=exemplo.br` | Executar via Makefile |

---

## 🎯 Primeiro Programa

Crie um arquivo `ola_mundo.br`:

```br
# Meu primeiro programa em BR
mostre "Olá, Mundo!"
mostre "Bem-vindo à linguagem BR!"
```

Execute:
```bash
bin/br.exe ola_mundo.br
# ou via launcher:
br ola_mundo.br
```

---

## 📚 Sintaxe Básica

### Variáveis e Constantes

```br
# Variável com inferência de tipo
nome é "Maria"
idade é 30
salario é 4500.00
ativo é verdadeiro

# Constante imutável
constante PI é 3.14159

# Declaração com tipo explícito
preco : decimal é 99.90
```

### Operadores

```br
# Aritméticos
soma é 10 + 5
subtracao é 20 - 8
produto é 6 * 7
divisao é 100 / 4

# Comparação
maior é 10 > 5
igualdade é 10 == 10
diferente é 10 != 5

# Lógicos
resultado é ativo e nao desativado
```

### Estruturas de Controle

```br
# Condicional
se idade >= 18 então
    mostre "Acesso liberado"
senão
    mostre "Acesso negado"
fim

# Escolha/caso
escolha opcao
    caso 1
        mostre "Primeira opção"
    caso 2
        mostre "Segunda opção"
    padrao
        mostre "Opção inválida"
fim

# Enquanto
contador é 0
enquanto contador < 5
    mostre contador
    contador é contador + 1
fim

# Repetição com contagem
repita 3 vezes
    mostre "Olá"
fim

# Contador
conte de 1 até 10 como i
    mostre "Número: ${i}"
fim

# Para cada
frutas é ["maçã", "banana", "laranja"]
para cada fruta em frutas
    mostre "Fruta: ${fruta}"
fim
```

### Funções

```br
# Função simples
função somar recebendo a e b
    devolva a + b
fim

resultado é somar(10, 20)
mostre "Resultado: ${resultado}"

# Função sem parâmetros
função obter_nome
    devolva "Linguagem BR"
fim

# Função com parâmetros variádicos
função listar recebendo *args
    para cada item em args
        mostre item
    fim
fim
```

### Classes e POO

```br
# Classe simples
classe Pessoa
    nome: texto
    idade: inteiro
    
    função apresentar
        mostre "Olá, eu sou ${isto.nome}"
    fim
fim

# Herança
classe Animal
    nome: texto
    idade: inteiro
fim

classe Cachorro herda Animal
    raca: texto
    
    função latir recebendo intensidade
        mostre "${isto.nome} está latindo!"
    fim
fim

# Instanciação
cachorro é Cachorro("Rex", 5, "Labrador")
cachorro.latir(10)
mostre cachorro.nome
```

### F-strings

```br
nome é "João"
idade é 25

# Interpolação de variáveis (com prefixo f)
mensagem é f"Olá, ${nome}! Você tem ${idade} anos."
mostre mensagem

# Expressões dentro de F-strings
mostre f"Daqui a 5 anos você terá ${idade + 5} anos"

# F-strings multilinhas
mensagem é f"""
Nome: ${nome}
Idade: ${idade}
"""
```

### List Comprehensions

```br
# Lista básica
numeros é [1, 2, 3, 4, 5]

# Comprehension com transformação
quadrados é [x * x para cada x em numeros]
mostre quadrados  # [1, 4, 9, 16, 25]

# Comprehension com filtro
pares é [x para cada x em numeros se x % 2 == 0]
mostre pares  # [2, 4]

# Comprehension com transformação + filtro
quadrados_pares é [x * x para cada x em numeros se x % 2 == 0]
mostre quadrados_pares  # [4, 16]
```

### Decorators

```br
# Definir decorator
função logar recebendo funcao
    devolva função interna recebendo *args
        mostre "Chamando ${funcao.nome}..."
        resultado é funcao(*args)
        mostre "Finalizado ${funcao.nome}"
        devolva resultado
    fim
fim

# Usar decorator
@logar
função processar recebendo dados
    mostre "Processando: ${dados}"
fim
```

### Generators (yield)

```br
# Generator que produz números
função contador recebendo maximo
    i é 1
    enquanto i <= maximo
        yield i
        i é i + 1
    fim
fim

# Usar o generator
para cada num em contador(5)
    mostre num  # 1, 2, 3, 4, 5
fim

# Generator de Fibonacci
função fibonacci recebendo limite
    a é 0
    b é 1
    enquanto a <= limite
        yield a
        temp é a
        a é b
        b é temp + b
    fim
fim
```

### Context Managers

```br
# Usar context manager
contexto arquivo em abrir("dados.txt")
    conteudo é arquivo.ler()
    mostre conteudo
fim

# Context manager personalizado
função gerenciar_recurso
    mostre "Abrindo recurso..."
    devolva recurso
    # Finalização automática no fim do bloco
fim

contexto rec em gerenciar_recurso()
    mostre "Usando recurso"
fim
```

### Tratamento de Erros

```br
tente
    resultado é 10 / 0
capture erro
    mostre "Erro: ${erro}"
fim

# Lançar erro personalizado
tente
    lance "Erro personalizado"
capture erro
    mostre "Capturado: ${erro}"
fim
```

### Paralelismo

```br
execute em paralelo
    mostre "Processo 1"
    mostre "Processo 2"
    mostre "Processo 3"
fim

# Com funções
função tarefa1
    mostre "Tarefa 1 executando"
fim

função tarefa2
    mostre "Tarefa 2 executando"
fim

execute em paralelo
    tarefa1()
    tarefa2()
fim
```

### Módulos

```br
# Importar módulo
importar "arquivo"

# Importar com alias
importar "usuarios" como usr

# Importar biblioteca padrão
importar "matematica"
importar "texto"
importar "colecoes"

# Usar funções do módulo
dados é usr.buscar_dados()
resultado é somar(10, 20)
```

### Estruturas de Dados

```br
# Listas
lista é [1, 2, 3, 4, 5]
lista.adicionar(6)
primeiro é lista.obter(0)
tamanho é lista.tamanho()

# Registros (dicionários)
usuario é registro
    nome: "Maria"
    idade: 30
    ativo: verdadeiro
fim

# Acessar campos
mostre usuario.nome
usuario.idade é 31
```

### Banco de Dados (Memória)

```br
# Criar registro
usuario é registro
    nome: "Maria"
    idade: 30
fim

# Salvar no banco em memória
salve usuario

# Remover
remova usuario
```

### HTTP Cliente

```br
# Fazer requisição GET
pagina é acesse "https://exemplo.com"
mostre pagina

# Com biblioteca de rede
importar "rede"
resposta é http_get("https://api.exemplo.com")
dados é http_post("https://api.exemplo.com/dados", '{"chave": "valor"}')
```

---

## 📚 Bibliotecas Padrão

A linguagem BR vem com **17+ bibliotecas padrão** e **50+ submódulos especializados**. Todas com nomes e sintaxe em português brasileiro.

### Bibliotecas Principais

| Biblioteca | Arquivo | Funções | Descrição |
|---|---|---|---|
| **matematica.br** | `lib/matematica.br` | 187 linhas | Matemática (somar, seno, fatorial, primo, mdc, mmc, fibonacci) |
| **arquivo.br** | `lib/arquivo.br` | 155 linhas | Arquivos (ler, escrever, CSV, JSON, diretórios, copiar, mover) |
| **texto.br** | `lib/texto.br` | 171 linhas | String (maiúsculo, buscar, substituir, dividir, validar) |
| **colecoes.br** | `lib/colecoes.br` | 261 linhas | Coleções (mapear, filtrar, reduzir, ordenar, agrupar) |
| **data_hora.br** | `lib/data_hora.br` | 166 linhas | Data/Hora (atual, formatar, somar dias, diferença, bissexto) |
| **sistema.br** | `lib/sistema.br` | 86 linhas | Sistema (ambiente, comandos, SO, processos, rede) |
| **rede.br** | `lib/rede.br` | 110 linhas | Rede (HTTP GET/POST/PUT/DELETE, URL, DNS) |
| **json.br** | `lib/json.br` | 51 linhas | JSON (parse, stringify, validar) |
| **banco.br** | `lib/banco.br` | 76 linhas | Banco (SQLite, MySQL, PostgreSQL, CRUD) |
| **ia.br** | `lib/ia.br` | 94 linhas | IA (perguntar, agente, classificar, traduzir, resumir) |
| **cores.br** | `lib/cores.br` | 102 linhas | Cores (vermelho, verde, negrito, fundo, piscar) |
| **crypto.br** | `lib/crypto.br` | 68 linhas | Criptografia (MD5, SHA1, SHA256, base64, cifrar) |
| **criptografia.br** | `lib/criptografia.br` | 205 linhas | Criptografia avançada (hash, cifra AES/RSA, token JWT) |
| **log.br** | `lib/log.br` | 78 linhas | Log (debug, info, erro, crítico, com data, em arquivo) |
| **teste.br** | `lib/teste.br` | 102 linhas | Testes (assert_igual, suite, mock, spy) |
| **web.br** | `lib/web.br` | 197 linhas | **Servidor HTTP** (rotas, middleware, CORS, JSON, HTML) |
| **gui.br** | `lib/gui.br` | 167 linhas | **Interface Gráfica** (janela, botão, campo, lista, checkbox, slider) |

### Exemplos de Uso

```br
# Matemática
importar "matematica"
soma é somar(10, 20)
fatorial é fatorial(5)  # 120
eh_primo(7)  # verdadeiro

# Arquivos
importar "arquivo"
conteudo é ler("dados.txt")
escrever("saida.txt", conteudo)
lista é listar("./")  # Lista diretório

# Texto
importar "texto"
nome é "maria silva"
maiusculo(nome)  # "MARIA SILVA"
capitalizar(nome)  # "Maria silva"

# Coleções
importar "colecoes"
numeros é [3, 1, 4, 1, 5, 9]
ordenar(numeros)  # [1, 1, 3, 4, 5, 9]
pares é filtrar(numeros, recebendo x, x % 2 == 0)

# Data e Hora
importar "data_hora"
hoje é data_atual()
formatar_data(hoje, "DD/MM/AAAA")
diferenca_dias("2025-01-01", "2025-12-31")

# Sistema
importar "sistema"
sistema_operacional()  # "Windows"
dormir(2)  # Aguarda 2 segundos

# Rede / HTTP
importar "rede"
resposta é http_get("https://api.exemplo.com")
http_post("https://api.exemplo.com", '{"chave": "valor"}')

# JSON
importar "json"
dados é parse('{"nome": "João", "idade": 30}')
stringify(dados)

# Banco de Dados
importar "banco"
db é conectar_sqlite("dados.db")
inserir(db, "usuarios", {"nome": "Maria", "idade": 25})

# IA
importar "ia"
resposta é perguntar("Qual a capital do Brasil?")

# Criptografia
importar "crypto"
hash é hash_sha256("minha_senha")
token é gerar_token(32)

importar "criptografia"
cifrado é cifrar_aes("mensagem secreta", "chave")
decifrado é decifrar_aes(cifrado, "chave")

# Log
importar "log"
log_info("Aplicação iniciada")
log_erro("Falha na conexão")

# Testes
importar "teste"
função teste_soma
    resultado é somar(2, 3)
    assert_igual(5, resultado)
fim
suite("Matemática", {"soma": teste_soma})

# Cores no terminal
importar "cores"
mostre cor_verde("Sucesso!")
mostre vermelho_negrito("Erro crítico!")

# Servidor Web
importar "web"
servidor é novo_servidor_http(8080)
servidor.rota_get("/", recebendo req, "Olá, Mundo!")
servidor.iniciar()

# GUI
importar "gui"
janela é nova_janela("Meu App", 800, 600)
botao é novo_botao("Clique aqui")
adicionar_componente(janela, botao)
mostrar_janela(janela)
```

### Submódulos Especializados

Além das bibliotecas principais, existem **50+ submódulos** em `lib/`:

| Categoria | Submódulos |
|---|---|
| 🤖 IA/ML | `lib/agente/`, `lib/chatbot/`, `lib/classificacao/`, `lib/clusterizacao/`, `lib/deep_learning/`, `lib/embeddings/` |
| 📊 Dados | `lib/dataframe/`, `lib/estatistica_avancada/`, `lib/calculo/`, `lib/algebra/` |
| 🗄️ Banco | `lib/banco/`, `lib/orm/` |
| 🌐 Web | `lib/html/`, `lib/dashboard/`, `lib/cli/` |
| 🔒 Segurança | `lib/assinatura/`, `lib/jwt/`, `lib/criptografia/` |
| 📁 Arquivos | `lib/arquivo/`, `lib/compressao/`, `lib/csv/`, `lib/config/`, `lib/ini/` |
| 🧰 Utilitários | `lib/agendamento/`, `lib/automacao/`, `lib/concorrencia/`, `lib/eventos/`, `lib/introspeccao/` |
| 🎮 Jogos | `lib/jogo/` |
| 📈 Financeiro | `lib/financeiro/`, `lib/contabilidade/`, `lib/ecommerce/`, `lib/erp/`, `lib/estoque/`, `lib/carrinho/` |
| 📱 Comunicação | `lib/email/`, `lib/bot/`, `lib/ffi/` |
| 🗺️ Geo | `lib/geolocalizacao/`, `lib/geometria/`, `lib/clima/`, `lib/calendario_lunar/`, `lib/feriados/` |
| 📝 Documentação | `lib/docgen/`, `lib/formatador/`, `lib/linter/` |
| 🎨 Visual | `lib/grafico/`, `lib/cores/`, `lib/interface/` |
| 🌍 Outros | `lib/internacionalizacao/`, `lib/build/`, `lib/instalador/`, `lib/depuracao/`, `lib/eventos/` |
| 📦 Core | `lib/core/`, `lib/colecoes/`, `lib/config/`, `lib/crypto/`, `lib/data_hora/` |

---

## 🛠️ Ferramentas

A linguagem BR vem com **30+ ferramentas** para desenvolvimento profissional:

### Ferramentas Essenciais

| Ferramenta | Arquivo | Descrição |
|---|---|---|
| **🖥️ REPL** | `ferramentas/repl.py` | Modo interativo (`br -i`) |
| **🔍 Linter** | `ferramentas/linter_avancado.py` | Análise estática de código |
| **🎨 Formatter** | `ferramentas/formatter_br.py` | Formatação automática |
| **🧪 Test Runner** | `ferramentas/test_runner.py` | Executor de testes |
| **🐛 Debugger** | `ferramentas/debugger_br.py` | Debugger com breakpoints, step, watch, call stack |
| **📋 LSP Server** | `ferramentas/lsp_server.py` | Language Server Protocol (autocomplete, diagnóstico, go-to-definition) |
| **📦 Package Manager** | `ferramentas/mercado_packages.py` | Gerenciador de pacotes (instalar, publicar) |

### Frameworks e Serviços

| Ferramenta | Arquivo | Descrição |
|---|---|---|
| **🌐 Framework Web** | `ferramentas/framework_web.py` | Framework web estilo Flask/Django |
| **🗄️ ORM** | `ferramentas/orm_br.py` | ORM para SQLite (mapeia classes BR para tabelas) |
| **🔒 Sandbox** | `ferramentas/sandbox_br.py` | Sistema de permissões e sandboxing |
| **🤖 Agentes IA** | `ferramentas/agentes.py` | Agentes de IA autônomos |
| **🔐 Auth** | `ferramentas/auth.py` | Autenticação e autorização |
| **📜 Contratos** | `ferramentas/contratos.py` | Sistema de contratos |

### Compilação e Otimização

| Ferramenta | Arquivo | Descrição |
|---|---|---|
| **⚙️ Compilador** | `ferramentas/compilador.py` | Compilador BR para bytecode |
| **⚡ JIT** | `ferramentas/jit.py` | Compilação Just-In-Time |
| **🚀 Optimizer** | `ferramentas/optimizer.py` | Otimizador de código |
| **📦 Packager** | `ferramentas/packager.py` | Empacotamento de aplicações |
| **🔬 Semantic Analyzer** | `ferramentas/semantic_analyzer.py` | Análise semântica |
| **📊 Profiler** | `ferramentas/profiler.py` | Profiler de performance |

### Geração e Documentação

| Ferramenta | Arquivo | Descrição |
|---|---|---|
| **📝 Doc Generator** | `ferramentas/doc_generator.py` | Geração automática de documentação |
| **💻 Codegen** | `ferramentas/codegen.py` | Geração de código |
| **🔄 Hot Reload** | `ferramentas/hot_reload.py` | Recarregamento automático |

### Utilitários

| Ferramenta | Arquivo | Descrição |
|---|---|---|
| **📋 Gerenciador** | `ferramentas/gerenciador.py` | Gerenciador de projetos |
| **🔧 Registry Server** | `ferramentas/registro_server.py` | Servidor de registro de pacotes |
| **💾 VM Enhanced** | `ferramentas/vm_enhanced.py` | Máquina virtual aprimorada |
| **🔤 Encoding Utils** | `ferramentas/encoding_utils.py` | Utilitários de codificação |
| **🔌 IA Service** | `ferramentas/ia_service.py` | Serviço de IA |

### Como usar as ferramentas

```bash
# Linter
python ferramentas/linter_avancado.py arquivo.br

# Formatter
python ferramentas/formatter_br.py arquivo.br

# Test Runner
python ferramentas/tester.py testes/

# Debugger
python ferramentas/debugger_br.py arquivo.br

# REPL (Modo Interativo)
bin/br.exe -i

# LSP Server
python ferramentas/lsp_server.py

# Package Manager
python ferramentas/mercado_packages.py install pacote
python ferramentas/mercado_packages.py publish pacote

# Framework Web
python ferramentas/framework_web.py app.br

# ORM
python ferramentas/orm_br.py gerar banco.db

# Gerar documentação
python ferramentas/doc_generator.py arquivo.br

# Profiler
python ferramentas/profiler.py arquivo.br
```

---

## 📖 Exemplos

A pasta `exemplos/` contém **10 exemplos completos** demonstrando todas as funcionalidades:

| Exemplo | Descrição |
|---|---|
| `demo_funcionalidades.br` | Funcionalidades básicas da linguagem |
| `demo_funcionalidades_avancadas.br` | Funcionalidades avançadas |
| `classes_e_heranca.br` | Programação orientada a objetos completa |
| `fstrings_e_listas.br` | F-strings e list comprehensions |
| `paralelismo_e_erros.br` | Concorrência e tratamento de erros |
| `decorators.br` | Decorators (@logar, @medir_tempo) |
| `generators_e_context.br` | Generators (yield) e context managers |
| `http_server.br` | Servidor HTTP completo com rotas |
| `ia_nativa.br` | IA nativa com agentes |
| `aplicacao_completa.br` | **Sistema de Biblioteca** completo (151 linhas) |

Execute qualquer exemplo:
```bash
bin/br.exe exemplos/aplicacao_completa.br
```

---

## 🔌 Extensão VS Code

A linguagem BR possui uma **extensão completa para Visual Studio Code** em `vscode-extension/` com:

### Funcionalidades
- ✅ **Syntax highlighting** para arquivos `.br`
- ✅ **Diagnóstico em tempo real** (strings não fechadas, parênteses desbalanceados, blocos não fechados)
- ✅ **LSP Integration** (autocomplete, hover, go-to-definition)
- ✅ **Formatter** automático (indentação baseada em blocos)
- ✅ **Code Lens** (botões "▶ Executar" em funções, "▶ Rodar Teste" em testes)
- ✅ **Status Bar** (contagem de funções, classes e linhas)
- ✅ **Comandos**:
  - `BR: Run` - Executar arquivo atual
  - `BR: Debug` - Depurar arquivo atual
  - `BR: Format` - Formatar código
  - `BR: Open REPL` - Abrir terminal interativo
  - `BR: New File` - Criar novo arquivo BR
  - `BR: Version` - Verificar versão

### Instalação
```bash
# Navegue até a pasta da extensão
cd vscode-extension

# Instale as dependências
npm install

# Compile a extensão
npm run compile

# No VS Code, pressione F5 para testar
```

---

## ⚙️ Runtime C++ e Serviços Nativos

O runtime C++ possui **2700+ linhas** de código com serviços integrados:

| Serviço | Arquivo | Descrição |
|---|---|---|
| **🧠 VM Principal** | `runtime/src/main.cpp` | Lexer → Parser → AST → VM (~1924 linhas) |
| **📁 Serviço de Arquivo** | `runtime/src/servico_arquivo.cpp` | Leitura/escrita de arquivos |
| **🌐 Serviço de Rede** | `runtime/src/servico_rede.cpp` | HTTP cliente e servidor (~588 linhas) |
| **🖥️ Serviço GUI** | `runtime/src/servico_gui.cpp` | Interface gráfica nativa (~237 linhas) |
| **🗄️ SQLite Service** | `runtime/src/sqlite_servico.cpp` | Integração com SQLite (~110 linhas) |
| **📦 VM Bytecode** | `runtime/src/vm_bytecode.cpp` | Máquina virtual de bytecode (~412 linhas) |

### Arquitetura do Runtime

```cpp
// Componentes principais:
// - Lexer: Tokenização com UTF-8 + F-strings
// - Parser: Recursivo descendente
// - AST: Árvore de Sintaxe Abstrata
// - VM: Máquina Virtual com threads
// - Serviços: Arquivo, Rede, GUI, SQLite, Bytecode
```

---

## 🏗️ Arquitetura

```
┌─────────────────────────────────────────┐
│         Código Fonte (.br)              │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│           Lexer BR                      │
│  (Tokenização com UTF-8 + F-strings)    │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│          Parser BR                      │
│   (Recursivo descendente)               │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│            AST                          │
│     (Árvore de Sintaxe Abstrata)        │
├─────────────────────────────────────────┤
│     Analisador Semântico                │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│      VM BR (Máquina Virtual)            │
│  (Interpretador com threads)            │
├─────────────────────────────────────────┤
│  Serviços: Arquivo | Rede | GUI | SQL   │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│         Saída / Resultado               │
└─────────────────────────────────────────┘
```

---

## 📖 Documentação

A documentação completa está disponível em `docs/`:

| Documento | Descrição |
|---|---|
| `docs/guia_referencia_rapida.md` | Guia de referência rápida (285 linhas) |
| `docs/bibliotecas_padrao.md` | Documentação de todas as bibliotecas (299 linhas) |
| `docs/roadmap_definitivo_br.md` | Roadmap completo e detalhado (377 linhas) |
| `docs/status_final_projeto.md` | Status final do projeto (314 linhas) |
| `docs/relatorio_implementacao_completo.md` | Relatório de implementação (232 linhas) |
| `docs/especificacao_instalacao.md` | Especificação de instalação (603 linhas) |
| `docs/visao_futura_ia_nativa.md` | Visão da IA nativa na linguagem (226 linhas) |
| `docs/plano_implementacao.md` | Plano de implementação |
| `docs/status_implementacao.md` | Status de implementação |
| `docs/roadmap_completo.md` | Roadmap completo |
| `docs/implementacoes_concluidas.md` | Implementações concluídas (182 linhas) |
| `especificacao.md` | Especificação completa da linguagem |
| `projeto.brm` | Manifesto do projeto |

---

## 🎓 Filosofia

### 1. Semântica Brasileira
Sem palavras reservadas em inglês. Tudo em português brasileiro natural.

### 2. IA como Cidadão de Primeira Classe
A IA não é uma biblioteca externa. É parte da linguagem:
```br
# IA de execução
resposta é pergunte para ia
    "Qual a capital do Brasil?"
fim

# IA de compilação (meta-programação)
objetivo
    criar sistema de gestão
    com relatórios automáticos
fim
```

### 3. Regra de Ouro
> Nenhum recurso da linguagem BR deve exigir que o programador escreva código em outra linguagem.

### 4. Indentação é Estrutura
Sem chaves `{}` ou ponto e vírgula `;`. A indentação define o escopo.

---

## 🗺️ Roadmap

### ✅ Implementado (100%)
- [x] Runtime C++ estável (2700+ linhas)
- [x] Sintaxe em português brasileiro
- [x] F-strings modernas
- [x] List Comprehensions
- [x] Decorators
- [x] Generators/yield
- [x] Context managers
- [x] Classes com herança
- [x] Paralelismo real (threads)
- [x] Tratamento de erros (tente/capture)
- [x] HTTP Cliente e Servidor
- [x] Banco de dados em memória
- [x] Módulos e importação
- [x] 17+ bibliotecas padrão
- [x] 50+ submódulos especializados
- [x] LSP Server (autocomplete, diagnóstico)
- [x] Debugger (breakpoints, step, watch)
- [x] Framework Web
- [x] ORM para SQLite
- [x] Package Manager
- [x] Sandbox e permissões
- [x] Formatter e Linter
- [x] VS Code Extension
- [x] Test Runner
- [x] Build system (Makefile, build.py, build.bat)
- [x] Documentação completa
- [x] 10 exemplos executáveis
- [x] **SQLite nativo no runtime C++** (`runtime/src/sqlite_servico.cpp`)
- [x] **GUI nativa (Qt)** (`runtime/include/servico_gui_qt.h/cpp`)
- [x] **Bytecode real (.brc)** (`runtime/include/compilador_bytecode.h/cpp`)
- [x] **Compilador nativo CLI** (`runtime/src/compilador_br.cpp`)
- [x] **IA de execução nativa** (`runtime/include/servico_ia.h/cpp`)
- [x] **Agentes autônomos persistentes** (integrado no serviço IA)
- [x] **Geração de código por IA** (funções no serviço IA)
- [x] **Auto-debugging inteligente** (funções no serviço IA)
- [x] **Package manager completo com registro online** (`runtime/include/gerenciador_pacotes.h/cpp`)
- [x] **Async/await (`assincrono`, `aguardar`)** (`runtime/include/execucao_assincrona.h/cpp`)
- [x] **Pattern matching (`corresponda`)** (`runtime/include/pattern_matching.h/cpp`)
- [x] **Type hints opcionais** (`runtime/include/type_hints.h/cpp`)
- [x] **Docstrings integradas** (`runtime/include/docstrings.h/cpp`)
- [x] **ML/AI libraries nativas** (`runtime/include/biblioteca_ml.h/cpp`)
- [x] **Compilador cruzado (cross-compiler)** (`runtime/include/compilador_cruzado.h/cpp`)
- [x] **Suporte a WASM** (`runtime/include/runtime_wasm.h/cpp`)
- [x] **IDE Web (online playground)** (`web/ide/index.html`)
- [x] **Marketplace de pacotes e comunidade** (`web/marketplace/index.html`)

---

## 🤝 Contribuindo

1. Escolha uma funcionalidade no roadmap
2. Leia a especificação em `especificacao.md`
3. Implemente no runtime ou ferramentas
4. Adicione testes
5. Atualize a documentação

### Áreas para contribuir
- **Runtime C++**: Correções de bugs, novos recursos na VM
- **Ferramentas Python**: Linter, Debugger, LSP, Framework Web
- **Bibliotecas**: Novas funcionalidades em `lib/`
- **Documentação**: Melhorias em `docs/`
- **Exemplos**: Novos exemplos em `exemplos/`
- **Extensão VS Code**: Melhorias em `vscode-extension/`

---

## 📄 Licença

MIT License - Sinta-se livre para usar e modificar!

---

## 🙏 Agradecimentos

Criado com ❤️ para a comunidade brasileira de programação.

**"Programar em português é mais natural para quem pensa em português."**

---

**Última atualização**: 06/08/2026  
**Versão**: 2.0.0  
**Status**: Completo (100% funcionalidades implementadas)