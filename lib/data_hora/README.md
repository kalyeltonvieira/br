# Datas e horários

Módulo `data_hora` da biblioteca padrão BR.

## Escopo

Este módulo cobre datas, horários, duração, calendário e fusos.

## Arquivos

- `data_hora.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `data_hora.bri`: contrato público.
- `data_hora.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/data_hora_teste.brt`: testes do módulo.

## API Inicial

- `agora()`: Retorna data/hora atual.
- `hoje()`: Retorna data atual.
- `diferenca(inicio, fim_valor)`: Calcula duração entre datas.
- `fuso()`: Retorna fuso padrão.
