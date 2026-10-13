# -*- coding: utf-8 -*-
"""
Resolucao e runtime de modulos da Linguagem BR.
"""
import json
import math
import os
import random
import base64
import csv as csv_module
import datetime as _datetime
import hashlib
import io
import platform
import re
import secrets
import shlex
import shutil
import subprocess
import sqlite3
import sys
import unicodedata
import urllib.parse
import urllib.request
import uuid as uuid_module
from pathlib import Path
from typing import Any, Callable, Dict, Iterable, Optional
from encoding_utils import ler_texto, escrever_texto


class ErroModuloBR(Exception):
    pass


_DB_CONN = None


def _texto(valor: Any) -> str:
    return "" if valor is None else str(valor)


def _caminho(valor: Any) -> Path:
    return Path(_texto(valor))


def _escrever_arquivo(caminho: Any, conteudo: Any) -> int:
    path = _caminho(caminho)
    if str(path.parent) not in ("", "."):
        path.parent.mkdir(parents=True, exist_ok=True)
    texto = _texto(conteudo)
    escrever_texto(path, texto)
    return len(texto)


def _arquivo_anexar(caminho: Any, conteudo: Any) -> int:
    path = _caminho(caminho)
    if str(path.parent) not in ("", "."):
        path.parent.mkdir(parents=True, exist_ok=True)
    texto = _texto(conteudo)
    with open(path, "a", encoding="utf-8") as arquivo:
        arquivo.write(texto)
    return len(texto)


def _arquivo_listar(caminho: Any = ".") -> list:
    path = _caminho(caminho or ".")
    return sorted(item.name for item in path.iterdir())


def _arquivo_criar_diretorio(caminho: Any) -> bool:
    _caminho(caminho).mkdir(parents=True, exist_ok=True)
    return True


def _arquivo_remover(caminho: Any) -> bool:
    path = _caminho(caminho)
    if not path.exists():
        return False
    if path.is_dir():
        path.rmdir()
    else:
        path.unlink()
    return True


def _arquivo_copiar(origem: Any, destino: Any) -> bool:
    origem_path = _caminho(origem)
    destino_path = _caminho(destino)
    if str(destino_path.parent) not in ("", "."):
        destino_path.parent.mkdir(parents=True, exist_ok=True)
    if origem_path.is_dir():
        shutil.copytree(origem_path, destino_path, dirs_exist_ok=True)
    else:
        shutil.copy2(origem_path, destino_path)
    return True


def _arquivo_mover(origem: Any, destino: Any) -> bool:
    destino_path = _caminho(destino)
    if str(destino_path.parent) not in ("", "."):
        destino_path.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(_caminho(origem)), str(destino_path))
    return True


def _arquivo_linhas(caminho: Any) -> list:
    return ler_texto(_caminho(caminho)).splitlines()


def _bool_texto(valor: Any) -> bool:
    if isinstance(valor, bool):
        return valor
    return _texto(valor).strip().lower() in ("1", "sim", "true", "verdadeiro", "yes")


def _slug(valor: Any) -> str:
    texto = unicodedata.normalize("NFKD", _texto(valor))
    texto = "".join(ch for ch in texto if not unicodedata.combining(ch))
    texto = re.sub(r"[^A-Za-z0-9]+", "-", texto.lower()).strip("-")
    return texto


def _json_parsear(texto: Any) -> Any:
    return json.loads(_texto(texto))


def _json_validar(dados: Any) -> bool:
    try:
        json.loads(_texto(dados))
        return True
    except (TypeError, ValueError):
        return False


def _json_gerar(valor: Any) -> str:
    return json.dumps(valor, ensure_ascii=False, separators=(",", ":"))


def _json_bonito(valor: Any) -> str:
    if isinstance(valor, str):
        valor = json.loads(valor)
    return json.dumps(valor, ensure_ascii=False, indent=2, sort_keys=True)


def _csv_rows(dados: Any):
    return list(csv_module.reader(io.StringIO(_texto(dados))))


def _csv_cabecalho(dados: Any) -> str:
    rows = _csv_rows(dados)
    return ",".join(rows[0]) if rows else ""


def _csv_linhas(dados: Any) -> int:
    return len(_csv_rows(dados))


