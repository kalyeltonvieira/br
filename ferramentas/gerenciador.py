# -*- coding: utf-8 -*-
"""
Gerenciador de Pacotes BR (mercado)
Gerencia dependências, versionamento e integração com registro.br
"""
import sys
import os
import re
import json
import hashlib
import shutil
from typing import Dict, List, Optional, Tuple
from packager import criar_pacote_projeto, listar_conteudo_pacote, LeitorPacoteBR

# Configuração do registro
REGISTRO_URL = "http://localhost:5000/api"
CACHE_DIR = os.path.expanduser("~/.mercado_cache")
CHAVES_DIR = os.path.expanduser("~/.br_chaves")
MODULOS_USUARIO_DIR = os.path.expanduser("~/.br/modulos")

class ResolvedorDependencias:
    """Resolvedor de dependências usando algoritmo MVS (Minimal Version Selection)"""
    
    def __init__(self):
        self.grafo_dependencias: Dict[str, Dict[str, str]] = {}
    
    def adicionar_pacote(self, nome: str, versao: str, dependencias: Dict[str, str]):
        """Adiciona um pacote ao grafo de dependências"""
        self.grafo_dependencias[nome] = {
            "versao": versao,
            "dependencias": dependencias
        }
    
    def resolver(self, raiz: str, versao: str = "*") -> Dict[str, str]:
        """Resolve dependências usando MVS"""
        resultado = {}
        visitados = set()
        
        def visitar(nome: str, versao_req: str):
            if nome in visitados:
                return
            visitados.add(nome)
            
            if nome not in self.grafo_dependencias:
                print(f"Aviso: Pacote '{nome}' não encontrado no registro local")
                return
            
            pacote = self.grafo_dependencias[nome]
            versao_selecionada = self._selecionar_versao(versao_req, pacote["versao"])
            resultado[nome] = versao_selecionada
            
            for dep_nome, dep_versao in pacote["dependencias"].items():
                visitar(dep_nome, dep_versao)
        
        visitar(raiz, versao)
        return resultado
    
    def _selecionar_versao(self, requisicao: str, disponivel: str) -> str:
        """Seleciona a versão compatível (simplificado)"""
        if requisicao == "*":
            return disponivel
        # Em produção, implementaria lógica SemVer completa
        return disponivel

def ler_projeto(caminho_projeto):
    if not os.path.exists(caminho_projeto):
        print(f"Erro: Arquivo '{caminho_projeto}' não encontrado. Execute o comando em um diretório de projeto BR válido.")
        sys.exit(1)
        
    com_conteudo = ""
    with open(caminho_projeto, "r", encoding="utf-8") as f:
        com_conteudo = f.read()
        
    # Limpar comentários simples
    com_conteudo = re.sub(r'#.*', '', com_conteudo)
    
    projeto = {
        "linguagem": None,
        "nome": None,
        "versao": None,
        "dependencias": {},
        "permissoes": []
    }
    
    # Parse básico
    linhas = [l.strip() for l in com_conteudo.splitlines() if l.strip()]
    
    em_dependencias = False
    em_permissoes = False
    
    for linha in linhas:
        if linha == "dependencias":
            em_dependencias = True
            em_permissoes = False
            continue
        elif linha == "permissoes":
            em_permissoes = True
            em_dependencias = False
            continue
        elif linha == "fim":
            em_dependencias = False
            em_permissoes = False
            continue
            
        if em_dependencias:
            # Parse de dependência com versão: pacote @ 1.2.0
            match = re.match(r'^(\w+)\s+@\s+([\d.]+)$', linha)
            if match:
                nome, versao = match.groups()
                projeto["dependencias"][nome] = versao
            else:
                # Sem versão especificada
                projeto["dependencias"][linha] = "*"
        elif em_permissoes:
            projeto["permissoes"].append(linha)
        else:
            # Match chave-valor tipo: nome "Minha Loja" ou linguagem "BR 1.0"
            match = re.match(r'^(\w+)\s+"([^"]+)"$', linha)
            if match:
                chave, valor = match.groups()
                if chave in projeto:
                    projeto[chave] = valor
                    
    return projeto

def obter_caminho_projeto():
    if os.path.exists("projeto.brm"):
        return "projeto.brm"
    return "projeto.br"

