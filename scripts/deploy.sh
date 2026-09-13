#!/usr/bin/env bash
# =============================================================================
# deploy.sh - Despliega el binario compilado en la Raspberry Pi
# =============================================================================
# Copia el binario bin/App a la Raspberry Pi mediante scp (sin contraseña)
# y lo ejecuta de forma opcional.
#
# Requisitos:
#   - SSH por clave pública (sin contraseña):  ssh-copy-id pi@raspi.local
#   - Variables opcionales:
#       RPI_HOST   -> alias/dirección de la Pi         (por defecto: pi@raspi.local)
#       RPI_BINDIR -> directorio destino del binario   (por defecto: /home/pi/bin)
#       RPI_ARGS   -> argumentos de la app al ejecutar (por defecto: vacío → no ejecuta)
#       RPI_RUN    -> "1" para ejecutar la app tras copiarla
#
# Uso:
#   bash scripts/deploy.sh                 # solo copia el binario
#   RPI_RUN=1 RPI_ARGS="--list" bash scripts/deploy.sh
# =============================================================================

set -euo pipefail

GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'
ok()  { echo -e "${GREEN}[deploy]${NC} $*"; }
warn(){ echo -e "${YELLOW}[deploy]${NC} $*"; }
die() { echo -e "${RED}[deploy]${NC} $*" >&2; exit 1; }

RPI_HOST="${RPI_HOST:-pi@raspi.local}"
RPI_BINDIR="${RPI_BINDIR:-/home/pi/bin}"
RPI_ARGS="${RPI_ARGS:-}"
RPI_RUN="${RPI_RUN:-0}"

BINARY="bin/App"

# --- Validaciones ---------------------------------------------------------------
[[ -x "${BINARY}" ]] || die "El binario ${BINARY} no existe o no es ejecutable. Compile primero: make"

ok "Desplegando ${BINARY} a ${RPI_HOST}:${RPI_BINDIR}/..."
ssh -o BatchMode=yes -o ConnectTimeout=5 "${RPI_HOST}" "mkdir -p ${RPI_BINDIR}" 2>/dev/null \
    || die "No se puede conectar a ${RPI_HOST}. Configure la clave pública (ssh-copy-id)."

scp "${BINARY}" "${RPI_HOST}:${RPI_BINDIR}/${RPI_BINDIR##*/}+.tmp" &&
ssh "${RPI_HOST}" "mv -f ${RPI_BINDIR}/${RPI_BINDIR##*/}+.tmp ${RPI_BINDIR}/App"

ok "Binario desplegado en ${RPI_HOST}:${RPI_BINDIR}/App"

if [[ "${RPI_RUN}" == "1" ]]; then
    ok "Ejecutando en la Pi: sudo ${RPI_BINDIR}/App ${RPI_ARGS}"
    ssh -t "${RPI_HOST}" "sudo ${RPI_BINDIR}/App ${RPI_ARGS}"
fi

ok "Despliegue completado."