def _csv_escrever(dados: Any) -> str:
    if isinstance(dados, list):
        output = io.StringIO()
        writer = csv_module.writer(output, lineterminator="\n")
        for row in dados:
            writer.writerow(row if isinstance(row, (list, tuple)) else [row])
        return output.getvalue().rstrip("\n")
    return _texto(dados)


def _path_juntar(base: Any, nome: Any) -> str:
    return str(Path(_texto(base)) / _texto(nome))


def _path_extensao(caminho: Any) -> str:
    return Path(_texto(caminho)).suffix


def _path_diretorio(caminho: Any) -> str:
    return str(Path(_texto(caminho)).parent)


def _path_normalizar(caminho: Any) -> str:
    return os.path.normpath(_texto(caminho))


def _regex_buscar(padrao: Any, valor: Any) -> str:
    match = re.search(_texto(padrao), _texto(valor))
    return match.group(0) if match else ""


def _regex_todos(padrao: Any, valor: Any) -> list:
    return [match.group(0) for match in re.finditer(_texto(padrao), _texto(valor))]


def _regex_contar(padrao: Any, valor: Any) -> int:
    return len(_regex_todos(padrao, valor))


def _regex_grupos(padrao: Any, valor: Any) -> str:
    match = re.search(_texto(padrao), _texto(valor))
    if not match:
        return ""
    return ",".join(_texto(item) for item in match.groups())


def _hash_sha256(valor: Any) -> str:
    return hashlib.sha256(_texto(valor).encode("utf-8")).hexdigest()


def _base64(valor: Any) -> str:
    return base64.b64encode(_texto(valor).encode("utf-8")).decode("ascii")


def _uuid_validar(valor: Any) -> bool:
    try:
        uuid_module.UUID(_texto(valor))
        return True
    except ValueError:
        return False


def _senha_forca(valor: Any) -> int:
    senha = _texto(valor)
    pontos = 0
    pontos += int(len(senha) >= 8)
    pontos += int(bool(re.search(r"[a-z]", senha)))
    pontos += int(bool(re.search(r"[A-Z]", senha)))
    pontos += int(bool(re.search(r"\d", senha)))
    pontos += int(bool(re.search(r"[^A-Za-z0-9]", senha)))
    return pontos


def _email_valido(valor: Any) -> bool:
    return bool(re.fullmatch(r"[^@\s]+@[^@\s]+\.[^@\s]+", _texto(valor)))


def _digitos(valor: Any) -> str:
    return re.sub(r"\D+", "", _texto(valor))


def _cpf_valido(valor: Any) -> bool:
    digitos = _digitos(valor)
    if len(digitos) != 11 or len(set(digitos)) == 1:
        return False
    nums = [int(d) for d in digitos]
    soma = sum(nums[i] * (10 - i) for i in range(9))
    d1 = (soma * 10) % 11
    d1 = 0 if d1 == 10 else d1
    soma = sum(nums[i] * (11 - i) for i in range(10))
    d2 = (soma * 10) % 11
    d2 = 0 if d2 == 10 else d2
    return nums[9] == d1 and nums[10] == d2


def _cnpj_valido(valor: Any) -> bool:
    digitos = _digitos(valor)
    if len(digitos) != 14 or len(set(digitos)) == 1:
        return False
    nums = [int(d) for d in digitos]

    def calcula(posicoes):
        soma = sum(nums[i] * peso for i, peso in enumerate(posicoes))
        resto = soma % 11
        return 0 if resto < 2 else 11 - resto

    d1 = calcula([5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2])
    d2 = calcula([6, 5, 4, 3, 2, 9, 8, 7, 6, 5, 4, 3, 2])
    return nums[12] == d1 and nums[13] == d2


def _cep_valido(valor: Any) -> bool:
    return bool(re.fullmatch(r"\d{5}-?\d{3}", _texto(valor)))


def _data_parse(valor: Any):
    texto = _texto(valor)
    for fmt in ("%Y-%m-%d", "%Y-%m-%dT%H:%M:%S"):
        try:
            return _datetime.datetime.strptime(texto, fmt)
        except ValueError:
            pass
    return _datetime.datetime.fromisoformat(texto)


def _data_diferenca(inicio: Any, fim_valor: Any) -> int:
    return (_data_parse(fim_valor) - _data_parse(inicio)).days


def _colecao_tamanho(dados: Any) -> int:
    try:
        return len(dados)
    except TypeError:
        return 0


def _colecao_vazia(dados: Any) -> bool:
    return _colecao_tamanho(dados) == 0


