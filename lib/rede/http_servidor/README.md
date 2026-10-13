# HTTP Servidor

Módulo para criação de servidores HTTP em BR.

## Instalação

```br
usar rede.http_servidor
```

## Exemplo de Uso

```br
usar rede.http_servidor

# Criar servidor na porta 8080
servidor é criar 8080, funcao(requisicao, resposta)
    mostre "Requisição: " + requisicao.metodo + " " + requisicao.caminho
    resposta.status é 200
    resposta.corpo é "Olá, mundo!"
fim

# Adicionar rota
adicionar_rota servidor, "GET", "/ola", funcao(req, res)
    res.corpo é "Olá!"
fim

# Iniciar servidor
iniciar servidor
```

## Funções Disponíveis

- `criar(porta, callback)` - Cria servidor HTTP
- `iniciar(servidor)` - Inicia servidor
- `parar(servidor)` - Para servidor
- `adicionar_rota(servidor, metodo, caminho, handler)` - Adiciona rota
- `adicionar_middleware(servidor, middleware)` - Adiciona middleware
- `definir_timeout(servidor, segundos)` - Define timeout
- `definir_max_conexoes(servidor, maximo)` - Define max conexões
