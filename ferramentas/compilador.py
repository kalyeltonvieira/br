# -*- coding: utf-8 -*-
import sys
import os
import re
import json
import struct

OPCODE_MAP = {
    "EMPILHAR_CONST": 1,
    "DEFINIR_VAR": 2,
    "OBTER_VAR": 3,
    "OBTER_PROPRIEDADE": 4,
    "SOMAR": 5,
    "SUBTRAIR": 6,
    "MULTIPLICAR": 7,
    "DIVIDIR": 8,
    "IGUAL_A": 9,
    "MAIOR_QUE": 10,
    "RETORNAR": 11,
    "ESPERE_IGUAL": 12,
    "CHAMAR_METODO": 13,
    "CHAMAR_SERVICO": 14,
    "CRIAR_LISTA_BLOCO": 15,
    "LISTA_ELEMENTO": 16,
    "CRIAR_REGISTRO_BLOCO": 17,
    "REGISTRO_PROPRIEDADE": 18,
    "IA_PERGUNTE_BLOCO": 19,
    "IA_AGENTE_BLOCO": 20,
    "TENTE_CAPTURE_BLOCO": 21,
    "PARALELO_EXECUTE": 22,
    "CONDICIONAL_SE": 23,
    "ENQUANTO_BLOCO": 24,
    "MENOR_QUE": 25,
}

def serializar(obj):
    if isinstance(obj, bool):
        return struct.pack('>BB', 4, 1 if obj else 0)
    elif obj is None:
        return struct.pack('>B', 5)
    elif isinstance(obj, int):
        return struct.pack('>Bi', 1, obj)
    elif isinstance(obj, float):
        return struct.pack('>Bd', 2, obj)
    elif isinstance(obj, str):
        utf = obj.encode('utf-8')
        return struct.pack('>BI', 3, len(utf)) + utf
    elif isinstance(obj, list):
        if len(obj) > 0 and isinstance(obj[0], str) and obj[0] in OPCODE_MAP:
            op_id = OPCODE_MAP[obj[0]]
            args = obj[1:]
            res = struct.pack('>BBB', 8, op_id, len(args))
            for arg in args:
                res += serializar(arg)
            return res
        else:
            res = struct.pack('>BI', 6, len(obj))
            for item in obj:
                res += serializar(item)
            return res
    elif isinstance(obj, dict):
        res = struct.pack('>BI', 7, len(obj))
        for k, v in obj.items():
            res += serializar(k)
            res += serializar(v)
        return res
    raise TypeError(f"Tipo não serializável: {type(obj)}")

