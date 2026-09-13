# RULES — Reglas del proyecto IR-LG-Remote

## Código

- C++17, sin `new`/`delete`: gestión de memoria automática (RAII, `unique_ptr`).
- Siempre `#include <memory>`.
- Namespace principal: `Device`; clase `Device_t` en `core/Device_t.hpp`.
- Formato del `main.cpp` fijado (ver `docs/PROMPT.md`):
  create `unique_ptr<Device_t>` y llame a `run()`.
- GPIO con numeración **BCM**, configurables en `config/hardware.cfg`.
- Código comentado (español o inglés, consistente); Doxygen en cabeceras.
- No usar `-Werror`.

## Configuración

- Códigos IR: `config/ir_codes.cfg` (formato `TECLA=0xNN`), editables sin
  recompilar.
- Pines, dirección NEC y OLED: `config/hardware.cfg`.
- Macros y tiempos: `config/config.cfg`.

## Versionado (obligatorio)

- `VERSION` = último tag sin `v`; tag avec `v` (`v1.0.0`).
- Ciclo patch 0-9: `v1.0.9` → `v1.1.0`.
- Cada commit significativo lleva tag; no reemplazar tags, no retroceder.
- Convencional commits: `feat:`, `fix:`, `docs:`, `chore:`, `refactor:`,
  `test:`.
- Ver `docs/LEARNINGS.md`.

## Git / flujo

- Trabajar en `$PWD` local; en la Pi solo `git pull` + build + test
  (ver `docs/WORKFLOW.md`).
- Push siempre con tag.
- No exponer tokens en URLs; limpiar remote tras usar credenciales en línea.

## Documentación

- Todos los archivos de `docs/` y `README.md` deben estar completos y
  coherentes con el código.
- Nuevas funcionalidades actualizan `docs/CHANGELOG.md`.

## Seguridad

- No loguear secretos ni tokens.
- Ejecutar con `sudo` solo lo necesario (acceso `/dev/mem` de bcm2835).