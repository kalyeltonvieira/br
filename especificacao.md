# Manual de Especificação Oficial da Linguagem BR
**Versão da Especificação: BR 1.0**
**Status: Aprovado**

---

## 1. Filosofia da Linguagem

A linguagem **BR** é uma linguagem de programação revolucionária criada para ser **independente**, **segura**, **concorrente** e **nativamente inteligente**. Ela foi projetada com base em quatro pilares fundamentais:

1. **Semântica Brasileira Natural**: A sintaxe lê-se e escreve-se como português brasileiro formal e limpo. Não há palavras reservadas em inglês ou símbolos desnecessários (como chaves `{}` ou ponto e vírgula `;`).
2. **Inteligência Artificial como Elemento de Linguagem**: A IA não é uma biblioteca externa; é um cidadão de primeira classe tanto na fase de compilação (sintetizando sistemas a partir de intenções/objetivos) quanto em tempo de execução (agentes persistentes e tomadas de decisão).
3. **Regra de Ouro**: *Nenhum recurso da linguagem BR deve exigir que o programador escreva código em outra linguagem. O usuário BR programa apenas em BR.*
4. **Independência de Ambiente**: O código BR é compilado para Bytecode BR (`.brc`), executado pela Máquina Virtual BR (portável para qualquer OS) e distribuído em pacotes executáveis BR (`.brp`).

### 1.1 Formatos e Extensões Oficiais

A linguagem BR padroniza e especifica os seguintes formatos de arquivo oficiais no seu ecossistema:

| Extensão | Função | Descrição |
|---|---|---|
| **`.br`** | Código-fonte | Arquivos contendo o código de programação BR padrão. |
| **`.brc`** | Bytecode compilado | Arquivo binário de bytecode (`BRC1`) executado pela VM BR. |
| **`.brp`** | Pacote executável BR | Pacote distribuído compilado/gerado pelo gerenciador `mercado`. |
| **`.brm`** | Manifesto do projeto | Arquivo de metadados, dependências e sandbox (`projeto.brm`). |
| **`.brd`** | Documentação gerada | Arquivos de Documentação Viva extraídos automaticamente do código. |
| **`.brt`** | Arquivos de teste | Scripts de testes executados pela ferramenta `brasil testar`. |
| **`.bri`** | Interfaces e contratos | Declarações de contratos (`contrato`) compartilhadas. |
| **`.bra`** | Agentes de IA | Definição estruturada de agentes de IA persistentes. |

---

## 2. Palavras Reservadas

As seguintes palavras são reservadas e não podem ser usadas como identificadores:

| Categoria | Palavras Reservadas |
|---|---|
| **Declarações** | `é`, `constante`, `classe`, `contrato`, `função`, `recebendo`, `devolva`, `fim` |
| **Controle de Fluxo** | `se`, `então`, `senão`, `escolha`, `caso`, `enquanto`, `repita`, `vezes`, `conte`, `de`, `até`, `como`, `para`, `cada`, `em` |
| **Coleções** | `lista`, `registro` |
| **Tratamento de Erros** | `tente`, `capture`, `erro`, `falhou` |
| **Concorrência** | `execute`, `em`, `paralelo`, `tarefa` |
| **Inteligência Artificial** | `objetivo`, `pergunte`, `para`, `ia`, `agente`, `crie`, `especialidade` |
| **Módulos / Projeto** | `importar`, `de`, `importe`, `como`, `linguagem`, `nome`, `versao`, `dependencias`, `permissoes` |
| **Valores Nativos** | `verdadeiro`, `falso`, `nulo` |
| **Comentários/Doc** | `explicacao` |
| **Testes** | `teste`, `espere`, `igual`, `a` |

---

## 3. Sistema de Tipos

A linguagem BR utiliza tipagem forte com inferência automática de tipos em tempo de compilação. A tipagem também pode ser declarada de forma opcional (estática) para contratos rígidos.

