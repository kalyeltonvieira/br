# -*- coding: utf-8 -*-
"""
Agentes de IA persistentes BR (.bra / runtime).
"""
import json
from pathlib import Path
from typing import Any, Dict


AGENTES_DIR = Path.home() / ".br" / "agentes"


def _caminho_agente(nome: str) -> Path:
    seguro = "".join(ch if ch.isalnum() or ch in ("_", "-") else "_" for ch in nome)
    return AGENTES_DIR / f"{seguro}.json"


def carregar_agente(nome: str) -> Dict[str, Any]:
    caminho = _caminho_agente(nome)
    if not caminho.exists():
        return {}
    return json.loads(caminho.read_text(encoding="utf-8"))


def salvar_agente(nome: str, estado: Dict[str, Any]) -> Path:
    AGENTES_DIR.mkdir(parents=True, exist_ok=True)
    caminho = _caminho_agente(nome)
    caminho.write_text(json.dumps(estado, ensure_ascii=False, indent=2), encoding="utf-8")
    return caminho


def mesclar_estado(nome: str, estado: Dict[str, Any]) -> Dict[str, Any]:
    anterior = carregar_agente(nome)
    anterior.update(estado)
    anterior.setdefault("nome", nome)
    return anterior
