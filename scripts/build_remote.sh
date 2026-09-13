#!/usr/bin/env bash
# =============================================================================
# build_remote.sh - Compila y ejecuta IR-LG-Remote en la Raspberry Pi
# =============================================================================
# Se conecta por SSH a la Raspberry Pi (sin contraseña, clave pública) y:
#   1. Actualiza el repositorio:        git pull
#   2. Limpia la compilación previa:    make clean
#   3. Compila en paralelo:             make -j4
#   4. Ejecuta la aplicación:           make run
#
# Requisitos:
#   - SSH por clave pública configurado (sin contraseña):
#       ssh-copy-id pi@raspi.local
#   - En la Raspberry Pi:/home/pi/src/ir_remote_rpi clonado del repositorio
#     con las dependencias instaladas (scripts/install_deps.sh).
#   - I2C y GPIO de bajo nivel requieren sudo en la Raspberry Pi; por defecto
#     `sudo make run` se ejecuta en remoto (véase REMOTE_SUDO).
#
# Variables de entorno (opcionales):
#   RPI_HOST   -> dirección/alias de la Raspberry Pi  (por defecto: pi@raspi.local)
#   RPI_DIR    -> directorio del proyecto en la Pi    (por defecto: /home/pi/src/ir_remote_rpi)
#   RPI_ARGS   -> argumentos para la app (por defecto: "")
#   RPI_SUDO   -> "sudo" para ejecutar con permisos (por defecto: sudo)
#
# Uso:
#   bash scripts/build_remote.sh                # muy rápido, síncrono
#   bash scripts/build_remote.sh "ARGS"         # pasa argumentos a la app
#   make remote                                 # equivalente al modo por defecto
# =============================================================================

set -euo pipefail

GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'
ok()  { echo -e "${GREEN}[remote]${NC} $*"; }
warn(){ echo -e "${YELLOW}[remote]${NC} $*"; }
die() { echo -e "${RED}[remote]${NC} $*" >&2; exit 1; }

# --- Parámetros (entorno o argumentos) ---------------------------------------
RPI_HOST="${RPI_HOST:-pi@raspi.local}"
RPI_DIR="${RPI_DIR:-/home/pi/src/ir_remote_rpi}"
RPI_ARGS="${RPI_ARGS:-${1:-}}"
RPI_SUDO="${RPI_SUDO:-sudo}"

# --- Comprobar conexión SSH sin contraseña ------------------------------------
ok "Comprobando conexión SSH a ${RPI_HOST}..."
if ! ssh -o BatchMode=yes -o ConnectTimeout=5 "${RPI_HOST}" 'true' 2>/dev/null; then
    die "No se puede conectar a ${RPI_HOST} sin contraseña.
   Configure la clave pública primero:
       ssh-copy-id ${RPI_HOST}
   o use la variable RPI_HOST para elegir otra Pi."
fi

# --- Comando remoto ------------------------------------------------------------
# Combina pull + clean + make en paralelo + ejecución con los argumentos dados.
# La app necesita sudo (acceso a /dev/mem vía bcm2835); para solo --version
# puede usarse RPI_SUDO="" para omitir sudo.
RUN_ARGS=""
if [[ -n "${RPI_ARGS}" ]]; then
    RUN_ARGS=" ARGS=\"${RPI_ARGS}\""
fi

ok "Compilando y ejecutando en ${RPI_HOST}:${RPI_DIR}..."
ok "Comando remoto: cd ${RPI_DIR} && git pull && make clean && make -j4 && ${RPI_SUDO:+sudo }make run${RUN_ARGS}"

ssh -t "${RPI_HOST}" "cd ${RPI_DIR} && git pull && make clean && make -j4 && ${RPI_SUDO} make run${RUN_ARGS}"

ok "Compilación y ejecución remota completadas."