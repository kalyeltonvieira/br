# Docgen

Módulo `docgen` da biblioteca padrão BR.

## Escopo

Este módulo cobre geração automática de documentação.

## Arquivos

- `docgen.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `docgen.bri`: contrato público.
- `docgen.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/docgen_teste.brt`: testes do módulo.

## API Inicial

- `gerar(projeto)`: Gera documentação do projeto.
- `modulo(nome_item)`: Documenta módulo.
- `html(projeto)`: Gera documentação HTML.
- `markdown(projeto)`: Gera documentação Markdown.
