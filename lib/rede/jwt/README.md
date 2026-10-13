# JWT

Módulo para criação e validação de JSON Web Tokens em BR.

## Instalação

```br
usar rede.jwt
```

## Exemplo de Uso

```br
usar rede.jwt

# Criar token
payload é registro
    usuario_id: 123
    nome: "João"
    email: "joao@exemplo.com"
fim

payload é definir_expiracao payload, 3600
payload é definir_issuer payload, "meusite.com"
token é criar payload, "segredo_secreto"

# Validar token
se validar token, "segredo_secreto" então
    dados é decodificar token
    mostre dados
fim
```

## Funções Disponíveis

- `criar(payload, segredo)` - Cria token JWT
- `validar(token, segredo)` - Valida token
- `decodificar(token)` - Decodifica payload
- `definir_expiracao(payload, segundos)` - Define expiração
- `definir_issuer(payload, issuer)` - Define emissor
- `definir_audience(payload, audience)` - Define audiência
- `definir_subject(payload, subject)` - Define sujeito
- `verificar_expiracao(token)` - Verifica expiração
- `verificar_issuer(token, issuer)` - Verifica emissor
- `verificar_audience(token, audience)` - Verifica audiência
- `criar_refresh_token(payload, segredo)` - Cria refresh token
- `refrescar(refresh_token, segredo)` - Refresca token
