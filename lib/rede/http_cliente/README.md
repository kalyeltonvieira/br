# HTTP Cliente

Módulo para requisições HTTP em BR.

## Instalação

```br
usar rede.http_cliente
```

## Exemplo de Uso

```br
usar rede.http_cliente

# Requisição GET
resposta é get "https://api.exemplo.com/dados"
mostre resposta.status
mostre resposta.corpo

# Requisição POST
resposta é post "https://api.exemplo.com/criar", corpo, cabecalhos
mostre resposta.status
```

## Funções Disponíveis

- `get(url, cabecalhos)` - Requisição GET
- `post(url, corpo, cabecalhos)` - Requisição POST
- `put(url, corpo, cabecalhos)` - Requisição PUT
- `delete(url, cabecalhos)` - Requisição DELETE
- `patch(url, corpo, cabecalhos)` - Requisição PATCH
- `definir_timeout(cliente, segundos)` - Define timeout
- `definir_proxy(cliente, proxy)` - Define proxy
- `definir_autenticacao(cliente, tipo, credenciais)` - Define autenticação
