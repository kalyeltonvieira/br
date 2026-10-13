# -*- coding: utf-8 -*-
"""
Sistema de Erros em Português para Linguagem BR
Fornece mensagens de erro amigáveis, didáticas e com sugestões de correção
"""
from typing import List, Optional, Tuple
from dataclasses import dataclass

@dataclass
class ContextoErro:
    linha: int
    coluna: int
    arquivo: str
    codigo: str
    linha_codigo: str

class SistemaErrosBR:
    def __init__(self):
        self.erros: List[str] = []
        self.avisos: List[str] = []
    
    def formatar_erro_sintatico(self, contexto: ContextoErro, tipo: str, detalhes: str) -> str:
        """Formata erro sintático com contexto e sugestão"""
        mensagem = f"""
ERRO SINTÁTICO [Linha {contexto.linha}, Coluna {contexto.coluna}] no arquivo '{contexto.arquivo}'
-> {detalhes}

Código analisado:
{contexto.linha - 1} | {self._obter_linha(contexto.codigo, contexto.linha - 2) if contexto.linha > 1 else ""}
{contexto.linha} | {contexto.linha_codigo}
{contexto.linha + 1} | {self._obter_linha(contexto.codigo, contexto.linha) if self._obter_linha(contexto.codigo, contexto.linha) else ""}
"""
        
        # Adicionar sugestão específica
        sugestao = self._obter_sugestao_sintatica(tipo, detalhes, contexto)
        if sugestao:
            mensagem += f"\nO que falta:\n{sugestao}\n"
        
        return mensagem
    
    def formatar_erro_semantico(self, contexto: ContextoErro, tipo: str, detalhes: str) -> str:
        """Formata erro semântico com contexto e sugestão"""
        mensagem = f"""
ERRO SEMÂNTICO [Linha {contexto.linha}, Coluna {contexto.coluna}] no arquivo '{contexto.arquivo}'
-> {detalhes}

Código analisado:
{contexto.linha} | {contexto.linha_codigo}
"""
        
        # Adicionar sugestão específica
        sugestao = self._obter_sugestao_semantica(tipo, detalhes, contexto)
        if sugestao:
            mensagem += f"\nComo corrigir:\n{sugestao}\n"
        
        return mensagem
    
    def formatar_erro_runtime(self, contexto: ContextoErro, tipo: str, detalhes: str) -> str:
        """Formata erro de runtime com contexto e sugestão"""
        mensagem = f"""
ERRO DE EXECUÇÃO [Linha {contexto.linha}, Coluna {contexto.coluna}] no arquivo '{contexto.arquivo}'
-> {detalhes}

Código analisado:
{contexto.linha} | {contexto.linha_codigo}
"""
        
        # Adicionar sugestão específica
        sugestao = self._obter_sugestao_runtime(tipo, detalhes, contexto)
        if sugestao:
            mensagem += f"\nComo corrigir:\n{sugestao}\n"
        
        return mensagem
    
    def formatar_aviso(self, contexto: ContextoErro, detalhes: str) -> str:
        """Formata aviso com contexto"""
        return f"""
AVISO [Linha {contexto.linha}, Coluna {contexto.coluna}] no arquivo '{contexto.arquivo}'
-> {detalhes}

Código analisado:
{contexto.linha} | {contexto.linha_codigo}
"""
    
    def _obter_linha(self, codigo: str, numero: int) -> str:
        """Obtém uma linha específica do código"""
        linhas = codigo.splitlines()
        if 0 <= numero < len(linhas):
            return linhas[numero]
        return ""
    
    def _obter_sugestao_sintatica(self, tipo: str, detalhes: str, contexto: ContextoErro) -> Optional[str]:
        """Fornece sugestão para erro sintático"""
        detalhes_lower = detalhes.lower()
        
        if "fim" in detalhes_lower and "fechar" in detalhes_lower:
            return "Adicione a palavra reservada 'fim' na linha apropriada para fechar o bloco."
        
        if "então" in detalhes_lower:
            return "Adicione a palavra reservada 'então' após a condição do bloco 'se'."
        
        if "esperava" in detalhes_lower and "indentação" in detalhes_lower:
            return "Verifique a indentação do código. Use 4 espaços por nível de indentação."
        
        if "aspas" in detalhes_lower:
            return "Certifique-se de que todas as strings estão entre aspas duplas (\") ou simples (')."
        
        if "parênteses" in detalhes_lower:
            return "Verifique se todos os parênteses abertos foram fechados corretamente."
        
        if "dois pontos" in detalhes_lower:
            return "Adicione ':' após o nome da propriedade em registros."
        
        if "recebendo" in detalhes_lower:
            return "Use a palavra 'recebendo' para declarar parâmetros de função, seguida pelos nomes separados por 'e'."
        
        return None
    
    def _obter_sugestao_semantica(self, tipo: str, detalhes: str, contexto: ContextoErro) -> Optional[str]:
        """Fornece sugestão para erro semântico"""
        detalhes_lower = detalhes.lower()
        
        if "variável" in detalhes_lower and "não declarada" in detalhes_lower:
            nome_var = detalhes.split("'")[1] if "'" in detalhes else "a variável"
            return f"Declare a variável '{nome_var}' antes de usá-la, ou verifique se o nome está escrito corretamente."
        
        if "variável" in detalhes_lower and "já declarada" in detalhes_lower:
            return "Use um nome diferente para a variável, ou remova a declaração duplicada."
        
        if "tipo" in detalhes_lower and "incompatível" in detalhes_lower:
            return "Verifique se os tipos são compatíveis. Use conversões como 'para_inteiro()' ou 'para_decimal()' se necessário."
        
        if "função" in detalhes_lower and "não declarada" in detalhes_lower:
            return "Declare a função antes de usá-la, ou verifique se o nome está escrito corretamente."
        
        if "constante" in detalhes_lower and "reatribuição" in detalhes_lower:
            return "Constantes não podem ter seu valor alterado. Remova a palavra 'constante' se precisar modificar o valor."
        
        if "propriedade" in detalhes_lower and "não-registro" in detalhes_lower:
            return "Você só pode acessar propriedades em registros. Verifique se o objeto é do tipo correto."
        
        if "comparação" in detalhes_lower and "incompatíveis" in detalhes_lower:
            return "Você só pode comparar valores do mesmo tipo (exceto números, que podem ser comparados entre si)."
        
        if "não usada" in detalhes_lower:
            return "Remova a variável se não for necessária, ou use o prefixo '_' para indicar que é intencionalmente não usada."
        
        return None
    
    def _obter_sugestao_runtime(self, tipo: str, detalhes: str, contexto: ContextoErro) -> Optional[str]:
        """Fornece sugestão para erro de runtime"""
        detalhes_lower = detalhes.lower()
        
        if "divisão" in detalhes_lower and "zero" in detalhes_lower:
            return "Verifique se o divisor não é zero antes de realizar a divisão."
        
        if "índice" in detalhes_lower and "fora" in detalhes_lower:
            return "Verifique se o índice está dentro dos limites da lista. Use 'tamanho_lista()' para verificar o tamanho."
        
        if "chave" in detalhes_lower and "não existe" in detalhes_lower:
            return "Verifique se a chave existe no registro antes de acessá-la, ou use um valor padrão."
        
        if "arquivo" in detalhes_lower and "não encontrado" in detalhes_lower:
            return "Verifique se o caminho do arquivo está correto e se o arquivo existe."
        
        if "permissão" in detalhes_lower:
            return "Adicione a permissão necessária no arquivo 'projeto.br' na seção 'permissoes'."
        
        if "argumentos" in detalhes_lower and "incorretos" in detalhes_lower:
            return "Verifique a quantidade e tipos de argumentos passados para a função."
        
        if "recursão" in detalhes_lower and "muita" in detalhes_lower:
            return "A função está chamando a si mesma muitas vezes. Adicione uma condição de parada ou use iteração em vez de recursão."
        
        return None
    
    def relatorio_resumido(self) -> str:
        """Gera um relatório resumido de erros e avisos"""
        total_erros = len(self.erros)
        total_avisos = len(self.avisos)
        
        if total_erros == 0 and total_avisos == 0:
            return "✓ Compilação concluída com sucesso! Nenhum erro ou aviso encontrado."
        
        relatorio = f"""
Relatório de Compilação:
- Erros: {total_erros}
- Avisos: {total_avisos}
"""
        
        if total_avisos > 0:
            relatorio += "\nAvisos:\n"
            for aviso in self.avisos:
                relatorio += f"  • {aviso}\n"
        
        return relatorio

