# Especificação de Instalação da Linguagem BR

**Versão alvo:** BR 1.0.0  
**Diretório padrão Windows:** `D:\br`  
**Status:** especificação de distribuição, instalação e operação

---

## 1. Visão Geral

A linguagem BR deve ser distribuída como um ambiente completo de desenvolvimento e execução. A instalação não contém apenas o interpretador: ela inclui compilador, máquina virtual, gerenciador de pacotes, biblioteca padrão, documentação, exemplos, cache, logs, configurações, ferramentas auxiliares e serviços de apoio.

O objetivo do layout é permitir que um usuário instale a linguagem uma vez e tenha:

* execução direta de arquivos `.br`;
* compilação para bytecode `.brc`;
* execução de bytecode pela VM;
* empacotamento em `.brp`;
* instalação de módulos;
* formatação, lint, documentação, debug, profiling e testes;
* biblioteca padrão organizada por domínio;
* cache e logs centralizados;
* configuração global previsível;
* suporte a IDE via LSP.

A instalação padrão no Windows usa `D:\br`, mas o mesmo modelo pode ser adaptado para outros diretórios por meio de `config.ini`.

---

## 2. Estrutura Completa de Pastas

```text
D:\br
  bin\                  # Executáveis do sistema
    tools\              # Ferramentas auxiliares
    daemons\            # Serviços em background
  lib\                  # Biblioteca padrão
  modulos\              # Módulos instalados pelo usuário
  cache\                # Cache de bytecode e arquivos temporários
    bytecode\
    modulos\
    downloads\
    lsp\
  docs\                 # Documentação
  exemplos\             # Exemplos de código BR
  projetos\             # Projetos do usuário
  temp\                 # Arquivos temporários
  logs\                 # Logs do sistema
  etc\                  # Arquivos de configuração adicionais
  include\              # Headers para FFI
  scripts\              # Scripts utilitários
  web\                  # Interface web opcional

  br.exe
  br_compiler.exe
  br_vm.exe
  br_pkg.exe
  br_formatador.exe
  br_linter.exe
  br_docgen.exe
  br_repl.exe
  uninstall.exe
  config.ini
  LICENSE
  README.md
  CHANGELOG.md
  install.log
```

### 2.1 Regra de Organização

Os executáveis canônicos ficam em `D:\br\bin`. Os executáveis da raiz podem existir como atalhos, shims ou cópias de conveniência para compatibilidade com instaladores antigos.

O comando principal é `br.exe`. Ele deve funcionar como roteador para as outras ferramentas:

```powershell
br arquivo.br
br --compile arquivo.br
br fmt arquivo.br
br lint arquivo.br
br test
```

---

## 3. Diretórios da Instalação

