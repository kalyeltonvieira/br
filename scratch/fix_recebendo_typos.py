from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
lib_path = ROOT / "lib"

br_files = list(lib_path.rglob("*.br")) + list(lib_path.rglob("*.br.bak"))
print(f"Buscando typos em {len(br_files)} arquivos.")

substituicoes = 0
for filepath in br_files:
    content = filepath.read_text(encoding="utf-8")
    if "_recebendo" in content:
        print(f"Corrigindo typo em: {filepath.relative_to(lib_path)}")
        content = content.replace("_recebendo", " recebendo")
        filepath.write_text(content, encoding="utf-8")
        substituicoes += 1

print(f"Concluído. {substituicoes} arquivos corrigidos.")