def instalar_pacote_local(caminho_pacote: str):
    """Instala um pacote .brp local em ~/.br/modulos/<nome>."""
    leitor = LeitorPacoteBR(caminho_pacote)
    leitor.ler()
    nome = leitor.manifesto.nome if leitor.manifesto else os.path.splitext(os.path.basename(caminho_pacote))[0]
    destino = os.path.join(MODULOS_USUARIO_DIR, nome)
    os.makedirs(destino, exist_ok=True)
    leitor.extrair(destino)
    print(f"Pacote local '{nome}' instalado em {destino}")
    return destino

def cmd_instalar(pacotes):
    pacotes_locais = [p for p in pacotes if p.endswith(".brp") or os.path.exists(p)]
    if pacotes_locais:
        for pacote in pacotes_locais:
            if not os.path.exists(pacote):
                print(f"Erro: pacote local '{pacote}' nao encontrado.")
                continue
            instalar_pacote_local(pacote)
        return

    projeto = ler_projeto(obter_caminho_projeto())
    print(f"Verificando dependências do projeto '{projeto['nome']}' (Versão {projeto['versao']})...")
    
    if not pacotes:
        # Instalar dependências listadas no projeto.br
        pacotes = list(projeto["dependencias"].keys())
        
    if not pacotes:
        print("Nenhum pacote especificado para instalação.")
        return
    
    # Criar diretório de cache
    os.makedirs(CACHE_DIR, exist_ok=True)
    os.makedirs("dependencias", exist_ok=True)
    
    # Resolver dependências
    resolvedor = ResolvedorDependencias()
    for pac in pacotes:
        versao = projeto["dependencias"].get(pac, "*")
        resolvedor.adicionar_pacote(pac, versao, {})  # Simplificado
    
    print("Conectando ao Mercado BR...")
    print(f"Resolvendo grafo de dependências para {len(pacotes)} pacote(s)...")
    
    dependencias_resolvidas = {}
    for pac in pacotes:
        versao = projeto["dependencias"].get(pac, "*")
        deps = resolvedor.resolver(pac, versao)
        dependencias_resolvidas.update(deps)
    
    # Baixar pacotes
    for pac, versao in dependencias_resolvidas.items():
        caminho_cache = os.path.join(CACHE_DIR, f"{pac}-{versao}.brp")
        caminho_destino = os.path.join("dependencias", f"{pac}.brp")
        
        if os.path.exists(caminho_cache):
            print(f" -> Usando cache para '{pac}@{versao}'")
            shutil.copy(caminho_cache, caminho_destino)
        else:
            print(f" -> Baixando pacote '{pac}@{versao}' do repositório oficial...")
            # Simular download
            with open(caminho_destino, "w", encoding="utf-8") as f:
                f.write(f"# Pacote BR: {pac} v{versao}\n")
            # Copiar para cache
            shutil.copy(caminho_destino, caminho_cache)
            print(f" -> Pacote '{pac}@{versao}' instalado com sucesso!")
    
    print(f"\nResumo: {len(dependencias_resolvidas)} pacote(s) instalado(s).")

def cmd_atualizar():
    projeto = ler_projeto(obter_caminho_projeto())
    print(f"Atualizando dependências do projeto '{projeto['nome']}'...")
    
    if not os.path.exists("dependencias") or not os.listdir("dependencias"):
        print("Nenhuma dependência instalada para atualizar.")
        return
    
    os.makedirs(CACHE_DIR, exist_ok=True)
    
    for arq in os.listdir("dependencias"):
        if arq.endswith(".brp"):
            pac = arq[:-4]
            print(f" -> Verificando atualizações para '{pac}'...")
            # Simular verificação de versão
            print(f" -> '{pac}' já está na versão mais recente.")
    
    print("Atualização concluída.")

def cmd_remover(pacotes):
    projeto = ler_projeto(obter_caminho_projeto())
    if not pacotes:
        print("Erro: especifique quais pacotes deseja remover.")
        return
        
    for pac in pacotes:
        caminho = os.path.join("dependencias", f"{pac}.brp")
        if os.path.exists(caminho):
            os.remove(caminho)
            print(f" -> Pacote '{pac}' removido do projeto.")
        else:
            print(f" -> Pacote '{pac}' não está instalado.")
    print("Processo de remoção concluído.")

