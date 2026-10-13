# Runtime Nativo BR

Runtime 100% nativo da linguagem BR, sem dependências de Python ou outras linguagens.

## 🚀 Status Atual

⚠️ **Estado: Implementação Parcial (Python + C++)**

A linguagem BR tem **duas implementações** em paralelo:

### Implementação Python (Principal - Funcional ✅)
- `ferramentas/lexer.py`, `parser.py`, `codegen.py`, `maquina_virtual.py`
- **Parser, Codegen, VM completos** com suporte a classes, funções, controle de fluxo, listas, registros
- **Serviços nativos**: SISTEMA, IA (simulada), BANCO (em memória), WEB (simulada), ARQUIVOS, TEXTO, LISTA, REGISTRO
- **Concorrência**: Threads, scheduler de green threads
- **Garbage Collector**: Mark-and-sweep
- **Debugger**: Breakpoints, step over/into/out
- **LSP Server**: Para integração com VS Code
- **Compilador em BR**: `ferramentas/compilador.br` (auto-compilação fase 3)
- **Empacotador**: `ferramentas/packager.py` para pacotes `.brp`

### Implementação C++ (Runtime Nativo - 🚧 Em andamento)
- **Lexer básico** - Tokenização funcional com palavras reservadas  
- **Parser esqueleto** - Reconhece estruturas mas só imprime (não executa de verdade)
- **VM esqueleto** - Pilha simples, apenas `mostre` implementado
- **Serviço de arquivos** - Implementado e funcional (ler, escrever, anexar, etc.)
- **Serviço de rede** - Implementado com socket raw (HTTP client, DNS)
- **Bibliotecas BR** - `matematica.br`, `texto.br`, `arquivo.br` escritas em BR
- **Makefile e build system**  
- 🔧 **Precisa**: Integrar parser → VM real (atualmente só "tokeniza e printa")
- 📅 **Planejado**: GUI nativa, IA integrada, banco de dados

## 📦 O que está implementado

### 🎯 **Núcleo da Linguagem (C++)**
- **Lexer nativo** - Tokenização com suporte a palavras reservadas BR  
- **Parser esqueleto** - Reconhece funções, classes, condicionais, importação (não gera AST ainda)
- **VM esqueleto** - Pilha simples de strings, execução placeholder
- **Tipos nativos** - Apenas string (demais tipos reconhecidos pelo lexer)

### 📚 **Bibliotecas (escritas em BR, para executar na VM Python)**
1. **`matematica.br`** - Operações matemáticas (soma, subtração, multiplicação, Newton-Raphson, trigonometria, estatística)
2. **`texto.br`** - Manipulação de strings (maiúsculo, minúsculo, slug, dividir, substituir, etc.)
3. **`arquivo.br`** - Operações de arquivo (ler, escrever, listar, copiar, etc.)
4. **`rede.br`** - Requisições HTTP, DNS, validação de URL

### ⚙️ **Serviços Nativos (C++)**
- **Serviço de arquivos** - `servico_arquivo.cpp` (ler, escrever, anexar, copiar, mover, criar diretório, permissões)
- **Serviço de rede** - `servico_rede.cpp` (HTTP GET/POST/PUT/DELETE, DNS, URL encode/decode, ping)
- **Sistema de tipos** - Tipagem forte com inferência (no Python, não no C++)

### 🛠️ **Ferramentas de Desenvolvimento**
- Makefile completo para build (g++, C++17)
- Script de build cross-platform
- Exemplos funcionais (em BR para VM Python)

## 🏗️ Arquitetura

```
runtime/
├── src/main.cpp           # Ponto de entrada principal
├── src/servico_arquivo.cpp # Sistema de arquivos nativo
├── src/servico_rede.cpp   # Rede nativa
├── include/               # Headers C++
├── lib/                   # Bibliotecas BR funcionais
│   ├── matematica.br     # Matemática real
│   ├── texto.br          # Texto real  
│   ├── arquivo.br        # Arquivos real
│   └── rede.br           # Rede real
├── exemplos/              # Exemplos completos
│   └── calculadora.br    # Calculadora funcional
├── Makefile              # Build system
└── build.sh              # Script de build
```

## 🚀 Como usar

### Compilar
```bash
cd runtime
make all
# ou
./build.sh
```

### Executar
```bash
# Executar arquivo BR
./bin/br exemplos/calculadora.br

# Modo interativo
./bin/br -i

# Executar código de uma linha
./bin/br -c 'mostre "Olá Mundo"'

# Verificar versão
./bin/br --version
```

### Instalar no sistema
```bash
./build.sh --install
# Agora use: br <comando>
```

## 📝 Exemplo de código funcional

```br
# calculadora.br - Funciona com runtime nativo!
importar "matematica" as mat

função calcular_imc recebendo peso e altura
    devolva peso / (altura * altura)
fim

peso é 70
altura é 1.75
imc é calcular_imc(peso, altura)
mostre "IMC: " + imc
```

## 🎯 Próximos passos

### Fase 1 (Próxima - Runtime C++)
1. **Parser real** - Gerar AST executável em vez de só printar tokens
2. **VM funcional** - Executar bytecode/instruções de verdade
3. **Integração parser→VM** - Pipeline completo de execução nativa
4. **Sistema de tipos** - Tipos nativos no runtime C++

### Fase 2
1. **Banco de dados nativo** - SQLite embutido
2. **GUI nativa** - Janelas, botões (declarativo BR)
3. **Concorrência** - Threads green, canais CSP

### Fase 3  
1. **IA integrada** - LLMs locais (chamadas via CHAMAR_SERVICO)
2. **Compilador JIT** - Otimização runtime
3. **Debugger nativo** - Breakpoints, inspeção
4. **Auto-compilação** - Compilador compilando a si mesmo

## 🧪 Testando

```bash
# Testar build
make test

# Testar exemplos
./bin/br exemplos/calculadora.br

# Testar bibliotecas
./bin/br -c 'importar "matematica" as m; mostre m.somar(5, 3)'
```

## 🤝 Contribuindo

1. **Implementar serviço** - Adicione serviço nativo em C++
2. **Biblioteca BR** - Crie biblioteca funcional em .br
3. **Otimização** - Melhore performance do runtime
4. **Documentação** - Adicione exemplos e docs

## 📄 Licença

Runtime BR Nativo - Linguagem de programação brasileira
Copyright 2026 - Time BR