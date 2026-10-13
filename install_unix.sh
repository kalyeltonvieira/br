#!/bin/bash
# Instalador da Linguagem BR para Linux e macOS
# Este script instala o BR e configura o sistema

echo "Instalando Linguagem BR para Linux/macOS..."

# Verificar se está rodando como root
if [ "$EUID" -ne 0 ]; then
    echo "Por favor, execute como root (sudo)"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
INSTALL_DIR="/usr/local/lib/br"
BIN_DIR="/usr/local/bin"
mkdir -p "$INSTALL_DIR" "$BIN_DIR"

# Copiar arquivos
install -m 644 "$SCRIPT_DIR/br.py" "$INSTALL_DIR/br.py"
install -m 755 "$SCRIPT_DIR/scripts/br_unix" "$INSTALL_DIR/br"
rm -rf "$INSTALL_DIR/ferramentas"
cp -R "$SCRIPT_DIR/ferramentas" "$INSTALL_DIR/ferramentas"
find "$INSTALL_DIR/ferramentas" -type d -exec chmod 755 {} \;
find "$INSTALL_DIR/ferramentas" -type f -exec chmod 644 {} \;

# Criar link simbólico em /usr/local/bin
ln -sf "$INSTALL_DIR/br" "$BIN_DIR/br"

# Dar permissão de execução
chmod +x "$BIN_DIR/br" "$INSTALL_DIR/br"

echo ""
echo "Instalação concluída com sucesso!"
echo ""
echo "Você pode agora usar o comando 'br' em qualquer lugar."
echo "Para usar shebang, adicione #!/usr/bin/env br no início dos arquivos .br"
echo ""
