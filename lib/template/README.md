# Templates

Módulo `template` da biblioteca padrão BR.

## Escopo

Este módulo cobre renderizar templates de texto, HTML e código.

## Arquivos

- `template.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `template.bri`: contrato público.
- `template.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/template_teste.brt`: testes do módulo.

## API Inicial

- `renderizar(modelo, dados)`: Renderiza template.
- `html(modelo)`: Renderiza HTML.
- `codigo(modelo)`: Renderiza código.
- `variavel(nome_item)`: Cria variável de template.
