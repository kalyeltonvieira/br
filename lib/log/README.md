# Logs

Módulo `log` da biblioteca padrão BR.

## Escopo

Este módulo cobre logger, níveis, arquivos de log, rotação e formatação.

## Arquivos

- `log.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `log.bri`: contrato público.
- `log.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/log_teste.brt`: testes do módulo.

## API Inicial

- `info(mensagem)`: Registra mensagem INFO.
- `aviso(mensagem)`: Registra mensagem de aviso.
- `erro(mensagem)`: Registra erro.
- `nivel(valor)`: Define nível de log.
