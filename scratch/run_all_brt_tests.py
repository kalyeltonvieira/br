import sys
import os
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "ferramentas"))

from tester import FrameworkTestesBR

def run_tests():
    lib_path = ROOT / "lib"
    test_files = sorted(list(lib_path.rglob("*_teste.brt")))
    print(f"Encontrados {len(test_files)} arquivos de teste.")
    
    framework = FrameworkTestesBR()
    failed = 0
    passed = 0
    
    for f in test_files:
        print(f"Executando: {f.relative_to(ROOT)}")
        suite = framework.executar_arquivo(str(f))
        suite_failed = False
        for t in suite.testes:
            if not t.sucesso:
                print(f"  [FALHA] {t.nome}")
                if t.erro:
                    print(f"    Erro: {t.erro}")
                suite_failed = True
            else:
                pass
        if suite_failed:
            failed += 1
        else:
            passed += 1
            
    print("\n--- RESUMO DOS TESTES ---")
    print(f"Suítes que passaram: {passed}")
    print(f"Suítes que falharam: {failed}")
    if failed > 0:
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == "__main__":
    run_tests()
