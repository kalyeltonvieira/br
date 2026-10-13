# Banco de dados

Módulo `banco` da biblioteca padrão BR.

## Escopo

Este módulo cobre SQLite, conexão, consultas, transações e migrações.

## Arquivos

- `banco.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `banco.bri`: contrato público.
- `banco.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/banco_teste.brt`: testes do módulo.

## API Inicial

- `conectar(url)`: Abre conexão.
- `consulta(sql)`: Executa consulta.
- `transacao()`: Inicia transação.
- `migrar()`: Executa migrações.
