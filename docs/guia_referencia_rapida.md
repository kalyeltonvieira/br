# Guia de Referência Rápida - Linguagem BR
**Versão 2.0.0**

---

## 📝 Sintaxe Básica

### Variáveis
```br
nome é "valor"                    # Inferência automática
idade : inteiro é 25              # Tipo explícito
constante PI é 3.14159           # Constante
```

### Operadores
```br
+  -  *  /                       # Aritméticos
== != > < >= <=                  # Comparação
e  nao                           # Lógicos
```

### Saída
```br
mostre "texto"
mostre "Valor: ${variavel}"
```

---

## 🔄 Controle de Fluxo

### Condicional
```br
se condição então
    # código
senão
    # código
fim
```

### Escolha/Caso
```br
escolha variavel
    caso "valor1"
        # código
    caso "valor2"
        # código
    padrao
        # código padrão
fim
```

### Loops
```br
# Enquanto
enquanto condição
    # código
fim

# Repetição
repita 5 vezes
    # código
fim

# Contador
conte de 1 até 10 como i
    mostre i
fim

# Para cada
para cada item em lista
    mostre item
fim
```

---

## 🎯 Funções

```br
# Função com parâmetros
função nome recebendo param1 e param2
    devolva param1 + param2
fim

# Função sem parâmetros
função nome
    devolva "valor"
fim

# Chamada
resultado é nome(10, 20)
```

---

## 🏛️ Classes

```br
# Classe simples
classe Nome
    campo1: tipo
    campo2: tipo
    
    função metodo recebendo param
        mostre "${isto.campo1}"
    fim
fim

# Herança
classe Filha herda Pai
    campo3: tipo
fim

# Instanciação
obj é Nome("valor1", "valor2")
obj.metodo("param")
```

---

## 🎨 F-strings

```br
nome é "Maria"
idade é 30

# Básica
"Olá, ${nome}!"

# Com expressão
"Idade em 5 anos: ${idade + 5}"

# Múltiplas variáveis
"${nome} tem ${idade} anos"
```

---

## 📊 List Comprehensions

```br
# Transformação
quadrados é [x * x para x em lista]

# Filtro
pares é [x para x em lista se x % 2 == 0]

# Transformação + Filtro
quadrados_pares é [x * x para x em lista se x % 2 == 0]
```

---

## ⚠️ Tratamento de Erros

```br
tente
    # código que pode falhar
    resultado é 10 / 0
capture erro
    mostre "Erro: ${erro}"
fim
```

---

## 🧵 Paralelismo

```br
execute em paralelo
    tarefa1()
    tarefa2()
    tarefa3()
fim
```

---

## 📦 Módulos

```br
importar "modulo"
importar "modulo" como alias

# Usar
modulo.funcao()
alias.funcao()
```

---

## 🗂️ Estruturas de Dados

### Lista
```br
lista é [1, 2, 3]
lista é ["a", "b", "c"]
lista.adicionar(4)
elemento é lista.obter(0)
```

### Registro (Dicionário)
```br
reg é registro
    nome: "Maria"
    idade: 30
fim

valor é reg.obter("nome")
reg.definir("chave", "valor")
```

---

## 🔧 Tipos

- `inteiro` - Números inteiros
- `decimal` - Números com ponto flutuante
- `texto` - Strings
- `booleano` - verdadeiro/falso
- `lista` - Listas/arrays
- `registro` - Dicionários
- `nulo` - Valor nulo

---

## 🎓 Exemplos Rápidos

### Olá Mundo
```br
mostre "Olá, Mundo!"
```

### Função
```br
função soma recebendo a e b
    devolva a + b
fim

mostre soma(10, 20)
```

### Classe
```br
classe Pessoa
    nome: texto
    
    função falar
        mostre "Olá, eu sou ${isto.nome}"
    fim
fim

p é Pessoa("Ana")
p.falar()
```

### Loop
```br
conte de 1 até 5 como i
    mostre "Número: ${i}"
fim
```

---

## 🚀 Comandos

```bash
# Build
python build.py

# Executar
bin/br.exe arquivo.br

# Modo interativo
bin/br.exe -i

# Ajuda
bin/br.exe --help
```

---

**Dica**: Use `mostre` para debug e `tente/capture` para tratamento de erros!