# Funções auxiliares para uso em outros módulos

def criar_contexto_erro(codigo: str, linha: int, coluna: int, arquivo: str = "desconhecido") -> ContextoErro:
    """Cria um contexto de erro a partir de informações básicas"""
    linhas = codigo.splitlines()
    linha_codigo = linhas[linha - 1] if 0 < linha <= len(linhas) else ""
    
    return ContextoErro(
        linha=linha,
        coluna=coluna,
        arquivo=arquivo,
        codigo=codigo,
        linha_codigo=linha_codigo
    )

def reportar_erro_sintatico(sistema: SistemaErrosBR, codigo: str, linha: int, coluna: int, arquivo: str, tipo: str, detalhes: str):
    """Reporta um erro sintático ao sistema"""
    contexto = criar_contexto_erro(codigo, linha, coluna, arquivo)
    mensagem = sistema.formatar_erro_sintatico(contexto, tipo, detalhes)
    sistema.erros.append(mensagem)
    print(mensagem)

def reportar_erro_semantico(sistema: SistemaErrosBR, codigo: str, linha: int, coluna: int, arquivo: str, tipo: str, detalhes: str):
    """Reporta um erro semântico ao sistema"""
    contexto = criar_contexto_erro(codigo, linha, coluna, arquivo)
    mensagem = sistema.formatar_erro_semantico(contexto, tipo, detalhes)
    sistema.erros.append(mensagem)
    print(mensagem)

def reportar_aviso(sistema: SistemaErrosBR, codigo: str, linha: int, coluna: int, arquivo: str, detalhes: str):
    """Reporta um aviso ao sistema"""
    contexto = criar_contexto_erro(codigo, linha, coluna, arquivo)
    mensagem = sistema.formatar_aviso(contexto, detalhes)
    sistema.avisos.append(mensagem)
    print(mensagem)
