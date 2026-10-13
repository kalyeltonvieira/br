# Concorrência

Módulo `concorrencia` da biblioteca padrão BR.

## Escopo

Este módulo cobre threads, tarefas assíncronas e promessas.

## Arquivos

- `concorrencia.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `concorrencia.bri`: contrato público.
- `concorrencia.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/concorrencia_teste.brt`: testes do módulo.

## API Inicial

- `tarefa(nome_item)`: Cria tarefa.
- `aguardar(valor)`: Aguarda resultado.
- `promessa(valor)`: Cria promessa.
- `em_paralelo(valor)`: Executa em paralelo.
