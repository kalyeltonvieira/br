# Pacotes

Módulo `pacote` da biblioteca padrão BR.

## Escopo

Este módulo cobre instalação, remoção, atualização, manifesto, versões e dependências.

## Arquivos

- `pacote.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `pacote.bri`: contrato público.
- `pacote.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/pacote_teste.brt`: testes do módulo.

## API Inicial

- `instalar(nome_item)`: Instala pacote.
- `remover(nome_item)`: Remove pacote.
- `atualizar(nome_item)`: Atualiza pacote.
- `versao(nome_item)`: Consulta versão.
