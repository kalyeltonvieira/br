# Arquivos e pastas

Módulo `arquivo` da biblioteca padrão BR.

## Escopo

Este módulo cobre leitura, escrita e manipulação de arquivos e pastas.

## Arquivos

- `arquivo.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `arquivo.bri`: contrato público.
- `arquivo.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/arquivo_teste.brt`: testes do módulo.

## API Inicial

- `ler(caminho)`: Lê um arquivo de texto.
- `escrever(caminho, conteudo)`: Escreve texto em arquivo.
- `anexar(caminho, conteudo)`: Acrescenta texto ao fim de um arquivo.
- `linhas(caminho)`: Retorna uma lista com as linhas do arquivo.
- `existe(caminho)`: Verifica existência de caminho.
- `arquivo(caminho)`: Verifica se o caminho é um arquivo.
- `diretorio_existe(caminho)`: Verifica se o caminho é um diretório.
- `criar_diretorio(caminho)`: Cria diretório, incluindo pais.
- `listar(caminho)`: Lista nomes dentro de um diretório.
- `remover(caminho)` / `apagar(caminho)`: Remove arquivo ou diretório vazio.
- `copiar(origem, destino)`: Copia arquivo ou diretório.
- `mover(origem, destino)`: Move arquivo ou diretório.
- `tamanho(caminho)`: Retorna tamanho em bytes.
- `nome_arquivo(caminho)`: Retorna o nome final do caminho.
- `absoluto(caminho)`: Retorna caminho absoluto.
- `extensao(caminho)`: Obtém a extensão do caminho.
- `diretorio(caminho)`: Retorna o diretório do caminho.
