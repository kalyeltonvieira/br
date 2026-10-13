# Auth

Módulo para autenticação de usuários em BR.

## Instalação

```br
usar rede.auth
```

## Exemplo de Uso

```br
usar rede.auth

# Criar usuário
usuario é criar_usuario "João", "joao@exemplo.com", "senha123"

# Autenticar
autenticado é autenticar "joao@exemplo.com", "senha123"
se autenticado então
    mostre "Login bem-sucedido"
    definir_token autenticado, "token_gerado"
senão
    mostre "Login falhou"
fim

# Gerenciar permissões
adicionar_permissao usuario, "admin"
se verificar_permissao usuario, "admin" então
    mostre "Usuário é admin"
fim
```

## Funções Disponíveis

- `criar_usuario(nome, email, senha)` - Cria usuário
- `autenticar(email, senha)` - Autentica usuário
- `buscar_usuario(email)` - Busca usuário por email
- `buscar_usuario_por_id(id)` - Busca usuário por ID
- `atualizar_senha(usuario, senha_antiga, senha_nova)` - Atualiza senha
- `definir_token(usuario, token)` - Define token
- `validar_token(token)` - Valida token
- `revogar_token(usuario)` - Revoga token
- `ativar_usuario(usuario)` - Ativa usuário
- `desativar_usuario(usuario)` - Desativa usuário
- `definir_papel(usuario, papel)` - Define papel/role
- `verificar_permissao(usuario, permissao)` - Verifica permissão
- `adicionar_permissao(usuario, permissao)` - Adiciona permissão
- `remover_permissao(usuario, permissao)` - Remove permissão
- `registrar_tentativa_login(usuario, sucesso)` - Registra tentativa
- `bloquear_usuario(usuario)` - Bloqueia usuário
- `desbloquear_usuario(usuario)` - Desbloqueia usuário
