# -*- coding: utf-8 -*-
"""
Utilidades de texto para a toolchain BR.

Arquivos BR oficiais devem ser UTF-8, mas versões antigas no Windows podem ter
sido salvas como ANSI/Windows-1252. A leitura tolerante preserva compatibilidade
sem mascarar a escrita oficial em UTF-8.
"""
from pathlib import Path
import sys


TEXT_ENCODINGS = ("utf-8-sig", "utf-8", "cp1252", "latin-1")


def ler_texto(caminho):
    path = Path(caminho)
    ultimo_erro = None
    for encoding in TEXT_ENCODINGS:
        try:
            return path.read_text(encoding=encoding)
        except UnicodeDecodeError as exc:
            ultimo_erro = exc
    raise ultimo_erro


def escrever_texto(caminho, conteudo):
    Path(caminho).write_text(str(conteudo), encoding="utf-8", newline="\n")


def configurar_stdio():
    for stream_name in ("stdout", "stderr"):
        stream = getattr(sys, stream_name, None)
        reconfigure = getattr(stream, "reconfigure", None)
        if reconfigure is None:
            continue
        try:
            reconfigure(encoding="utf-8", errors="replace", line_buffering=True)
        except (OSError, ValueError):
            try:
                reconfigure(line_buffering=True)
            except (OSError, ValueError):
                pass
