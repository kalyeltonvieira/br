# -*- coding: utf-8 -*-
"""
ORM (Object-Relational Mapping) para Linguagem BR
Mapeia classes BR para tabelas SQLite
"""
import sqlite3
import json
from typing import List, Dict, Any, Optional, Type

class Campo:
    def __init__(self, nome: str, tipo: str, chave: bool = False, auto: bool = False, nulo: bool = True, padrao: Any = None):
        self.nome = nome
        self.tipo = tipo
        self.chave = chave
        self.auto = auto
        self.nulo = nulo
        self.padrao = padrao
    
    def sql_tipo(self) -> str:
        mapa = {"inteiro": "INTEGER", "decimal": "REAL", "texto": "TEXT", "booleano": "INTEGER"}
        return mapa.get(self.tipo, "TEXT")
    
    def sql_def(self) -> str:
        partes = [self.nome, self.sql_tipo()]
        if self.chave:
            partes.append("PRIMARY KEY")
        if self.auto:
            partes.append("AUTOINCREMENT")
        if not self.nulo:
            partes.append("NOT NULL")
        if self.padrao is not None:
            if isinstance(self.padrao, str):
                partes.append(f"DEFAULT '{self.padrao}'")
            else:
                partes.append(f"DEFAULT {self.padrao}")
        return " ".join(partes)

class ModeloMeta(type):
    def __new__(cls, nome, bases, attrs):
        campos = []
        for attr_name, attr_value in attrs.items():
            if isinstance(attr_value, Campo):
                campos.append(attr_value)
        attrs['_campos'] = campos
        attrs['_tabela'] = attrs.get('_tabela', nome.lower())
        return super().__new__(cls, nome, bases, attrs)

class ModeloBR(metaclass=ModeloMeta):
    _tabela: str = ""
    _campos: List[Campo] = []
    _db: Optional[sqlite3.Connection] = None
    
    def __init__(self, **kwargs):
        for campo in self._campos:
            setattr(self, campo.nome, kwargs.get(campo.nome, campo.padrao))
    
    @classmethod
    def conectar(cls, caminho: str = ":memory:"):
        cls._db = sqlite3.connect(caminho)
        cls._db.row_factory = sqlite3.Row
        cls._criar_tabela()
        return cls._db
    
    @classmethod
    def _criar_tabela(cls):
        if not cls._db:
            return
        campos_sql = [c.sql_def() for c in cls._campos]
        sql = f"CREATE TABLE IF NOT EXISTS {cls._tabela} ({', '.join(campos_sql)})"
        cls._db.execute(sql)
        cls._db.commit()
    
    def salvar(self):
        if not self._db:
            raise Exception("Banco não conectado. Use Modelo.conectar()")
        
        nomes = [c.nome for c in self._campos if not c.auto]
        valores = [getattr(self, c.nome) for c in self._campos if not c.auto]
        placeholders = ['?' for _ in nomes]
        
        sql = f"INSERT OR REPLACE INTO {self._tabela} ({', '.join(nomes)}) VALUES ({', '.join(placeholders)})"
        self._db.execute(sql, valores)
        self._db.commit()
        return self
    
    @classmethod
    def buscar(cls, **kwargs) -> List[Any]:
        if not cls._db:
            raise Exception("Banco não conectado")
        
        if kwargs:
            where = " AND ".join([f"{k} = ?" for k in kwargs.keys()])
            valores = list(kwargs.values())
            sql = f"SELECT * FROM {cls._tabela} WHERE {where}"
            rows = cls._db.execute(sql, valores).fetchall()
        else:
            sql = f"SELECT * FROM {cls._tabela}"
            rows = cls._db.execute(sql).fetchall()
        
        resultados = []
        for row in rows:
            obj = cls(**dict(row))
            resultados.append(obj)
        return resultados
    
    @classmethod
    def buscar_um(cls, **kwargs) -> Optional[Any]:
        resultados = cls.buscar(**kwargs)
        return resultados[0] if resultados else None
    
    @classmethod
    def todos(cls) -> List[Any]:
        return cls.buscar()
    
    def deletar(self):
        if not self._db:
            raise Exception("Banco não conectado")
        
        # Encontra chave primária
        chave = None
        for c in self._campos:
            if c.chave:
                chave = c
                break
        
        if chave:
            valor = getattr(self, chave.nome)
            sql = f"DELETE FROM {self._tabela} WHERE {chave.nome} = ?"
            self._db.execute(sql, (valor,))
            self._db.commit()
    
    @classmethod
    def contar(cls) -> int:
        if not cls._db:
            return 0
        sql = f"SELECT COUNT(*) FROM {cls._tabela}"
        return cls._db.execute(sql).fetchone()[0]
    
    @classmethod
    def executar_sql(cls, sql: str, params: tuple = ()):
        if not cls._db:
            raise Exception("Banco não conectado")
        return cls._db.execute(sql, params)
    
    def to_dict(self) -> Dict[str, Any]:
        return {c.nome: getattr(self, c.nome, None) for c in self._campos}
    
    def to_json(self) -> str:
        return json.dumps(self.to_dict(), ensure_ascii=False, default=str)
    
    def __repr__(self):
        campos_str = ", ".join([f"{c.nome}={getattr(self, c.nome, None)}" for c in self._campos[:3]])
        return f"<{self.__class__.__name__} {campos_str}>"

# ============ Exemplo de uso ============
class Usuario(ModeloBR):
    _tabela = "usuarios"
    id = Campo("id", "inteiro", chave=True, auto=True)
    nome = Campo("nome", "texto", nulo=False)
    email = Campo("email", "texto", nulo=False)
    idade = Campo("idade", "inteiro", nulo=True)
    ativo = Campo("ativo", "booleano", nulo=False, padrao=1)

class Produto(ModeloBR):
    _tabela = "produtos"
    id = Campo("id", "inteiro", chave=True, auto=True)
    nome = Campo("nome", "texto", nulo=False)
    preco = Campo("preco", "decimal", nulo=False)
    quantidade = Campo("quantidade", "inteiro", nulo=False, padrao=0)

def exemplo_orm():
    """Exemplo de uso do ORM"""
    # Conecta ao banco
    Usuario.conectar("exemplo_orm.db")
    Produto.conectar("exemplo_orm.db")
    
    # Cria usuários
    joao = Usuario(nome="João Silva", email="joao@email.com", idade=30)
    joao.salvar()
    
    maria = Usuario(nome="Maria Santos", email="maria@email.com", idade=25)
    maria.salvar()
    
    # Cria produtos
    camisa = Produto(nome="Camisa BR", preco=49.90, quantidade=10)
    camisa.salvar()
    
    caneca = Produto(nome="Caneca BR", preco=29.90, quantidade=20)
    caneca.salvar()
    
    # Buscas
    print("=== Usuários ===")
    for u in Usuario.todos():
        print(f"  {u.id}: {u.nome} ({u.email}) - {u.idade} anos")
    
    print(f"\nTotal: {Usuario.contar()} usuários")
    
    print("\n=== Produtos ===")
    for p in Produto.todos():
        print(f"  {p.id}: {p.nome} - R$ {p.preco:.2f} ({p.quantidade} em estoque)")
    
    # Busca específica
    print("\n=== Busca ===")
    encontrado = Usuario.buscar_um(nome="João Silva")
    if encontrado:
        print(f"  Encontrado: {encontrado.to_json()}")

if __name__ == "__main__":
    exemplo_orm()