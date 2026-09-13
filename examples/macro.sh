#!/usr/bin/env bash
# =============================================================================
# examples/macro.sh - Ejemplo de ejecución de macros predefinidas
# =============================================================================
# Ejecuta las macros definidas en config/config.cfg.
# Requiere sudo (acceso al hardware IR).
#
# Uso:
#   sudo bash examples/macro.sh
# =============================================================================

set -euo pipefail

APP="./bin/App"

[[ -x "${APP}" ]] || { echo "Compile primero: make"; exit 1; }

echo "==> Encender la TV (macro encender_tv)"
${APP} --macro encender_tv
sleep 2

echo "==> Silenciar (macro mute)"
${APP} --macro mute
sleep 2

echo "==> Apagar la TV (macro apagar_tv)"
${APP} --macro apagar_tv

echo "==> Macros ejecutadas."
echo "    Defina sus propias macros en config/config.cfg (MACRO_<nombre>=<tecla1>,<tecla2>,...)"