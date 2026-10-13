# Depuração

Módulo `depuracao` da biblioteca padrão BR.

## Escopo

Este módulo cobre breakpoints, pilha de chamadas, rastreamento e inspeção.

## Arquivos

- `depuracao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `depuracao.bri`: contrato público.
- `depuracao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/depuracao_teste.brt`: testes do módulo.

## API Inicial

- `breakpoint(linha)`: Cria breakpoint.
- `pilha()`: Obtém pilha.
- `rastrear(valor)`: Rastreia valor.
- `inspecionar(valor)`: Inspeciona variável.
