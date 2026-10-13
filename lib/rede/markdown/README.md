# Markdown

Módulo para leitura, escrita e conversão de Markdown em BR.

## Instalação

```br
usar rede.markdown
```

## Exemplo de Uso

```br
usar rede.markdown

# Converter Markdown para HTML
md é "# Título\n\n**Texto em negrito**"
html é para_html md
mostre html

# Criar tabela
tabela é criar_tabela lista "Nome", "Email", lista
    lista "João", "joao@exemplo.com"
    lista "Maria", "maria@exemplo.com"
fim
```

## Funções Disponíveis

- `para_html(markdown)` - Converte para HTML
- `para_markdown(html)` - Converte para Markdown
- `ler(arquivo)` - Lê arquivo Markdown
- `escrever(arquivo, markdown)` - Escreve arquivo
- `criar_tabela(cabecalho, linhas)` - Cria tabela
- `criar_lista(itens, ordenada)` - Cria lista
- `criar_link(texto, url)` - Cria link
- `criar_imagem(alt, url)` - Cria imagem
- `criar_codigo(codigo, linguagem)` - Cria bloco de código
- `criar_citacao(texto)` - Cria citação
- `validar(markdown)` - Valida sintaxe