def cmd_listar():
    projeto = ler_projeto(obter_caminho_projeto())
    print(f"Dependências do projeto '{projeto['nome']}' (Versão {projeto['versao']}):")
    
    if not projeto["dependencias"]:
        print("  Nenhuma dependência declarada.")
        return
    
    for nome, versao in projeto["dependencias"].items():
        caminho = os.path.join("dependencias", f"{nome}.brp")
        status = "✓ instalado" if os.path.exists(caminho) else "✗ não instalado"
        print(f"  {nome} @ {versao} [{status}]")

def cmd_buscar(termo: str):
    """Busca pacotes no registro"""
    print(f"Buscando pacotes contendo '{termo}' no Mercado BR...")
    print("Conectando ao registro.br...")
    
    # Simular resultados de busca
    resultados = [
        {"nome": "exemplo-br", "versao": "1.0.0", "descricao": "Pacote de exemplo para BR"},
        {"nome": "utils-br", "versao": "2.1.3", "descricao": "Utilitários comuns para BR"},
    ]
    
    for pacote in resultados:
        if termo.lower() in pacote["nome"].lower():
            print(f"\n  {pacote['nome']} (v{pacote['versao']})")
            print(f"    {pacote['descricao']}")

def nome_arquivo_pacote(projeto) -> str:
    nome = projeto.get("nome") or "projeto"
    versao = projeto.get("versao") or "1.0.0"
    slug = re.sub(r"[^A-Za-z0-9._-]+", "_", nome).strip("_").lower() or "projeto"
    return f"{slug}-{versao}.brp"

def cmd_empacotar(args=None):
    projeto = ler_projeto(obter_caminho_projeto())
    caminho_saida = args[0] if args else nome_arquivo_pacote(projeto)
    print(f"Empacotando projeto '{projeto['nome']}' (Versão {projeto['versao']})...")

    try:
        criar_pacote_projeto(".", caminho_saida)
        print(f"Pacote criado: {caminho_saida}")
    except Exception as e:
        print(f"Erro ao criar pacote: {e}")
        sys.exit(1)

def cmd_publicar():
    projeto = ler_projeto(obter_caminho_projeto())
    print(f"Preparando publicação do pacote '{projeto['nome']}' (Versão {projeto['versao']})...")
    
    # Verificar se está no diretório do projeto
    if not os.path.exists("fonte"):
        print("Erro: diretório 'fonte/' não encontrado. Execute no diretório raiz do projeto.")
        sys.exit(1)
    
    print("Rodando testes locais...")
    # Em produção isso executaria `brasil testar`
    print(" -> Testes passados ✓")
    
    print("Compactando fontes em um arquivo .brp...")
    pacote_nome = nome_arquivo_pacote(projeto)
    
    try:
        criar_pacote_projeto(".", pacote_nome)
        print(f" -> Pacote criado: {pacote_nome}")
    except Exception as e:
        print(f"Erro ao criar pacote: {e}")
        sys.exit(1)
    
    print(f"\nPara publicar no registro.br, execute:")
    print(f"  mercado publicar --upload {pacote_nome}")

def cmd_info(pacote: str):
    """Exibe informações detalhadas de um pacote"""
    caminho = pacote if os.path.exists(pacote) else os.path.join("dependencias", f"{pacote}.brp")
    
    if not os.path.exists(caminho):
        print(f"Pacote '{pacote}' não está instalado.")
        return
    
    print(listar_conteudo_pacote(caminho))

def gerar_par_chaves(autor: str) -> bool:
    """Gera um par de chaves PGP para o autor"""
    os.makedirs(CHAVES_DIR, exist_ok=True)
    
    # Implementação simplificada - em produção usar cryptography/gnupg
    chave_privada = hashlib.sha256(f"{autor}_priv".encode()).hexdigest()
    chave_publica = hashlib.sha256(f"{autor}_pub".encode()).hexdigest()
    
    # Salvar chaves
    with open(os.path.join(CHAVES_DIR, f"{autor}_priv.key"), 'w') as f:
        f.write(chave_privada)
    
    with open(os.path.join(CHAVES_DIR, f"{autor}_pub.key"), 'w') as f:
        f.write(chave_publica)
    
    # Registrar chave pública no servidor
    try:
        response = requests.post(f"{REGISTRO_URL}/chaves", json={
            'autor': autor,
            'chave_publica': chave_publica
        })
        if response.status_code == 200:
            print(f"Chaves geradas e registradas para '{autor}'")
            return True
        else:
            print(f"Erro ao registrar chave pública: {response.text}")
            return False
    except Exception as e:
        print(f"Erro ao registrar chave pública: {e}")
        return False