### 3.1 Tipos Básicos
* `inteiro`: Números inteiros (ex: `10`, `-50`, `0`).
* `decimal`: Números de ponto flutuante (ex: `3.14`, `-120.45`).
* `texto`: Cadeias de caracteres delimitadas por aspas duplas (ex: `"Olá, Mundo!"`).
* `booleano`: Representa valores lógicos `verdadeiro` ou `falso`.

### 3.2 Coleções Nativas
* **Lista (`lista`)**: Coleção ordenada de elementos.
  ```
  nomes : lista é lista
      "Ana"
      "Carlos"
      "Beatriz"
  fim
  ```
* **Registro (`registro`)**: Estrutura chave-valor (dicionário).
  ```
  cliente : registro é registro
      nome: "Marcos"
      idade: 30
      saldo: 1500.50
  fim
  ```

### 3.3 Tipagem Opcional (Declaração Explícita)
Pode-se forçar a declaração do tipo utilizando `:` antes do token de atribuição `é`:
```
idade : inteiro é 25
salário : decimal é 4500.00
ativo : booleano é verdadeiro
```

---

## 4. Sintaxe e Gramática

### 4.1 Declarações de Variáveis e Constantes
```
# Variável mutável com inferência
contador é 0

# Variável com tipo estático
mensagem : texto é "Processo Concluído"

# Constante imutável
constante LIMITE é 100
```

### 4.2 Funções
As funções podem receber parâmetros delimitados por `recebendo` e separados por `e`. Elas devolvem valores com `devolva` e terminam com `fim`.
```
função calcular_area recebendo largura e altura
    devolva largura * altura
fim
```
Funções sem parâmetros omitem a palavra `recebendo`:
```
função obter_data_atual
    devolva "2026-06-06"
fim
```

### 4.3 Controle de Fluxo

#### Condicionais
```
se idade >= 18 então
    mostre "Acesso liberado."
senão
    mostre "Acesso negado."
fim
```

#### Escolha (Match/Switch)
```
escolha status
    caso "pendente"
        mostre "Aguardando aprovação"
    caso "aprovado"
        mostre "Processado com sucesso"
    senão
        mostre "Status desconhecido"
fim
```

#### Laços de Repetição
* **Enquanto**:
  ```
  enquanto contador < 10
      mostre contador
      contador é contador + 1
  fim
  ```
* **Repita (Contagem específica)**:
  ```
  repita 5 vezes
      mostre "Carregando..."
  fim
  ```
* **Conte de até**:
  ```
  conte de 1 até 5 como i
      mostre i
  fim
  ```
* **Para cada em**:
  ```
  para cada item em clientes
      mostre item.nome
  fim
  ```

### 4.4 Módulos e Importação
Módulos são importados usando `importar`:
```
importar "banco"
importar "usuarios" como usr

cliente é usr.buscar_cliente 5
```

---

## 5. Concorrência Moderna

A concorrência em BR é nativa e de altíssimo nível.

### 5.1 Execução em Paralelo
O bloco `execute em paralelo` roda múltiplas instruções concorrentemente em threads de nível de OS ou fibras cooperativas na VM.
```
execute em paralelo
    baixar_dados_servidor()
    atualizar_banco_local()
    renderizar_interface()
fim
```

### 5.2 Tarefas Assíncronas
Uma `tarefa` é uma unidade isolada de execução que retorna uma promessa de valor.
```
tarefa carregar_imagem recebendo url
    imagem é acesse url
    devolva imagem
fim

# Executando a tarefa
promessa é carregar_imagem "https://site.com/foto.png"
# Para aguardar o resultado:
foto é aguarde promessa
```

---

## 6. Banco de Dados, Internet e GUI Nativos

Para cumprir a **Regra de Ouro**, a biblioteca padrão da linguagem disponibiliza serviços nativos de persistência, comunicação de rede e renderização de janelas sem dependências.

### 6.1 Banco de Dados Nativo
O banco de dados é manipulado por expressões em linguagem natural que a VM traduz internamente para operações no motor de persistência padrão (SQLite embutido no runtime):
```
# Consulta nativa
usuarios_ativos é busque todos os usuarios onde ativo == verdadeiro

# Persistência
salve novo_cliente

# Remoção
remova cliente
```

