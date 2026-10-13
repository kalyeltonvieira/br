# ENV

Módulo `env` da biblioteca padrão BR.

## Escopo

Este módulo cobre leitura e gerenciamento de variáveis de ambiente.

## Arquivos

- `env.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `env.bri`: contrato público.
- `env.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/env_teste.brt`: testes do módulo.

## API Inicial

- `ler(chave)`: Lê variável de ambiente.
- `definir(chave, valor)`: Define variável de ambiente.
- `remover(chave)`: Remove variável de ambiente.
- `listar()`: Lista variáveis de ambiente.
