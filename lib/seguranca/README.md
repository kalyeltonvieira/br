# Segurança

Módulo `seguranca` da biblioteca padrão BR.

## Escopo

Este módulo cobre hash, Base64, bcrypt, JWT, permissões e sandbox.

## Arquivos

- `seguranca.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `seguranca.bri`: contrato público.
- `seguranca.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/seguranca_teste.brt`: testes do módulo.

## API Inicial

- `hash(valor)`: Gera hash textual.
- `base64(valor)`: Codifica em Base64.
- `permitir(permissao)`: Verifica permissão.
- `jwt_validar(token)`: Valida token JWT.
