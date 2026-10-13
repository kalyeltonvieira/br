# -*- coding: utf-8 -*-
"""
Autenticacao local para ferramentas BR.
"""
import hashlib
import hmac
import json
import os
import secrets
import time
from pathlib import Path


AUTH_DIR = Path.home() / ".br" / "auth"
USUARIOS = AUTH_DIR / "usuarios.json"
SESSOES = AUTH_DIR / "sessoes.json"


def _carregar(caminho: Path):
    if not caminho.exists():
        return {}
    return json.loads(caminho.read_text(encoding="utf-8"))


def _salvar(caminho: Path, dados):
    AUTH_DIR.mkdir(parents=True, exist_ok=True)
    caminho.write_text(json.dumps(dados, ensure_ascii=False, indent=2), encoding="utf-8")


def hash_senha(senha: str, salt: str) -> str:
    return hashlib.pbkdf2_hmac("sha256", senha.encode(), salt.encode(), 100_000).hex()


def registrar_usuario(usuario: str, senha: str):
    usuarios = _carregar(USUARIOS)
    if usuario in usuarios:
        raise ValueError("Usuario ja existe")
    salt = secrets.token_hex(16)
    usuarios[usuario] = {"salt": salt, "hash": hash_senha(senha, salt)}
    _salvar(USUARIOS, usuarios)


def autenticar(usuario: str, senha: str) -> str:
    usuarios = _carregar(USUARIOS)
    dados = usuarios.get(usuario)
    if not dados:
        raise PermissionError("Usuario ou senha invalidos")
    esperado = dados["hash"]
    recebido = hash_senha(senha, dados["salt"])
    if not hmac.compare_digest(esperado, recebido):
        raise PermissionError("Usuario ou senha invalidos")

    token = secrets.token_urlsafe(32)
    sessoes = _carregar(SESSOES)
    sessoes[token] = {"usuario": usuario, "criado_em": time.time()}
    _salvar(SESSOES, sessoes)
    return token


def validar_token(token: str) -> bool:
    return token in _carregar(SESSOES)
