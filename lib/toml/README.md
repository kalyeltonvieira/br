# TOML

Módulo `toml` da biblioteca padrão BR.

## Escopo

Este módulo cobre leitura, escrita e validação de arquivos TOML.

## Arquivos

- `toml.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `toml.bri`: contrato público.
- `toml.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/toml_teste.brt`: testes do módulo.

## API Inicial

- `ler(dados)`: Lê conteúdo TOML.
- `escrever(dados)`: Gera texto TOML.
- `validar(dados)`: Valida sintaxe TOML.
- `secao(dados, nome_item)`: Consulta seção TOML.
