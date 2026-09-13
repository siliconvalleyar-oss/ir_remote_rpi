# USAGE — Guía de uso

## Modo línea de comandos

```bash
sudo ./bin/App --send POWER
sudo ./bin/App --send VOLUME_UP
sudo ./bin/App --list
```

## Consola interactiva

```bash
sudo ./bin/App --interactive
```
Comandos: escribir el nombre de una tecla para enviarla, `list`, `help`,
`exit`.

## Macros

Definidas en `config/config.cfg`:

```
MACRO_<nombre>=<tecla1>,<tecla2>,...
```

```bash
sudo ./bin/App --macro encender_tv
```

Entre teclas se espera `MACRO_PAUSE_MS` ms (por defecto 50).

## Aprendizaje de códigos (VS1838B)

```bash
sudo ./bin/App --learn VOLUME_UP
```

Apunte el mando original al receptor y pulse la tecla. El código se guarda
en `config/ir_codes.cfg`.

## Estado / versión

```bash
./bin/App --version          # no requiere sudo
./bin/App --help
```

## Ejemplos

- `examples/send_keys.sh` — secuencia de teclas básica.
- `examples/macro.sh` — ejecución de macros.
- `examples/ir_command.py` — envío de teclas desde Python.

## Notas

- Los códigos NEC se pueden editar en `config/ir_codes.cfg` sin recompilar.
- La dirección NEC (0x04 para LG) se configura en `config/hardware.cfg`.
- Todo el acceso al hardware necesita sudo (acceso a `/dev/mem` vía bcm2835).