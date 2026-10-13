# Serialização

Módulo `serializacao` da biblioteca padrão BR.

## Escopo

Este módulo cobre converter dados entre objeto, texto, binário e formatos externos.

## Arquivos

- `serializacao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `serializacao.bri`: contrato público.
- `serializacao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/serializacao_teste.brt`: testes do módulo.

## API Inicial

- `para_texto(dados)`: Serializa para texto.
- `de_texto(dados)`: Desserializa texto.
- `para_binario(dados)`: Serializa para binário.
- `clonar(dados)`: Clona estrutura.
