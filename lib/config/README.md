# Configuração

Módulo `config` da biblioteca padrão BR.

## Escopo

Este módulo cobre carregar arquivos INI, ENV, YAML e TOML.

## Arquivos

- `config.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `config.bri`: contrato público.
- `config.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/config_teste.brt`: testes do módulo.

## API Inicial

- `ini(dados)`: Carrega INI.
- `env(dados)`: Carrega ENV.
- `yaml(dados)`: Carrega YAML.
- `toml(dados)`: Carrega TOML.
