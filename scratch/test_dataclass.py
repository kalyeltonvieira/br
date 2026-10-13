from dataclasses import dataclass, field

@dataclass
class NoAST:
    posicao: int
    tipo_no: str = field(init=False)

@dataclass
class LiteralInteiro(NoAST):
    valor: int
    def __post_init__(self):
        self.tipo_no = "literal_inteiro"

x = LiteralInteiro(10, 20)
print(x)
print(x.tipo_no)
