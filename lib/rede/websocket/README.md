# WebSocket

Módulo para comunicação em tempo real via WebSocket em BR.

## Instalação

```br
usar rede.websocket
```

## Exemplo de Uso

```br
usar rede.websocket

# Criar servidor
servidor é criar_servidor 8080, funcao(mensagem)
    mostre "Mensagem recebida: " + mensagem
fim

iniciar_servidor servidor

# Criar cliente
cliente é criar_cliente "ws://localhost:8080"
definir_callback cliente, "message", funcao(msg)
    mostre "Recebido: " + msg
fim

conectar cliente
enviar cliente, "Olá, servidor!"
```

## Funções Disponíveis

- `criar_servidor(porta, callback)` - Cria servidor WS
- `iniciar_servidor(servidor)` - Inicia servidor
- `parar_servidor(servidor)` - Para servidor
- `criar_cliente(url)` - Cria cliente WS
- `conectar(cliente)` - Conecta cliente
- `desconectar(cliente)` - Desconecta cliente
- `enviar(cliente, mensagem)` - Envia mensagem
- `broadcast(servidor, mensagem)` - Broadcast
- `definir_callback(cliente, evento, callback)` - Define callback
- `adicionar_cliente(servidor, cliente)` - Adiciona cliente
- `remover_cliente(servidor, cliente)` - Remove cliente
- `obter_clientes(servidor)` - Obtém clientes
- `definir_ping_interval(servidor, segundos)` - Define ping interval