def _colecao_primeiro(dados: Any) -> Any:
    if isinstance(dados, dict):
        return next(iter(dados.values()), None)
    if isinstance(dados, (list, tuple, str)):
        return dados[0] if dados else None
    return None


def _colecao_ordenar(dados: Any) -> Any:
    try:
        return sorted(dados)
    except Exception:
        return dados


def _banco_conectar(url: Any = ":memory:") -> bool:
    global _DB_CONN
    destino = _texto(url) or ":memory:"
    if destino in ("memoria", "memory"):
        destino = ":memory:"
    if _DB_CONN is not None:
        _DB_CONN.close()
    _DB_CONN = sqlite3.connect(destino)
    return True


def _banco_conn():
    global _DB_CONN
    if _DB_CONN is None:
        _banco_conectar(":memory:")
    return _DB_CONN


def _banco_consulta(sql: Any) -> str:
    conn = _banco_conn()
    cursor = conn.execute(_texto(sql))
    if cursor.description:
        linhas = cursor.fetchall()
        if len(linhas) == 1 and len(linhas[0]) == 1:
            return _texto(linhas[0][0])
        colunas = [desc[0] for desc in cursor.description]
        dados = [dict(zip(colunas, linha)) for linha in linhas]
        return json.dumps(dados, ensure_ascii=False, separators=(",", ":"))
    conn.commit()
    return _texto(cursor.rowcount)


def _banco_transacao() -> bool:
    _banco_conn().execute("BEGIN")
    return True


def _url_valida(url: Any) -> bool:
    partes = urllib.parse.urlparse(_texto(url))
    return partes.scheme in ("http", "https") and bool(partes.netloc)


def _rede_get(url: Any) -> str:
    with urllib.request.urlopen(_texto(url), timeout=10) as resp:
        return resp.read().decode("utf-8", errors="replace")


def _rede_post(url: Any, dados: Any) -> str:
    payload = _texto(dados).encode("utf-8")
    req = urllib.request.Request(_texto(url), data=payload, method="POST")
    with urllib.request.urlopen(req, timeout=10) as resp:
        return resp.read().decode("utf-8", errors="replace")


def _sistema_executar(comando: Any, timeout: Any = 30) -> int:
    texto = _texto(comando).strip()
    if not texto:
        return 0
    try:
        limite = float(timeout)
    except (TypeError, ValueError):
        limite = 30.0

    try:
        if os.name == "nt":
            resultado = subprocess.run(
                texto,
                shell=True,
                capture_output=True,
                text=True,
                timeout=limite,
            )
        else:
            resultado = subprocess.run(
                shlex.split(texto),
                shell=False,
                capture_output=True,
                text=True,
                timeout=limite,
            )
        return int(resultado.returncode)
    except FileNotFoundError:
        return 127
    except subprocess.TimeoutExpired:
        return 124


def _sistema_executar_detalhado(comando: Any, timeout: Any = 30) -> Dict[str, Any]:
    texto = _texto(comando).strip()
    if not texto:
        return {"codigo": 0, "saida": "", "erro": ""}
    try:
        limite = float(timeout)
    except (TypeError, ValueError):
        limite = 30.0

    try:
        if os.name == "nt":
            resultado = subprocess.run(
                texto,
                shell=True,
                capture_output=True,
                text=True,
                timeout=limite,
            )
        else:
            resultado = subprocess.run(
                shlex.split(texto),
                shell=False,
                capture_output=True,
                text=True,
                timeout=limite,
            )
        return {
            "codigo": int(resultado.returncode),
            "saida": resultado.stdout,
            "erro": resultado.stderr,
        }
    except FileNotFoundError as exc:
        return {"codigo": 127, "saida": "", "erro": str(exc)}
    except subprocess.TimeoutExpired as exc:
        return {
            "codigo": 124,
            "saida": exc.stdout or "",
            "erro": exc.stderr or "tempo limite excedido",
        }


_LOG_LEVEL = "info"


def _log_nivel(valor: Any = None) -> str:
    global _LOG_LEVEL
    if valor is not None:
        _LOG_LEVEL = _texto(valor).lower() or "info"
    return _LOG_LEVEL


def _log_escrever(nivel: str, mensagem: Any) -> bool:
    saida = sys.stderr if nivel == "erro" else sys.stdout
    print(f"[{nivel.upper()}] {_texto(mensagem)}", file=saida)
    return True


