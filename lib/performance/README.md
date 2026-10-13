# Performance

Módulo `performance` da biblioteca padrão BR.

## Escopo

Este módulo cobre profiler, benchmark, tempo de execução e memória.

## Arquivos

- `performance.br`: implementação principal.
- `api.br`: API complementar e metadados.
- `performance.bri`: contrato público.
- `performance.brd`: documentação gerada/estática.
- `exemplos/basico.br`: exemplo executável.
- `testes/performance_teste.brt`: testes do módulo.

## API Inicial

- `tempo(nome_item)`: Mede tempo.
- `benchmark(nome_item)`: Executa benchmark.
- `perfil(nome_item)`: Executa profiling.
- `memoria()`: Mede memória.