class CompiladorBR:
    def __init__(self, caminho_fonte):
        self.caminho_fonte = caminho_fonte
        self.constantes = []
        self.instrucoes = []
        self.funcoes = {}
        self.testes = []
        self.documentacoes = []
        
    def adicionar_constante(self, valor):
        for i, val in enumerate(self.constantes):
            if type(val) is type(valor) and val == valor:
                return i
        self.constantes.append(valor)
        return len(self.constantes) - 1

    def compilar(self):
        if not os.path.exists(self.caminho_fonte):
            print(f"Erro: Arquivo '{self.caminho_fonte}' não encontrado.")
            sys.exit(1)
            
        with open(self.caminho_fonte, "r", encoding="utf-8") as f:
            codigo = f.read()
            
        linhas = codigo.splitlines()
        self.instrucoes = self.compilar_bloco(linhas, 0, parent_type=None)[0]

    def compilar_bloco(self, linhas, idx, parent_type=None):
        insts = []
        total_linhas = len(linhas)
        
        while idx < total_linhas:
            linha = linhas[idx].strip()
            if not linha or linha.startswith("#"):
                idx += 1
                continue
                
            if parent_type is not None:
                if linha == "fim":
                    break
                if parent_type == "tente" and linha.startswith("capture "):
                    break
                if parent_type == "se" and linha == "senão":
                    break
            
            idx += 1
            
            if parent_type == "lista":
                self.compilar_expressao(linha, insts)
                insts.append(["LISTA_ELEMENTO", None])
                continue
            
            # Bloco explique (Documentação Viva)
            if linha == "explicacao":
                doc_linhas = []
                while idx < total_linhas and linhas[idx].strip() != "fim":
                    doc_linhas.append(linhas[idx])
                    idx += 1
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                doc_texto = "\n".join(doc_linhas)
                self.documentacoes.append(doc_texto)
                continue
                
            # Bloco objetivo (IA de Compilação)
            if linha == "objetivo":
                obj_linhas = []
                while idx < total_linhas and linhas[idx].strip() != "fim":
                    obj_linhas.append(linhas[idx].strip())
                    idx += 1
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                obj_texto = " ".join(obj_linhas)
                self.processar_objetivo(obj_texto)
                continue
                
            # Bloco teste (Testes Nativos)
            match_teste = re.match(r'^teste\s+"([^"]+)"$', linha)
            if match_teste:
                nome_teste = match_teste.group(1)
                corpo_teste, novo_idx = self.compilar_bloco(linhas, idx, parent_type="teste")
                idx = novo_idx
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                self.testes.append({
                    "nome": nome_teste,
                    "instrucoes": corpo_teste
                })
                continue

            # Bloco função
            match_func = re.match(r'^função\s+(\w+)(?:\s+recebendo\s+(.+))?$', linha)
            if match_func:
                nome_func, params_raw = match_func.groups()
                params = []
                if params_raw:
                    params = [p.strip() for p in params_raw.split(" e ")]
                
                func_insts = []
                for p in reversed(params):
                    func_insts.append(["DEFINIR_VAR", p])
                    
                corpo_func, novo_idx = self.compilar_bloco(linhas, idx, parent_type="funcao")
                idx = novo_idx
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                    
                func_insts.extend(corpo_func)
                self.funcoes[nome_func] = func_insts
                continue

            # Bloco tente-capture
            if linha == "tente":
                corpo_tente, novo_idx = self.compilar_bloco(linhas, idx, parent_type="tente")
                idx = novo_idx
                
                var_erro = "erro"
                corpo_capture = []
                
                if idx < total_linhas and linhas[idx].strip().startswith("capture "):
                    linha_cap = linhas[idx].strip()
                    idx += 1
                    match_cap = re.match(r'^capture\s+(\w+)$', linha_cap)
                    if match_cap:
                        var_erro = match_cap.group(1)
                    corpo_capture, novo_idx = self.compilar_bloco(linhas, idx, parent_type="capture")
                    idx = novo_idx
                    
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                    
                insts.append(["TENTE_CAPTURE_BLOCO", {
                    "tente": corpo_tente,
                    "var_erro": var_erro,
                    "capture": corpo_capture
                }])
                continue

            # Bloco enquanto
            match_enquanto = re.match(r'^enquanto\s+(.+)$', linha)
            if match_enquanto:
                cond = match_enquanto.group(1)
                cond_insts = []
                self.compilar_expressao(cond, cond_insts)
                corpo_enquanto, novo_idx = self.compilar_bloco(linhas, idx, parent_type="enquanto")
                idx = novo_idx
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                insts.append(["ENQUANTO_BLOCO", {
                    "condicao": cond_insts,
                    "corpo": corpo_enquanto
                }])
                continue

            # Bloco para cada em
            match_para = re.match(r'^para\s+cada\s+(\w+)\s+em\s+(.+)$', linha)
            if match_para:
                var_item, var_lista = match_para.groups()
                lista_insts = []
                self.compilar_expressao(var_lista, lista_insts)
                
                corpo_para, novo_idx = self.compilar_bloco(linhas, idx, parent_type="para")
                idx = novo_idx
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                    
                insts.append(["PARA_CADA_BLOCO", {
                    "var_item": var_item,
                    "lista_insts": lista_insts,
                    "corpo": corpo_para
                }])
                continue

            # Bloco execute em paralelo
            if linha == "execute em paralelo":
                blocos_paralelos = []
                while idx < total_linhas and linhas[idx].strip() != "fim":
                    linha_p = linhas[idx].strip()
                    idx += 1
                    if linha_p and not linha_p.startswith("#"):
                        bloco_inst = []
                        self.compilar_linha_instrucao(linha_p, bloco_inst)
                        blocos_paralelos.append(bloco_inst)
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                insts.append(["PARALELO_EXECUTE", blocos_paralelos])
                continue

            # Bloco condicional se-senão
            match_se = re.match(r'^se\s+(.+)\s+então$', linha)
            if match_se:
                cond = match_se.group(1)
                cond_insts = []
                self.compilar_expressao(cond, cond_insts)
                
                corpo_se, novo_idx = self.compilar_bloco(linhas, idx, parent_type="se")
                idx = novo_idx
                
                corpo_senao = []
                if idx < total_linhas and linhas[idx].strip() == "senão":
                    idx += 1
                    corpo_senao, novo_idx = self.compilar_bloco(linhas, idx, parent_type="senao")
                    idx = novo_idx
                    
                if idx < total_linhas and linhas[idx].strip() == "fim":
                    idx += 1
                    
                insts.append(["CONDICIONAL_SE", {
                    "condicao": cond_insts,
                    "se": corpo_se,
                    "senao": corpo_senao
                }])
                continue

            # Propriedades de blocos complexos em registros/agentes
            if parent_type in ("agente", "registro"):
                match_prop_complex = re.match(r'^(\w+):\s*(lista|registro|pergunte para ia|crie agente)$', linha)
                if match_prop_complex:
                    prop_nome, expr = match_prop_complex.groups()
                    if expr == "lista":
                        corpo_lista, novo_idx = self.compilar_bloco(linhas, idx, parent_type="lista")
                        idx = novo_idx
                        if idx < total_linhas and linhas[idx].strip() == "fim":
                            idx += 1
                        insts.append(["CRIAR_LISTA_BLOCO", prop_nome, corpo_lista])
                        insts.append(["OBTER_VAR", prop_nome])
                        insts.append(["REGISTRO_PROPRIEDADE", prop_nome])
                        continue
                    if expr == "registro":
                        corpo_registro, novo_idx = self.compilar_bloco(linhas, idx, parent_type="registro")
                        idx = novo_idx
                        if idx < total_linhas and linhas[idx].strip() == "fim":
                            idx += 1
                        insts.append(["CRIAR_REGISTRO_BLOCO", prop_nome, corpo_registro])
                        insts.append(["OBTER_VAR", prop_nome])
                        insts.append(["REGISTRO_PROPRIEDADE", prop_nome])
                        continue
                    if expr == "pergunte para ia":
                        corpo_ia, novo_idx = self.compilar_bloco(linhas, idx, parent_type="ia")
                        idx = novo_idx
                        if idx < total_linhas and linhas[idx].strip() == "fim":
                            idx += 1
                        insts.append(["IA_PERGUNTE_BLOCO", prop_nome, corpo_ia])
                        insts.append(["OBTER_VAR", prop_nome])
                        insts.append(["REGISTRO_PROPRIEDADE", prop_nome])
                        continue
                    if expr == "crie agente":
                        corpo_agente, novo_idx = self.compilar_bloco(linhas, idx, parent_type="agente")
                        idx = novo_idx
                        if idx < total_linhas and linhas[idx].strip() == "fim":
                            idx += 1
                        insts.append(["IA_AGENTE_BLOCO", prop_nome, corpo_agente])
                        insts.append(["OBTER_VAR", prop_nome])
                        insts.append(["REGISTRO_PROPRIEDADE", prop_nome])
                        continue

            # Declaração de constante
            match_const = re.match(r'^constante\s+(\w+)\s+é\s+(.+)$', linha)
            if match_const:
                const_nome, expr = match_const.groups()
                self.compilar_expressao(expr, insts)
                insts.append(["DEFINIR_VAR", const_nome])
                continue

            # Atribuição de variáveis
            match_atr = re.match(r'^(\w+)(?:\s*:\s*\w+)?\s+é\s+(.+)$', linha)
            if match_atr:
                var_nome, expr = match_atr.groups()
                
                if expr == "lista":
                    corpo_lista, novo_idx = self.compilar_bloco(linhas, idx, parent_type="lista")
                    idx = novo_idx
                    if idx < total_linhas and linhas[idx].strip() == "fim":
                        idx += 1
                    insts.append(["CRIAR_LISTA_BLOCO", var_nome, corpo_lista])
                    continue
                if expr == "registro":
                    corpo_registro, novo_idx = self.compilar_bloco(linhas, idx, parent_type="registro")
                    idx = novo_idx
                    if idx < total_linhas and linhas[idx].strip() == "fim":
                        idx += 1
                    insts.append(["CRIAR_REGISTRO_BLOCO", var_nome, corpo_registro])
                    continue
                if expr == "pergunte para ia":
                    corpo_ia, novo_idx = self.compilar_bloco(linhas, idx, parent_type="ia")
                    idx = novo_idx
                    if idx < total_linhas and linhas[idx].strip() == "fim":
                        idx += 1
                    insts.append(["IA_PERGUNTE_BLOCO", var_nome, corpo_ia])
                    continue
                if expr == "crie agente":
                    corpo_agente, novo_idx = self.compilar_bloco(linhas, idx, parent_type="agente")
                    idx = novo_idx
                    if idx < total_linhas and linhas[idx].strip() == "fim":
                        idx += 1
                    insts.append(["IA_AGENTE_BLOCO", var_nome, corpo_agente])
                    continue

                self.compilar_expressao(expr, insts)
                insts.append(["DEFINIR_VAR", var_nome])
                continue

            self.compilar_linha_instrucao(linha, insts, parent_type=parent_type)
            
        return insts, idx

    def processar_objetivo(self, objetivo):
        print(f"\n[IA de Compilação] Objetivo detectado: '{objetivo}'")
        print(" -> Analisando intenção e arquitetando o sistema...")
        print(" -> Gerando estrutura de pastas: fonte/, telas/, banco/, testes/, documentacao/")
        
        os.makedirs("fonte", exist_ok=True)
        os.makedirs("telas", exist_ok=True)
        os.makedirs("banco", exist_ok=True)
        os.makedirs("testes", exist_ok=True)
        os.makedirs("documentacao", exist_ok=True)
        
        with open("banco/esquema.br", "w", encoding="utf-8") as f:
            f.write("# Esquema do banco gerado automaticamente por IA de Compilação\n")
            f.write("explicacao\nEsquema de banco de dados para " + objetivo + "\nfim\n")
            
        with open("telas/painel.br", "w", encoding="utf-8") as f:
            f.write("# Interface gerada por IA de Compilação\n")
            f.write("crie uma janela\n    titulo \"Painel Gerado\"\n    botao botao_ok \"OK\"\nfim\n")
            
        print(" -> Estrutura gerada com sucesso!")

    def compilar_linha_instrucao(self, linha, insts, parent_type=None):
        if parent_type in ("agente", "registro"):
            match_prop = re.match(r'^(\w+):\s*(.+)$', linha)
            if match_prop:
                prop_nome, expr = match_prop.groups()
                self.compilar_expressao(expr, insts)
                insts.append(["REGISTRO_PROPRIEDADE", prop_nome])
                return
            match_prop_no_colon = re.match(r'^(\w+)\s+(.+)$', linha)
            if match_prop_no_colon:
                prop_nome, expr = match_prop_no_colon.groups()
                self.compilar_expressao(expr, insts)
                insts.append(["REGISTRO_PROPRIEDADE", prop_nome])
                return

        match_espere = re.match(r'^espere\s+(.+)\s+igual\s+a\s+(.+)$', linha)
        if match_espere:
            expr_val, expr_esperado = match_espere.groups()
            self.compilar_expressao(expr_val, insts)
            self.compilar_expressao(expr_esperado, insts)
            insts.append(["ESPERE_IGUAL", None])
            return

        if linha.startswith("devolva "):
            expr = linha[8:].strip()
            self.compilar_expressao(expr, insts)
            insts.append(["RETORNAR", None])
            return

        if linha.startswith("mostre "):
            expr = linha[7:].strip()
            self.compilar_expressao(expr, insts)
            insts.append(["CHAMAR_SERVICO", "SISTEMA", "MOSTRAR", 1])
            return

        if linha.startswith("salve "):
            expr = linha[6:].strip()
            self.compilar_expressao(expr, insts)
            insts.append(["CHAMAR_SERVICO", "BANCO", "SALVAR", 1])
            return

        if linha.startswith("remova "):
            expr = linha[7:].strip()
            self.compilar_expressao(expr, insts)
            insts.append(["CHAMAR_SERVICO", "BANCO", "REMOVER", 1])
            return
            
        match_prop = re.match(r'^(\w+):\s*(.+)$', linha)
        if match_prop:
            prop_nome, expr = match_prop.groups()
            self.compilar_expressao(expr, insts)
            insts.append(["REGISTRO_PROPRIEDADE", prop_nome])
            return

        self.compilar_expressao(linha, insts)

    def compilar_expressao(self, expr, insts):
        expr = expr.strip()
        
        if expr in ("argumentos_cli", "argumentos_cli()"):
            insts.append(["CHAMAR_SERVICO", "SISTEMA", "ARGUMENTOS", 0])
            return

        # Chamada de função sem argumentos: compilar()
        match_call_no_args = re.match(r'^(\w+)\(\)$', expr)
        if match_call_no_args:
            func_nome = match_call_no_args.group(1)
            insts.append(["CHAMAR_METODO", func_nome, 0])
            return

        if expr.startswith('"') and expr.endswith('"'):
            val = expr[1:-1].replace('\\"', '"').replace('\\n', '\n').replace('\\t', '\t')
            c_idx = self.adicionar_constante(val)
            insts.append(["EMPILHAR_CONST", c_idx])
            return
            
        if re.match(r'^\d+$', expr):
            c_idx = self.adicionar_constante(int(expr))
            insts.append(["EMPILHAR_CONST", c_idx])
            return
            
        if re.match(r'^\d+\.\d+$', expr):
            c_idx = self.adicionar_constante(float(expr))
            insts.append(["EMPILHAR_CONST", c_idx])
            return
            
        if expr == "verdadeiro":
            c_idx = self.adicionar_constante(True)
            insts.append(["EMPILHAR_CONST", c_idx])
            return
        if expr == "falso":
            c_idx = self.adicionar_constante(False)
            insts.append(["EMPILHAR_CONST", c_idx])
            return
        if expr == "nulo":
            c_idx = self.adicionar_constante(None)
            insts.append(["EMPILHAR_CONST", c_idx])
            return

        match_op = re.match(r'^(.+?)\s+([\+\-\*\/]|igual a|maior que|menor que|[<>])\s+(.+)$', expr)
        if match_op:
            esq, op, dir_ = match_op.groups()
            self.compilar_expressao(esq, insts)
            self.compilar_expressao(dir_, insts)
            if op == "+":
                insts.append(["SOMAR", None])
            elif op == "-":
                insts.append(["SUBTRAIR", None])
            elif op == "*":
                insts.append(["MULTIPLICAR", None])
            elif op == "/":
                insts.append(["DIVIDIR", None])
            elif op in ("igual a", "=="):
                insts.append(["IGUAL_A", None])
            elif op in ("maior que", ">"):
                insts.append(["MAIOR_QUE", None])
            elif op in ("menor que", "<"):
                insts.append(["MENOR_QUE", None])
            return

        match_prop_access = re.match(r'^(\w+)\.(\w+)$', expr)
        if match_prop_access:
            obj_name, prop_name = match_prop_access.groups()
            insts.append(["OBTER_VAR", obj_name])
            insts.append(["OBTER_PROPRIEDADE", prop_name])
            return

        match_agente_metodo = re.match(r'^(\w+)\.pergunte\s+(.+)$', expr)
        if match_agente_metodo:
            agente_var, prompt_expr = match_agente_metodo.groups()
            insts.append(["OBTER_VAR", agente_var])
            self.compilar_expressao(prompt_expr, insts)
            insts.append(["CHAMAR_SERVICO", "IA", "PERGUNTAR_AGENTE", 2])
            return

        match_call = re.match(r'^(\w+)\s+(.+)$', expr)
        if match_call:
            func_nome, args_raw = match_call.groups()
            
            if func_nome == "busque" and args_raw == "todos os clientes":
                insts.append(["CHAMAR_SERVICO", "BANCO", "BUSQUE_TODOS_CLIENTES", 0])
                return
                
            if func_nome == "acesse":
                self.compilar_expressao(args_raw, insts)
                insts.append(["CHAMAR_SERVICO", "WEB", "ACESSE", 1])
                return
                
            if func_nome == "leia_arquivo":
                self.compilar_expressao(args_raw, insts)
                insts.append(["CHAMAR_SERVICO", "ARQUIVOS", "LER", 1])
                return
                
            if func_nome in ("escreva_arquivo", "escreva_arquivo_binario"):
                args = [a.strip() for a in args_raw.split(" e ") if a.strip()]
                for arg in args:
                    self.compilar_expressao(arg, insts)
                metodo = "ESCREVER" if func_nome == "escreva_arquivo" else "ESCREVER_BINARIO"
                insts.append(["CHAMAR_SERVICO", "ARQUIVOS", metodo, len(args)])
                return

            if func_nome == "divida_texto":
                self.compilar_expressao(args_raw, insts)
                insts.append(["CHAMAR_SERVICO", "TEXTO", "DIVIDIR", 1])
                return

            if func_nome in ("texto_contem", "dividir_por", "texto_contem_inicio", "adicione_item", "obter_item", "tamanho_lista", "definir_propriedade", "obter_propriedade", "tipo_de", "para_inteiro", "para_decimal", "dividir_primeiro", "argumentos_cli"):
                args = []
                if args_raw:
                    args = [a.strip() for a in args_raw.split(" e ") if a.strip()]
                for arg in args:
                    self.compilar_expressao(arg, insts)
                if func_nome == "texto_contem":
                    metodo = "CONTEM"
                    serv = "TEXTO"
                elif func_nome == "dividir_por":
                    metodo = "DIVIDIR_POR"
                    serv = "TEXTO"
                elif func_nome == "texto_contem_inicio":
                    metodo = "CONTEM_INICIO"
                    serv = "TEXTO"
                elif func_nome == "adicione_item":
                    metodo = "ADICIONAR"
                    serv = "LISTA"
                elif func_nome == "obter_item":
                    metodo = "OBTER"
                    serv = "LISTA"
                elif func_nome == "tamanho_lista":
                    metodo = "TAMANHO"
                    serv = "LISTA"
                elif func_nome == "definir_propriedade":
                    metodo = "DEFINIR"
                    serv = "REGISTRO"
                elif func_nome == "obter_propriedade":
                    metodo = "OBTER"
                    serv = "REGISTRO"
                elif func_nome == "tipo_de":
                    metodo = "TIPO"
                    serv = "SISTEMA"
                elif func_nome == "para_inteiro":
                    metodo = "INTEIRO"
                    serv = "SISTEMA"
                elif func_nome == "para_decimal":
                    metodo = "DECIMAL"
                    serv = "SISTEMA"
                elif func_nome == "dividir_primeiro":
                    metodo = "DIVIDIR_PRIMEIRO"
                    serv = "TEXTO"
                elif func_nome == "argumentos_cli":
                    metodo = "ARGUMENTOS"
                    serv = "SISTEMA"
                insts.append(["CHAMAR_SERVICO", serv, metodo, len(args)])
                return

            if func_nome == "texto_substituir":
                args = [a.strip() for a in args_raw.split(" e ") if a.strip()]
                for arg in args:
                    self.compilar_expressao(arg, insts)
                insts.append(["CHAMAR_SERVICO", "TEXTO", "SUBSTITUIR", len(args)])
                return
                
            args = []
            if args_raw.startswith('"') and args_raw.endswith('"'):
                args = [args_raw]
            else:
                args = [a.strip() for a in args_raw.split(" e ") if a.strip()]
                
            for arg in args:
                self.compilar_expressao(arg, insts)
                
            insts.append(["CHAMAR_METODO", func_nome, len(args)])
            return

        insts.append(["OBTER_VAR", expr])

    def salvar_bytecode(self, caminho_saida):
        bytecode = {
            "constantes": self.constantes,
            "instrucoes": self.instrucoes,
            "funcoes": self.funcoes,
            "testes": self.testes,
            "documentacoes": self.documentacoes
        }
        
        serialized = b"BRC1" + serializar(bytecode)
        with open(caminho_saida, "wb") as f:
            f.write(serialized)
            
        print(f" -> Bytecode BR compilado com sucesso (Formato Binário): {caminho_saida}")
        
        if self.documentacoes:
            os.makedirs("documentacao", exist_ok=True)
            ext_fonte = os.path.splitext(self.caminho_fonte)[1]
            nome_doc = os.path.basename(self.caminho_fonte).replace(ext_fonte, ".brd")
            caminho_doc = os.path.join("documentacao", nome_doc)
            with open(caminho_doc, "w", encoding="utf-8") as f:
                f.write(f"# Documentação Viva - {os.path.splitext(nome_doc)[0].capitalize()}\n\n")
                for doc in self.documentacoes:
                    f.write(doc + "\n\n")
            print(f" -> Documentação Viva gerada em: {caminho_doc}")

