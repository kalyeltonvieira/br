# Sessão

Módulo para gerenciamento de sessões web em BR.

## Instalação

```br
usar rede.sessao
```

## Exemplo de Uso

```br
usar rede.sessao

# Criar sessão
sessao é criar "user123", registro
    nome: "João"
    email: "joao@exemplo.com"
fim

# Definir dados
definir_dado sessao, "ultimo_acesso", data atual
definir_expiracao sessao, 3600

# Verificar expiração
se expirada sessao então
    destruir sessao.id
fim
```

## Funções Disponíveis

- `criar(id, dados)` - Cria sessão
- `obter(id)` - Obtém sessão
- `salvar(sessao)` - Salva sessão
- `destruir(id)` - Destrói sessão
- `definir_dado(sessao, chave, valor)` - Define dado
- `obter_dado(sessao, chave)` - Obtém dado
- `remover_dado(sessao, chave)` - Remove dado
- `definir_expiracao(sessao, segundos)` - Define expiração
- `expirada(sessao)` - Verifica expiração
- `regenerar_id(sessao)` - Regenera ID
