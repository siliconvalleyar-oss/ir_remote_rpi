#!/usr/bin/env bash
# =============================================================================
# examples/send_keys.sh - Ejemplo de uso desde línea de comandos
# =============================================================================
# Envía varios comandos IR del mando LG de forma secuencial.
# Requiere ejecución con sudo (acceso a /dev/mem vía bcm2835).
#
# Uso:
#   sudo bash examples/send_keys.sh
# =============================================================================

set -euo pipefail

APP="./bin/App"

[[ -x "${APP}" ]] || { echo "Compile primero: make"; exit 1; }

echo "==> Enviando teclas del mando LG..."
${APP} --send POWER
sleep 1
${APP} --send VOLUME_UP
sleep 1
${APP} --send VOLUME_DOWN
sleep 1
${APP} --send MUTE

echo "==> Ejemplo completado."