# Linter

Módulo `linter` da biblioteca padrão BR.

## Escopo

Este módulo cobre análise estática e avisos de qualidade.

## Arquivos

- `linter.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `linter.bri`: contrato público.
- `linter.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/linter_teste.brt`: testes do módulo.

## API Inicial

- `analisar(codigo)`: Analisa código BR.
- `avisos(codigo)`: Retorna avisos de qualidade.
- `regra(nome_item)`: Ativa regra de lint.
- `corrigir(codigo)`: Aplica correções simples.
