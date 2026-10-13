# Núcleo da linguagem

Módulo `core` da biblioteca padrão BR.

## Escopo

Este módulo cobre tipos, erros, memória, conversão e funções básicas.

## Arquivos

- `core.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `core.bri`: contrato público.
- `core.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/core_teste.brt`: testes do módulo.

## API Inicial

- `identidade(valor)`: Devolve o próprio valor.
- `eh_nulo(valor)`: Verifica se um valor é nulo.
- `para_texto(valor)`: Converte um valor para texto.
- `verdade()`: Constante lógica verdadeira.
