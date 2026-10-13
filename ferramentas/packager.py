# -*- coding: utf-8 -*-
"""
Gerador de Pacotes BR (.brp)
Empacota bytecode, metadados, recursos e assinaturas em formato binário padronizado
"""
import os
import zipfile
import json
import hashlib
from typing import Dict, List, Optional, Any
from dataclasses import dataclass, asdict
from pathlib import Path
from sandbox import ValidadorAssinaturaDigital
from encoding_utils import ler_texto

@dataclass
class ManifestoBRM:
    """Estrutura do manifesto de pacote BR (.brm)"""
    nome: str
    versao: str
    descricao: str
    autor: str
    linguagem: str = "BR"
    versao_minima: str = "1.0.0"
    dependencias: Dict[str, str] = None
    permissoes: List[str] = None
    ponto_entrada: str = "principal.brc"
    
    def __post_init__(self):
        if self.dependencias is None:
            self.dependencias = {}
        if self.permissoes is None:
            self.permissoes = []

@dataclass
class RecursoPacote:
    """Representa um recurso estático do pacote"""
    caminho_origem: str
    caminho_destino: str
    tipo: str = "binario"

class GeradorPacoteBR:
    """Gerador de pacotes .brp"""
    
    def __init__(self):
        self.manifesto: Optional[ManifestoBRM] = None
        self.arquivos_bytecode: Dict[str, bytes] = {}
        self.recursos: List[RecursoPacote] = []
        self.assinatura: Optional[str] = None
    
    def definir_manifesto(self, manifesto: ManifestoBRM):
        """Define o manifesto do pacote"""
        self.manifesto = manifesto
    
    def adicionar_bytecode(self, nome_arquivo: str, caminho_brc: str):
        """Adiciona um arquivo bytecode ao pacote"""
        with open(caminho_brc, "rb") as f:
            self.arquivos_bytecode[nome_arquivo] = f.read()
    
    def adicionar_bytecode_bytes(self, nome_arquivo: str, conteudo: bytes):
        """Adiciona bytecode diretamente como bytes"""
        self.arquivos_bytecode[nome_arquivo] = conteudo
    
    def adicionar_recurso(self, caminho_origem: str, caminho_destino: str, tipo: str = "binario"):
        """Adiciona um recurso estático ao pacote"""
        self.recursos.append(RecursoPacote(caminho_origem, caminho_destino, tipo))
    
    def gerar_manifesto_json(self) -> str:
        """Gera o manifesto em formato JSON"""
        if not self.manifesto:
            raise ValueError("Manifesto não definido")
        
        return json.dumps(asdict(self.manifesto), indent=2, ensure_ascii=False)
    
    def gerar_assinatura(self, chave_privada: str) -> str:
        """Gera assinatura digital do pacote"""
        # Em produção, usaria criptografia assimétrica real
        dados_assinar = self.gerar_manifesto_json()
        for nome, conteudo in self.arquivos_bytecode.items():
            dados_assinar += hashlib.sha256(conteudo).hexdigest()
        
        assinatura = hashlib.sha256((dados_assinar + chave_privada).encode()).hexdigest()
        self.assinatura = assinatura
        return assinatura
    
    def empacotar(self, caminho_saida: str, assinar: bool = False, chave_privada: str = "") -> str:
        """Empacota tudo em um arquivo .brp"""
        if not self.manifesto:
            raise ValueError("Manifesto não definido. Use definir_manifesto() primeiro.")
        
        # Gerar assinatura se solicitado
        if assinar and chave_privada:
            self.gerar_assinatura(chave_privada)
        
        # Criar arquivo ZIP
        with zipfile.ZipFile(caminho_saida, 'w', zipfile.ZIP_DEFLATED) as zipf:
            # Adicionar manifesto
            manifesto_json = self.gerar_manifesto_json()
            zipf.writestr("manifesto.brm", manifesto_json)
            
            # Adicionar bytecode
            for nome, conteudo in self.arquivos_bytecode.items():
                caminho_zip = f"bin/{nome}"
                zipf.writestr(caminho_zip, conteudo)
            
            # Adicionar recursos
            for recurso in self.recursos:
                if os.path.exists(recurso.caminho_origem):
                    caminho_zip = f"recursos/{recurso.caminho_destino}"
                    zipf.write(recurso.caminho_origem, caminho_zip)
            
            # Adicionar assinatura se existir
            if self.assinatura:
                zipf.writestr("assinatura.sig", self.assinatura)
        
        return caminho_saida

