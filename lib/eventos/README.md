# Eventos

Módulo `eventos` da biblioteca padrão BR.

## Escopo

Este módulo cobre criar emissores, ouvintes e filas de eventos.

## Arquivos

- `eventos.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `eventos.bri`: contrato público.
- `eventos.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/eventos_teste.brt`: testes do módulo.

## API Inicial

- `emissor(nome_item)`: Cria emissor.
- `ouvir(nome_item)`: Registra ouvinte.
- `emitir(nome_item)`: Emite evento.
- `fila()`: Cria fila de eventos.
