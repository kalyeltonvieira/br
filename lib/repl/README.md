# REPL

Módulo `repl` da biblioteca padrão BR.

## Escopo

Este módulo cobre console interativo da linguagem.

## Arquivos

- `repl.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `repl.bri`: contrato público.
- `repl.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/repl_teste.brt`: testes do módulo.

## API Inicial

- `iniciar()`: Inicia console interativo.
- `avaliar(codigo)`: Avalia código BR.
- `historico()`: Retorna histórico do console.
- `sair()`: Finaliza sessão REPL.
