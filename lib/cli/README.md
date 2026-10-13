# CLI

Módulo `cli` da biblioteca padrão BR.

## Escopo

Este módulo cobre criação de comandos, flags e argumentos de terminal.

## Arquivos

- `cli.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `cli.bri`: contrato público.
- `cli.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/cli_teste.brt`: testes do módulo.

## API Inicial

- `comando(nome_item)`: Cria comando de terminal.
- `flag(nome_item)`: Define flag de comando.
- `argumento(nome_item)`: Define argumento de comando.
- `executar_cli(nome_item)`: Executa comando CLI.
