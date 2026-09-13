#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Ejemplo de control de IR-LG-Remote desde Python.

Este script invoca el binario App para enviar teclas del mando LG.
No sustituye al binario C++; es simplemente un ejemplo de integración
desde scripts (por ejemplo, programarlo con cron o desde un dashboard).

Requiere:
    make        # compilar el binario
    sudo        # el binario necesita acceso a /dev/mem

Uso:
    sudo python3 examples/ir_command.py POWER
    sudo python3 examples/ir_command.py --macro encender_tv
"""

import subprocess
import sys


def run_app(*args: str) -> None:
    """Ejecuta ./bin/App con los argumentos dados."""
    cmd = ["./bin/App", *args]
    print(">>", " ".join(cmd))
    subprocess.run(cmd, check=True)


def main() -> None:
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    key = sys.argv[1]
    if key in ("--interactive", "--list", "--version", "--help"):
        run_app(key)
    elif key == "--macro":
        run_app("--macro", sys.argv[2])
    elif key == "--learn":
        run_app("--learn", sys.argv[2])
    else:
        run_app("--send", key)


if __name__ == "__main__":
    main()