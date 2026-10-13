# Build e Empacotamento da Extensão BR

## Pré-requisitos

- Node.js 16 ou superior
- npm

## Instalação de Dependências

```bash
cd vscode-extension
npm install
```

## Compilação

```bash
npm run compile
```

Isso compila o TypeScript para JavaScript na pasta `out/`.

## Empacotamento (.vsix)

### Gerar o arquivo .vsix (usando npx)

```bash
npx @vscode/vsce package
```

Ou adicione um script ao package.json e use:

```bash
npm run package
```

Isso criará o arquivo `br-language-1.0.0.vsix` no diretório atual.

## Instalação Local do .vsix

1. Abra o VS Code
2. Pressione F1 para abrir a paleta de comandos
3. Digite "Extensions: Install from VSIX"
4. Selecione o arquivo `br-language-1.0.0.vsix`
5. Recarregue o VS Code quando solicitado

## Publicação no Marketplace

### Criar Publisher

1. Acesse https://marketplace.visualstudio.com/manage
2. Crie um publisher (ex: "br-lang")
3. Crie um Personal Access Token

### Publicar

```bash
vsce publish
```

Ou para uma versão específica:

```bash
vsce publish minor
```

## Estrutura da Extensão

```
vscode-extension/
├── package.json              # Manifesto da extensão
├── icon.svg                  # Ícone da extensão
├── README.md                 # Documentação
├── tsconfig.json             # Configuração TypeScript
├── .vscodeignore             # Arquivos ignorados no pacote
├── src/
│   └── extension.ts          # Código principal da extensão
├── syntaxes/
│   └── br.tmLanguage.json    # Gramática de sintaxe
├── snippets/
│   └── snippets.json         # Snippets de código
├── themes/
│   └── br-color-theme.json   # Tema de cores
└── language-configuration.json # Configuração da linguagem
```
