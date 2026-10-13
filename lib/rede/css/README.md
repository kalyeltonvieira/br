# CSS

Módulo para geração e organização de estilos CSS em BR.

## Instalação

```br
usar rede.css
```

## Exemplo de Uso

```br
usar rede.css

# Criar regra
regra é criar_regra ".container", registro
    "width": "100%"
    "padding": "20px"
    "background-color": "#fff"
fim

# Criar estilo
estilo é criar_estilo "main", lista regra
css é renderizar estilo
mostre css
```

## Funções Disponíveis

- `criar_regra(seletor, propriedades)` - Cria regra CSS
- `adicionar_propriedade(regra, propriedade, valor)` - Adiciona propriedade
- `criar_estilo(nome, regras)` - Cria estilo CSS
- `renderizar(estilo)` - Renderiza como string
- `criar_media_query(condicao, regras)` - Cria media query
- `criar_animacao(nome, quadros)` - Cria animação CSS
- `criar_variavel(nome, valor)` - Cria variável CSS
- `minificar(css)` - Minifica CSS
- `prefixar(css, prefixos)` - Adiciona vendor prefixes
- `validar(css)` - Valida sintaxe CSS