| Diretório | Função | Regras principais |
|---|---|---|
| `bin\` | Executáveis da linguagem | Deve estar no `PATH`. Não deve conter código-fonte de projetos. |
| `bin\tools\` | Ferramentas administrativas | Migração, análise, performance e segurança. |
| `bin\daemons\` | Serviços de fundo | LSP, watcher e métricas. Podem ser iniciados por IDEs. |
| `lib\` | Biblioteca padrão | Somente módulos oficiais versionados com a distribuição. |
| `modulos\` | Módulos do usuário | Instalados por `br install`. Não deve misturar biblioteca padrão. |
| `cache\` | Cache de bytecode, downloads e LSP | Pode ser apagado por `br clean`. Nunca deve conter dados essenciais. |
| `docs\` | Documentação local | Manual, API da biblioteca padrão, tutoriais e guias. |
| `exemplos\` | Exemplos executáveis | Programas pequenos por tema, usados também para testes manuais. |
| `projetos\` | Projetos do usuário | Espaço padrão para `br novo`. Cada projeto tem manifesto próprio. |
| `temp\` | Temporários de execução | Arquivos de curta duração. Podem ser removidos ao encerrar. |
| `logs\` | Logs do sistema | Logs do instalador, runtime, compilador, pacote e serviços. |
| `etc\` | Configurações adicionais | Perfis, políticas, certificados, mirrors e configuração por ambiente. |
| `include\` | Headers para FFI | Arquivos `.h`, `.bri` e metadados para integração nativa. |
| `scripts\` | Scripts utilitários | Instalação, diagnóstico, atualização, limpeza e empacotamento. |
| `web\` | Interface web opcional | Painel local, documentação navegável e dashboards. |

---

## 4. Executáveis

### 4.1 Executáveis Principais em `D:\br\bin`

| Executável | Função |
|---|---|
| `br.exe` | Interpretador principal e roteador de comandos. Executa `.br`, `.brc` e comandos de alto nível. |
| `br_compiler.exe` | Compila `.br` para bytecode `.brc`. Aplica análise léxica, sintática, semântica e otimizações. |
| `br_vm.exe` | Executa bytecode `.brc` diretamente, sem reprocessar código-fonte. |
| `br_pkg.exe` | Gerenciador de pacotes. Instala, remove, atualiza e publica módulos. |
| `br_formatador.exe` | Formata código BR conforme padrão oficial. |
| `br_linter.exe` | Executa análise estática, avisos de estilo, segurança e qualidade. |
| `br_docgen.exe` | Gera documentação `.brd`, HTML ou Markdown a partir do código. |
| `br_repl.exe` | Abre o REPL interativo. |
| `br_debug.exe` | Debugger com breakpoints, stepping, pilha e inspeção de variáveis. |
| `br_profiler.exe` | Mede tempo por função, chamadas e serviços nativos. |
| `br_test.exe` | Executa testes `.brt` e blocos `teste`. |
| `br_bench.exe` | Executa benchmarks controlados. |
| `br_agent.exe` | Executa agentes persistentes `.bra`. |
| `br_pack.exe` | Empacota projetos em `.brp`. |
| `br_contract.exe` | Valida contratos `.bri` contra implementações `.br`. |

### 4.2 Ferramentas Auxiliares em `D:\br\bin\tools`

| Executável | Função |
|---|---|
| `br_migrate.exe` | Migra projetos entre versões da linguagem. |
| `br_analyze.exe` | Faz análise aprofundada de dependências, símbolos e estrutura. |
| `br_perf.exe` | Diagnóstico de performance, cache, hot paths e consumo de memória. |
| `br_security.exe` | Auditoria de permissões, sandbox, pacotes e dependências. |

### 4.3 Serviços em `D:\br\bin\daemons`

| Executável | Função |
|---|---|
| `br_lsp.exe` | Servidor de linguagem para IDEs. |
| `br_watch.exe` | Observa arquivos e recompila/testa automaticamente. |
| `br_metrics.exe` | Coleta métricas locais de build, execução e performance. |

---

## 5. Biblioteca Padrão

A biblioteca padrão fica em `D:\br\lib`. Ela deve ser versionada junto com a linguagem e tratada como parte do runtime oficial.

Cada módulo pode conter:

```text
modulo\
  modulo.br          # Código BR do módulo
  modulo.bri         # Contrato público
  modulo.brd         # Documentação gerada
  exemplos\          # Exemplos do módulo
  testes\            # Testes do módulo
  README.md          # Explicação humana
```

### 5.1 Módulos Oficiais

| Módulo | Conteúdo esperado |
|---|---|
| `core\` | Funções básicas, tipos, memória, erros, conversões e introspecção. |
| `matematica\` | Matemática básica, avançada, estatística e finanças. |
| `texto\` | Strings, regex, validações, acentos, slugs e normalização. |
| `lista\` | Arrays, dicionários, filas, pilhas, árvores e coleções. |
| `arquivo\` | Leitura, escrita, CSV, JSON, XML e binários. |
| `json\` | Parser, gerador, validador e schema JSON. |
| `data_hora\` | Datas, horários, calendários, fusos e duração. |
| `rede\` | HTTP, sockets, WebSocket, DNS, email e download. |
| `criptografia\` | Hash, Base64, AES, RSA, JWT e bcrypt. |
| `testes\` | Testes unitários, mocks, cobertura e fixtures. |
| `banco\` | SQLite, PostgreSQL, MySQL, MongoDB e Redis. |
| `interface\` | Menus, perguntas, progresso, cores e tabelas no terminal. |
| `log\` | Sistema de logs, rotação e níveis. |
| `tempo\` | Esperar, cronômetro, temporizador e intervalo. |
| `processo\` | Comandos do sistema, variáveis de ambiente e subprocessos. |
| `grafico\` | Gráficos no terminal e visualizações simples. |
| `inteligencia\` | IA, agentes, modelos, treino e embeddings. |
| `jogos\` | Engine, sprites, colisão, áudio e input. |
| `web\` | Servidor HTTP, rotas, templates e sessões. |
| `mobile\` | Android, iOS e componentes de UI. |
| `iot\` | MQTT, sensores, GPIO e serial. |
| `ciencia\` | DataFrames, arrays, ciência de dados e visualização. |
| `arte\` | Desenho, animação, pixel art e música. |
| `saude\` | Prontuário, exames e medicamentos. |
| `finanzas\` | Banco, investimentos, impostos, NFC-e e boletos. |
| `blockchain\` | Carteira, transações, contratos e mineração. |
| `compat\` | Integração com Python, JavaScript, Lua e Shell. |

### 5.2 Importação

A forma oficial de importação em BR é em português:

```br
importar "matematica"

