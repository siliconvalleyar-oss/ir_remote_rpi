# CONTRIBUTING — Cómo contribuir al proyecto IR-LG-Remote

Gracias por contribuir. Este proyecto sigue reglas estrictas de versionado y
estilo; léalas antes de abrir un PR.

## Flujo recomendado

1. Cree una rama: `git checkout -b feat/nombre-corto`.
2. Implemente y valide localmente: `make && make run ARGS="--version"`.
3. Commit con mensaje semántico:
   - `feat:` nueva funcionalidad
   - `fix:` corrección de errores
   - `refactor:` cambio sin cambiar comportamiento
   - `docs:` documentación
   - `chore:` tareas de mantenimiento
   - `test:` pruebas
4. Incluya test o verificación manual descrita en `docs/TESTING.md`.
5. Abra PR contra `main`.

## Reglas de estilo

- C++17, nombres descriptivos, `const`-correctness, RAII.
- Sin `new`/`delete` explícitos: usar `unique_ptr`.
- Comentarios Doxygen en cabeceras y comentarios en español consistentes.
- Mantener el switch de `main.cpp` exactamente como especifica `docs/PROMPT.md`.
- Numeración BCM para GPIO; pines configurables vía `config/hardware.cfg`.

## Versión y tags (reglas de LEARNINGS)

- `VERSION` siempre coincide con el último tag (tag con `v`, archivo sin `v`).
- Ciclo patch 0-9 obligatorio: `v1.0.9` → `1.1.0`, no `1.1.1`.
- No eliminar tags publicados ni retroceder de versión.

## Documentación

- Todo cambio de comportamiento debe actualizar `docs/` correspondiente,
  `README.md` y `docs/CHANGELOG.md`.
- Comandos nuevos: actualice `docs/API.md` y `docs/USAGE.md`.

## Revisión del PR

- Verifique que compila con `make clean && make`.
- Verifique que los objetivos del Makefile (`crossover`, `remote`, `deploy`)
  siguen funcionando.
- Sin emojis en el código ni en los commits (salvo que se pidan).