### 6.2 Internet Nativa
Acesso a requisições e servidores HTTP declarativos:
```
# Cliente HTTP
pagina é acesse "https://g1.globo.com"

# Servidor HTTP
crie um site
    porta 8080
    
    rota "/usuarios"
        devolva "Lista de Usuários"
    fim
fim
```

### 6.3 Interface Gráfica (GUI) Nativa
Renderização declarativa de janelas nativas multiplataforma gerenciadas pelo runtime:
```
crie uma janela
    titulo "Painel de Controle"
    largura 400
    altura 300
    
    campo nome_usuario
    botao botao_enviar "Enviar Dados"
    
    quando clicar em botao_enviar
        mostre "Enviado: " + nome_usuario.texto
    fim
fim
```

---

## 7. Tratamento de Erros e Segurança

### 7.1 Bloco Tente-Capture
```
tente
    arquivo é leia_arquivo "dados.txt"
capture erro
    mostre "Erro ao ler arquivo: " + erro.mensagem
fim
```

### 7.2 Tratamento por Expressão
```
resultado é tente buscar_usuario 15
se resultado falhou
    mostre resultado.erro
fim
```

### 7.3 Sandboxing e Permissões
No arquivo `projeto.brm`, o programador declara quais recursos da máquina o programa tem autorização para usar. Se o programa tentar realizar uma chamada de rede ou acesso a arquivos sem ter permissão declarada, a VM encerra a execução com um erro de segurança.

```
permissoes
    internet
    arquivos
fim
```

---

## 8. Testes Nativos

O compilador `brasil` executa blocos `teste` quando chamado via `brasil testar`.
```
teste "verificacao da soma"
    a é 10
    b é 20
    resultado é somar a e b
    espere resultado igual a 30
fim
```

---

## 9. IA como Parte da Linguagem (Compilação vs. Execução)

A IA na BR é estritamente dividida em duas camadas com propósitos e tempos de execução distintos:

### 9.1 IA de Compilação (Meta-programação por Objetivos)
Instrui o compilador a gerar o boilerplate estrutural do sistema em arquivos `.br` físicos antes da compilação real do código de máquina/bytecode. É executada durante o comando `brasil compilar`.
```
objetivo
    criar sistema de estoque
    com controle de entrada e saida
    com geracao de relatorios semanais
fim
```
O compilador detecta este bloco, chama o LLM integrado passando o contexto do projeto, gera os arquivos na árvore de diretórios (`fonte/estoque.br`, `telas/painel.br`, etc.), e depois continua a compilação.

### 9.2 IA de Execução (Tempo de Execução/Runtime)
Executa em tempo de execução para tomadas de decisão dinâmicas ou processamento de dados não estruturados.

#### Perguntas Rápidas (Stateless)
```
resposta é pergunte para ia
    "Classifique este texto como positivo ou negativo: " + texto_cliente
fim
```

#### Agentes Persistentes (Atores de IA com Contexto)
```
analista é crie agente
    especialidade "financeiro"
fim

resposta_agente é analista.pergunte "analise as vendas de ontem"
```

---

## 10. Arquitetura da Máquina Virtual (VM BR) e Bytecode

A VM BR (`brasil-vm`) é uma máquina virtual baseada em pilha com suporte nativo a serviços. Ela não interpreta texto ou Python. Ela consome exclusivamente arquivos `.brc` que contêm bytecode binário BR.

### 10.1 Formato do Arquivo `.brc`
O arquivo compila as seguintes seções:
1. **Cabeçalho (Header)**: Assinatura mágica `BRC\x01`, versão da linguagem e metadados.
2. **Tabela de Constantes (Pool)**: Strings, decimais e inteiros literais.
3. **Seção de Permissões**: Lista de flags de privilégios (`INTERNET`, `ARQUIVO`, `BANCO`).
4. **Instruções de Inicialização**: Código de entrada principal.
5. **Seção de Funções e Classes**: Assinaturas de métodos e declarações de tipos.

