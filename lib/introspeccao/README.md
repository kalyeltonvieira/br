# Introspecção

Módulo `introspeccao` da biblioteca padrão BR.

## Escopo

Este módulo cobre inspecionar objetos, módulos, tipos e funções.

## Arquivos

- `introspeccao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `introspeccao.bri`: contrato público.
- `introspeccao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/introspeccao_teste.brt`: testes do módulo.

## API Inicial

- `tipo(valor)`: Inspeciona tipo.
- `funcoes(valor)`: Lista funções.
- `campos(valor)`: Lista campos.
- `modulo(valor)`: Inspeciona módulo.
