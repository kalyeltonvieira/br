# Scaffold

Módulo `scaffold` da biblioteca padrão BR.

## Escopo

Este módulo cobre criação automática de projetos e templates.

## Arquivos

- `scaffold.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `scaffold.bri`: contrato público.
- `scaffold.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/scaffold_teste.brt`: testes do módulo.

## API Inicial

- `projeto(nome_item)`: Cria projeto BR.
- `arquivo(nome_item)`: Cria arquivo BR.
- `template(nome_item)`: Aplica template.
- `estrutura(nome_item)`: Cria estrutura de diretórios.
