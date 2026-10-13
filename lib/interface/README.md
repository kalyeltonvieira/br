# Interface de terminal

Módulo `interface` da biblioteca padrão BR.

## Escopo

Este módulo cobre terminal, cores, menus, perguntas, progresso e tabelas.

## Arquivos

- `interface.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `interface.bri`: contrato público.
- `interface.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/interface_teste.brt`: testes do módulo.

## API Inicial

- `linha(mensagem)`: Monta linha de terminal.
- `perguntar(mensagem)`: Solicita entrada do usuário.
- `progresso(valor)`: Exibe progresso.
- `tabela(dados)`: Renderiza tabela.
