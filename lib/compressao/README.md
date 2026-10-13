# Compressão

Módulo `compressao` da biblioteca padrão BR.

## Escopo

Este módulo cobre compactar e descompactar ZIP, GZIP e TAR.

## Arquivos

- `compressao.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `compressao.bri`: contrato público.
- `compressao.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/compressao_teste.brt`: testes do módulo.

## API Inicial

- `zip(caminho)`: Compacta ZIP.
- `unzip(caminho)`: Descompacta ZIP.
- `gzip(caminho)`: Compacta GZIP.
- `tar(caminho)`: Cria TAR.
