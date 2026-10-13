# LSP

Módulo `lsp` da biblioteca padrão BR.

## Escopo

Este módulo cobre suporte para editor, autocomplete e diagnóstico.

## Arquivos

- `lsp.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `lsp.bri`: contrato público.
- `lsp.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/lsp_teste.brt`: testes do módulo.

## API Inicial

- `iniciar()`: Inicia servidor LSP.
- `autocomplete(codigo)`: Retorna sugestões de autocomplete.
- `diagnostico(codigo)`: Retorna diagnósticos do código.
- `simbolo(codigo)`: Lista símbolos do documento.
