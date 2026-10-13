# CORS

Módulo para configuração de CORS em BR.

## Instalação

```br
usar rede.cors
```

## Exemplo de Uso

```br
usar rede.cors

cors é criar registro
    origens: lista "https://exemplo.com"
    metodos: lista "GET", "POST", "PUT", "DELETE"
    cabecalhos: lista "Content-Type", "Authorization"
fim

permitir_credenciais cors
definir_max_idade cors, 3600

# Gerar cabeçalhos
cabecalhos é gerar_cabecalhos cors, "https://exemplo.com"
```

## Funções Disponíveis

- `criar(opcoes)` - Cria configuração CORS
- `adicionar_origem(cors, origem)` - Adiciona origem
- `adicionar_metodo(cors, metodo)` - Adiciona método
- `adicionar_cabecalho(cors, cabecalho)` - Adiciona cabeçalho
- `permitir_todas_origens(cors)` - Permite todas origens
- `permitir_credenciais(cors)` - Permite credenciais
- `definir_max_idade(cors, segundos)` - Define max idade
- `gerar_cabecalhos(cors, origem)` - Gera cabeçalhos
- `verificar_origem(cors, origem)` - Verifica origem