class LeitorPacoteBR:
    """Leitor de pacotes .brp"""
    
    def __init__(self, caminho_pacote: str):
        self.caminho_pacote = caminho_pacote
        self.manifesto: Optional[ManifestoBRM] = None
        self.arquivos_bytecode: Dict[str, bytes] = {}
        self.recursos: Dict[str, bytes] = {}
        self.assinatura: Optional[str] = None
    
    def ler(self):
        """Lê o pacote .brp"""
        if not os.path.exists(self.caminho_pacote):
            raise FileNotFoundError(f"Pacote não encontrado: {self.caminho_pacote}")
        
        with zipfile.ZipFile(self.caminho_pacote, 'r') as zipf:
            # Ler manifesto
            try:
                with zipf.open("manifesto.brm") as f:
                    manifesto_dict = json.loads(f.read().decode('utf-8'))
                    self.manifesto = ManifestoBRM(**manifesto_dict)
            except KeyError:
                raise ValueError("Pacote inválido: manifesto.brm não encontrado")
            
            # Ler bytecode
            for nome in zipf.namelist():
                if nome.startswith("bin/"):
                    nome_arquivo = nome[4:]  # Remover "bin/"
                    with zipf.open(nome) as f:
                        self.arquivos_bytecode[nome_arquivo] = f.read()
            
            # Ler recursos
            for nome in zipf.namelist():
                if nome.startswith("recursos/"):
                    nome_recurso = nome[9:]  # Remover "recursos/"
                    with zipf.open(nome) as f:
                        self.recursos[nome_recurso] = f.read()
            
            # Ler assinatura
            try:
                with zipf.open("assinatura.sig") as f:
                    self.assinatura = f.read().decode('utf-8')
            except KeyError:
                self.assinatura = None
    
    def extrair(self, diretorio_destino: str):
        """Extrai o conteúdo do pacote para um diretório"""
        os.makedirs(diretorio_destino, exist_ok=True)
        
        with zipfile.ZipFile(self.caminho_pacote, 'r') as zipf:
            zipf.extractall(diretorio_destino)
    
    def verificar_assinatura(self, assinatura_esperada: str) -> bool:
        """Verifica se a assinatura do pacote corresponde à esperada"""
        return self.assinatura == assinatura_esperada
    
    def obter_bytecode(self, nome_arquivo: str) -> Optional[bytes]:
        """Obtém bytecode de um arquivo específico"""
        return self.arquivos_bytecode.get(nome_arquivo)
    
    def obter_recurso(self, nome_recurso: str) -> Optional[bytes]:
        """Obtém um recurso específico"""
        return self.recursos.get(nome_recurso)

def _valor_manifesto(linha: str) -> Optional[str]:
    partes = linha.split('"')
    if len(partes) >= 3:
        return partes[1]
    partes = linha.split(maxsplit=1)
    return partes[1].strip() if len(partes) == 2 else None

def _ler_manifesto_projeto(diretorio_projeto: str) -> ManifestoBRM:
    caminho_projeto_br = os.path.join(diretorio_projeto, "projeto.br")
    caminho_projeto_brm = os.path.join(diretorio_projeto, "projeto.brm")

    caminho_manifesto = None
    if os.path.exists(caminho_projeto_brm):
        caminho_manifesto = caminho_projeto_brm
    elif os.path.exists(caminho_projeto_br):
        caminho_manifesto = caminho_projeto_br

    dados = {
        "nome": Path(diretorio_projeto).resolve().name or "projeto",
        "versao": "1.0.0",
        "descricao": "",
        "autor": "",
        "ponto_entrada": "principal.brc",
    }
    dependencias = {}
    permissoes = []

    if caminho_manifesto:
        em_dependencias = False
        em_permissoes = False
        for linha in ler_texto(caminho_manifesto).splitlines():
            linha = linha.strip()
            if not linha or linha.startswith("#"):
                continue
            if linha == "dependencias":
                em_dependencias = True
                em_permissoes = False
                continue
            if linha == "permissoes":
                em_permissoes = True
                em_dependencias = False
                continue
            if linha == "fim":
                em_dependencias = False
                em_permissoes = False
                continue

            if em_dependencias:
                partes = linha.split("@", 1)
                nome = partes[0].strip()
                versao = partes[1].strip() if len(partes) == 2 else "*"
                if nome:
                    dependencias[nome] = versao
                continue
            if em_permissoes:
                permissoes.append(linha)
                continue

            for chave in ("nome", "versao", "descricao", "autor", "ponto_entrada", "entrada"):
                if linha.startswith(chave):
                    valor = _valor_manifesto(linha)
                    if valor:
                        dados["ponto_entrada" if chave == "entrada" else chave] = valor
                    break

    return ManifestoBRM(
        nome=dados["nome"],
        versao=dados["versao"],
        descricao=dados["descricao"],
        autor=dados["autor"],
        dependencias=dependencias,
        permissoes=permissoes,
        ponto_entrada=dados["ponto_entrada"],
    )

