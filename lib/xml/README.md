# XML

Módulo `xml` da biblioteca padrão BR.

## Escopo

Este módulo cobre ler, gerar, validar e consultar arquivos XML.

## Arquivos

- `xml.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `xml.bri`: contrato público.
- `xml.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/xml_teste.brt`: testes do módulo.

## API Inicial

- `validar(dados)`: Valida XML.
- `ler(dados)`: Lê XML.
- `gerar(dados)`: Gera XML.
- `consultar(dados, caminho)`: Consulta nó XML.
