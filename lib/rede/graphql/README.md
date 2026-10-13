# GraphQL

Módulo para criação e consumo de APIs GraphQL em BR.

## Instalação

```br
usar rede.graphql
```

## Exemplo de Uso

```br
usar rede.graphql

# Criar schema
schema é criar_schema "MinhaAPI", "type Query { ola: String }"
adicionar_query schema, "ola", funcao()
    devolva "Olá, mundo!"
fim

# Executar query
resultado é executar_query schema, "{ ola }"
mostre resultado.dados

# Criar cliente
cliente é criar_cliente "https://api.exemplo.com/graphql"
definir_cabecalho cliente, "Authorization", "Bearer token"

# Fazer query
resposta é query cliente, "{ usuarios { nome email } }"
```

## Funções Disponíveis

- `criar_schema(nome, definicao)` - Cria schema
- `adicionar_tipo(schema, nome, campos)` - Adiciona tipo
- `adicionar_query(schema, nome, resolver)` - Adiciona query
- `adicionar_mutation(schema, nome, resolver)` - Adiciona mutation
- `adicionar_subscription(schema, nome, resolver)` - Adiciona subscription
- `executar_query(schema, query, variaveis)` - Executa query
- `executar_mutation(schema, mutation, variaveis)` - Executa mutation
- `criar_cliente(url)` - Cria cliente
- `definir_cabecalho(cliente, chave, valor)` - Define cabeçalho
- `query(cliente, query, variaveis)` - Faz query
- `mutation(cliente, mutation, variaveis)` - Faz mutation
- `subscription(cliente, subscription, callback)` - Cria subscription
- `validar_schema(schema)` - Valida schema
- `gerar_introspecao(schema)` - Gera introspecção
