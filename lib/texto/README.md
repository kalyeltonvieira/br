# Texto e strings

Módulo `texto` da biblioteca padrão BR.

## Escopo

Este módulo cobre strings, regex, validações, formatação e acentos.

## Arquivos

- `texto.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `texto.bri`: contrato público.
- `texto.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/texto_teste.brt`: testes do módulo.

## API Inicial

- `vazio(valor)`: Verifica texto vazio.
- `contem(valor, trecho)`: Verifica presença de trecho.
- `slug(valor)`: Gera slug textual.
- `formatar(modelo, valor)`: Formata texto simples.
