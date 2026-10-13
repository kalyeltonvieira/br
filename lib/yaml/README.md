# YAML

Módulo `yaml` da biblioteca padrão BR.

## Escopo

Este módulo cobre leitura, escrita e validação de arquivos YAML.

## Arquivos

- `yaml.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `yaml.bri`: contrato público.
- `yaml.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/yaml_teste.brt`: testes do módulo.

## API Inicial

- `ler(dados)`: Lê conteúdo YAML.
- `escrever(dados)`: Gera texto YAML.
- `validar(dados)`: Valida sintaxe YAML.
- `chave(dados, nome_item)`: Consulta chave YAML.