def main():
    if len(sys.argv) < 3:
        print("Uso do Compilador BR:")
        print("  brasil compilar <arquivo.br/brt/bri/bra> [saida.brc]")
        print("  brasil testar <arquivo.br/brt>")
        sys.exit(1)
        
    acao = sys.argv[1]
    caminho = sys.argv[2]
    
    if acao == "compilar":
        ext_fonte = os.path.splitext(caminho)[1]
        saida = sys.argv[3] if len(sys.argv) > 3 else caminho.replace(ext_fonte, ".brc")
        comp = CompiladorBR(caminho)
        comp.compilar()
        comp.salvar_bytecode(saida)
    elif acao == "testar":
        comp = CompiladorBR(caminho)
        comp.compilar()
        if not comp.testes:
            print("Nenhum bloco de 'teste' encontrado no arquivo.")
            return
            
        print(f"\nRodando testes do arquivo {caminho}:")
        from maquina_virtual import MaquinaVirtualBR
        
        sucessos = 0
        falhas = 0
        for t in comp.testes:
            print(f" -> Teste: '{t['nome']}'...")
            vm = MaquinaVirtualBR({
                "constantes": comp.constantes,
                "instrucoes": t["instrucoes"],
                "funcoes": comp.funcoes,
                "testes": [],
                "documentacoes": []
            })
            try:
                vm.executar()
                print("    [PASSOU]")
                sucessos += 1
            except Exception as e:
                print(f"    [FALHOU]: {str(e)}")
                falhas += 1
                
        print(f"\nResultado final: {sucessos} passou, {falhas} falhou.")
        if falhas > 0:
            sys.exit(1)

if __name__ == "__main__":
    main()
