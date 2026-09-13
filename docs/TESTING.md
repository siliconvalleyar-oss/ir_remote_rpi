# TESTING — Pruebas del proyecto

## Pruebas de compilación

```bash
make clean && make -j4          # debe compilar sin errores
./bin/App --version             # muestra IR-LG-Remote v1.0.0
```

Con `-Wall -Wextra` activado. No se usa `-Werror` para no romper el build.

## Pruebas sin hardware

| Comando | Resultado esperado |
|---|---|
| `./bin/App --version` | Banner con versión `v1.0.0` |
| `./bin/App --help` | Ayuda con todas las opciones |
| `./bin/App --list` | 36 teclas cargadas con sus códigos |
| `./bin/App --send DESCONOCIDA` | Error "tecla no encontrada" |

## Pruebas con hardware (en la Pi)

### Emisor (mínimo)

1. LED IR + transistor en GPIO 22; receptor del TV LG a 1-2 m.
2. `sudo ./bin/App --send POWER` → el TV responde.

### Aprendizaje

1. VS1838B en GPIO 17 (con pull-up).
2. `sudo ./bin/App --learn POWER` y pulsar en el mando original.
3. Comprobar el código guardado en `config/ir_codes.cfg`.

### Depuración visual

- Apunte el LED a una cámara de móvil: debe verse parpadeo violeta.
- Con un osciloscopio/lógica: marcas de 9 ms, espacios 4.5 ms, bits de 562 µs.

## Prueba remota (SSH)

```bash
make remote                      # git pull + build + run en la Pi
make remote ARGS="--list"
```

## Tests de integración sugeridos (TDD futuro)

- Validar trama NEC de un comando con un analizador lógico.
- Probar macro con pausas y la carga de `config.cfg` con/sin macros.
- Adición de tecla en `ir_codes.cfg` y verificación con `--list`.

Cobertura actual: validación manual sobre hardware real y smoke tests de CLI.