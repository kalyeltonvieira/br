# OAuth

Módulo para login com provedores externos em BR.

## Instalação

```br
usar rede.oauth
```

## Exemplo de Uso

```br
usar rede.oauth

# Criar cliente Google
cliente é criar_google_cliente "client_id", "client_secret"
definir_redirect_uri cliente, "https://meusite.com/callback"

# Obter URL de autorização
url é obter_url_autorizacao cliente
mostre "Visite: " + url

# Processar callback
usuario é processar_callback cliente, codigo_autorizacao
mostre "Usuário autenticado: " + usuario.nome
```

## Funções Disponíveis

- `criar_cliente(provedor, client_id, client_secret)` - Cria cliente OAuth
- `definir_redirect_uri(cliente, uri)` - Define redirect URI
- `adicionar_scope(cliente, scope)` - Adiciona scope
- `obter_url_autorizacao(cliente)` - Obtém URL de autorização
- `trocar_codigo(cliente, codigo)` - Troca código por token
- `atualizar_token(cliente, refresh_token)` - Atualiza token
- `obter_usuario(cliente, token)` - Obtém usuário
- `revogar_token(cliente, token)` - Revoga token
- `validar_token(cliente, token)` - Valida token
- `criar_google_cliente(client_id, client_secret)` - Cria cliente Google
- `criar_github_cliente(client_id, client_secret)` - Cria cliente GitHub
- `criar_facebook_cliente(client_id, client_secret)` - Cria cliente Facebook
- `definir_callback(cliente, callback)` - Define callback
- `iniciar_fluxo(cliente)` - Inicia fluxo OAuth
- `processar_callback(cliente, codigo)` - Processa callback
