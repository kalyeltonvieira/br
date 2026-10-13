# CSV

Módulo `csv` da biblioteca padrão BR.

## Escopo

Este módulo cobre ler, escrever e transformar dados CSV.

## Arquivos

- `csv.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `csv.bri`: contrato público.
- `csv.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/csv_teste.brt`: testes do módulo.

## API Inicial

- `ler(dados)`: Lê CSV.
- `escrever(dados)`: Gera CSV.
- `cabecalho(dados)`: Obtém cabeçalho.
- `linhas(dados)`: Conta linhas.