### 10.2 Opcodes da VM (Instruções Principais)
A VM trabalha com opcodes de 8 bits. Para manter a VM imutável e estável a longo prazo, todas as chamadas de serviços complexos (IA, GUI, DB, HTTP) são encapsuladas no opcode dinâmico `CHAMAR_SERVICO`.

* `EMPILHAR_CONST <const_idx>`: Coloca um valor constante no topo da pilha.
* `DEFINIR_VAR <var_idx>`: Remove o topo da pilha e armazena na variável.
* `OBTER_VAR <var_idx>`: Coloca o valor da variável no topo da pilha.
* `SOMAR`, `SUBTRAIR`, `MULTIPLICAR`, `DIVIDIR`: Operações matemáticas na pilha.
* `SALTO <endereco>`: Desvio incondicional de fluxo.
* `SALTO_SE_FALSO <endereco>`: Desvio condicional baseado no topo da pilha.
* `CHAMAR_METODO <func_idx>`: Chama uma função/método BR.
* `CHAMAR_SERVICO <servico_id> <metodo_id>`: O opcode flexível da VM BR.
  * **Serviço 1 (IA)**:
    * `METODO_PERGUNTAR`: Envia um prompt à IA de execução.
    * `METODO_CRIAR_AGENTE`: Cria um agente de IA contextual.
  * **Serviço 2 (WEB)**:
    * `METODO_REQUISICAO`: Realiza chamada HTTP.
    * `METODO_SERVIDOR`: Inicializa servidor HTTP.
  * **Serviço 3 (BANCO)**:
    * `METODO_BUSCAR`: Executa busca no SQLite interno.
    * `METODO_SALVAR`: Salva um registro no banco de dados.
  * **Serviço 4 (GRAFICOS)**:
    * `METODO_JANELA`: Inicializa janela de interface gráfica.

---

## 11. Sistema de Classes e POO

A linguagem BR suporta programação orientada a objetos com classes declarativas, campos tipados, instanciação por chamada e acesso a propriedades por ponto. O objetivo é manter POO legível em português, sem exigir anotações extensas para casos simples.

### 11.1 Declaração de Classes
Uma classe é declarada com `classe`, seguida do nome e de um bloco indentado de campos. Campos podem declarar tipo usando `:`.
```
classe Pessoa
    nome: texto
    idade: inteiro
fim
```

### 11.2 Instanciação e Propriedades
A instanciação usa chamada pelo nome da classe. Os argumentos são atribuídos aos campos na ordem declarada.
```
pessoa é Pessoa("Ana", 30)
mostre pessoa.nome
mostre pessoa.idade
```

### 11.3 Encapsulamento
Campos declarados em classe formam o contrato público inicial do objeto. A validação semântica deve rejeitar acesso a propriedades inexistentes quando o tipo do objeto for conhecido em tempo de compilação.

### 11.4 Métodos
Métodos seguem a mesma regra de chamada de funções e são acessados por ponto. A forma canônica para evolução da linguagem é:
```
classe Conta
    titular: texto
    saldo: decimal

    função depositar recebendo valor
        saldo é saldo + valor
    fim
fim
```
Na implementação atual, o núcleo já reconhece classes, campos, instanciação e acesso a propriedades. Métodos de instância fazem parte do contrato da linguagem e devem ser compilados para chamadas `CHAMAR_METODO`.

### 11.5 Herança e Composição
A BR privilegia composição antes de herança. O recurso de herança será reservado para contratos explícitos e hierarquias pequenas:
```
classe Usuario
    nome: texto
fim

classe Administrador herda Usuario
    nivel: inteiro
fim
```
Enquanto `herda` não estiver disponível no compilador, bibliotecas oficiais devem modelar reutilização por composição.

---

## 12. Estruturas de Controle

As estruturas de controle da BR usam blocos por indentação e encerramento opcional por `fim`, mantendo leitura linear em português.

### 12.1 Condicionais
```
se saldo > 0 então
    mostre "conta ativa"
senão
    mostre "conta sem saldo"
fim
```

### 12.2 Escolha e Casos
`escolha` avalia uma expressão uma vez e executa o primeiro `caso` compatível. `padrao` cobre o fluxo alternativo.
```
escolha status
    caso "novo"
        mostre "cadastrar"
    caso "ativo"
        mostre "processar"
    padrao
        mostre "ignorar"
fim
```

