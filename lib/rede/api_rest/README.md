# API REST

Módulo para criação de APIs RESTful em BR.

## Instalação

```br
usar rede.api_rest
```

## Exemplo de Uso

```br
usar rede.api_rest

api é criar "Minha API", "1.0.0"
adicionar_recurso api, "usuarios", registro
    operacoes: lista "criar", "ler", "atualizar", "deletar"
fim

definir_autenticacao api, "jwt"
adicionar_rate_limit api, 100, 60

# Gerar documentação OpenAPI
doc é gerar_documentacao api
```

## Funções Disponíveis

- `criar(nome, versao)` - Cria API REST
- `adicionar_recurso(api, recurso, operacoes)` - Adiciona recurso
- `adicionar_operacao(api, recurso, metodo, handler)` - Adiciona operação
- `definir_autenticacao(api, tipo)` - Define autenticação
- `adicionar_rate_limit(api, maximo, periodo)` - Adiciona rate limit
- `gerar_documentacao(api)` - Gera OpenAPI
- `validar_requisicao(api, requisicao)` - Valida requisição
- `formatar_resposta(api, dados, status)` - Formata resposta
- `adicionar_paginacao(api, padrao, maximo)` - Adiciona paginação
- `adicionar_filtro(api, campo, tipo)` - Adiciona filtro
- `adicionar_ordenacao(api, campo, direcao)` - Adiciona ordenação
