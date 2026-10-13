# Scraping

Módulo para extração de dados de páginas web em BR.

## Instalação

```br
usar rede.scraping
```

## Exemplo de Uso

```br
usar rede.scraping

# Obter HTML
html é obter_html "https://exemplo.com"

# Selecionar elementos
elementos é selecionar_todos html, ".produto"
para cada elemento em elementos
    texto é obter_texto elemento
    preco é obter_atributo elemento, "data-preco"
    mostre texto + ": " + preco
fim

# Obter links
links é obter_links html
para cada link em links
    mostre link
fim
```

## Funções Disponíveis

- `obter_html(url)` - Obtém HTML da página
- `selecionar_todos(html, seletor)` - Seleciona elementos
- `selecionar_um(html, seletor)` - Seleciona um elemento
- `obter_texto(elemento)` - Obtém texto
- `obter_atributo(elemento, atributo)` - Obtém atributo
- `obter_links(html)` - Obtém links
- `obter_imagens(html)` - Obtém imagens
- `obter_tabelas(html)` - Obtém tabelas
- `parsear_tabela(tabela)` - Parseia tabela
- `limpar_texto(texto)` - Limpa texto
- `extrair_json(html, seletor)` - Extrai JSON
- `seguir_links(url, profundidade)` - Segue links
- `definir_cabecalho(scraper, chave, valor)` - Define cabeçalho
- `definir_proxy(scraper, proxy)` - Define proxy
- `definir_timeout(scraper, segundos)` - Define timeout
- `definir_delay(scraper, segundos)` - Define delay
