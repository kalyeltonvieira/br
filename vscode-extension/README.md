# BR Language Extension for VS Code

Suporte completo para a linguagem de programação BR no Visual Studio Code.

## Recursos

- **Syntax Highlighting**: Colorização de sintaxe para arquivos .br e .brm
- **Snippets**: Atalhos de código para construções comuns da linguagem BR
- **Color Theme**: Tema de cores personalizado inspirado nas cores do Brasil
- **File Icons**: Ícones personalizados para arquivos BR

## Instalação

### Via Marketplace (quando publicado)
1. Abra VS Code
2. Vá para Extensions (Ctrl+Shift+X)
3. Busque por "BR Language"
4. Clique em Install

### Manual (arquivo .vsix)
1. Baixe o arquivo `br-language-1.0.0.vsix`
2. No VS Code, pressione F1
3. Digite "Extensions: Install from VSIX"
4. Selecione o arquivo .vsix baixado

## Desenvolvimento

### Pré-requisitos
- Node.js 16+
- npm

### Compilar
```bash
npm install
npm run compile
```

### Empacotar (.vsix)
```bash
npm install -g vsce
vsce package
```

Isso gerará o arquivo `br-language-1.0.0.vsix`.

## Snippets Disponíveis

- `fun` - Declaração de função
- `se` - Estrutura condicional
- `sese` - Estrutura condicional com senão
- `enquanto` - Loop while
- `paracada` - Loop for-each
- `tente` - Tratamento de erros
- `mostre` - Imprimir valor
- `devolva` - Retornar valor
- `lista` - Declaração de lista
- `registro` - Declaração de registro
- `explicacao` - Bloco de documentação
- `teste` - Declaração de teste

## Configuração

```json
{
  "br.format.indentSize": 4,
  "br.lint.enabled": true
}
```

## Licença

MIT
