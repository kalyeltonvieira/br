# UUID

Módulo `uuid` da biblioteca padrão BR.

## Escopo

Este módulo cobre geração e validação de identificadores únicos.

## Arquivos

- `uuid.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `uuid.bri`: contrato público.
- `uuid.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/uuid_teste.brt`: testes do módulo.

## API Inicial

- `gerar()`: Gera UUID.
- `validar(valor)`: Valida UUID.
- `v4()`: Gera UUID v4.
- `curto()`: Gera identificador curto.
