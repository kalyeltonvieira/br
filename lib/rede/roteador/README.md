# Roteador

Módulo para roteamento HTTP em BR.

## Instalação

```br
usar rede.roteador
```

## Exemplo de Uso

```br
usar rede.roteador

roteador é criar
adicionar_rota roteador, "GET", "/usuarios", funcao(req, res)
    res.corpo é "Lista de usuários"
fim

adicionar_grupo roteador, "/api", funcao()
    adicionar_rota roteador, "GET", "/produtos", funcao(req, res)
        res.corpo é "Lista de produtos"
    fim
fim
```

## Funções Disponíveis

- `criar()` - Cria roteador
- `adicionar_rota(roteador, metodo, caminho, handler)` - Adiciona rota
- `adicionar_grupo(roteador, prefixo, callback)` - Adiciona grupo
- `adicionar_parametro(rota, nome, tipo)` - Adiciona parâmetro
- `adicionar_validacao(rota, validador)` - Adiciona validador
- `rotear(roteador, metodo, caminho)` - Roteia requisição
- `definir_prefixo(roteador, prefixo)` - Define prefixo
