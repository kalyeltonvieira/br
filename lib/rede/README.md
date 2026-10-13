# Rede

Módulo `rede` da biblioteca padrão BR.

## Escopo

Este módulo cobre HTTP, URL, download, sockets, WebSocket e email.

## Arquivos

- `rede.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `rede.bri`: contrato público.
- `rede.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/rede_teste.brt`: testes do módulo.

## API Inicial

- `get(url)`: Executa requisição GET.
- `post(url, dados)`: Executa requisição POST.
- `baixar(url, caminho)`: Baixa conteúdo para arquivo.
- `url_valida(url)`: Valida URL.
