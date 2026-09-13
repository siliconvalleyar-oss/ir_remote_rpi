# SECURITY — Seguridad del proyecto IR-LG-Remote

## Privilegios

- La app requiere **`sudo`** para el acceso al hardware (`bcm2835` abre
  `/dev/mem`). Únicamente `--version` y `--help` funcionan sin privilegios.
- No ejecutar como root servicios que no lo necesiten; usar un usuario normal
  y elevar solo el comando.

## Configuración

- Los pines GPIO y la dirección NEC son configurables vía `config/*.cfg`;
  revisar estos archivos antes de desplegar en producción.
- No incluir secretos ni tokens en estos archivos.

## Credenciales de Git

- **Nunca** incrustar tokens en URLs de remote. Si se usa una URL con token
  (ver `docs/LEARNINGS.md`), limpiarla después:
  `git remote set-url origin https://github.com/USUARIO/REPO.git`.
- Preferir `gh auth login`, `GH_TOKEN` o SSH keys.
- El script `setup_git.sh` guía la configuración sin exponer credenciales.

## Hardening sugerido

- Restringir el acceso a `/dev/mem` y a los GPIO al usuario apropiado.
- No exponer el puerto I2C ni GPIOs a la red sin necesidad.
- Actualizar Raspberry Pi OS (`sudo apt full-upgrade`).

## Riesgos asociados al IR

- Un emisor IR puede controlar equipos cercanos; usar responsablemente.
- No usar en equipos donde pueda causar daños (equipos médicos, etc.).