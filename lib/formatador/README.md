# Formatador

Módulo `formatador` da biblioteca padrão BR.

## Escopo

Este módulo cobre formatação automática de código .br.

## Arquivos

- `formatador.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `formatador.bri`: contrato público.
- `formatador.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/formatador_teste.brt`: testes do módulo.

## API Inicial

- `formatar(codigo)`: Formata código BR.
- `arquivo(caminho)`: Formata arquivo BR.
- `verificar(codigo)`: Verifica se o código está formatado.
- `configurar(opcoes)`: Configura regras de formatação.
