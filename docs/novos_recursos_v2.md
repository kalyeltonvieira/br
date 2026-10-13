# Novos Recursos da Linguagem BR v2.0

Este documento descreve os novos recursos implementados na versão 2.0 da linguagem BR.

## 📋 Índice

1. [SQLite Nativo](#sqlite-nativo)
2. [GUI Nativa (Qt)](#gui-nativa-qt)
3. [Bytecode Real (.brc)](#bytecode-real-brc)
4. [Compilador Nativo](#compilador-nativo)
5. [IA de Execução Nativa](#ia-de-execucao-nativa)
6. [Agentes Autônomos](#agentes-autonomos)
7. [Geração de Código por IA](#geracao-de-codigo-por-ia)
8. [Auto-Debugging](#auto-debugging)
9. [Package Manager](#package-manager)
10. [Async/Await](#asyncawait)
11. [Pattern Matching](#pattern-matching)
12. [Type Hints](#type-hints)
13. [Docstrings](#docstrings)
14. [ML/AI Libraries](#mlai-libraries)
15. [Cross-Compiler](#cross-compiler)
16. [Suporte WASM](#suporte-wasm)
17. [IDE Web](#ide-web)
18. [Marketplace](#marketplace)

---

## SQLite Nativo

### Arquivos
- `runtime/src/sqlite_servico.cpp` - Implementação do serviço SQLite
- `runtime/include/sqlite_servico.h` - Header do serviço

### Funcionalidades
- Queries em português (SELECIONE, INSIRA, ATUALIZE, DELETE)
- Suporte a CREATE TABLE, DROP TABLE
- Tradução automática de queries BR para SQL
- Integração nativa sem dependências externas

### Exemplo de Uso
```br
importar "sqlite"

banco = abrir_banco("dados.db")

crie tabela usuarios (
    id inteiro chave primaria,
    nome texto,
    email texto
)

insira em usuarios (nome, email) valores ("João", "joao@email.com")

resultados = selecione * de usuarios onde nome = "João"
mostre resultados
```

---

## GUI Nativa (Qt)

### Arquivos
- `runtime/include/servico_gui_qt.h` - Header do serviço GUI Qt
- `runtime/src/servico_gui_qt.cpp` - Implementação GUI Qt

### Funcionalidades
- Suporte cross-plataforma (Windows, Linux, macOS)
- Componentes: janelas, botões, campos de texto, labels, sliders, checkboxes
- Layouts e gerenciamento de eventos
- Compilação condicional com `USE_QT`

### Exemplo de Uso
```br
importar "gui"

janela = criar_janela("Minha Aplicação", 800, 600)

botao = adicionar_botao(janela, "btn1", "Clique Aqui", 50, 50)
campo = adicionar_campo(janela, "campo1", "Digite algo", 50, 100)

definir_evento(botao, "clique", funcao
    mostre "Botão clicado!"
fim)

mostrar_janela(janela)
```

---

## Bytecode Real (.brc)

### Arquivos
- `runtime/include/compilador_bytecode.h` - Header do compilador
- `runtime/src/compilador_bytecode.cpp` - Implementação do compilador
- `runtime/src/vm_bytecode.cpp` - VM de bytecode

### Formato BRC1
- Magic header: "BRC1"
- Seções: header, constantes, funções, classes, bytecode
- Opcodes definidos para todas as operações
- Suporte a permissões de segurança

### Exemplo de Uso
```bash
# Compilar para bytecode
br_compilar programa.br -o programa.brc

# Executar bytecode
bin/br.exe programa.brc
```

---

## Compilador Nativo

### Arquivos
- `runtime/src/compilador_br.cpp` - CLI do compilador

### Funcionalidades
- Compilação de .br para .brc
- Flags de otimização (-O, -O1, -O2, -O3)
- Configuração de permissões (--perm-internet, --perm-arquivos, etc.)
- Modo verbose para debug

### Exemplo de Uso
```bash
br_compilar programa.br -o saida.brc --perm-internet --perm-arquivos -v
```

---

## IA de Execução Nativa

### Arquivos
- `runtime/include/servico_ia.h` - Header do serviço IA
- `runtime/src/servico_ia.cpp` - Implementação do serviço IA

### Funcionalidades
- Integração com OpenAI, Anthropic, Google, Ollama
- Perguntas simples e com contexto
- Agentes persistentes com histórico
- Funções especializadas (classificar, traduzir, resumir, etc.)
- Cache de respostas

### Exemplo de Uso
```br
importar "ia"

resposta = perguntar("Qual a capital do Brasil?")
mostre resposta

agente = criar_agente("assistente", "ajuda geral")
resposta = agente_perguntar(agente, "Como compilar código BR?")
```

---

## Agentes Autônomos

### Funcionalidades
- Agentes com especialidade e instruções de sistema
- Histórico de conversa persistente
- Limpeza de histórico
- Múltiplos agentes simultâneos

### Exemplo de Uso
```br
agente_codigo = criar_agente("programador", "desenvolvimento de software",
    "Você é um programador experiente em BR.")

resposta = agente_perguntar(agente_codigo, "Crie uma função de fibonacci")
```

---

## Geração de Código por IA

### Funcionalidades
- Gerar código a partir de descrição
- Explicar código existente
- Refatorar código com objetivos específicos

### Exemplo de Uso
```br
codigo = gerar_codigo("função para calcular fatorial", "br")
mostre codigo

explicacao = explicar_codigo(codigo)
mostre explicacao

codigo_refatorado = refatorar_codigo(codigo, "adicionar memoização")
```

---

## Auto-Debugging

### Funcionalidades
- Análise de erros no código
- Sugestões de correção
- Identificação de causas raiz

### Exemplo de Uso
```br
analise = analisar_erro(codigo, "variável não definida")
mostre analise

correcao = sugerir_correcao(codigo, erro)
mostre correcao
```

---

## Package Manager

### Arquivos
- `runtime/include/gerenciador_pacotes.h` - Header do gerenciador
- `runtime/src/gerenciador_pacotes.cpp` - Implementação

### Funcionalidades
- Instalar/desinstalar pacotes
- Busca e listagem de pacotes
- Atualização automática
- Registro online de pacotes
- Gerenciamento de dependências
- Cache de pacotes

### Exemplo de Uso
```bash
# Instalar pacote
br instalar br-web

# Listar pacotes instalados
br listar

# Buscar pacote
br buscar "http"

# Atualizar pacote
br atualizar br-web

# Publicar pacote
br publicar ./meu-pacote
```

---

## Async/Await

### Arquivos
- `runtime/include/execucao_assincrona.h` - Header do runtime assíncrono
- `runtime/src/execucao_assincrona.cpp` - Implementação

### Funcionalidades
- Pool de threads para execução paralela
- Tarefas assíncronas com `assincrono`
- Await com `aguardar`
- Sleep não-bloqueante
- Execução após delay
- Execução periódica

### Exemplo de Uso
```br
importar "async"

funcao tarefa recebendo tempo
    aguarar(tempo)
    retorne "Concluído após " + tempo + "ms"
fim

resultado = assincrono tarefa(1000)
mostre resultado

# Sleep não-bloqueante
dormir_assincrono(500, funcao
    mostre "Após 500ms"
fim)
```

---

## Pattern Matching

### Arquivos
- `runtime/include/pattern_matching.h` - Header do pattern matching
- `runtime/src/pattern_matching.cpp` - Implementação

### Funcionalidades
- Patterns: literal, variável, wildcard, lista, registro
- Patterns compostos: OR, AND
- Pattern matching com guards
- Extração de variáveis

### Exemplo de Uso
```br
corresponda valor com
    caso 1
        mostre "É um"
    caso 2
        mostre "É dois"
    caso x
        mostre "É outro: " + x
fim
```

---

## Type Hints

### Arquivos
- `runtime/include/type_hints.h` - Header de type hints
- `runtime/src/type_hints.cpp` - Implementação

### Funcionalidades
- Anotações de tipo opcionais
- Inferência de tipos
- Verificação de compatibilidade
- Tipos: inteiro, decimal, texto, booleano, lista, registro, função
- Tipos genéricos e union

### Exemplo de Uso
```br
nome : texto é "João"
idade : inteiro é 25
lista : lista<inteiro> é [1, 2, 3]

funcao somar recebendo a : inteiro, b : inteiro -> inteiro
    retorne a + b
fim
```

---

## Docstrings

### Arquivos
- `runtime/include/docstrings.h` - Header de docstrings
- `runtime/src/docstrings.cpp` - Implementação

### Funcionalidades
- Docstrings integradas ao código
- Parser de docstrings
- Geração de documentação (Markdown, HTML, texto)
- Sistema de ajuda em tempo de execução
- Seções: @param, @retorno, @exemplo, @excecao

### Exemplo de Uso
```br
funcao calcular_fatorial recebendo n
    """Calcula o fatorial de um número.
    
    @param n Número inteiro positivo
    @return Fatorial de n
    @exemplo calcular_fatorial(5) -> 120
    """
    se n <= 1 entao
        retorne 1
    senao
        retorne n * calcular_fatorial(n - 1)
    fim
fim

# Obter ajuda
ajuda "calcular_fatorial"
```

---

## ML/AI Libraries

### Arquivos
- `runtime/include/biblioteca_ml.h` - Header da biblioteca ML
- `runtime/src/biblioteca_ml.cpp` - Implementação

### Funcionalidades
- Regressão Linear
- Regressão Logística
- K-Nearest Neighbors (KNN)
- Redes Neurais (Perceptron)
- K-Means Clustering
- Processamento de dados (normalização, divisão treino/teste)
- Métricas de avaliação (acurácia, precisão, recall, F1, MSE, R²)

### Exemplo de Uso
```br
importar "ml"

modelo = criar_regressao_linear("modelo")
treinar_modelo("modelo", dados)

previsao = prever_modelo("modelo", [10, 20, 30])
mostre previsao

# Classificação
classificador = criar_regressao_logistica("clf")
treinar_modelo("clf", dados)
classe = prever_classe_modelo("clf", [1, 2, 3])
```

---

## Cross-Compiler

### Arquivos
- `runtime/include/compilador_cruzado.h` - Header do cross-compiler
- `runtime/src/compilador_cruzado.cpp` - Implementação

### Funcionalidades
- Compilação para múltiplas plataformas (Windows, Linux, macOS, Android, iOS)
- Suporte a múltiplas arquiteturas (x86_64, ARM64, RISC-V, WASM)
- Formatos: PE, ELF, Mach-O, WASM
- Toolchains configuráveis
- Geração de LLVM IR e Assembly

### Exemplo de Uso
```bash
# Compilar para Linux ARM64
br_compilar --plataforma linux-arm64 programa.br

# Compilar para WebAssembly
br_compilar --plataforma wasm programa.br

# Compilar para macOS ARM64
br_compilar --plataforma macos-arm64 programa.br
```

---

## Suporte WASM

### Arquivos
- `runtime/include/runtime_wasm.h` - Header do runtime WASM
- `runtime/src/runtime_wasm.cpp` - Implementação

### Funcionalidades
- Runtime WebAssembly completo
- Compilação BR para WASM
- Bridge JavaScript-WASM
- Sandbox de segurança
- Memória e funções host

### Exemplo de Uso
```br
# Compilar para WASM
br_compilar --wasm programa.br -o programa.wasm

# No HTML
<script>
    const runtime = new BRWasmRuntime();
    runtime.carregarModulo('programa', 'programa.wasm');
    runtime.executar('programa');
</script>
```

---

## IDE Web

### Arquivos
- `web/ide/index.html` - Interface da IDE
- `web/ide/styles.css` - Estilos
- `web/ide/ide.js` - Lógica JavaScript
- `web/ide/runtime-br.js` - Runtime JavaScript

### Funcionalidades
- Editor CodeMirror com syntax highlighting
- Execução de código em tempo real
- Exemplos pré-carregados
- Documentação integrada
- Compartilhamento de código via URL
- Console de saída
- Estatísticas de código

### Acesso
Abra `web/ide/index.html` no navegador para usar a IDE online.

---

## Marketplace

### Arquivos
- `web/marketplace/index.html` - Interface do marketplace
- `web/marketplace/styles.css` - Estilos
- `web/marketplace/marketplace.js` - Lógica JavaScript

### Funcionalidades
- Busca de pacotes
- Filtros por categoria
- Ordenação (popularidade, recentes, nome, downloads)
- Detalhes de pacotes
- Instalação via CLI
- Estatísticas globais
- Categorias: Web, Dados, GUI, IA/ML, Rede, Utilitários

### Pacotes Disponíveis
- br-web - Framework web
- br-sqlite - Wrapper SQLite
- br-gui - Biblioteca GUI
- br-ml - Machine Learning
- br-http - Cliente HTTP
- br-test - Framework de testes
- br-orm - ORM para bancos
- br-auth - Autenticação
- br-async - Runtime assíncrono
- E mais...

### Acesso
Abra `web/marketplace/index.html` no navegador para acessar o marketplace.

---

## 📊 Resumo

| Recurso | Arquivos | Status |
|---------|----------|--------|
| SQLite Nativo | `sqlite_servico.cpp/h` | ✅ Completo |
| GUI Qt | `servico_gui_qt.cpp/h` | ✅ Completo |
| Bytecode BRC1 | `compilador_bytecode.cpp/h` | ✅ Completo |
| Compilador CLI | `compilador_br.cpp` | ✅ Completo |
| IA Nativa | `servico_ia.cpp/h` | ✅ Completo |
| Agentes | Integrado em IA | ✅ Completo |
| Geração Código | Funções IA | ✅ Completo |
| Auto-Debugging | Funções IA | ✅ Completo |
| Package Manager | `gerenciador_pacotes.cpp/h` | ✅ Completo |
| Async/Await | `execucao_assincrona.cpp/h` | ✅ Completo |
| Pattern Matching | `pattern_matching.cpp/h` | ✅ Completo |
| Type Hints | `type_hints.cpp/h` | ✅ Completo |
| Docstrings | `docstrings.cpp/h` | ✅ Completo |
| ML/AI Libraries | `biblioteca_ml.cpp/h` | ✅ Completo |
| Cross-Compiler | `compilador_cruzado.cpp/h` | ✅ Completo |
| WASM | `runtime_wasm.cpp/h` | ✅ Completo |
| IDE Web | `web/ide/*` | ✅ Completo |
| Marketplace | `web/marketplace/*` | ✅ Completo |

---

**Versão**: 2.0.0  
**Data**: 06/08/2026  
**Status**: Todos os recursos implementados
