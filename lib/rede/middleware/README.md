# Middleware

Módulo para interceptadores de requisição e resposta em BR.

## Instalação

```br
usar rede.middleware
```

## Exemplo de Uso

```br
usar rede.middleware

# Criar middleware de log
middleware_log é criar "log", funcao(req, res)
    log_requisicao req
    log_resposta res
fim

# Middleware de autenticação
middleware_auth é criar "auth", funcao(req, res)
    se autenticar req então
        devolva verdadeiro
    senão
        res.status é 401
        devolva falso
    fim
fim
```

## Funções Disponíveis

- `criar(nome, handler)` - Cria middleware
- `executar(middleware, requisicao, resposta)` - Executa middleware
- `definir_ordem(middleware, ordem)` - Define ordem
- `adicionar_condicao(middleware, condicao)` - Adiciona condição
- `log_requisicao(requisicao)` - Log de requisições
- `log_resposta(resposta)` - Log de respostas
- `autenticar(requisicao)` - Middleware de autenticação
- `cors(requisicao, resposta)` - Middleware de CORS
