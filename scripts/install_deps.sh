#!/usr/bin/env bash
# =============================================================================
# install_deps.sh - Instala las dependencias de IR-LG-Remote
# =============================================================================
# Instala el toolchain básico (build-essential, g++, make, git, wget, curl),
# la librería bcm2835 (v1.71) desde la fuente oficial de Mike McCauley y
# GitHub CLI (gh) de forma opcional.
#
# Compatible con Raspberry Pi OS de 32 bits (armhf) y 64 bits (arm64).
#
# Uso:
#   sudo bash scripts/install_deps.sh
# =============================================================================

set -euo pipefail

# --- Colores de salida -------------------------------------------------------
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'
ok()  { echo -e "${GREEN}[deps]${NC} $*"; }
warn(){ echo -e "${YELLOW}[deps]${NC} $*"; }
die() { echo -e "${RED}[deps]${NC} $*" >&2; exit 1; }

# --- Comprobación de privilegios ---------------------------------------------
if [[ "$(id -u)" -ne 0 ]]; then
    die "Este script requiere permisos de superusuario. Ejecute: sudo bash scripts/install_deps.sh"
fi

# --- Arquitectura -------------------------------------------------------------
ARCH="$(uname -m)"
case "${ARCH}" in
    armv6l|armv7l|armhf)  ARCH_TYPE="32-bit (armhf)" ;;
    aarch64|arm64)        ARCH_TYPE="64-bit (arm64)" ;;
    *)                    ARCH_TYPE="desconocida (${ARCH})" ;;
esac
ok "Arquitectura detectada: ${ARCH_TYPE}"

BCM2835_VERSION="1.71"
BCM2835_URL="http://www.airspayce.com/mikem/bcm2835/bcm2835-${BCM2835_VERSION}.tar.gz"

# --- 1. Paquetes base ---------------------------------------------------------
echo ""
ok "[1/4] Instalando paquetes base (build-essential, git, wget, curl)..."
apt-get update -qq
apt-get install -y -qq build-essential g++ gcc make git wget curl

# --- 2. Librería bcm2835 ------------------------------------------------------
echo ""
ok "[2/4] Instalando librería bcm2835..."
if [[ -f /usr/local/include/bcm2835.h && -f /usr/local/lib/libbcm2835.a ]]; then
    warn "bcm2835 ya instalada en /usr/local (omitiendo)."
else
    TMP="$(mktemp -d)"
    trap 'rm -rf "${TMP}"' EXIT
    ok "Descargando bcm2835 v${BCM2835_VERSION}..."
    wget -q -O "${TMP}/bcm2835.tar.gz" "${BCM2835_URL}"
    tar -xzf "${TMP}/bcm2835.tar.gz" -C "${TMP}"
    (
        cd "${TMP}/bcm2835-${BCM2835_VERSION}"
        ./configure --disable-docs >/dev/null
        make >/dev/null
        make install >/dev/null
    )
    ldconfig || true
    ok "bcm2835 v${BCM2835_VERSION} instalada en /usr/local."
fi

# --- 3. Herramientas I2C (para la OLED SSD1306) -------------------------------
echo ""
ok "[3/4] Instalando herramientas I2C..."
apt-get install -y -qq i2c-tools python3-smbus || warn "Los paquetes i2c no se instalaron (opcional)."

# --- 4. GitHub CLI (opcional) --------------------------------------------------
echo ""
ok "[4/4] Verificando GitHub CLI (opcional)..."
if command -v gh >/dev/null 2>&1; then
    warn "GitHub CLI ya instalado: $(gh --version | head -1)"
else
    warn "GitHub CLI no encontrado. Instalándolo..."
    apt-get install -y -qq gnupg
    curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | \
        dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" \
        | tee /etc/apt/sources.list.d/github-cli.list >/dev/null
    apt-get update -qq
    apt-get install -y -qq gh
fi

# --- Resumen -------------------------------------------------------------------
echo ""
ok "Dependencias instaladas. Resumen:"
echo "  - Arquitectura:          ${ARCH_TYPE}"
echo "  - bcm2835:               v${BCM2835_VERSION}"
echo "  - GitHub CLI:            $([[ -x "$(command -v gh)" ]] && echo 'instalado' || echo 'no disponible')"
echo ""
ok "Compile con:  make clean && make -j4"
ok "Ejecute con:  sudo ./bin/App --send POWER"