mostre matematica.pi()
mostre matematica.somar(5, 3)
```

Importação seletiva:

```br
de "texto" importe "maiusculo" e "minusculo"

mostre maiusculo("brasil")
```

---

## 6. Módulos do Usuário

Módulos instalados pelo usuário ficam em `D:\br\modulos`.

Estrutura recomendada:

```text
D:\br\modulos
  nome_modulo\
    versao\
      modulo.br
      modulo.bri
      README.md
      pacote.brm
      checksum.txt
```

O gerenciador de pacotes deve:

1. baixar o pacote para `D:\br\cache\downloads`;
2. validar assinatura e checksum;
3. descompactar em `D:\br\cache\modulos`;
4. instalar em `D:\br\modulos\nome\versao`;
5. atualizar índice local;
6. registrar logs em `D:\br\logs\pacotes.log`.

Comandos:

```powershell
br install nome_modulo
br remove nome_modulo
br update
br clean
```

---

## 7. Cache de Bytecode

O cache fica em `D:\br\cache\bytecode`.

Cada arquivo `.br` compilado pode gerar um `.brc` em cache. O nome deve ser derivado de:

* caminho absoluto do arquivo;
* hash do conteúdo;
* versão da linguagem;
* nível de otimização;
* permissões do projeto;
* versão da biblioteca padrão.

Exemplo:

```text
D:\br\cache\bytecode
  1.0.0\
    otimizacao-2\
      9f3a8c2d.principal.brc
```

### 7.1 Invalidação

O cache deve ser descartado quando:

* o código-fonte muda;
* `config.ini` muda em opções relevantes;
* a versão da linguagem muda;
* a biblioteca padrão muda;
* um módulo importado muda;
* o usuário executa `br clean`.

### 7.2 Segurança

Bytecode em cache nunca deve ser tratado como confiável sem validação de versão e checksum.

---

## 8. Projetos

Projetos do usuário ficam por padrão em `D:\br\projetos`.

Estrutura recomendada:

```text
D:\br\projetos\meu_app
  projeto.brm
  fonte\
    principal.br
  testes\
    principal_teste.brt
  docs\
  build\
    principal.brc
  dist\
    meu_app.brp
  recursos\
  logs\
```

### 8.1 Manifesto `projeto.brm`

```br
nome "meu_app"
versao "1.0.0"

dependencias
    web "1.0"
    banco "1.0"
fim

permissoes
    arquivos
    rede
fim
```

### 8.2 Fluxo de Projeto

```powershell
br novo meu_app
cd D:\br\projetos\meu_app
br fonte\principal.br
br --compile fonte\principal.br
br test
br doc .
br pack
```

---

## 9. Logs, Configurações e Scripts

### 9.1 Logs

Logs ficam em `D:\br\logs`.

Arquivos recomendados:

```text
sistema.log       # Runtime e comandos principais
compilador.log    # Compilação e otimização
pacotes.log       # Instalação e remoção de módulos
lsp.log           # Servidor de linguagem
debug.log         # Sessões de debug
security.log      # Sandbox e permissões
install.log       # Instalação e atualização
```

Logs devem respeitar o nível definido em `config.ini`.

### 9.2 Configurações

`D:\br\config.ini` é a configuração global. Configurações adicionais ficam em `D:\br\etc`.

`etc` pode conter:

```text
perfis.ini
repositorios.ini
sandbox.ini
certificados\
politicas\
```

### 9.3 Scripts

`D:\br\scripts` deve conter scripts administrativos:

```text
install.ps1
uninstall.ps1
update.ps1
diagnostico.ps1
limpar_cache.ps1
criar_projeto.ps1
```

---

## 10. Arquivo `config.ini`

Modelo oficial:

```ini
[BR]
versao = 1.0.0
diretorio_instalacao = D:\br

[PATH]
caminhos_adicionais =
    D:\br\modulos
    D:\br\lib
    D:\br\projetos

[INTERPRETADOR]
encoding = utf-8
max_memoria_mb = 512
timeout_segundos = 30
debug = false

[COMPILADOR]
otimizacao = 2
gerar_bytecode = true
cache_dir = D:\br\cache

[SANDBOX]
ativado = true
permissoes_padrao = ler, escrever, rede
diretorios_bloqueados = C:\Windows, C:\System32

[LOGS]
nivel = INFO
arquivo = D:\br\logs\sistema.log
rotacao_dias = 7

