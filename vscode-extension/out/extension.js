"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
const vscode = require("vscode");
const path = require("path");
const fs = require("fs");
const node_1 = require("vscode-languageclient/node");
let diagnosticCollection;
let client;
let statusBarItem;
function activate(context) {
    console.log('BR Language Extension v2.0 ativada');
    // ========== STATUS BAR ==========
    statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 100);
    statusBarItem.text = "$(hubot) BR";
    statusBarItem.tooltip = "Linguagem BR - Clique para executar";
    statusBarItem.command = "br.run";
    statusBarItem.show();
    context.subscriptions.push(statusBarItem);
    // ========== DIAGNÓSTICOS ==========
    diagnosticCollection = vscode.languages.createDiagnosticCollection('br');
    context.subscriptions.push(diagnosticCollection);
    // Diagnóstico em tempo real (backup/fallback)
    let diagnosticTimeout;
    const diagnosticListener = vscode.workspace.onDidChangeTextDocument(event => {
        if (event.document.languageId !== 'br')
            return;
        if (diagnosticTimeout)
            clearTimeout(diagnosticTimeout);
        diagnosticTimeout = setTimeout(() => {
            realizarDiagnosticoLocal(event.document);
        }, 500);
    });
    context.subscriptions.push(diagnosticListener);
    // ========== LSP CLIENT ==========
    const serverModule = context.asAbsolutePath(path.join('out', 'server.js'));
    const debugOptions = { execArgv: ['--nolazy', '--inspect=6009'] };
    const serverOptions = {
        run: { module: serverModule, transport: node_1.TransportKind.ipc },
        debug: { module: serverModule, transport: node_1.TransportKind.ipc, options: debugOptions }
    };
    const clientOptions = {
        documentSelector: [{ scheme: 'file', language: 'br' }],
        synchronize: {
            configurationSection: 'br',
            fileEvents: vscode.workspace.createFileSystemWatcher('**/*.{br,brm,brt,bri,bra}')
        },
        diagnosticCollectionName: 'br-lsp'
    };
    client = new node_1.LanguageClient('BR Language Server', 'BR Language Server', serverOptions, clientOptions);
    client.start();
    // ========== FORMATTER ==========
    const formatter = vscode.languages.registerDocumentFormattingEditProvider('br', {
        provideDocumentFormattingEdits(document) {
            const text = document.getText();
            const formatted = formatBR(text);
            const fullRange = new vscode.Range(document.positionAt(0), document.positionAt(text.length));
            return [vscode.TextEdit.replace(fullRange, formatted)];
        }
    });
    context.subscriptions.push(formatter);
    // ========== CODE LENS ==========
    const codeLensProvider = vscode.languages.registerCodeLensProvider('br', {
        provideCodeLenses(document) {
            const lenses = [];
            const text = document.getText();
            const lines = text.split('\n');
            for (let i = 0; i < lines.length; i++) {
                const line = lines[i].trim();
                // Run button em funções
                if (line.match(/^(função|funcao)\s+(\w+)/)) {
                    const funcName = line.match(/^(função|funcao)\s+(\w+)/)[2];
                    const range = new vscode.Range(i, 0, i, lines[i].length);
                    lenses.push(new vscode.CodeLens(range, {
                        title: "▶ Executar",
                        command: "br.runFunction",
                        arguments: [document.uri, funcName]
                    }));
                }
                // Run button em testes
                if (line.match(/^teste\s+"([^"]+)"/)) {
                    const range = new vscode.Range(i, 0, i, lines[i].length);
                    lenses.push(new vscode.CodeLens(range, {
                        title: "▶ Rodar Teste",
                        command: "br.runTest",
                        arguments: [document.uri]
                    }));
                }
            }
            return lenses;
        }
    });
    context.subscriptions.push(codeLensProvider);
    // ========== COMANDOS ==========
    // Formatar código
    let formatCommand = vscode.commands.registerCommand('br.format', () => {
        vscode.commands.executeCommand('editor.action.formatDocument');
    });
    context.subscriptions.push(formatCommand);
    // Executar arquivo BR
    let runCommand = vscode.commands.registerCommand('br.run', () => {
        const editor = vscode.window.activeTextEditor;
        if (editor && editor.document.languageId === 'br') {
            runBRFile(editor.document.uri);
        }
        else {
            vscode.window.showInformationMessage('Abra um arquivo BR para executar');
        }
    });
    context.subscriptions.push(runCommand);
    // Executar função específica
    let runFuncCommand = vscode.commands.registerCommand('br.runFunction', (uri, funcName) => {
        const terminal = vscode.window.createTerminal('BR Run');
        const config = vscode.workspace.getConfiguration('br');
        const executablePath = config.get('executablePath', 'br');
        terminal.sendText(`${executablePath} -c "importar \\"${uri.fsPath}\\" como mod; mod.${funcName}()"`);
        terminal.show();
    });
    context.subscriptions.push(runFuncCommand);
    // Rodar testes
    let runTestCommand = vscode.commands.registerCommand('br.runTest', (uri) => {
        const terminal = vscode.window.createTerminal('BR Test');
        const config = vscode.workspace.getConfiguration('br');
        const executablePath = config.get('executablePath', 'br');
        terminal.sendText(`${executablePath} --test "${uri.fsPath}"`);
        terminal.show();
    });
    context.subscriptions.push(runTestCommand);
    // Debug
    let debugCommand = vscode.commands.registerCommand('br.debug', () => {
        const editor = vscode.window.activeTextEditor;
        if (editor && editor.document.languageId === 'br') {
            debugBRFile(editor.document.uri);
        }
    });
    context.subscriptions.push(debugCommand);
    // Novo arquivo BR
    let newFileCommand = vscode.commands.registerCommand('br.newFile', () => {
        vscode.workspace.openTextDocument({
            content: `# Novo programa BR\n\nnome é "Mundo"\nmostre "Olá, " + nome\n`
        }).then(doc => vscode.window.showTextDocument(doc));
    });
    context.subscriptions.push(newFileCommand);
    // Abrir REPL
    let replCommand = vscode.commands.registerCommand('br.openRepl', () => {
        const terminal = vscode.window.createTerminal('BR REPL');
        const config = vscode.workspace.getConfiguration('br');
        const executablePath = config.get('executablePath', 'br');
        terminal.sendText(`${executablePath} -i`);
        terminal.show();
    });
    context.subscriptions.push(replCommand);
    // Verificar versão
    let versionCommand = vscode.commands.registerCommand('br.version', () => {
        const terminal = vscode.window.createTerminal('BR Version');
        const config = vscode.workspace.getConfiguration('br');
        const executablePath = config.get('executablePath', 'br');
        terminal.sendText(`${executablePath} --version`);
        terminal.show();
    });
    context.subscriptions.push(versionCommand);
    // Atualizar status bar com informações do documento
    vscode.window.onDidChangeActiveTextEditor(editor => {
        if (editor && editor.document.languageId === 'br') {
            const lineCount = editor.document.lineCount;
            const text = editor.document.getText();
            const lines = text.split('\n');
            let funcCount = 0;
            let classCount = 0;
            for (const line of lines) {
                if (line.trim().match(/^(função|funcao)\s+/))
                    funcCount++;
                if (line.trim().match(/^classe\s+/))
                    classCount++;
            }
            statusBarItem.text = `$(hubot) BR | ${funcCount} fn | ${classCount} cls | ${lineCount} lin`;
        }
    });
    console.log('BR Language Extension v2.0 totalmente carregada');
}
exports.activate = activate;
// ========== FORMATADOR LOCAL ==========
function formatBR(text) {
    const lines = text.split('\n');
    const indentSize = 4;
    let indentLevel = 0;
    const formattedLines = [];
    const blockStarters = [
        'se', 'enquanto', 'para cada', 'para', 'tente', 'execute em paralelo',
        'função', 'funcao', 'lista', 'registro', 'explicacao', 'objetivo',
        'teste', 'escolha', 'classe', 'repita', 'conte', 'crie'
    ];
    for (const rawLine of lines) {
        const trimmed = rawLine.trim();
        if (!trimmed) {
            formattedLines.push('');
            continue;
        }
        // Blocos que fecham indentação (antes de escrever a linha)
        if (trimmed === 'fim' || trimmed.startsWith('senão') || trimmed.startsWith('senao') ||
            trimmed.startsWith('capture') || trimmed.startsWith('padrao') || trimmed.startsWith('caso ')) {
            indentLevel = Math.max(0, indentLevel - 1);
        }
        formattedLines.push(' '.repeat(indentLevel * indentSize) + trimmed);
        // Blocos que abrem indentação (depois de escrever a linha)
        if (blockStarters.some(starter => trimmed.startsWith(starter))) {
            indentLevel++;
        }
    }
    return formattedLines.join('\n');
}
// ========== DIAGNÓSTICO LOCAL (FALLBACK) ==========
function realizarDiagnosticoLocal(document) {
    if (!document || document.languageId !== 'br')
        return;
    const diagnostics = [];
    const text = document.getText();
    const lines = text.split('\n');
    let indentLevel = 0;
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();
        const lineNum = i;
        if (!line || line.startsWith('#'))
            continue;
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
            diagnostics.push(new vscode.Diagnostic(new vscode.Range(lineNum, 0, lineNum, line.length), `String não fechada (esperava ${stringChar})`, vscode.DiagnosticSeverity.Error));
        }
        // Parênteses desbalanceados
        const openP = (line.match(/\(/g) || []).length;
        const closeP = (line.match(/\)/g) || []).length;
        if (openP !== closeP) {
            diagnostics.push(new vscode.Diagnostic(new vscode.Range(lineNum, 0, lineNum, line.length), `Parênteses desbalanceados: ${openP} abertos, ${closeP} fechados`, vscode.DiagnosticSeverity.Error));
        }
        // Controle de blocos
        if (line === 'fim') {
            if (indentLevel > 0) {
                indentLevel--;
            }
            else {
                diagnostics.push(new vscode.Diagnostic(new vscode.Range(lineNum, 0, lineNum, 3), "'fim' sem bloco correspondente", vscode.DiagnosticSeverity.Error));
            }
        }
        if (['se', 'enquanto', 'para', 'tente', 'função', 'funcao', 'classe', 'escolha',
            'repita', 'conte', 'lista', 'registro'].some(s => line.startsWith(s))) {
            indentLevel++;
        }
    }
    // Blocos não fechados
    if (indentLevel > 0) {
        diagnostics.push(new vscode.Diagnostic(new vscode.Range(lines.length - 1, 0, lines.length - 1, 5), `${indentLevel} bloco(s) não fechado(s). Adicione 'fim'.`, vscode.DiagnosticSeverity.Warning));
    }
    diagnosticCollection.set(document.uri, diagnostics);
}
// ========== EXECUÇÃO ==========
function runBRFile(uri) {
    const config = vscode.workspace.getConfiguration('br');
    let executablePath = config.get('executablePath', 'br');
    // Se for 'br' e não existir no PATH, usa python br.py
    if (executablePath === 'br') {
        const pythonPath = 'python';
        const brHome = path.join(__dirname, '..', '..', '..', 'br.py');
        const brBat = path.join(__dirname, '..', '..', '..', 'br.bat');
        if (fs.existsSync(brBat)) {
            executablePath = `"${brBat}"`;
        }
        else if (fs.existsSync(brHome)) {
            executablePath = `${pythonPath} "${brHome}"`;
        }
    }
    const terminal = vscode.window.createTerminal('BR Run');
    terminal.sendText(`${executablePath} "${uri.fsPath}"`);
    terminal.show();
}
function debugBRFile(uri) {
    const debugConfig = {
        type: 'br',
        request: 'launch',
        name: 'Debug BR File',
        program: uri.fsPath,
        console: 'integratedTerminal'
    };
    vscode.debug.startDebugging(undefined, debugConfig);
}
function deactivate() {
    console.log('BR Language Extension desativada');
    if (statusBarItem)
        statusBarItem.dispose();
    if (diagnosticCollection)
        diagnosticCollection.dispose();
    if (client)
        return client.stop();
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map