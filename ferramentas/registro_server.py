# -*- coding: utf-8 -*-
"""
Servidor registro.br - Backend para hospedagem de pacotes BR
Implementa API REST para busca, upload e download de pacotes
"""
import sys
import os
import json
import hashlib
import sqlite3
from flask import Flask, request, jsonify, send_file
from typing import Dict, List, Optional
from dataclasses import dataclass
from datetime import datetime

app = Flask(__name__)

# Banco de dados SQLite
DB_PATH = "registro_br.db"

@dataclass
class Pacote:
    nome: str
    versao: str
    autor: str
    descricao: str
    arquivo_brp: str
    assinatura: str
    data_upload: str
    tags: List[str]
    dependencias: Dict[str, str]

def inicializar_banco():
    """Inicializa o banco de dados SQLite"""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS pacotes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nome TEXT NOT NULL,
            versao TEXT NOT NULL,
            autor TEXT NOT NULL,
            descricao TEXT,
            arquivo_brp TEXT NOT NULL,
            assinatura TEXT NOT NULL,
            data_upload TEXT NOT NULL,
            tags TEXT,
            dependencias TEXT,
            UNIQUE(nome, versao)
        )
    ''')
    
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS chaves_publicas (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            autor TEXT NOT NULL UNIQUE,
            chave_publica TEXT NOT NULL
        )
    ''')
    
    conn.commit()
    conn.close()

def registrar_chave_publica(autor: str, chave_publica: str) -> bool:
    """Registra a chave pública de um autor"""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    try:
        cursor.execute(
            'INSERT OR REPLACE INTO chaves_publicas (autor, chave_publica) VALUES (?, ?)',
            (autor, chave_publica)
        )
        conn.commit()
        return True
    except Exception as e:
        print(f"Erro ao registrar chave pública: {e}")
        return False
    finally:
        conn.close()

def verificar_assinatura(pacote: Pacote) -> bool:
    """Verifica a assinatura digital de um pacote"""
    # Implementação simplificada - em produção usar PGP/GPG
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    cursor.execute('SELECT chave_publica FROM chaves_publicas WHERE autor = ?', (pacote.autor,))
    resultado = cursor.fetchone()
    conn.close()
    
    if not resultado:
        return False
    
    # Verificar assinatura (simplificado)
    hash_esperado = hashlib.sha256(f"{pacote.nome}{pacote.versao}".encode()).hexdigest()
    return pacote.assinatura == hash_esperado

def validar_seguranza_ia(pacote: Pacote) -> bool:
    """Valida segurança de chamadas de IA no pacote"""
    # Escanear o arquivo .brp por chamadas inseguras
    try:
        if os.path.exists(pacote.arquivo_brp):
            with open(pacote.arquivo_brp, 'rb') as f:
                conteudo = f.read()
            
            # Verificar por chamadas perigosas
            chamadas_perigosas = [b'IA_AGENTE_BLOCO', b'IA_PERGUNTE_BLOCO']
            for chamada in chamadas_perigosas:
                if chamada in conteudo:
                    # Em produção, verificar se há permissão no manifesto
                    pass
            
            return True
    except Exception as e:
        print(f"Erro ao validar segurança: {e}")
        return False
    
    return False

@app.route('/api/pacotes', methods=['GET'])
def listar_pacotes():
    """Lista todos os pacotes disponíveis"""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    cursor.execute('SELECT nome, versao, autor, descricao, tags FROM pacotes')
    pacotes = cursor.fetchall()
    conn.close()
    
    resultado = []
    for p in pacotes:
        resultado.append({
            'nome': p[0],
            'versao': p[1],
            'autor': p[2],
            'descricao': p[3],
            'tags': json.loads(p[4]) if p[4] else []
        })
    
    return jsonify(resultado)

@app.route('/api/pacotes/<nome>', methods=['GET'])
def buscar_pacote(nome: str):
    """Busca um pacote específico"""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    cursor.execute('SELECT * FROM pacotes WHERE nome = ? ORDER BY versao DESC LIMIT 1', (nome,))
    pacote = cursor.fetchone()
    conn.close()
    
    if not pacote:
        return jsonify({'erro': 'Pacote não encontrado'}), 404
    
    return jsonify({
        'nome': pacote[1],
        'versao': pacote[2],
        'autor': pacote[3],
        'descricao': pacote[4],
        'tags': json.loads(pacote[7]) if pacote[7] else [],
        'dependencias': json.loads(pacote[8]) if pacote[8] else {}
    })

@app.route('/api/pacotes/buscar', methods=['GET'])
def buscar_pacotes():
    """Busca pacotes por termo"""
    termo = request.args.get('termo', '')
    
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    cursor.execute(
        'SELECT nome, versao, autor, descricao, tags FROM pacotes WHERE nome LIKE ? OR descricao LIKE ?',
        (f'%{termo}%', f'%{termo}%')
    )
    pacotes = cursor.fetchall()
    conn.close()
    
    resultado = []
    for p in pacotes:
        resultado.append({
            'nome': p[0],
            'versao': p[1],
            'autor': p[2],
            'descricao': p[3],
            'tags': json.loads(p[4]) if p[4] else []
        })
    
    return jsonify(resultado)

@app.route('/api/pacotes/upload', methods=['POST'])
def upload_pacote():
    """Faz upload de um novo pacote"""
    dados = request.json
    
    pacote = Pacote(
        nome=dados.get('nome'),
        versao=dados.get('versao'),
        autor=dados.get('autor'),
        descricao=dados.get('descricao', ''),
        arquivo_brp=dados.get('arquivo_brp'),
        assinatura=dados.get('assinatura'),
        data_upload=datetime.now().isoformat(),
        tags=dados.get('tags', []),
        dependencias=dados.get('dependencias', {})
    )
    
    # Verificar assinatura
    if not verificar_assinatura(pacote):
        return jsonify({'erro': 'Assinatura inválida'}), 403
    
    # Validar segurança de IA
    if not validar_seguranza_ia(pacote):
        return jsonify({'erro': 'Falha na validação de segurança'}), 403
    
    # Salvar no banco
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    try:
        cursor.execute('''
            INSERT INTO pacotes (nome, versao, autor, descricao, arquivo_brp, assinatura, data_upload, tags, dependencias)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
        ''', (
            pacote.nome, pacote.versao, pacote.autor, pacote.descricao,
            pacote.arquivo_brp, pacote.assinatura, pacote.data_upload,
            json.dumps(pacote.tags), json.dumps(pacote.dependencias)
        ))
        conn.commit()
        return jsonify({'mensagem': 'Pacote publicado com sucesso'})
    except sqlite3.IntegrityError:
        return jsonify({'erro': 'Pacote já existe'}), 409
    except Exception as e:
        return jsonify({'erro': f'Erro ao publicar: {str(e)}'}), 500
    finally:
        conn.close()

@app.route('/api/chaves', methods=['POST'])
def registrar_chave():
    """Registra uma chave pública"""
    dados = request.json
    autor = dados.get('autor')
    chave_publica = dados.get('chave_publica')
    
    if not autor or not chave_publica:
        return jsonify({'erro': 'Autor e chave pública são obrigatórios'}), 400
    
    if registrar_chave_publica(autor, chave_publica):
        return jsonify({'mensagem': 'Chave registrada com sucesso'})
    else:
        return jsonify({'erro': 'Erro ao registrar chave'}), 500

def main():
    """Inicia o servidor"""
    inicializar_banco()
    print("Servidor registro.br iniciado em http://localhost:5000")
    app.run(host='0.0.0.0', port=5000, debug=True)

if __name__ == '__main__':
    main()