[MODULOS]
repositorio_url = https://repositorio.br-lang.org
cache_instalacao = D:\br\cache\modulos
```

---

## 11. PATH no Windows

O instalador deve adicionar `D:\br\bin` ao `PATH` do sistema.

Verificação:

```powershell
$env:Path -split ';' | Select-String 'D:\\br\\bin'
where br
```

Configuração manual recomendada:

```powershell
$dir = 'D:\br\bin'
$key = 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment'
$path = (Get-ItemProperty -Path $key -Name Path).Path
$parts = $path -split ';' | Where-Object { $_ }
if ($parts -notcontains $dir) {
    Set-ItemProperty -Path $key -Name Path -Value (($parts + $dir) -join ';')
}
```

Depois, abra um novo terminal e teste:

```powershell
br --version
```

---

## 12. Comandos Principais

| Comando | Ação |
|---|---|
| `br arquivo.br` | Executa código-fonte BR. |
| `br --version` | Mostra versão instalada. |
| `br --compile arquivo.br` | Compila para `.brc`. |
| `br arquivo.brc` | Executa bytecode pela VM. |
| `br install nome_modulo` | Instala módulo do repositório. |
| `br remove nome_modulo` | Remove módulo instalado. |
| `br update` | Atualiza índice e módulos. |
| `br fmt arquivo.br` | Formata arquivo. |
| `br lint arquivo.br` | Executa análise estática. |
| `br doc projeto\` | Gera documentação. |
| `br test` | Executa testes do projeto atual. |
| `br clean` | Limpa cache e temporários. |
| `br repl` | Abre o REPL. |

---

## 13. Verificação Rápida

Após instalar:

```powershell
cd D:\br
br --version
br -c "mostre 'Funcionou!'"
br -c "importar 'matematica'; mostre matematica.pi()"
dir
```

Resultado esperado:

```text
BR 1.0.0
Funcionou!
3.141592653589793
```

---

## 14. Exemplos Básicos da Linguagem BR

### 14.1 Hello World

```br
mostre "Olá, mundo!"
```

### 14.2 Variáveis

Forma oficial:

```br
nome é "Brasil"
idade é 25

mostre nome
mostre idade
```

Alias aceito por compatibilidade:

```br
nome = "Brasil"
idade = 25
```

### 14.3 Função

Forma oficial da BR:

```br
função soma recebendo a e b
    devolva a + b
fim

resultado é soma(10, 20)
mostre resultado
```

### 14.4 Importando Biblioteca

```br
importar "matematica"

mostre matematica.pi()
mostre matematica.somar(5, 3)
```

### 14.5 Teste

```br
teste "soma basica"
    espere 2 + 3 igual a 5
fim
```

---

## 15. Checklist de Instalação

1. Criar `D:\br`.
2. Criar as pastas `bin`, `lib`, `modulos`, `cache`, `docs`, `exemplos`, `projetos`, `temp`, `logs`, `etc`, `include`, `scripts` e `web`.
3. Copiar executáveis para `D:\br\bin`.
4. Criar shims opcionais na raiz (`br.exe`, `br_compiler.exe`, etc.).
5. Copiar biblioteca padrão para `D:\br\lib`.
6. Criar `D:\br\config.ini`.
7. Criar `D:\br\install.log`.
8. Adicionar `D:\br\bin` ao `PATH`.
9. Registrar associação de arquivos `.br`, `.brc`, `.brp`, `.brt`, `.bri`, `.bra` e `.brm`.
10. Executar `br --version`.
11. Executar `br -c "mostre 'Funcionou!'"`.
12. Executar um exemplo de `D:\br\exemplos`.
13. Executar `br test` em um projeto de exemplo.
14. Verificar logs em `D:\br\logs`.

---

## 16. Próximos Passos de Implementação

1. Criar instalador que materialize a árvore `D:\br` completa.
2. Separar executáveis reais por responsabilidade em `bin`.
3. Implementar `br.exe` como roteador dos comandos principais.
4. Criar `config.ini` real e leitor de configuração compartilhado.
5. Implementar cache de bytecode com hash de fonte, versão e otimização.
6. Criar índice local de módulos instalados em `modulos`.
7. Implementar `br_pkg.exe` com download, checksum, assinatura e rollback.
8. Expandir a biblioteca padrão em `lib` com contratos `.bri`.
9. Adicionar `br_lsp.exe`, `br_watch.exe` e `br_metrics.exe`.
10. Criar suíte de testes de instalação para validar PATH, execução, compilação, VM, pacotes, logs e cache.
11. Gerar documentação local em `docs`.
12. Criar exemplos executáveis por área em `exemplos`.