### 12.3 Repetição por Condição
```
enquanto tentativas < 3
    mostre "tentando"
    tentativas é tentativas + 1
fim
```

### 12.4 Repetição por Contagem
```
repita 5 vezes
    mostre "processando"
fim
```

### 12.5 Iteração em Coleções
```
para cada cliente em clientes
    mostre cliente.nome
fim
```

### 12.6 Regras de Bloco
Um bloco inicia quando a linha seguinte aumenta a indentação. O bloco termina quando a indentação retorna ao nível anterior. A palavra `fim` é aceita para clareza, especialmente em documentação, exemplos didáticos e arquivos longos.

---

## 13. Debugging e Profiling

O ecossistema BR inclui depuração e medição de desempenho como ferramentas oficiais, não como bibliotecas externas obrigatórias.

### 13.1 Depurador
O depurador BR deve permitir:
* breakpoints por arquivo e linha;
* execução passo a passo;
* entrada e saída de funções;
* inspeção de variáveis;
* exibição da pilha de chamadas;
* listagem de contexto do código-fonte.

Comandos canônicos do depurador:
```
ponto_parada principal.br 12
continuar
passo
entrar
sair
variaveis
pilha
inspecionar cliente
```

### 13.2 Erros com Localização
Erros léxicos, sintáticos, semânticos e de execução devem reportar linha e coluna sempre que possível:
```
Erro Sintático [Linha 4, Coluna 12]: Esperava 'fim'
```

### 13.3 Profiling
O profiler mede tempo por função, quantidade de chamadas e custo acumulado por trecho.
```
brasil perfil fonte/principal.br
```
Relatórios oficiais devem priorizar:
* funções mais caras por tempo total;
* funções mais caras por chamada;
* serviços nativos mais usados;
* pontos de alocação de listas e registros;
* tempo gasto em IA, rede, banco e GUI.

### 13.4 Saída de Perfil
Formato textual mínimo:
```
Função                 Chamadas    Total(ms)    Médio(ms)
calcular_relatorio     10          185.4        18.5
buscar_clientes        1           42.0         42.0
```

---

## 14. Otimização e Performance

A BR aplica otimizações antes da geração de bytecode, preservando a semântica do código-fonte.

### 14.1 Otimizações de Compilação
O compilador pode executar:
* dobramento de constantes;
* eliminação de código morto;
* simplificação de expressões booleanas;
* remoção de variáveis temporárias não observáveis;
* resolução antecipada de imports;
* validação de tipos antes da emissão de bytecode.

Exemplo:
```
limite é 10 * 2
```
Pode ser compilado como:
```
limite é 20
```

### 14.2 Otimizações da VM
A VM deve otimizar execução por:
* tabela de constantes compacta;
* acesso rápido a variáveis locais;
* cache de chamadas de função;
* cache de resolução de propriedades;
* despacho eficiente de opcodes;
* serviços nativos encapsulados em `CHAMAR_SERVICO`.

### 14.3 JIT
O JIT é uma etapa opcional para trechos quentes. Um trecho quente é uma função ou laço executado muitas vezes durante a mesma execução.
```
função calcular recebendo n
    total é 0
    enquanto n > 0
        total é total + n
        n é n - 1
    fim
    devolva total
fim
```

### 14.4 Metas de Performance
Implementações oficiais devem buscar:
* inicialização rápida para scripts pequenos;
* execução previsível para automações;
* baixo custo de memória em `.brc`;
* isolamento de custos altos de IA, banco, rede e GUI;
* desempenho suficiente para ferramentas escritas na própria BR.

---

## 15. IDE e Ferramentas

O conjunto oficial de ferramentas da BR deve cobrir edição, execução, compilação, testes, empacotamento e documentação.

### 15.1 CLI Oficial
O comando `br` executa os fluxos principais:
```
br arquivo.br
br -i
br -c "mostre \"Olá\""
br --compile arquivo.br
br --run arquivo.brc
```