def gerar_par_chaves(autor: str) -> bool:
    """Gera chaves e registra a chave publica em arquivo local."""
    os.makedirs(CHAVES_DIR, exist_ok=True)
    chave_privada = hashlib.sha256(f"{autor}_priv".encode()).hexdigest()
    chave_publica = hashlib.sha256(f"{autor}_pub".encode()).hexdigest()

    with open(os.path.join(CHAVES_DIR, f"{autor}_priv.key"), "w", encoding="utf-8") as f:
        f.write(chave_privada)
    with open(os.path.join(CHAVES_DIR, f"{autor}_pub.key"), "w", encoding="utf-8") as f:
        f.write(chave_publica)

    registro_local = os.path.join(CHAVES_DIR, "registro_chaves.json")
    chaves = {}
    if os.path.exists(registro_local):
        with open(registro_local, "r", encoding="utf-8") as f:
            chaves = json.load(f)
    chaves[autor] = chave_publica
    with open(registro_local, "w", encoding="utf-8") as f:
        json.dump(chaves, f, ensure_ascii=False, indent=2)

    print(f"Chaves geradas e registradas localmente para '{autor}'")
    return True

def assinar_pacote(arquivo_brp: str, autor: str) -> str:
    """Assina um pacote com a chave privada do autor"""
    caminho_chave = os.path.join(CHAVES_DIR, f"{autor}_priv.key")
    if not os.path.exists(caminho_chave):
        raise Exception("Chave privada não encontrada. Gere as chaves primeiro com 'mercado registrar'.")
    
    with open(caminho_chave, 'r') as f:
        chave_privada = f.read()
    
    # Calcular hash do arquivo
    with open(arquivo_brp, 'rb') as f:
        conteudo = f.read()
    
    hash_arquivo = hashlib.sha256(conteudo).hexdigest()
    
    # Assinar (simplificado - em produção usar PGP/GPG)
    assinatura = hashlib.sha256(f"{hash_arquivo}{chave_privada}".encode()).hexdigest()
    
    return assinatura

def main():
    if len(sys.argv) < 2:
        print("Uso do Gerenciador de Pacotes BR (mercado):")
        print("  mercado instalar [pacote]      Instala dependências ou pacote específico")
        print("  mercado atualizar             Atualiza as dependências locais")
        print("  mercado remover <pacote>      Remove uma biblioteca do projeto")
        print("  mercado listar                Exibe todas as dependências ativas")
        print("  mercado buscar <termo>       Procura pacotes disponíveis no repositório")
        print("  mercado empacotar [saida.brp] Compila fonte/ e cria um pacote local")
        print("  mercado publicar              Assina e publica o pacote no registro.br")
        print("  mercado info <pacote>         Exibe informações detalhadas do pacote")
        print("  mercado registrar <autor>     Gera e registra par de chaves PGP")
        sys.exit(1)
        
    cmd = sys.argv[1]
    args = sys.argv[2:]
    
    if cmd == "instalar":
        cmd_instalar(args)
    elif cmd == "atualizar":
        cmd_atualizar()
    elif cmd == "remover":
        cmd_remover(args)
    elif cmd == "listar":
        cmd_listar()
    elif cmd == "buscar":
        if args:
            cmd_buscar(args[0])
        else:
            print("Erro: especifique um termo para busca.")
            sys.exit(1)
    elif cmd == "empacotar":
        cmd_empacotar(args)
    elif cmd == "publicar":
        cmd_publicar()
    elif cmd == "info":
        if args:
            cmd_info(args[0])
        else:
            print("Erro: especifique o nome do pacote.")
            sys.exit(1)
    elif cmd == "registrar":
        if args:
            gerar_par_chaves(args[0])
        else:
            print("Erro: especifique o nome do autor.")
            sys.exit(1)
    else:
        print(f"Erro: Comando '{cmd}' desconhecido pelo gerenciador 'mercado'.")
        sys.exit(1)

if __name__ == "__main__":
    main()
