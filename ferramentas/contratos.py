# -*- coding: utf-8 -*-
"""
Sistema de contratos BR (.bri).
"""
import re
from dataclasses import dataclass
from pathlib import Path
from typing import List
from encoding_utils import ler_texto


@dataclass
class AssinaturaContrato:
    nome: str
    parametros: List[str]


class ErroContratoBR(Exception):
    pass


def ler_contrato(caminho_bri: str) -> List[AssinaturaContrato]:
    texto = ler_texto(caminho_bri)
    assinaturas = []
    padrao = re.compile(r"^\s*fun(?:c|ç)[aã]o\s+(\w+)(?:\s+recebendo\s+(.+))?\s*$", re.IGNORECASE | re.MULTILINE)
    for match in padrao.finditer(texto):
        nome, params_raw = match.groups()
        params = []
        if params_raw:
            params = [p.strip() for p in re.split(r"\s+e\s+|,", params_raw) if p.strip()]
        assinaturas.append(AssinaturaContrato(nome, params))
    return assinaturas


def validar_contrato_fonte(caminho_fonte: str, caminho_bri: str) -> List[str]:
    fonte = ler_texto(caminho_fonte)
    erros = []
    for assinatura in ler_contrato(caminho_bri):
        padrao = re.compile(rf"^\s*fun(?:c|ç)[aã]o\s+{re.escape(assinatura.nome)}(?:\s+recebendo\s+(.+))?\s*$", re.IGNORECASE | re.MULTILINE)
        match = padrao.search(fonte)
        if not match:
            erros.append(f"Contrato exige funcao '{assinatura.nome}', mas ela nao foi implementada")
            continue
        params_raw = match.group(1) or ""
        params_fonte = [p.strip() for p in re.split(r"\s+e\s+|,", params_raw) if p.strip()]
        if len(params_fonte) != len(assinatura.parametros):
            erros.append(
                f"Funcao '{assinatura.nome}' esperava {len(assinatura.parametros)} parametro(s), "
                f"mas implementa {len(params_fonte)}"
            )
    return erros


def contrato_adjacente(caminho_fonte: str):
    caminho = Path(caminho_fonte)
    candidato = caminho.with_suffix(".bri")
    return str(candidato) if candidato.exists() else None
