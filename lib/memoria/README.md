# Memória

Módulo `memoria` da biblioteca padrão BR.

## Escopo

Este módulo cobre monitorar uso de memória, referências e coleta de lixo.

## Arquivos

- `memoria.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `memoria.bri`: contrato público.
- `memoria.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/memoria_teste.brt`: testes do módulo.

## API Inicial

- `uso()`: Mede uso de memória.
- `coletar()`: Solicita coleta de lixo.
- `referencias(valor)`: Conta referências.
- `limite()`: Consulta limite de memória.
