# Path

Módulo `path` da biblioteca padrão BR.

## Escopo

Este módulo cobre manipulação de caminhos, diretórios e extensões.

## Arquivos

- `path.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `path.bri`: contrato público.
- `path.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/path_teste.brt`: testes do módulo.

## API Inicial

- `juntar(base, nome_item)`: Junta partes de caminho.
- `diretorio(caminho)`: Obtém diretório de um caminho.
- `extensao(caminho)`: Obtém extensão de arquivo.
- `normalizar(caminho)`: Normaliza separadores de caminho.