def _compilar_fonte_br_para_brc1(caminho_fonte: str) -> bytes:
    from lexer import LexerBR
    from parser import ParserBR
    from semantic_analyzer import analisar_semantica
    from optimizer import otimizar_codigo
    from codegen import gerar_bytecode, serializar_brc1

    codigo = ler_texto(caminho_fonte)
    tokens = LexerBR(codigo).tokenizar()
    parser = ParserBR(tokens)
    ast = parser.parse()
    if parser.erros:
        erros = "\n".join(str(erro) for erro in parser.erros)
        raise ValueError(f"Erro de sintaxe ao empacotar '{caminho_fonte}':\n{erros}")

    sucesso, erros, _avisos = analisar_semantica(ast)
    if not sucesso:
        erros_txt = "\n".join(str(erro) for erro in erros)
        raise ValueError(f"Erro semantico ao empacotar '{caminho_fonte}':\n{erros_txt}")

    ast_otimizado, _relatorio = otimizar_codigo(ast)
    bytecode = gerar_bytecode(ast_otimizado)
    return b"BRC1" + serializar_brc1(bytecode)

def criar_pacote_projeto(diretorio_projeto: str, caminho_saida: str) -> str:
    """Cria um pacote .brp a partir de um diretório de projeto"""
    gerador = GeradorPacoteBR()
    gerador.definir_manifesto(_ler_manifesto_projeto(diretorio_projeto))

    diretorio_fonte = os.path.join(diretorio_projeto, "fonte")
    if os.path.exists(diretorio_fonte):
        for caminho in Path(diretorio_fonte).rglob("*"):
            if not caminho.is_file():
                continue
            relativo = caminho.relative_to(diretorio_fonte)
            if caminho.suffix == ".brc":
                gerador.adicionar_bytecode(relativo.as_posix(), str(caminho))
            elif caminho.suffix == ".br":
                if caminho.with_suffix(".brc").exists():
                    continue
                nome_bytecode = relativo.with_suffix(".brc").as_posix()
                gerador.adicionar_bytecode_bytes(nome_bytecode, _compilar_fonte_br_para_brc1(str(caminho)))

    if not gerador.arquivos_bytecode:
        raise ValueError("Nenhum arquivo .br ou .brc encontrado em fonte/ para empacotar.")

    diretorio_recursos = os.path.join(diretorio_projeto, "recursos")
    if os.path.exists(diretorio_recursos):
        for caminho in Path(diretorio_recursos).rglob("*"):
            if caminho.is_file():
                relativo = caminho.relative_to(diretorio_recursos).as_posix()
                gerador.adicionar_recurso(str(caminho), relativo)

    return gerador.empacotar(caminho_saida)

def listar_conteudo_pacote(caminho_pacote: str) -> str:
    """Lista o conteúdo de um pacote .brp"""
    leitor = LeitorPacoteBR(caminho_pacote)
    leitor.ler()
    
    relatorio = f"""
Pacote: {caminho_pacote}
"""
    
    if leitor.manifesto:
        relatorio += f"""
Manifesto:
  Nome: {leitor.manifesto.nome}
  Versão: {leitor.manifesto.versao}
  Descrição: {leitor.manifesto.descricao}
  Autor: {leitor.manifesto.autor}
  Ponto de Entrada: {leitor.manifesto.ponto_entrada}
"""
    
    relatorio += f"\nArquivos Bytecode ({len(leitor.arquivos_bytecode)}):\n"
    for nome in leitor.arquivos_bytecode.keys():
        relatorio += f"  - bin/{nome}\n"
    
    relatorio += f"\nRecursos ({len(leitor.recursos)}):\n"
    for nome in leitor.recursos.keys():
        relatorio += f"  - recursos/{nome}\n"
    
    if leitor.assinatura:
        relatorio += f"\nAssinatura Digital: Presente\n"
    else:
        relatorio += f"\nAssinatura Digital: Ausente\n"
    
    return relatorio
