# Cookie

Módulo para gerenciamento de cookies HTTP em BR.

## Instalação

```br
usar rede.cookie
```

## Exemplo de Uso

```br
usar rede.cookie

# Criar cookie
cookie é criar "usuario", "joao", registro
    expira: 3600
    seguro: verdadeiro
    http_only: verdadeiro
    same_site: "Strict"
fim

# Assinar cookie para segurança
assinar cookie, "segredo_secreto"

# Escrever cookie
escrever cookie

# Ler cookie
valor é ler "usuario"
```

## Funções Disponíveis

- `criar(nome, valor, opcoes)` - Cria cookie
- `ler(nome)` - Lê cookie
- `escrever(cookie)` - Escreve cookie
- `deletar(nome)` - Deleta cookie
- `definir_expiracao(cookie, segundos)` - Define expiração
- `definir_seguro(cookie, seguro)` - Define seguro
- `definir_http_only(cookie, http_only)` - Define HTTP only
- `definir_same_site(cookie, same_site)` - Define SameSite
- `definir_dominio(cookie, dominio)` - Define domínio
- `definir_caminho(cookie, caminho)` - Define caminho
- `assinar(cookie, segredo)` - Assina cookie
- `verificar_assinatura(cookie, segredo)` - Verifica assinatura
