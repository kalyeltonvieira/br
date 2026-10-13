# INI

Módulo `ini` da biblioteca padrão BR.

## Escopo

Este módulo cobre leitura, escrita e edição de arquivos INI.

## Arquivos

- `ini.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `ini.bri`: contrato público.
- `ini.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/ini_teste.brt`: testes do módulo.

## API Inicial

- `ler(dados)`: Lê conteúdo INI.
- `escrever(dados)`: Gera texto INI.
- `editar(dados, chave, valor)`: Edita uma chave INI.
- `secao(dados, nome_item)`: Consulta seção INI.
