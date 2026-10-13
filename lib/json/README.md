# JSON

Módulo `json` da biblioteca padrão BR.

## Escopo

Este módulo cobre parser, gerador e validador JSON.

## Arquivos

- `json.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `json.bri`: contrato público.
- `json.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/json_teste.brt`: testes do módulo.

## API Inicial

- `validar(dados)`: Valida texto JSON.
- `gerar(dados)`: Gera texto JSON.
- `parsear(dados)`: Converte JSON em estrutura BR.
- `bonito(dados)`: Formata JSON para leitura.
