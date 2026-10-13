"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const node_1 = require("vscode-languageserver/node");
const vscode_languageserver_textdocument_1 = require("vscode-languageserver-textdocument");
const connection = (0, node_1.createConnection)(node_1.ProposedFeatures.all);
const documents = new node_1.TextDocuments(vscode_languageserver_textdocument_1.TextDocument);
let hasConfigurationCapability = false;
let hasWorkspaceFolderCapability = false;
const documentosIndex = new Map();
// Palavras-chave com documentação detalhada
const brKeywords = new Map([
    ['função', { doc: 'Declara uma função', exemplo: 'função somar recebendo a e b\n    devolva a + b\nfim', sintaxe: 'função <nome> recebendo <param1> e <param2> ... <params>\n    <corpo>\nfim' }],
    ['funcao', { doc: 'Declara uma função (sem acento)', exemplo: 'funcao somar recebendo a e b\n    devolva a + b\nfim', sintaxe: 'funcao <nome> recebendo <param1> e <param2>\n    <corpo>\nfim' }],
    ['se', { doc: 'Estrutura condicional', exemplo: 'se idade >= 18 então\n    mostre "Maior"\nsenão\n    mostre "Menor"\nfim', sintaxe: 'se <condição> então\n    <bloco>\n[senão\n    <bloco>]\nfim' }],
    ['senão', { doc: 'Bloco alternativo do se', exemplo: 'se x > 0 então\n    mostre "Positivo"\nsenão\n    mostre "Negativo"\nfim', sintaxe: 'senão\n    <bloco>' }],
    ['senao', { doc: 'Bloco alternativo do se (sem acento)', exemplo: 'se x > 0 entao\n    mostre "Positivo"\nsenao\n    mostre "Negativo"\nfim', sintaxe: 'senao\n    <bloco>' }],
    ['enquanto', { doc: 'Loop de repetição com condição', exemplo: 'enquanto contador < 10\n    mostre contador\n    contador é contador + 1\nfim', sintaxe: 'enquanto <condição>\n    <bloco>\nfim' }],
    ['para', { doc: 'Iteração sobre elementos', exemplo: 'para cada item em lista\n    mostre item\nfim', sintaxe: 'para cada <variavel> em <lista>\n    <bloco>\nfim' }],
    ['repita', { doc: 'Repete um bloco N vezes', exemplo: 'repita 5 vezes\n    mostre "Olá"\nfim', sintaxe: 'repita <n> vezes\n    <bloco>\nfim' }],
    ['conte', { doc: 'Loop com contador numérico', exemplo: 'conte de 1 até 10 como i\n    mostre i\nfim', sintaxe: 'conte de <inicio> até <fim> como <var>\n    <bloco>\nfim' }],
    ['escolha', { doc: 'Seleção múltipla (switch/match)', exemplo: 'escolha opcao\n    caso 1\n        mostre "Um"\n    caso 2\n        mostre "Dois"\n    padrao\n        mostre "Outro"\nfim', sintaxe: 'escolha <expressão>\n    caso <valor>\n        <bloco>\n    padrao\n        <bloco>\nfim' }],
    ['tente', { doc: 'Tratamento de exceções', exemplo: 'tente\n    arquivo é leia_arquivo "dados.txt"\ncapture erro\n    mostre "Erro: " + erro.mensagem\nfim', sintaxe: 'tente\n    <bloco>\ncapture <variavel>\n    <bloco>\nfim' }],
    ['mostre', { doc: 'Exibe valores no console', exemplo: 'mostre "Olá, Mundo!"', sintaxe: 'mostre <expressão>' }],
    ['devolva', { doc: 'Retorna valor de uma função', exemplo: 'devolva resultado', sintaxe: 'devolva <expressão>' }],
    ['constante', { doc: 'Declara uma constante imutável', exemplo: 'constante PI é 3.14159', sintaxe: 'constante <nome> é <valor>' }],
    ['classe', { doc: 'Define uma classe (tipo de objeto)', exemplo: 'classe Pessoa\n    nome\n    idade\nfim', sintaxe: 'classe <nome>\n    <campos>\nfim' }],
    ['importar', { doc: 'Importa um módulo', exemplo: 'importar "matematica"', sintaxe: 'importar "<módulo>" [como <alias>]' }],
    ['lista', { doc: 'Cria uma coleção ordenada', exemplo: 'frutas é lista\n    "Maçã"\n    "Banana"\nfim', sintaxe: '<var> é lista\n    <elementos>\nfim' }],
    ['registro', { doc: 'Cria um objeto com propriedades', exemplo: 'pessoa é registro\n    nome: "João"\n    idade: 30\nfim', sintaxe: '<var> é registro\n    <chave>: <valor>\nfim' }],
    ['é', { doc: 'Operador de atribuição', exemplo: 'x é 42', sintaxe: '<variável> é <valor>' }],
    ['recebendo', { doc: 'Declara parâmetros de função', exemplo: 'função somar recebendo a e b', sintaxe: 'recebendo <param1> e <param2>' }],
    ['fim', { doc: 'Finaliza um bloco', exemplo: 'se x > 0 então\n    ...\nfim', sintaxe: 'fim' }],
    ['verdadeiro', { doc: 'Valor booleano verdadeiro', exemplo: 'ativo é verdadeiro', sintaxe: 'verdadeiro' }],
    ['falso', { doc: 'Valor booleano falso', exemplo: 'inativo é falso', sintaxe: 'falso' }],
    ['nulo', { doc: 'Valor nulo (ausência de valor)', exemplo: 'resultado é nulo', sintaxe: 'nulo' }],
    ['execute', { doc: 'Executa blocos em paralelo', exemplo: 'execute em paralelo\n    tarefa1()\n    tarefa2()\nfim', sintaxe: 'execute em paralelo\n    <instruções>\nfim' }],
    ['tarefa', { doc: 'Declara uma tarefa assíncrona', exemplo: 'tarefa carregar_dados recebendo url\n    ...\nfim', sintaxe: 'tarefa <nome> recebendo <params>\n    <corpo>\nfim' }],
    ['teste', { doc: 'Define um bloco de teste', exemplo: 'teste "soma correta"\n    espere 2 + 2 igual a 4\nfim', sintaxe: 'teste "<nome>"\n    <asserções>\nfim' }],
    ['espere', { doc: 'Afirmação de teste', exemplo: 'espere resultado igual a 42', sintaxe: 'espere <valor> igual a <esperado>' }],
    ['salve', { doc: 'Persiste um registro no banco', exemplo: 'salve cliente', sintaxe: 'salve <registro>' }],
    ['remova', { doc: 'Remove um registro do banco', exemplo: 'remova cliente_antigo', sintaxe: 'remova <registro>' }],
    ['busque', { doc: 'Busca registros no banco', exemplo: 'resultados é busque todos os clientes', sintaxe: 'busque <consulta>' }],
    ['acesse', { doc: 'Faz requisição HTTP/Internet', exemplo: 'pagina é acesse "https://exemplo.com"', sintaxe: 'acesse <url>' }],
    ['pergunte', { doc: 'Pergunta à IA integrada', exemplo: 'resposta é pergunte para ia "Qual é a capital do Brasil?"', sintaxe: 'pergunte para ia <prompt>' }],
    ['agente', { doc: 'Cria um agente de IA persistente', exemplo: 'analista é crie agente\n    especialidade: "financeiro"\nfim', sintaxe: 'crie agente\n    <propriedades>\nfim' }],
    ['objetivo', { doc: 'Bloco de IA de compilação', exemplo: 'objetivo\n    criar sistema de estoque\nfim', sintaxe: 'objetivo\n    <descrição>\nfim' }],
    ['explicacao', { doc: 'Bloco de documentação viva', exemplo: 'explicacao\n    Este módulo faz X\nfim', sintaxe: 'explicacao\n    <texto>\nfim' }],
    ['isto', { doc: 'Referência ao objeto atual (self/this)', exemplo: 'isto.nome é "João"', sintaxe: 'isto' }],
    ['herda', { doc: 'Declara herança de classe', exemplo: 'classe Admin herda Usuario', sintaxe: 'herda <ClasseBase>' }],
]);
const brTypes = ['inteiro', 'decimal', 'texto', 'booleano', 'lista', 'registro'];
const servicosNativos = [
    { nome: 'leia_arquivo', desc: 'Lê conteúdo de arquivo', params: ['caminho'] },
    { nome: 'escreva_arquivo', desc: 'Escreve conteúdo em arquivo', params: ['caminho', 'conteudo'] },
    { nome: 'texto_substituir', desc: 'Substitui ocorrências em texto', params: ['texto', 'antigo', 'novo'] },
    { nome: 'dividir_por', desc: 'Divide texto por delimitador', params: ['texto', 'delimitador'] },
    { nome: 'texto_contem', desc: 'Verifica se texto contém substring', params: ['texto', 'substring'] },
    { nome: 'adicione_item', desc: 'Adiciona item a uma lista', params: ['lista', 'item'] },
    { nome: 'obter_item', desc: 'Obtém item de lista por índice', params: ['lista', 'indice'] },
    { nome: 'tamanho_lista', desc: 'Retorna quantidade de elementos', params: ['lista'] },
    { nome: 'tipo_de', desc: 'Retorna o tipo de um valor', params: ['valor'] },
    { nome: 'para_inteiro', desc: 'Converte valor para inteiro', params: ['valor'] },
    { nome: 'para_decimal', desc: 'Converte valor para decimal', params: ['valor'] },
];
// ==================== DIAGNÓSTICO AVANÇADO ====================
function analisarDocumento(textDocument) {
    const diagnostics = [];
    const text = textDocument.getText();
    const lines = text.split('\n');
    const docInfo = {
        uri: textDocument.uri,
        symbols: [],
        erros: diagnostics,
        importacoes: new Map()
    };
    // Estado do parser simplificado
    let dentroBloco = []; // pilha de blocos
    let escopoAtual = 'global';
    let indentLevel = 0;
    for (let i = 0; i < lines.length; i++) {
        const lineNum = i;
        const rawLine = lines[i];
        const line = rawLine.trim();
        const col = rawLine.length - rawLine.trimStart().length;
        if (!line || line.startsWith('#'))
            continue;
        // ========== VALIDAÇÕES LÉXICAS ==========
        // String não fechada
        let inString = false;
        let stringChar = '';
        for (let c = 0; c < line.length; c++) {
            if ((line[c] === '"' || line[c] === "'") && (c === 0 || line[c - 1] !== '\\')) {
                if (!inString) {
                    inString = true;
                    stringChar = line[c];
                }
                else if (line[c] === stringChar) {
                    inString = false;
                }
            }
        }
        if (inString) {
            diagnostics.push({
                severity: node_1.DiagnosticSeverity.Error,
                range: node_1.Range.create(lineNum, 0, lineNum, line.length),
                message: `String não fechada (esperava ${stringChar})`,
                source: 'BR Language Server',
                code: 'br-unclosed-string'
            });
        }
        // Parênteses desbalanceados
        const openP = (line.match(/\(/g) || []).length;
        const closeP = (line.match(/\)/g) || []).length;
        if (openP !== closeP) {
            diagnostics.push({
                severity: node_1.DiagnosticSeverity.Error,
                range: node_1.Range.create(lineNum, 0, lineNum, line.length),
                message: `Parênteses desbalanceados: ${openP} abertos, ${closeP} fechados`,
                source: 'BR Language Server',
                code: 'br-unbalanced-parens'
            });
        }
        // Colchetes desbalanceados
        const openB = (line.match(/\[/g) || []).length;
        const closeB = (line.match(/\]/g) || []).length;
        if (openB !== closeB) {
            diagnostics.push({
                severity: node_1.DiagnosticSeverity.Error,
                range: node_1.Range.create(lineNum, 0, lineNum, line.length),
                message: `Colchetes desbalanceados: ${openB} abertos, ${closeB} fechados`,
                source: 'BR Language Server',
                code: 'br-unbalanced-brackets'
            });
        }
        // Chaves desbalanceadas
        const openC = (line.match(/\{/g) || []).length;
        const closeC = (line.match(/\}/g) || []).length;
        if (openC !== closeC) {
            diagnostics.push({
                severity: node_1.DiagnosticSeverity.Error,
                range: node_1.Range.create(lineNum, 0, lineNum, line.length),
                message: `Chaves desbalanceadas: ${openC} abertas, ${closeC} fechadas`,
                source: 'BR Language Server',
                code: 'br-unbalanced-braces'
            });
        }
        // ========== DETECÇÃO DE SINTAXE ==========
        // Detectar função
        const funcMatch = line.match(/^(função|funcao)\s+([a-zA-ZáéíóúãõâêôàçÁÉÍÓÚÃÕÂÊÔÀÇ\w]+)(\s+recebendo\s+(.+))?$/);
        if (funcMatch) {
            const nome = funcMatch[2];
            const params = funcMatch[4] ? funcMatch[4].split(/\s+e\s+/).map(p => p.trim()) : [];
            docInfo.symbols.push({
                nome, tipo: 'funcao', kind: node_1.SymbolKind.Function,
                linha: lineNum, coluna: col, escopo: escopoAtual,
                parametros: params,
                documentacao: `Função ${nome}(${params.join(', ')})`
            });
            dentroBloco.push('funcao');
            escopoAtual = nome;
            indentLevel++;
            continue;
        }
        // Detectar classe
        const classMatch = line.match(/^classe\s+([a-zA-ZáéíóúãõâêôàçÁÉÍÓÚÃÕÂÊÔÀÇ\w]+)(\s+herda\s+([a-zA-ZáéíóúãõâêôàçÁÉÍÓÚÃÕÂÊÔÀÇ\w]+))?$/);
        if (classMatch) {
            const nome = classMatch[1];
            const mae = classMatch[3] || '';
            docInfo.symbols.push({
                nome, tipo: 'classe', kind: node_1.SymbolKind.Class,
                linha: lineNum, coluna: col, escopo: 'global',
                documentacao: mae ? `Classe ${nome} herda ${mae}` : `Classe ${nome}`
            });
            dentroBloco.push('classe');
            escopoAtual = nome;
            indentLevel++;
            continue;
        }
        // Detectar importação
        const importMatch = line.match(/^importar\s+(?:"([^"]+)"|'([^']+)'|(\w+))(\s+como\s+(\w+))?$/);
        if (importMatch) {
            const modulo = importMatch[1] || importMatch[2] || importMatch[3] || '';
            const alias = importMatch[5] || modulo;
            docInfo.importacoes.set(alias, modulo);
            docInfo.symbols.push({
                nome: alias, tipo: 'modulo', kind: node_1.SymbolKind.Module,
                linha: lineNum, coluna: col, escopo: 'global',
                documentacao: `Módulo: ${modulo}`
            });
            continue;
        }
        // Detectar variável/constante
        const varMatch = line.match(/^(constante\s+)?([a-zA-ZáéíóúãõâêôàçÁÉÍÓÚÃÕÂÊÔÀÇ\w]+)\s*(\:)\s*([a-zA-ZáéíóúãõâêôàçÁÉÍÓÚÃÕÂÊÔÀÇ\w]+)\s*(é|=)\s*(.+)$/);
        if (varMatch) {
            const nome = varMatch[2];
            const tipo = varMatch[4];
            const valor = varMatch[6];
            docInfo.symbols.push({
                nome, tipo: 'variavel', kind: node_1.SymbolKind.Variable,
                linha: lineNum, coluna: col, escopo: escopoAtual,
                documentacao: `Tipo: ${tipo} = ${valor}`
            });
            continue;
        }
        // Detectar variável simples
        const varSimpleMatch = line.match(/^([a-zA-ZáéíóúãõâêôàçÁÉÍÓÚÃÕÂÊÔÀÇ\w]+)\s*(é|=)\s*(.+)$/);
        if (varSimpleMatch) {
            const nome = varSimpleMatch[1];
            const valor = varSimpleMatch[3];
            const ehConstante = line.startsWith('constante ');
            docInfo.symbols.push({
                nome, tipo: ehConstante ? 'constante' : 'variavel',
                kind: ehConstante ? node_1.SymbolKind.Constant : node_1.SymbolKind.Variable,
                linha: lineNum, coluna: col, escopo: escopoAtual,
                documentacao: `= ${valor}`
            });
            continue;
        }
        // Detectar teste
        const testMatch = line.match(/^teste\s+"([^"]+)"$/);
        if (testMatch) {
            docInfo.symbols.push({
                nome: testMatch[1], tipo: 'teste', kind: node_1.SymbolKind.Event,
                linha: lineNum, coluna: col, escopo: escopoAtual
            });
            indentLevel++;
            continue;
        }
        // Detectar lista
        if (line === 'lista' || line.startsWith('lista ')) {
            indentLevel++;
            continue;
        }
        // Detectar registro
        if (line === 'registro' || line.startsWith('registro ')) {
            indentLevel++;
            continue;
        }
        // Blocos que aumentam indentação
        if (/^(se|enquanto|para cada|para\s+|repita|conte|execute em paralelo|execute\s+|tente|escolha|objetivo|explicacao|crie\s+)/.test(line)) {
            indentLevel++;
            continue;
        }
        // Bloco que diminui indentação
        if (line === 'fim' || line === 'fim ') {
            if (indentLevel > 0) {
                indentLevel--;
                // Voltar escopo
                escopoAtual = dentroBloco.length > 1 ? dentroBloco[dentroBloco.length - 2] : 'global';
                if (dentroBloco.length > 0)
                    dentroBloco.pop();
            }
            else {
                diagnostics.push({
                    severity: node_1.DiagnosticSeverity.Error,
                    range: node_1.Range.create(lineNum, col, lineNum, col + 3),
                    message: "'fim' sem bloco correspondente",
                    source: 'BR Language Server',
                    code: 'br-unexpected-end'
                });
            }
            continue;
        }
        // Misturar espaços e tabs
        if (rawLine.includes('\t') && rawLine.includes('    ')) {
            diagnostics.push({
                severity: node_1.DiagnosticSeverity.Warning,
                range: node_1.Range.create(lineNum, 0, lineNum, line.length),
                message: 'Mistura de tabs e espaços na indentação. Use apenas espaços.',
                source: 'BR Language Server',
                code: 'br-mixed-indent'
            });
        }
    }
    // Verificar blocos não fechados
    if (indentLevel > 0) {
        diagnostics.push({
            severity: node_1.DiagnosticSeverity.Error,
            range: node_1.Range.create(lines.length - 1, 0, lines.length - 1, 5),
            message: `${indentLevel} bloco(s) não fechado(s). Adicione 'fim' ao final.`,
            source: 'BR Language Server',
            code: 'br-unclosed-block'
        });
    }
    // Índice de símbolos para referência rápida
    docInfo.symbols.sort((a, b) => a.linha - b.linha);
    documentosIndex.set(textDocument.uri, docInfo);
    return diagnostics;
}
connection.onInitialize((params) => {
    const capabilities = params.capabilities;
    hasConfigurationCapability = !!(capabilities.workspace && !!capabilities.workspace.configuration);
    hasWorkspaceFolderCapability = !!(capabilities.workspace && !!capabilities.workspace.workspaceFolders);
    const result = {
        capabilities: {
            textDocumentSync: node_1.TextDocumentSyncKind.Full,
            completionProvider: {
                resolveProvider: true,
                triggerCharacters: [' ', '.', ':', '(', 'é', '=']
            },
            definitionProvider: true,
            referencesProvider: true,
            hoverProvider: true,
            documentSymbolProvider: true,
            workspaceSymbolProvider: true,
            signatureHelpProvider: {
                triggerCharacters: ['(', ' ']
            },
            codeActionProvider: {
                codeActionKinds: [node_1.CodeActionKind.QuickFix, node_1.CodeActionKind.Source]
            },
            documentFormattingProvider: true
        }
    };
    return result;
});
connection.onInitialized(() => {
    if (hasConfigurationCapability) {
        connection.client.register(node_1.DidChangeConfigurationNotification.type, undefined);
    }
});
documents.onDidChangeContent((change) => {
    const diagnostics = analisarDocumento(change.document);
    connection.sendDiagnostics({ uri: change.document.uri, diagnostics });
});
// ==================== COMPLETION ====================
connection.onCompletion((_textDocumentPosition) => {
    const document = documents.get(_textDocumentPosition.textDocument.uri);
    if (!document)
        return [];
    const text = document.getText();
    const lines = text.split('\n');
    const lineNum = _textDocumentPosition.position.line;
    const col = _textDocumentPosition.position.character;
    const currentLine = lineNum < lines.length ? lines[lineNum].substring(0, col) : '';
    const prefix = currentLine.split(/[\s.]+/).pop() || '';
    const completions = [];
    // Palavras-chave com snippet
    brKeywords.forEach((info, keyword) => {
        if (keyword.startsWith(prefix) || !prefix) {
            const item = {
                label: keyword,
                kind: node_1.CompletionItemKind.Keyword,
                detail: info.doc,
                documentation: {
                    kind: 'markdown',
                    value: `## ${keyword}\n\n${info.doc}\n\n**Sintaxe:**\n\`\`\`\n${info.sintaxe}\n\`\`\`\n\n**Exemplo:**\n\`\`\`br\n${info.exemplo}\n\`\`\``
                },
                insertText: keyword,
                data: 1
            };
            // Snippets para estruturas comuns
            if (keyword === 'função') {
                item.insertText = 'função $1 recebendo $2\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'funcao') {
                item.insertText = 'funcao $1 recebendo $2\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'se') {
                item.insertText = 'se $1 então\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'enquanto') {
                item.insertText = 'enquanto $1\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'para') {
                item.insertText = 'para cada $1 em $2\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'repita') {
                item.insertText = 'repita $1 vezes\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'conte') {
                item.insertText = 'conte de $1 até $2 como $3\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'escolha') {
                item.insertText = 'escolha $1\n    caso $2\n        $0\n    padrao\n        \nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'tente') {
                item.insertText = 'tente\n    $0\ncapture erro\n    \nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'classe') {
                item.insertText = 'classe $1\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'teste') {
                item.insertText = 'teste "$1"\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'explicacao') {
                item.insertText = 'explicacao\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            else if (keyword === 'objetivo') {
                item.insertText = 'objetivo\n    $0\nfim';
                item.insertTextFormat = 2;
            }
            completions.push(item);
        }
    });
    // Tipos
    brTypes.forEach(type => {
        if (type.startsWith(prefix) || !prefix) {
            completions.push({
                label: type,
                kind: node_1.CompletionItemKind.Class,
                detail: 'Tipo BR',
                documentation: `Tipo de dado: ${type}`
            });
        }
    });
    // Serviços nativos
    servicosNativos.forEach(svc => {
        if (svc.nome.startsWith(prefix) || !prefix) {
            completions.push({
                label: svc.nome,
                kind: node_1.CompletionItemKind.Function,
                detail: svc.desc,
                documentation: {
                    kind: 'markdown',
                    value: `## ${svc.nome}\n\n${svc.desc}\n\n**Parâmetros:** ${svc.params.join(', ')}`
                },
                insertText: `${svc.nome}(${svc.params.map((p, i) => `\${${i + 1}:${p}}`).join(', ')})`,
                insertTextFormat: 2
            });
        }
    });
    // Símbolos do documento
    const docInfo = documentosIndex.get(_textDocumentPosition.textDocument.uri);
    if (docInfo) {
        docInfo.symbols.forEach(sym => {
            if (sym.nome.startsWith(prefix) || !prefix) {
                const kind = sym.tipo === 'funcao' ? node_1.CompletionItemKind.Function :
                    sym.tipo === 'classe' ? node_1.CompletionItemKind.Class :
                        sym.tipo === 'constante' ? node_1.CompletionItemKind.Constant :
                            node_1.CompletionItemKind.Variable;
                completions.push({
                    label: sym.nome,
                    kind: kind,
                    detail: `${sym.tipo} (linha ${sym.linha + 1})`,
                    documentation: sym.documentacao || `Definido no escopo: ${sym.escopo}`
                });
            }
        });
    }
    return completions;
});
connection.onCompletionResolve((item) => {
    return item;
});
// ==================== HOVER ====================
connection.onHover((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return null;
    const text = document.getText();
    const lines = text.split('\n');
    const line = params.position.line;
    const character = params.position.character;
    if (line >= lines.length)
        return null;
    const currentLine = lines[line];
    const wordAtPosition = getWordAtPosition(currentLine, character);
    if (!wordAtPosition)
        return null;
    // Verificar keyword
    if (brKeywords.has(wordAtPosition)) {
        const info = brKeywords.get(wordAtPosition);
        return {
            contents: {
                kind: 'markdown',
                value: `## \`${wordAtPosition}\`\n\n${info.doc}\n\n**Sintaxe:**\n\`\`\`br\n${info.sintaxe}\n\`\`\`\n\n**Exemplo:**\n\`\`\`br\n${info.exemplo}\n\`\`\``
            }
        };
    }
    // Verificar tipo
    if (brTypes.includes(wordAtPosition)) {
        return {
            contents: {
                kind: 'markdown',
                value: `## \`${wordAtPosition}\`\n\nTipo de dado da linguagem BR.`
            }
        };
    }
    // Verificar símbolo
    const docInfo = documentosIndex.get(params.textDocument.uri);
    if (docInfo) {
        const sym = docInfo.symbols.find(s => s.nome === wordAtPosition);
        if (sym) {
            return {
                contents: {
                    kind: 'markdown',
                    value: `## \`${sym.nome}\`\n\n**Tipo:** ${sym.tipo}\n**Escopo:** ${sym.escopo}\n**Linha:** ${sym.linha + 1}\n\n${sym.documentacao || ''}`
                }
            };
        }
    }
    return null;
});
// ==================== GO TO DEFINITION ====================
connection.onDefinition((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return undefined;
    const text = document.getText();
    const lines = text.split('\n');
    const lineNum = params.position.line;
    const character = params.position.character;
    if (lineNum >= lines.length)
        return undefined;
    const currentLine = lines[lineNum];
    const wordAtPosition = getWordAtPosition(currentLine, character);
    if (!wordAtPosition)
        return undefined;
    const docInfo = documentosIndex.get(params.textDocument.uri);
    if (docInfo) {
        const sym = docInfo.symbols.find(s => s.nome === wordAtPosition);
        if (sym) {
            return node_1.Location.create(params.textDocument.uri, {
                start: { line: sym.linha, character: sym.coluna },
                end: { line: sym.linha, character: sym.coluna + sym.nome.length }
            });
        }
    }
    return undefined;
});
// ==================== REFERENCES ====================
connection.onReferences((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return [];
    const text = document.getText();
    const lines = text.split('\n');
    const lineNum = params.position.line;
    const character = params.position.character;
    if (lineNum >= lines.length)
        return [];
    const currentLine = lines[lineNum];
    const wordAtPosition = getWordAtPosition(currentLine, character);
    if (!wordAtPosition)
        return [];
    const references = [];
    for (let i = 0; i < lines.length; i++) {
        let searchFrom = 0;
        while (true) {
            const idx = lines[i].indexOf(wordAtPosition, searchFrom);
            if (idx === -1)
                break;
            references.push({
                uri: params.textDocument.uri,
                range: {
                    start: { line: i, character: idx },
                    end: { line: i, character: idx + wordAtPosition.length }
                }
            });
            searchFrom = idx + 1;
        }
    }
    return references;
});
// ==================== DOCUMENT SYMBOLS ====================
connection.onDocumentSymbol((params) => {
    const docInfo = documentosIndex.get(params.textDocument.uri);
    if (!docInfo)
        return [];
    return docInfo.symbols.map(sym => ({
        name: sym.nome,
        kind: sym.kind,
        range: {
            start: { line: sym.linha, character: sym.coluna },
            end: { line: sym.linha, character: sym.coluna + sym.nome.length }
        },
        selectionRange: {
            start: { line: sym.linha, character: sym.coluna },
            end: { line: sym.linha, character: sym.coluna + sym.nome.length }
        },
        detail: sym.tipo
    }));
});
// ==================== WORKSPACE SYMBOLS ====================
connection.onWorkspaceSymbol((params) => {
    const symbols = [];
    documentosIndex.forEach((docInfo, uri) => {
        docInfo.symbols.forEach(sym => {
            if (sym.nome.toLowerCase().includes(params.query.toLowerCase())) {
                symbols.push({
                    name: sym.nome,
                    kind: sym.kind,
                    location: {
                        uri: uri,
                        range: {
                            start: { line: sym.linha, character: sym.coluna },
                            end: { line: sym.linha, character: sym.coluna + sym.nome.length }
                        }
                    }
                });
            }
        });
    });
    return symbols;
});
// ==================== SIGNATURE HELP ====================
connection.onSignatureHelp((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return null;
    const text = document.getText();
    const lines = text.split('\n');
    const lineNum = params.position.line;
    const line = lines[lineNum];
    const prefix = line.substring(0, params.position.character);
    // Verificar se está dentro de parênteses
    const lastParenOpen = prefix.lastIndexOf('(');
    const lastParenClose = prefix.lastIndexOf(')');
    if (lastParenOpen < 0 || lastParenOpen < lastParenClose)
        return null;
    const beforeParen = prefix.substring(0, lastParenOpen).trim();
    const funcName = beforeParen.split(/[\s]+/).pop() || beforeParen;
    // Verificar serviços nativos
    const svc = servicosNativos.find(s => s.nome === funcName);
    if (svc) {
        return {
            signatures: [{
                    label: `${svc.nome}(${svc.params.join(', ')})`,
                    documentation: svc.desc,
                    parameters: svc.params.map(p => ({ label: p, documentation: `Parâmetro: ${p}` }))
                }],
            activeSignature: 0,
            activeParameter: 0
        };
    }
    // Verificar funções do documento
    const docInfo = documentosIndex.get(params.textDocument.uri);
    if (docInfo) {
        const funcSym = docInfo.symbols.find(s => s.nome === funcName && s.tipo === 'funcao');
        if (funcSym && funcSym.parametros) {
            return {
                signatures: [{
                        label: `${funcSym.nome}(${funcSym.parametros.join(', ')})`,
                        documentation: funcSym.documentacao || '',
                        parameters: funcSym.parametros.map(p => ({ label: p }))
                    }],
                activeSignature: 0,
                activeParameter: prefix.split(',').length - 1
            };
        }
    }
    return null;
});
// ==================== CODE ACTIONS ====================
connection.onCodeAction((params) => {
    const actions = [];
    params.context.diagnostics.forEach(diag => {
        if (diag.code === 'br-unclosed-block') {
            const fix = node_1.CodeAction.create('Adicionar blocos finais faltantes', { changes: {} }, node_1.CodeActionKind.QuickFix);
            actions.push(fix);
        }
        if (diag.code === 'br-mixed-indent') {
            const fix = node_1.CodeAction.create('Converter tabs para espaços', { changes: {} }, node_1.CodeActionKind.QuickFix);
            actions.push(fix);
        }
    });
    return actions;
});
// ==================== FORMATTING ====================
connection.onDocumentFormatting((params) => {
    const document = documents.get(params.textDocument.uri);
    if (!document)
        return [];
    const text = document.getText();
    const lines = text.split('\n');
    const indentSize = 4;
    let indentLevel = 0;
    const formattedLines = [];
    const blockStarters = ['se', 'enquanto', 'para cada', 'para', 'tente', 'execute em paralelo',
        'função', 'funcao', 'lista', 'registro', 'explicacao', 'objetivo',
        'teste', 'escolha', 'classe', 'repita', 'conte', 'crie'];
    for (const rawLine of lines) {
        const trimmed = rawLine.trim();
        // Blocos que fecham (antes da linha)
        if (trimmed === 'fim' || trimmed === 'senão' || trimmed === 'senao' ||
            trimmed.startsWith('senão ') || trimmed.startsWith('senao ') ||
            trimmed.startsWith('capture') || trimmed.startsWith('padrao') ||
            trimmed.startsWith('caso ')) {
            indentLevel = Math.max(0, indentLevel - 1);
        }
        // Linha formatada
        if (trimmed) {
            formattedLines.push(' '.repeat(indentLevel * indentSize) + trimmed);
        }
        else {
            formattedLines.push('');
        }
        // Blocos que abrem (depois da linha)
        if (blockStarters.some(starter => trimmed.startsWith(starter))) {
            indentLevel++;
        }
        // Exceto se é "para cada" que já foi contado como "para"
        if (trimmed.startsWith('para cada')) {
            indentLevel--; // "para" já incrementou, compensar
        }
    }
    const fullRange = {
        start: { line: 0, character: 0 },
        end: { line: lines.length - 1, character: lines[lines.length - 1].length }
    };
    return [node_1.TextEdit.replace(fullRange, formattedLines.join('\n'))];
});
// ==================== UTILITÁRIOS ====================
function getWordAtPosition(line, character) {
    // Encontrar a palavra no cursor, considerando acentos
    const matches = [...line.matchAll(/[a-zA-ZáéíóúãõâêôàçÁÉÍÓÚÃÕÂÊÔÀÇ\w]+/g)];
    for (const match of matches) {
        if (character >= match.index && character <= match.index + match[0].length) {
            return match[0];
        }
    }
    return '';
}
documents.listen(connection);
connection.listen();
//# sourceMappingURL=server.js.map