# Token

Módulo `token` da biblioteca padrão BR.

## Escopo

Este módulo cobre geração e validação de tokens simples.

## Arquivos

- `token.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `token.bri`: contrato público.
- `token.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/token_teste.brt`: testes do módulo.

## API Inicial

- `gerar()`: Gera token simples.
- `validar(valor)`: Valida token.
- `expirar(valor)`: Expira token.
- `renovar(valor)`: Renova token.
