# HTML

Módulo para geração e manipulação de HTML em BR.

## Instalação

```br
usar rede.html
```

## Exemplo de Uso

```br
usar rede.html

# Criar elemento
div é criar_elemento "div", registro
    class: "container"
    id: "main"
fim

definir_conteudo div, "Olá, mundo!"

# Adicionar filhos
p é criar_elemento "p", registro
definir_conteudo p, "Parágrafo"
adicionar_filho div, p

# Renderizar
html é renderizar div
mostre html
```

## Funções Disponíveis

- `criar_elemento(tag, atributos)` - Cria elemento HTML
- `adicionar_filho(elemento, filho)` - Adiciona filho
- `definir_atributo(elemento, nome, valor)` - Define atributo
- `definir_conteudo(elemento, conteudo)` - Define conteúdo
- `renderizar(elemento)` - Renderiza como string
- `criar_documento(titulo, conteudo)` - Cria documento HTML
- `adicionar_css(elemento, css)` - Adiciona CSS
- `adicionar_classe(elemento, classe)` - Adiciona classe
- `adicionar_id(elemento, id)` - Adiciona ID
- `selecionar(elemento, seletor)` - Seleciona elementos
- `parsear(html)` - Parseia HTML
