from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]
lib_path = ROOT / "lib"

baks = list(lib_path.rglob("*.br.bak"))
print(f"Encontrados {len(baks)} arquivos de backup.")

for bak in baks:
    br_file = bak.with_name(bak.name[:-4]) # remove .bak
    print(f"Restaurando {bak.relative_to(lib_path)} -> {br_file.relative_to(lib_path)}")
    shutil.copy2(bak, br_file)

print("Restauração concluída.")
