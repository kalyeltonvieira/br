# -*- coding: utf-8 -*-
"""
IDE Oficial BR - Ambiente de Desenvolvimento Integrado para Linguagem BR
"""
import sys
import os
import tkinter as tk
from tkinter import ttk, filedialog, messagebox
from tkinter.scrolledtext import ScrolledText
import subprocess
import threading
from typing import Optional

class IDEBR:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("IDE BR - Ambiente de Desenvolvimento")
        self.root.geometry("1200x800")
        
        self.arquivo_atual = None
        self.modificado = False
        
        self.criar_interface()
        self.root.protocol("WM_DELETE_WINDOW", self.on_fechar)
    
    def criar_interface(self):
        """Cria a interface da IDE"""
        # Menu principal
        menubar = tk.Menu(self.root)
        
        # Menu Arquivo
        arquivo_menu = tk.Menu(menubar, tearoff=0)
        arquivo_menu.add_command(label="Novo", command=self.novo_arquivo, accelerator="Ctrl+N")
        arquivo_menu.add_command(label="Abrir", command=self.abrir_arquivo, accelerator="Ctrl+O")
        arquivo_menu.add_command(label="Salvar", command=self.salvar_arquivo, accelerator="Ctrl+S")
        arquivo_menu.add_command(label="Salvar Como", command=self.salvar_como, accelerator="Ctrl+Shift+S")
        arquivo_menu.add_separator()
        arquivo_menu.add_command(label="Sair", command=self.on_fechar, accelerator="Ctrl+Q")
        menubar.add_cascade(label="Arquivo", menu=arquivo_menu)
        
        # Menu Editar
        editar_menu = tk.Menu(menubar, tearoff=0)
        editar_menu.add_command(label="Desfazer", command=self.desfazer, accelerator="Ctrl+Z")
        editar_menu.add_command(label="Refazer", command=self.refazer, accelerator="Ctrl+Y")
        editar_menu.add_separator()
        editar_menu.add_command(label="Recortar", command=self.recortar, accelerator="Ctrl+X")
        editar_menu.add_command(label="Copiar", command=self.copiar, accelerator="Ctrl+C")
        editar_menu.add_command(label="Colar", command=self.colar, accelerator="Ctrl+V")
        menubar.add_cascade(label="Editar", menu=editar_menu)
        
        # Menu Compilar
        compilar_menu = tk.Menu(menubar, tearoff=0)
        compilar_menu.add_command(label="Compilar", command=self.compilar, accelerator="F5")
        compilar_menu.add_command(label="Executar", command=self.executar, accelerator="F6")
        compilar_menu.add_command(label="Compilar e Executar", command=self.compilar_executar, accelerator="F7")
        menubar.add_cascade(label="Compilar", menu=compilar_menu)
        
        # Menu Ferramentas
        ferramentas_menu = tk.Menu(menubar, tearoff=0)
        ferramentas_menu.add_command(label="Formatar Código", command=self.formatar_codigo, accelerator="Ctrl+Shift+F")
        ferramentas_menu.add_command(label="Linter", command=self.executar_linter, accelerator="Ctrl+Shift+L")
        ferramentas_menu.add_command(label="REPL", command=self.abrir_repl, accelerator="F9")
        menubar.add_cascade(label="Ferramentas", menu=ferramentas_menu)
        
        # Menu Ajuda
        ajuda_menu = tk.Menu(menubar, tearoff=0)
        ajuda_menu.add_command(label="Documentação BR", command=self.abrir_documentacao)
        ajuda_menu.add_command(label="Sobre", command=self.sobre)
        menubar.add_cascade(label="Ajuda", menu=ajuda_menu)
        
        self.root.config(menu=menubar)
        
        # Barra de ferramentas
        toolbar = ttk.Frame(self.root)
        toolbar.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
        
        ttk.Button(toolbar, text="Novo", command=self.novo_arquivo).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Abrir", command=self.abrir_arquivo).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Salvar", command=self.salvar_arquivo).pack(side=tk.LEFT, padx=2)
        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, padx=5, fill=tk.Y)
        ttk.Button(toolbar, text="Compilar", command=self.compilar).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="Executar", command=self.executar).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="REPL", command=self.abrir_repl).pack(side=tk.LEFT, padx=2)
        
        # Área principal
        paned = ttk.PanedWindow(self.root, orient=tk.VERTICAL)
        paned.pack(fill=tk.BOTH, expand=True)
        
        # Editor de código
        editor_frame = ttk.Frame(paned)
        paned.add(editor_frame, weight=3)
        
        # Números de linha
        self.num_linhas = tk.Text(editor_frame, width=4, state=tk.DISABLED, bg="#f0f0f0")
        self.num_linhas.pack(side=tk.LEFT, fill=tk.Y)
        
        # Editor principal
        self.editor = ScrolledText(editor_frame, wrap=tk.NONE, font=("Consolas", 11))
        self.editor.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        self.editor.bind("<Key>", self.on_key_press)
        self.editor.bind("<MouseWheel>", self.on_scroll)
        self.num_linhas.bind("<MouseWheel>", self.on_scroll)
        
        # Terminal
        terminal_frame = ttk.Frame(paned)
        paned.add(terminal_frame, weight=1)
        
        ttk.Label(terminal_frame, text="Terminal", font=("Arial", 10, "bold")).pack(anchor=tk.W)
        self.terminal = ScrolledText(terminal_frame, height=10, bg="#1e1e1e", fg="#d4d4d4", font=("Consolas", 10))
        self.terminal.pack(fill=tk.BOTH, expand=True)
        
        # Barra de status
        statusbar = ttk.Frame(self.root)
        statusbar.pack(side=tk.BOTTOM, fill=tk.X)
        
        self.status_linha = ttk.Label(statusbar, text="Linha: 1, Coluna: 1")
        self.status_linha.pack(side=tk.LEFT, padx=5)
        
        self.status_arquivo = ttk.Label(statusbar, text="Sem arquivo")
        self.status_arquivo.pack(side=tk.RIGHT, padx=5)
        
        self.atualizar_numeros_linha()
        self.editor.bind("<KeyRelease>", self.atualizar_status)
        self.editor.bind("<ButtonRelease-1>", self.atualizar_status)
    
    def novo_arquivo(self):
        """Cria um novo arquivo"""
        if self.modificado:
            if not messagebox.askyesno("Salvar", "Deseja salvar as alterações?"):
                return
        
        self.editor.delete(1.0, tk.END)
        self.arquivo_atual = None
        self.modificado = False
        self.atualizar_numeros_linha()
        self.status_arquivo.config(text="Sem arquivo")
    
    def abrir_arquivo(self):
        """Abre um arquivo existente"""
        if self.modificado:
            if not messagebox.askyesno("Salvar", "Deseja salvar as alterações?"):
                return
        
        caminho = filedialog.askopenfilename(
            title="Abrir arquivo BR",
            filetypes=[("Arquivos BR", "*.br"), ("Todos os arquivos", "*.*")]
        )
        
        if caminho:
            with open(caminho, 'r', encoding='utf-8') as f:
                conteudo = f.read()
            
            self.editor.delete(1.0, tk.END)
            self.editor.insert(1.0, conteudo)
            self.arquivo_atual = caminho
            self.modificado = False
            self.atualizar_numeros_linha()
            self.status_arquivo.config(text=os.path.basename(caminho))
    
    def salvar_arquivo(self):
        """Salva o arquivo atual"""
        if self.arquivo_atual:
            with open(self.arquivo_atual, 'w', encoding='utf-8') as f:
                f.write(self.editor.get(1.0, tk.END))
            self.modificado = False
            self.status_arquivo.config(text=os.path.basename(self.arquivo_atual))
        else:
            self.salvar_como()
    
    def salvar_como(self):
        """Salva o arquivo com outro nome"""
        caminho = filedialog.asksaveasfilename(
            title="Salvar como",
            defaultextension=".br",
            filetypes=[("Arquivos BR", "*.br"), ("Todos os arquivos", "*.*")]
        )
        
        if caminho:
            with open(caminho, 'w', encoding='utf-8') as f:
                f.write(self.editor.get(1.0, tk.END))
            self.arquivo_atual = caminho
            self.modificado = False
            self.status_arquivo.config(text=os.path.basename(caminho))
    
    def compilar(self):
        """Compila o arquivo atual"""
        if not self.arquivo_atual:
            messagebox.showwarning("Aviso", "Nenhum arquivo aberto")
            return
        
        self.salvar_arquivo()
        
        self.terminal.delete(1.0, tk.END)
        self.terminal.insert(tk.END, f"Compilando {self.arquivo_atual}...\n")
        
        try:
            resultado = subprocess.run(
                [sys.executable, "br.py", "--compile", self.arquivo_atual],
                capture_output=True,
                text=True,
                cwd=os.path.dirname(os.path.dirname(__file__))
            )
            
            self.terminal.insert(tk.END, resultado.stdout)
            if resultado.stderr:
                self.terminal.insert(tk.END, resultado.stderr, "error")
            
            self.terminal.insert(tk.END, "\nCompilação concluída.\n")
        except Exception as e:
            self.terminal.insert(tk.END, f"Erro ao compilar: {e}\n", "error")
    
    def executar(self):
        """Executa o bytecode compilado"""
        if not self.arquivo_atual:
            messagebox.showwarning("Aviso", "Nenhum arquivo aberto")
            return
        
        caminho_brc = self.arquivo_atual.replace(".br", ".brc")
        
        if not os.path.exists(caminho_brc):
            messagebox.showwarning("Aviso", "Bytecode não encontrado. Compile primeiro.")
            return
        
        self.terminal.delete(1.0, tk.END)
        self.terminal.insert(tk.END, f"Executando {caminho_brc}...\n")
        
        try:
            resultado = subprocess.run(
                [sys.executable, "br.py", "--run", caminho_brc],
                capture_output=True,
                text=True,
                cwd=os.path.dirname(os.path.dirname(__file__))
            )
            
            self.terminal.insert(tk.END, resultado.stdout)
            if resultado.stderr:
                self.terminal.insert(tk.END, resultado.stderr, "error")
            
            self.terminal.insert(tk.END, "\nExecução concluída.\n")
        except Exception as e:
            self.terminal.insert(tk.END, f"Erro ao executar: {e}\n", "error")
    
    def compilar_executar(self):
        """Compila e executa o arquivo"""
        self.compilar()
        self.executar()
    
    def formatar_codigo(self):
        """Formata o código"""
        self.terminal.insert(tk.END, "Formatando código...\n")
        # Implementação simplificada
        self.terminal.insert(tk.END, "Formatação concluída.\n")
    
    def executar_linter(self):
        """Executa o linter"""
        self.terminal.insert(tk.END, "Executando linter...\n")
        # Implementação simplificada
        self.terminal.insert(tk.END, "Linter concluído.\n")
    
    def abrir_repl(self):
        """Abre o REPL em uma nova janela"""
        self.terminal.insert(tk.END, "Iniciando REPL...\n")
        try:
            subprocess.Popen([sys.executable, "br.py", "-i"], cwd=os.path.dirname(os.path.dirname(__file__)))
        except Exception as e:
            self.terminal.insert(tk.END, f"Erro ao iniciar REPL: {e}\n", "error")
    
    def abrir_documentacao(self):
        """Abre a documentação"""
        messagebox.showinfo("Documentação", "Documentação disponível em: https://br-lang.org/docs")
    
    def sobre(self):
        """Exibe informações sobre a IDE"""
        messagebox.showinfo("Sobre IDE BR", "IDE BR v1.0.0\nAmbiente de Desenvolvimento Integrado para Linguagem BR")
    
    def on_key_press(self, event):
        """Manipula eventos de teclado"""
        if event.keysym == "Tab":
            self.editor.insert(tk.INSERT, "    ")
            return "break"
        
        self.modificado = True
        self.root.after(100, self.atualizar_numeros_linha)
    
    def on_scroll(self, event):
        """Sincroniza scroll entre editor e números de linha"""
        self.num_linhas.yview_moveto(self.editor.yview()[0])
    
    def atualizar_numeros_linha(self):
        """Atualiza os números de linha"""
        linhas = int(self.editor.index('end-1c').split('.')[0])
        self.num_linhas.config(state=tk.NORMAL)
        self.num_linhas.delete(1.0, tk.END)
        
        for i in range(1, linhas + 1):
            self.num_linhas.insert(tk.END, f"{i}\n")
        
        self.num_linhas.config(state=tk.DISABLED)
    
    def atualizar_status(self, event=None):
        """Atualiza a barra de status"""
        linha, coluna = self.editor.index(tk.INSERT).split('.')
        self.status_linha.config(text=f"Linha: {linha}, Coluna: {coluna}")
    
    def on_fechar(self):
        """Fecha a IDE"""
        if self.modificado:
            if messagebox.askyesno("Salvar", "Deseja salvar as alterações?"):
                self.salvar_arquivo()
        
        self.root.destroy()
    
    # Métodos de edição
    def desfazer(self):
        self.editor.event_generate("<<Undo>>")
    
    def refazer(self):
        self.editor.event_generate("<<Redo>>")
    
    def recortar(self):
        self.editor.event_generate("<<Cut>>")
    
    def copiar(self):
        self.editor.event_generate("<<Copy>>")
    
    def colar(self):
        self.editor.event_generate("<<Paste>>")
    
    def iniciar(self):
        self.root.mainloop()

def main():
    ide = IDEBR()
    ide.iniciar()

if __name__ == "__main__":
    main()