### 15.2 REPL
O REPL permite testar expressões e instruções rapidamente:
```
br -i
> nome é "Ana"
> mostre nome
```

### 15.3 Linter e Formatador
O linter verifica qualidade, consistência e armadilhas comuns:
```
brasil lint fonte/principal.br
```
O formatador aplica indentação e espaçamento canônicos:
```
brasil formatar fonte/principal.br
```

### 15.4 Servidor de Linguagem
O LSP oficial deve oferecer:
* destaque semântico;
* diagnósticos em tempo real;
* completar símbolos;
* ir para definição;
* renomear símbolo;
* ajuda contextual para palavras reservadas;
* navegação entre `.br`, `.bri`, `.brt` e `.brd`.

### 15.5 Extensão VS Code
A extensão oficial deve registrar:
* arquivos `.br`, `.brt`, `.bri`, `.bra`, `.brm` e `.brd`;
* tema e gramática de sintaxe;
* snippets de funções, testes, classes e blocos de controle;
* comandos para executar, compilar e testar o arquivo atual.

---

## 16. CI/CD e Automação

Projetos BR devem ser fáceis de validar em ambientes automatizados.

### 16.1 Pipeline Mínimo
Um pipeline oficial deve executar:
1. validação de sintaxe das ferramentas;
2. análise léxica e sintática dos arquivos `.br`;
3. análise semântica;
4. testes `.brt` e blocos `teste`;
5. compilação para `.brc`;
6. execução de amostras críticas;
7. geração de pacote `.brp`, quando aplicável.

### 16.2 GitHub Actions
Exemplo de workflow:
```
name: BR CI

on:
  push:
  pull_request:

jobs:
  validar:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with:
          python-version: "3.10"
      - name: Validar ferramentas
        run: python -m py_compile br.py ferramentas\*.py
      - name: Rodar testes BR
        run: python ferramentas\tester.py testes\automatizados.br
```

### 16.3 Versionamento
Projetos BR usam manifesto `.brm` para declarar nome, versão, dependências e permissões. Releases devem empacotar o manifesto, os bytecodes e os arquivos de documentação gerada.

### 16.4 Automação Local
Comandos recomendados:
```
br --compile fonte/principal.br
br --run fonte/principal.brc
python ferramentas/tester.py testes/automatizados.br
```

---

## 17. Interface e UX

A experiência da BR deve priorizar clareza para quem programa em português e previsibilidade para quem mantém sistemas grandes.

### 17.1 Mensagens Humanas
Mensagens de erro, ajuda e diagnóstico devem ser escritas em português claro:
```
Erro: variável 'cliente' não foi declarada.
Sugestão: declare 'cliente' antes de usar ou verifique a grafia.
```

### 17.2 GUI Declarativa
Interfaces nativas usam blocos declarativos:
```
crie uma janela
    titulo "Cadastro"
    largura 480
    altura 320

    campo nome
    botao salvar "Salvar"

    quando clicar em salvar
        mostre nome.texto
    fim
fim
```

### 17.3 UX de CLI
Ferramentas oficiais devem:
* aceitar caminhos relativos e absolutos;
* procurar arquivos em projeto, biblioteca local e `BR_PATH`;
* retornar código de saída diferente de zero em falhas;
* imprimir ajuda com `--help`;
* evitar logs verbosos sem opção explícita.

### 17.4 Acessibilidade
Componentes GUI oficiais devem expor rótulos, foco por teclado e estados acessíveis. Interfaces geradas por IA também precisam seguir esses contratos.

---

## 18. Documentação e Ensino

A documentação da BR é parte do ecossistema da linguagem.

### 18.1 Documentação Viva
Blocos `explicacao` documentam decisões e intenção diretamente no código:
```
explicacao
    Este módulo calcula o saldo consolidado dos clientes ativos.
fim
```
O gerador de documentação transforma esses blocos e assinaturas públicas em arquivos `.brd`.

### 18.2 Exemplos Executáveis
Todo exemplo de documentação deve ser executável sempre que possível:
```
teste "exemplo de soma"
    resultado é 2 + 3
    espere resultado igual a 5
fim
```