def _jogo_rebatedor(titulo: Any = "Jogo BR", largura: Any = 800, altura: Any = 520) -> Dict[str, Any]:
    import time
    import random
    import pygame

    largura = int(largura)
    altura = int(altura)
    pygame.init()
    tela = pygame.display.set_mode((largura, altura))
    pygame.display.set_caption(_texto(titulo))
    relogio = pygame.time.Clock()
    fonte = pygame.font.SysFont("Segoe UI", 24)
    fonte_grande = pygame.font.SysFont("Segoe UI", 36, bold=True)

    jogador = pygame.Rect(largura // 2 - 55, altura - 54, 110, 16)
    bola = pygame.Rect(largura // 2 - 9, altura // 2 - 9, 18, 18)
    vel_x = 5
    vel_y = -5
    pontos = 0
    vidas = 3
    inicio = time.time()
    limite_teste = float(os.environ.get("BR_GAME_TEST_SECONDS", "0") or 0)

    blocos = []
    cores_blocos = [(0, 156, 59), (255, 223, 0), (0, 39, 118)]
    for linha in range(4):
        for coluna in range(8):
            blocos.append(
                {
                    "ret": pygame.Rect(48 + coluna * 88, 78 + linha * 34, 72, 22),
                    "cor": cores_blocos[(linha + coluna) % len(cores_blocos)],
                }
            )

    rodando = True
    while rodando:
        for evento in pygame.event.get():
            if evento.type == pygame.QUIT:
                rodando = False
            elif evento.type == pygame.KEYDOWN and evento.key == pygame.K_ESCAPE:
                rodando = False

        teclas = pygame.key.get_pressed()
        if teclas[pygame.K_LEFT] or teclas[pygame.K_a]:
            jogador.x -= 8
        if teclas[pygame.K_RIGHT] or teclas[pygame.K_d]:
            jogador.x += 8
        jogador.x = max(0, min(largura - jogador.width, jogador.x))

        bola.x += vel_x
        bola.y += vel_y

        if bola.left <= 0 or bola.right >= largura:
            vel_x *= -1
        if bola.top <= 0:
            vel_y *= -1
        if bola.colliderect(jogador) and vel_y > 0:
            vel_y *= -1
            deslocamento = (bola.centerx - jogador.centerx) / max(1, jogador.width / 2)
            vel_x = max(-8, min(8, int(deslocamento * 7)))

        atingido = None
        for bloco in blocos:
            if bola.colliderect(bloco["ret"]):
                atingido = bloco
                vel_y *= -1
                pontos += 10
                break
        if atingido:
            blocos.remove(atingido)

        if bola.top > altura:
            vidas -= 1
            bola.center = (largura // 2, altura // 2)
            vel_x = random.choice([-5, 5])
            vel_y = -5
            if vidas <= 0:
                blocos.clear()

        tela.fill((10, 18, 32))
        pygame.draw.rect(tela, (255, 223, 0), jogador, border_radius=8)
        pygame.draw.ellipse(tela, (230, 240, 255), bola)
        for bloco in blocos:
            pygame.draw.rect(tela, bloco["cor"], bloco["ret"], border_radius=5)

        titulo_txt = fonte_grande.render("BR Rebatedor", True, (255, 255, 255))
        placar_txt = fonte.render(f"Pontos: {pontos}   Vidas: {vidas}", True, (230, 240, 255))
        ajuda_txt = fonte.render("Setas/A-D movem | ESC sai", True, (160, 180, 210))
        tela.blit(titulo_txt, (24, 18))
        tela.blit(placar_txt, (24, altura - 32))
        tela.blit(ajuda_txt, (largura - ajuda_txt.get_width() - 24, altura - 32))

        if not blocos:
            mensagem = "Voce venceu!" if vidas > 0 else "Fim de jogo"
            msg = fonte_grande.render(mensagem, True, (255, 255, 255))
            tela.blit(msg, (largura // 2 - msg.get_width() // 2, altura // 2 - 28))

        pygame.display.flip()
        relogio.tick(60)

        if limite_teste > 0 and time.time() - inicio >= limite_teste:
            rodando = False

    pygame.quit()
    return {"pontos": pontos, "vidas": vidas}


MODULOS_NATIVOS: Dict[str, Dict[str, Callable[..., Any]]] = {
    "matematica": {
        "somar": lambda a, b: a + b,
        "subtrair": lambda a, b: a - b,
        "multiplicar": lambda a, b: a * b,
        "dividir": lambda a, b: a / b,
        "raiz_quadrada": lambda x: math.sqrt(x),
        "potencia": lambda a, b: a ** b,
        "arredondar": lambda x: round(x),
        "absoluto": lambda x: abs(x),
        "maximo": lambda a, b: max(a, b),
        "minimo": lambda a, b: min(a, b),
        "pi": lambda: math.pi,
        "aleatorio": lambda a=None, b=None: random.randint(a or 1, b or 100) if a is not None and b is not None else (random.random() if a is None else random.randint(0, int(a))),
    },
    "texto": {
        "maiusculo": lambda s: _texto(s).upper(),
        "minusculo": lambda s: _texto(s).lower(),
        "tamanho": lambda s: len(_texto(s)),
        "vazio": lambda valor: _texto(valor) == "",
        "contem": lambda s, trecho: _texto(trecho) in _texto(s),
        "substituir": lambda s, antigo, novo: _texto(s).replace(_texto(antigo), _texto(novo)),
        "dividir": lambda s, sep=" ": _texto(s).split(_texto(sep)),
        "juntar": lambda itens, sep="": _texto(sep).join([_texto(i) for i in itens]),
        "slug": _slug,
        "formatar": lambda modelo, valor: _texto(modelo).replace("{}", _texto(valor)),
    },
    "colecoes": {
        "tamanho": _colecao_tamanho,
        "vazia": _colecao_vazia,
        "primeiro": _colecao_primeiro,
        "ordenar": _colecao_ordenar,
    },
    "lista": {
        "tamanho": _colecao_tamanho,
        "adicionar": lambda xs, item: (xs.append(item) or xs) if isinstance(xs, list) else xs,
        "primeiro": _colecao_primeiro,
        "ultimo": lambda xs: xs[-1] if isinstance(xs, list) and xs else None,
        "vazia": _colecao_vazia,
    },
    "arquivo": {
        "ler": lambda caminho: ler_texto(caminho),
        "escrever": _escrever_arquivo,
        "anexar": _arquivo_anexar,
        "linhas": _arquivo_linhas,
        "existe": lambda caminho: _caminho(caminho).exists(),
        "arquivo": lambda caminho: _caminho(caminho).is_file(),
        "diretorio_existe": lambda caminho: _caminho(caminho).is_dir(),
        "criar_diretorio": _arquivo_criar_diretorio,
        "listar": _arquivo_listar,
        "remover": _arquivo_remover,
        "apagar": _arquivo_remover,
        "copiar": _arquivo_copiar,
        "mover": _arquivo_mover,
        "tamanho": lambda caminho: _caminho(caminho).stat().st_size,
        "nome_arquivo": lambda caminho: _caminho(caminho).name,
        "absoluto": lambda caminho: str(_caminho(caminho).resolve()),
        "extensao": _path_extensao,
        "diretorio": _path_diretorio,
    },
    "banco": {
        "conectar": _banco_conectar,
        "consulta": _banco_consulta,
        "transacao": _banco_transacao,
        "migrar": lambda: True,
    },
    "rede": {
        "get": _rede_get,
        "post": _rede_post,
        "baixar": lambda url, caminho: _escrever_arquivo(caminho, _rede_get(url)) > 0,
        "url_valida": _url_valida,
    },
    "json": {
        "validar": _json_validar,
        "gerar": _json_gerar,
        "parsear": _json_parsear,
        "bonito": _json_bonito,
        "texto": _json_gerar,
    },
    "csv": {
        "ler": _csv_rows,
        "escrever": _csv_escrever,
        "cabecalho": _csv_cabecalho,
        "linhas": _csv_linhas,
    },
    "path": {
        "juntar": _path_juntar,
        "diretorio": _path_diretorio,
        "extensao": _path_extensao,
        "normalizar": _path_normalizar,
    },
    "regex": {
        "buscar": _regex_buscar,
        "todos": _regex_todos,
        "contar": _regex_contar,
        "substituir": lambda padrao_texto, novo, valor: re.sub(_texto(padrao_texto), _texto(novo), _texto(valor)),
        "validar": lambda padrao_texto, valor: bool(re.fullmatch(_texto(padrao_texto), _texto(valor))),
        "grupos": _regex_grupos,
        "dividir": lambda padrao_texto, valor: re.split(_texto(padrao_texto), _texto(valor)),
        "escapar": lambda valor: re.escape(_texto(valor)),
    },
    "uuid": {
        "gerar": lambda: str(uuid_module.uuid4()),
        "validar": _uuid_validar,
        "v4": lambda: str(uuid_module.uuid4()),
        "curto": lambda: uuid_module.uuid4().hex[:8],
    },
    "data_hora": {
        "agora": lambda: _datetime.datetime.now().replace(microsecond=0).isoformat(),
        "hoje": lambda: _datetime.date.today().isoformat(),
        "diferenca": _data_diferenca,
        "fuso": lambda: _datetime.datetime.now().astimezone().tzname() or "local",
    },
    "validacao": {
        "cpf": _cpf_valido,
        "cnpj": _cnpj_valido,
        "email": _email_valido,
        "cep": _cep_valido,
    },
    "seguranca": {
        "hash": _hash_sha256,
        "base64": _base64,
        "permitir": lambda permissao: True,
        "jwt_validar": lambda token: len(_texto(token).split(".")) == 3,
    },
    "senha": {
        "gerar": lambda: secrets.token_urlsafe(16),
        "validar": lambda valor: _senha_forca(valor) >= 3,
        "forca": _senha_forca,
        "hash": _hash_sha256,
    },
    "token": {
        "gerar": lambda: secrets.token_hex(16),
        "validar": lambda valor: len(_texto(valor)) >= 16,
        "expirar": lambda valor: True,
        "renovar": lambda valor: secrets.token_hex(16),
    },
    "sistema": {
        "plataforma": lambda: platform.system().lower(),
        "argumentos": lambda: json.dumps(sys.argv[1:], ensure_ascii=False),
        "ambiente": lambda chave: os.environ.get(_texto(chave), ""),
        "executar": _sistema_executar,
        "executar_detalhado": _sistema_executar_detalhado,
        "ler_entrada": lambda prompt="": input(_texto(prompt) if prompt else "").strip(),
    },
    "env": {
        "ler": lambda chave: os.environ.get(_texto(chave), ""),
        "definir": lambda chave, valor: os.environ.__setitem__(_texto(chave), _texto(valor)) or True,
        "remover": lambda chave: os.environ.pop(_texto(chave), None) is not None,
        "listar": lambda: json.dumps(dict(os.environ), ensure_ascii=False),
    },
    "config": {
        "ini": lambda dados: _texto(dados),
        "env": lambda dados: _texto(dados),
        "yaml": lambda dados: _texto(dados),
        "toml": lambda dados: _texto(dados),
    },
    "introspeccao": {
        "tipo": lambda valor: type(valor).__name__,
        "funcoes": lambda valor: ",".join(sorted(valor.keys())) if isinstance(valor, dict) else "",
        "campos": lambda valor: ",".join(sorted(valor.keys())) if isinstance(valor, dict) else "",
        "modulo": lambda valor: valor.get("nome", "") if isinstance(valor, dict) else _texto(valor),
    },
    "log": {
        "info": lambda mensagem: _log_escrever("info", mensagem),
        "aviso": lambda mensagem: _log_escrever("aviso", mensagem),
        "erro": lambda mensagem: _log_escrever("erro", mensagem),
        "nivel": _log_nivel,
    },
    "std": {
        "tipo": lambda valor: type(valor).__name__,
        "texto": lambda valor: _texto(valor),
        "inteiro": lambda valor: int(valor),
        "decimal": lambda valor: float(valor),
    },
    "jogos": {
        "rebatedor": _jogo_rebatedor,
    },
}

for _nome_modulo, _funcoes_modulo in MODULOS_NATIVOS.items():
    _funcoes_modulo.setdefault("nome", lambda _nome=_nome_modulo: _nome)
    _funcoes_modulo.setdefault("descricao", lambda _nome=_nome_modulo: f"Modulo nativo BR: {_nome}")
    _funcoes_modulo.setdefault("versao", lambda: "1.0.0")


def normalizar_nome_modulo(nome: str) -> str:
    nome = nome.strip().strip('"').strip("'")
    if nome.endswith(".br"):
        nome = nome[:-3]
    return nome.replace("\\", "/")


def raizes_instalacao() -> Iterable[Path]:
    env_home = os.environ.get("BR_HOME")
    if env_home:
        yield Path(env_home)

    executavel = Path(sys.executable).resolve().parent
    yield executavel
    yield executavel.parent

    fonte = Path(__file__).resolve().parent.parent
    yield fonte

    if os.name == "nt":
        yield Path(r"D:\br")


def caminhos_instalacao() -> Iterable[Path]:
    vistos = set()
    for raiz in raizes_instalacao():
        for base in (raiz / "lib", raiz / "modulos"):
            chave = str(base).lower()
            if chave not in vistos:
                vistos.add(chave)
                yield base

    if os.name == "nt":
        program_files = os.environ.get("ProgramFiles", r"C:\Program Files")
        yield Path(program_files) / "BRlang" / "lib"
        yield Path(program_files) / "BRlang" / "modulos"
    else:
        yield Path("/usr/local/brlang/lib")
        yield Path("/usr/local/brlang/modulos")


def caminhos_usuario() -> Iterable[Path]:
    base = Path.home() / ".br"
    yield base / "modulos"
    yield base / "lib"


def separar_br_path() -> Iterable[Path]:
    valor = os.environ.get("BR_PATH", "")
    if not valor:
        return []
    return [Path(p) for p in valor.split(os.pathsep) if p.strip()]


def candidatos_modulo(nome: str, diretorio_base: Optional[str] = None) -> Iterable[Path]:
    nome = normalizar_nome_modulo(nome)
    caminho_nome = Path(nome)
    nomes = [caminho_nome]
    if caminho_nome.suffix not in (".br", ".brc"):
        nomes.append(Path(nome + ".br"))
        nomes.append(Path(nome + ".brc"))
        nomes.append(caminho_nome / (caminho_nome.name + ".br"))
        nomes.append(caminho_nome / (caminho_nome.name + ".brc"))
        nomes.append(caminho_nome / "principal.br")
        nomes.append(caminho_nome / "principal.brc")
        nomes.append(caminho_nome / "modulo.br")
        nomes.append(caminho_nome / "modulo.brc")
        nomes.append(caminho_nome / "bin" / "principal.brc")
        nomes.append(caminho_nome / "bin" / f"{caminho_nome.name}.brc")

    if caminho_nome.is_absolute():
        for candidato in nomes:
            yield candidato
        return

    bases = []
    if diretorio_base:
        base = Path(diretorio_base)
        bases.append(base)
        bases.append(base / "modulos")
        if base.name in ("testes", "exemplos"):
            bases.append(base.parent)
            bases.append(base.parent / "modulos")
    bases.extend([Path.cwd(), Path.cwd() / "modulos"])
    bases.extend(caminhos_usuario())
    bases.extend(caminhos_instalacao())
    bases.extend(separar_br_path())

    for base in bases:
        for candidato in nomes:
            yield base / candidato


def resolver_arquivo_modulo(nome: str, diretorio_base: Optional[str] = None) -> Optional[Path]:
    for candidato in candidatos_modulo(nome, diretorio_base):
        if candidato.exists() and candidato.is_file():
            return candidato.resolve()
    return None


def modulo_nativo(nome: str) -> Optional[Dict[str, Callable[..., Any]]]:
    return MODULOS_NATIVOS.get(normalizar_nome_modulo(nome))


def criar_handle_modulo(nome: str, caminho: Optional[str] = None) -> Dict[str, Any]:
    nome_normalizado = normalizar_nome_modulo(nome)
    return {
        "__tipo_br__": "modulo",
        "nome": nome_normalizado,
        "caminho": caminho,
    }


def criar_handle_funcao(modulo: str, simbolo: str) -> Dict[str, Any]:
    return {
        "__tipo_br__": "funcao_modulo",
        "modulo": normalizar_nome_modulo(modulo),
        "simbolo": simbolo,
    }


def obter_funcao_nativa(modulo: str, simbolo: str) -> Callable[..., Any]:
    funcs = modulo_nativo(modulo)
    if not funcs or simbolo not in funcs:
        raise ErroModuloBR(f"Modulo '{modulo}' nao exporta '{simbolo}'")
    return funcs[simbolo]


def chamar_funcao_modulo(modulo: str, simbolo: str, args: list) -> Any:
    func = obter_funcao_nativa(modulo, simbolo)
    try:
        return func(*args)
    except TypeError as exc:
        raise ErroModuloBR(
            f"Função nativa '{modulo}.{simbolo}' recebeu argumentos incompatíveis: {exc}"
        ) from exc
