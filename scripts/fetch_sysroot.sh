#!/usr/bin/env bash
# =============================================================================
# fetch_sysroot.sh - Obtiene sysroot con bcm2835 para compilación cruzada
# =============================================================================
# Descarga de una Raspberry Pi la cabecera y la librería bcm2835 y las coloca
# en sysroot/<triple>/ imitando la estructura del sistema de la Pi, de modo que
# el Makefile pueda compilar en cruz (make crossover / crossover32 / crossover64)
# desde un PC x86.
#
# Estructura generada:
#   sysroot/aarch64-linux-gnu/usr/include/bcm2835.h
#   sysroot/aarch64-linux-gnu/usr/lib/aarch64-linux-gnu/libbcm2835.a
#   sysroot/arm-linux-gnueabihf/... (equivalente para ARM 32 bits)
#
# Variable de entorno:
#   RPI_HOST -> alias/dirección de la Raspberry Pi (por defecto: pi@raspi.local)
#
# Uso:
#   bash scripts/fetch_sysroot.sh                       # ambas arquitecturas
#   bash scripts/fetch_sysroot.sh aarch64-linux-gnu     # solo ARM64
#   bash scripts/fetch_sysroot.sh arm-linux-gnueabihf   # solo ARM32
#   make fetch                                          # alias del Makefile
# =============================================================================

set -euo pipefail

GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'
ok()  { echo -e "${GREEN}[sysroot]${NC} $*"; }
warn(){ echo -e "${YELLOW}[sysroot]${NC} $*"; }
die() { echo -e "${RED}[sysroot]${NC} $*" >&2; exit 1; }

RPI_HOST="${RPI_HOST:-pi@raspi.local}"

# Triples soportados y sus sufijos de biblioteca en la Pi.
declare -A TRIPLE_TO_ARCH=(
    [aarch64-linux-gnu]=arm64
    [arm-linux-gnueabihf]=armhf
)

# --- Argumentos: opcionalmente un solo triple ---------------------------------
TRIPLES=()
if [[ $# -gt 0 ]]; then
    TRIPLES=("$1")
else
    TRIPLES=(aarch64-linux-gnu arm-linux-gnueabihf)
fi

# --- Comprobación de SSH --------------------------------------------------------
ok "Comprobando conexión SSH a ${RPI_HOST}..."
if ! ssh -o BatchMode=yes -o ConnectTimeout=5 "${RPI_HOST}" 'true' 2>/dev/null; then
    die "No se puede conectar a ${RPI_HOST} sin contraseña. Configure: ssh-copy-id ${RPI_HOST}"
fi

for TRIPLE in "${TRIPLES[@]}"; do
    ARCH_NAME="${TRIPLE_TO_ARCH[${TRIPLE}]:-${TRIPLE}}"
    DEST="sysroot/${TRIPLE}"
    LIBDIR="${DEST}/usr/lib/${TRIPLE}"
    INCDIR="${DEST}/usr/include"

    ok "Descargando bcm2835 para ${TRIPLE} (${ARCH_NAME})..."

    # Comprueba en la Pi si bcm2835 está instalada en la ruta canónica.
    REMOTE_HDR="/usr/local/include/bcm2835.h"
    REMOTE_LIB="/usr/local/lib/libbcm2835.a"

    if ! ssh "${RPI_HOST}" "test -f ${REMOTE_HDR} -a -f ${REMOTE_LIB}"; then
        warn "bcm2835 no está en /usr/local en la Pi. Intentando apt (libbcm2835-dev)..."
        ssh "${RPI_HOST}" "sudo apt-get update -qq && sudo apt-get install -y -qq libbcm2835-dev"
        REMOTE_HDR="/usr/include/bcm2835.h"
        REMOTE_LIB="/usr/lib/${ARCH_NAME}-linux-gnu/libbcm2835.a"
        ssh "${RPI_HOST}" "test -f ${REMOTE_HDR} -a -f ${REMOTE_LIB}" || \
            die "No se encontró bcm2835 en la Pi. Ejecute primero scripts/install_deps.sh en la Pi."
    fi

    mkdir -p "${INCDIR}" "${LIBDIR}"
    ssh "${RPI_HOST}" "cat ${REMOTE_HDR}" > "${INCDIR}/bcm2835.h"
    ssh "${RPI_HOST}" "cat ${REMOTE_LIB}" > "${LIBDIR}/libbcm2835.a"
    ok "Creado ${INCDIR}/bcm2835.h"
    ok "Creado ${LIBDIR}/libbcm2835.a"
done

ok "Sysroot creado. Compile en cruz con:"
echo "    make crossover        # detecta arquitectura"
echo "    make crossover64      # ARM 64-bit"
echo "    make crossover32      # ARM 32-bit"