### 18.3 Trilhas de Ensino
A documentação oficial deve cobrir:
* primeiros passos;
* variáveis e tipos;
* funções;
* controle de fluxo;
* listas e registros;
* classes;
* módulos;
* testes;
* banco, internet e GUI;
* IA de compilação e execução;
* empacotamento e publicação.

### 18.4 Estilo Didático
Materiais de ensino devem usar nomes de domínio reais e pequenos exemplos completos, evitando trechos desconectados do fluxo de execução.

---

## 19. Recursos de Linguagem

Esta seção consolida recursos transversais que não pertencem a uma única área do manual.

### 19.1 Comentários
Comentários de linha começam com `#`:
```
# Calcula o total com desconto
total é subtotal - desconto
```

### 19.2 Literais
A linguagem reconhece:
* inteiros;
* decimais;
* textos entre aspas;
* booleanos `verdadeiro` e `falso`;
* `nulo`;
* listas;
* registros.

### 19.3 Operadores
Operadores básicos:
```
soma é 2 + 3
diferenca é 10 - 4
produto é 5 * 6
quociente é 20 / 2
ativo é idade maior que 18
mesmo_nome é nome igual "Ana"
```

### 19.4 Importação
Importação de módulos:
```
importar "usuarios"
importar "usuarios" como usr
```
Importação seletiva:
```
de "usuarios" importe buscar e salvar
```

### 19.5 Contratos
Arquivos `.bri` descrevem interfaces públicas compartilhadas entre módulos:
```
contrato RepositorioUsuarios
    função buscar recebendo id
    função salvar recebendo usuario
fim
```

### 19.6 Compatibilidade
Versões futuras da BR devem manter compatibilidade com `.brc` sempre que o cabeçalho de versão permitir. Mudanças incompatíveis exigem incremento explícito da versão de bytecode.

---

## 20. Infraestrutura e DevOps

A infraestrutura da BR cobre distribuição, pacotes, ambientes, permissões e evolução rumo à autohospedagem.

### 20.1 Manifesto de Projeto
O arquivo `.brm` descreve o projeto:
```
nome "meu-projeto"
versao "1.0.0"

dependencias
    web "1.0"
    banco "1.0"
fim

permissoes
    internet
    arquivos
fim
```

### 20.2 Pacotes
Pacotes `.brp` agrupam manifesto, bytecode e recursos necessários para execução:
* `projeto.brm`;
* arquivos `.brc`;
* documentação `.brd`;
* recursos estáticos;
* metadados de permissões.

### 20.3 Registro de Pacotes
O gerenciador `mercado` deve prover:
```
mercado instalar web
mercado publicar
mercado atualizar
mercado remover web
```
Pacotes publicados precisam declarar versão, licença, compatibilidade de BR e permissões solicitadas.

### 20.4 Ambientes
A busca de módulos deve considerar:
* diretório atual;
* pasta `modulos` do projeto;
* módulos do usuário em `~/.br/modulos`;
* biblioteca padrão;
* caminhos declarados em `BR_PATH`.

### 20.5 Roadmap de Autohospedagem
A linguagem BR será evoluída através das seguintes fases:

1. **Fase 1: Transpilador de Validação (Bootstrap)**: transpilação simples de código `.br` para Python e validação estrita de sintaxe.
2. **Fase 2: Compilador e VM Opcodes (Bootstrap Atual)**: tradução de `.br` para bytecode BR binário `.brc`, executado pela Máquina Virtual BR.
3. **Fase 3: Auto-compilação (Compilador em BR)**: reescrita do compilador da BR na própria linguagem BR.
4. **Fase 4: VM Nativa**: reescrita da VM em C ou C++ para máxima performance e distribuição leve.
5. **Fase 5: Ecossistema Puro**: compilador, gerenciador de pacotes, VM e biblioteca padrão operando sob infraestrutura BR.

### 20.6 Operação Segura
Programas BR devem declarar permissões antes de acessar rede, arquivos, banco ou serviços externos. Ambientes de produção devem executar `.brp` com permissões mínimas e auditoria de pacote.
