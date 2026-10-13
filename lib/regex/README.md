# Regex

Módulo `regex` da biblioteca padrão BR.

## Escopo

Este módulo cobre expressões regulares e busca avançada em texto.

## Arquivos

- `regex.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `regex.bri`: contrato público.
- `regex.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/regex_teste.brt`: testes do módulo.

## API Inicial

- `buscar(padrao_texto, valor)`: Busca padrão em texto.
- `todos(padrao_texto, valor)`: Retorna todas as ocorrências do padrão.
- `contar(padrao_texto, valor)`: Conta ocorrências do padrão.
- `substituir(padrao_texto, novo, valor)`: Substitui padrões em texto.
- `validar(padrao_texto, valor)`: Valida texto por expressão regular.
- `grupos(padrao_texto, valor)`: Retorna grupos encontrados.
- `dividir(padrao_texto, valor)`: Divide texto usando expressão regular.
- `escapar(valor)`: Escapa texto para uso literal em regex.
