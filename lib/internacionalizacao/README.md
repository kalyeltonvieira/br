# Internacionalização

Módulo `internacionalizacao` da biblioteca padrão BR.

## Escopo

Este módulo cobre idiomas, tradução, moeda, números e localização.

## Arquivos

- `internacionalizacao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `internacionalizacao.bri`: contrato público.
- `internacionalizacao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/internacionalizacao_teste.brt`: testes do módulo.

## API Inicial

- `idioma()`: Consulta idioma.
- `traduzir(valor)`: Traduz texto.
- `moeda(valor)`: Formata moeda.
- `localizacao()`: Consulta localização.
