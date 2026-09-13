```markdown
INSTRUCCIONES COMPLETAS PARA EL ASISTENTE (PROMPT ÚNICO Y DEFINITIVO)
========================================================================
PROYECTO: "IR-LG-Remote" — Emulador de control remoto LG por infrarrojos
           sobre Raspberry Pi, escrito en C++ con bcm2835.
========================================================================

Debes generar un proyecto C++ completo para Raspberry Pi (compatible con 32 y 64 bits) que use la librería bcm2835, siguiendo la estructura de carpetas y archivos que se detalla abajo. El proyecto debe ser autocontenido, compilable con make, y debe incluir todo el código fuente, cabeceras, scripts de instalación, documentación y archivos de configuración.

OBJETIVO FUNCIONAL PRINCIPAL
----------------------------
El proyecto debe permitir a una Raspberry Pi **emitir señales infrarrojas** mediante un **LED IR emisor** conectado a un GPIO, con el fin de **emular un control remoto LG** (televisores LG que usan el protocolo NEC, típicamente NEC1 con dirección 0x04 o similar).

Funcionalidades mínimas:
- Emitir comandos IR del protocolo **NEC** (modulación 38 kHz, portadora generada por software con `nanosleep` o por hardware con PWM/SPI según convenga).
- Reproducir un conjunto de teclas de un mando LG: `POWER`, `VOLUME_UP`, `VOLUME_DOWN`, `MUTE`, `CHANNEL_UP`, `CHANNEL_DOWN`, `INPUT`, `MENU`, `OK`, flechas, dígitos 0–9.
- Cargar la tabla de códigos IR de un mando LG desde un archivo de configuración (`config/ir_codes.cfg`) para poder añadir/editar teclas sin recompilar.
- Permitir enviar comandos desde:
  * línea de comandos: `./App --send POWER`
  * consola interactiva: `./App --interactive`
  * un script predefinido (macro de secuencia): `./App --macro encender_tv`
- (Opcional) Integrar un **receptor IR** (VS1838B en GPIO 17) para aprender códigos nuevos y guardarlos en `config/ir_codes.cfg`.
- (Opcional) Mostrar estado por la **pantalla OLED SSD1306** (I2C) ya incluida en la estructura.

ESTRUCTURA OBLIGATORIA (crear todos los directorios y archivos)
----------------------------------------------------------------
├── bin
│   └── App                           # binario final
├── config
│   ├── config.cfg                    # configuración general (pines, tiempos, etc.)
│   ├── hardware.cfg                  # definición del hardware conectado
│   └── ir_codes.cfg                  # tabla de códigos IR del mando LG (tecla=código NEC)
├── docs
│   ├── ACTIVITY.md
│   ├── API.md
│   ├── ARCHITECTURE.md
│   ├── ARQUITECTURA.md
│   ├── BLUETOOTH.md
│   ├── BUILD.md
│   ├── CHANGELOG.md
│   ├── CONTRIBUTING.md
│   ├── DEPLOY.md
│   ├── DESING.md                     # (mantén este nombre aunque sea "DESIGN")
│   ├── DIAGRAMS.md
│   ├── doxygen/                      # directorio (vacío)
│   ├── HARDWARE.md                   # esquema del LED IR + transistor + GPIO
│   ├── INSTALL.md
│   ├── INFRARED.md                   # protocolo NEC, tiempos, portadora 38 kHz
│   ├── LEARNINGS.md                  # DEBES LEER Y COMPLETAR (si existe)
│   ├── MEMORY_MAP.md
│   ├── PROMPT.md
│   ├── REPORT.md
│   ├── ROADMAP.md
│   ├── RULES.md
│   ├── SECURITY.md
│   ├── SETUP.md
│   ├── SKILLS.md
│   ├── TESTING.md
│   ├── TODO.md
│   ├── TROUBLESHOOTING.md
│   ├── USAGE.md
│   └── WORKFLOW.md                   # DEBES LEER Y COMPLETAR (si existe)
├── examples                          # ejemplos de uso (scripts bash/python)
├── generate_basic_src.sh             # script auxiliar (puede estar vacío)
├── include
│   ├── core/
│   │   └── Device_t.hpp              # cabecera de la clase principal
│   ├── drivers/
│   │   ├── IR_Receiver.hpp           # driver receptor IR (opcional)
│   │   └── IR_Transmitter.hpp        # driver emisor IR (LED + transistor)
│   ├── engine/
│   │   └── IR_Engine.hpp             # motor de codificación NEC
│   ├── libraries/                    # cabeceras externas (vacío)
│   ├── nlohmann/
│   │   └── json.hpp                  # librería JSON (puede ser vacía o la oficial)
│   ├── oled/
│   │   ├── SSD1306_OLED_font.hpp
│   │   ├── SSD1306_OLED_graphics.hpp
│   │   ├── SSD1306_OLED.hpp
│   │   └── SSD1306_OLED_Print.hpp
│   └── security/                     # cabeceras de seguridad (vacío)
├── LICENSE                           # licencia (ej. MIT)
├── Makefile                          # archivo de compilación (debe definir VERSION)
├── obj/                              # directorio para objetos (se creará durante la compilación)
├── README.md                         # DEBES COMPLETARLO
├── scripts/
│   ├── install_deps.sh               # script para instalar dependencias
│   └── setup_git.sh                  # script interactivo para crear el repo Git
├── src/
│   ├── core/
│   │   └── Device_t.cpp              # implementación de la clase principal
│   ├── drivers/
│   │   ├── IR_Receiver.cpp           # implementación del receptor (opcional)
│   │   └── IR_Transmitter.cpp        # implementación del emisor (LED IR)
│   ├── engine/
│   │   └── IR_Engine.cpp             # implementación del motor NEC
│   ├── main.cpp                      # archivo principal (ver formato abajo)
│   └── oled/
│       ├── SSD1306_OLED.cpp
│       ├── SSD1306_OLED_font.cpp
│       ├── SSD1306_OLED_graphics.cpp
│       └── SSD1306_OLED_Print.cpp
└── VERSION                           # archivo con el número de versión (ej. 0.1.0)


REQUISITOS FUNCIONALES DEL CÓDIGO
----------------------------------
- Usa la librería **bcm2835** para el acceso a GPIO, SPI, I2C, PWM, etc.
- Incluye siempre `#include <memory>` para gestión inteligente de memoria.
- Define un namespace, por ejemplo `Device`, y dentro una clase `Device_t`.
- El `main.cpp` DEBE tener el siguiente formato exacto (sin cambios en la lógica):

    #include <memory>
    #include "core/Device_t.hpp"

    int main(int argc, char** argv) {
        auto device = std::make_unique<Device::Device_t>(argc, argv);
        device->run();
        return 0;
    }

- La clase `Device_t` debe:
  * Inicializar `bcm2835_init()` en el constructor (o en `run()`).
  * Parsear los argumentos de línea de comandos (`--version`, `--send <TECLA>`, `--interactive`, `--macro <nombre>`, `--learn`).
  * Cargar la tabla de códigos IR desde `config/ir_codes.cfg`.
  * Ejecutar la lógica principal en `run()`.
  * Liberar `bcm2835_close()` en el destructor.
- No uses `new`/`delete` explícitos; la memoria se libera automáticamente al salir del `main` gracias al `unique_ptr`.
- El motor IR (`IR_Engine`) debe:
  * Generar la secuencia de pulsos NEC (9 ms de burst de 38 kHz, 4.5 ms de espacio, 32 bits: dirección + ~dirección + comando + ~comando, y bit de parada).
  * Usar `bcm2835_gpio_write()` con retardos de microsegundos (`bcm2835_delayMicroseconds()` o `nanosleep()` de alta precisión) para generar la portadora de 38 kHz (periodo ≈ 26.3 µs: 13 µs ON + 13 µs OFF).
  * Ser lo suficientemente precisa para que el TV LG reconozca la señal (tolerancia típica ±20%).
- El emisor IR (`IR_Transmitter`) debe:
  * Encapsular el GPIO del LED emisor (por defecto GPIO 22).
  * Encender/apagar el LED IR según las órdenes del motor.
  * Permitir configurar el GPIO desde `config/hardware.cfg`.
- (Opcional) El receptor IR (`IR_Receiver`) debe:
  * Leer un GPIO (por defecto GPIO 17) con interrupciones o polling.
  * Decodificar la señal NEC capturada y devolver el código de 32 bits.
  * Usarse en modo `--learn` para grabar una nueva tecla en `config/ir_codes.cfg`.

PROTOCOLO NEC (REFERENCIA)
--------------------------
- Portadora: 38 kHz (periodo 26.3 µs).
- Bit "0": 560 µs de burst + 560 µs de espacio.
- Bit "1": 560 µs de burst + 1690 µs de espacio.
- Secuencia de inicio: 9000 µs de burst + 4500 µs de espacio.
- Secuencia de fin: 560 µs de burst.
- 32 bits transmitidos: `addr(8) | ~addr(8) | cmd(8) | ~cmd(8)` (LSB first).
- Códigos típicos LG (dirección 0x04): 
  * POWER       = 0x08
  * VOLUME_UP   = 0x02
  * VOLUME_DOWN = 0x03
  * MUTE        = 0x09
  * CHANNEL_UP  = 0x00
  * CHANNEL_DOWN= 0x01
  * INPUT       = 0x0B
  * MENU        = 0x43
  * OK          = 0x44
  * Flechas y números según se documenten en `config/ir_codes.cfg`.

VERSIÓN DE LA APLICACIÓN (EN TIEMPO DE COMPILACIÓN)
---------------------------------------------------
- La versión de la aplicación NO se lee del archivo VERSION en tiempo de ejecución, sino que se define como una macro en tiempo de compilación.
- El Makefile debe pasar la versión al compilador usando `-DVERSION="$(VERSION)"` (o similar), leyendo el número del archivo `VERSION`.
- En el código (por ejemplo, en `Device_t` o en `main`), se debe mostrar la versión al iniciar la aplicación (por ejemplo, imprimiendo por consola: `IR-LG-Remote v0.1.0`).
- Además, se debe soportar un argumento de línea de comandos (`--version`) que muestre la versión y termine la ejecución.
- Todo el código debe estar completamente documentado con comentarios (explicando qué hace cada función, clase, y partes importantes).

CREACIÓN DEL REPOSITORIO GIT (DESDE SHELL CONSOLA)
---------------------------------------------------
El proceso de generación del proyecto debe incluir la creación de un repositorio Git. Para ello, el script `scripts/setup_git.sh` debe interactuar con el usuario y preguntar:

    - Nombre de usuario en la plataforma (GitHub, GitLab, etc.).
    - Credenciales (token de acceso o pedir que ya estén configuradas globalmente).
    - Nombre del repositorio (que coincidirá con el nombre del proyecto, por defecto "IR-LG-Remote").
    - Visibilidad: público o privado.

Con estos datos, el script debe ejecutar los siguientes comandos desde la shell:

    1. `git init`
    2. `git add .`
    3. `git commit -m "Initial commit: IR-LG-Remote"`
    4. `gh repo create <nombre> --public (o --private) --source=. --remote=origin --push`
       (si se usa GitHub CLI) o alternativamente:
       - Crear el repositorio vía API (curl) y luego añadir el remote.
    5. `git push -u origin main` (o master)

Si no se dispone de GitHub CLI, se debe proporcionar instrucciones claras para que el usuario cree el repositorio manualmente y luego ejecute los comandos `git remote add` y `git push`. El script debe verificar si `gh` está instalado; si no, debe ofrecer la opción de crear el repositorio localmente y mostrar los pasos para el remoto.

El script también debe preguntar el **nombre del proyecto en Git** y el **usuario de Git**, y explicar dónde se deben colocar las credenciales (token de GitHub en `~/.config/gh/hosts.yml` o variable de entorno `GH_TOKEN`).

SCRIPTS Y MAKEFILE
------------------
- `scripts/install_deps.sh`: Debe instalar bcm2835 (descargando y compilando desde el sitio oficial o usando apt), g++, make, git, gh (opcional), y cualquier otra dependencia. Debe funcionar en Raspberry Pi de 32 y 64 bits.
- `Makefile`:
  * Debe leer la versión del archivo `VERSION` (ej. con `$(shell cat VERSION)`) y pasarla como `-DVERSION="..."` a todos los objetos.
  * Compila todos los `.cpp` de `src/` y sus subdirectorios, generando los `.o` en `obj/` manteniendo la jerarquía (ej. `obj/src/main.o`, `obj/src/core/Device_t.o`, `obj/src/drivers/IR_Transmitter.o`).
  * El binario final se llama `App` y se coloca en `bin/`.
  * Enlaza con la librería bcm2835 (opciones `-lbcm2835`).
  * Debe soportar ambas arquitecturas (32 y 64 bits) mediante flags condicionales o detección automática.
  * Incluye objetivos: `all`, `clean`, `distclean`, `install` (opcional), `run`.
- `generate_basic_src.sh`: puede ser un script auxiliar para generar archivos fuente (opcional, puede estar vacío).
- `scripts/setup_git.sh`: script que pregunta interactivamente los datos y configura el repositorio Git.

DOCUMENTACIÓN (archivos .md)
----------------------------
- Lee (si existen) los archivos `docs/LEARNINGS.md` y `docs/WORKFLOW.md` y complétalos con información relevante al proyecto (aprendizajes, flujo de trabajo, etc.). Si no existen, créalos con contenido útil.
- Completa TODOS los archivos `.md` de `docs/` con descripciones coherentes y apropiadas para cada tema (API, arquitectura, instalación, uso, etc.). No dejes ninguno vacío.
- `docs/INFRARED.md` debe explicar el protocolo NEC en detalle, la modulación a 38 kHz, los tiempos de bit, y cómo se generan por software.
- `docs/HARDWARE.md` debe incluir el esquema del circuito emisor (LED IR + transistor NPN + resistencias) y del receptor opcional (VS1838B), con los GPIO usados.
- `README.md` debe incluir:
  * Nombre del proyecto y descripción general (emulador de mando LG por IR).
  * Requisitos (Raspberry Pi, bcm2835, LED IR, transistor, resistencias, Git, gh opcional).
  * Esquema del circuito emisor IR (diagrama ASCII).
  * Instrucciones de compilación e instalación (usando `make` y el script de dependencias).
  * Indicación explícita de que la aplicación es compatible con Raspberry Pi de 32 bits y 64 bits.
  * Un ejemplo básico de uso (`./bin/App --send POWER`).
  * Mención de que la versión se muestra al inicio y con `--version`.
  * Instrucciones para configurar el repositorio Git (ejecutar `scripts/setup_git.sh` o los pasos manuales).

CONTENIDO ADICIONAL
-------------------
- Los archivos de cabecera y fuente de la pantalla OLED (SSD1306) deben estar implementados, aunque sea con funciones básicas o declaraciones, pero que compilen sin errores.
- El archivo `VERSION` debe contener un número de versión (ej. `0.1.0`).
- El archivo `config/ir_codes.cfg` debe contener al menos las teclas básicas del mando LG con sus códigos NEC.
- El archivo `config/hardware.cfg` debe indicar los GPIO usados (emisor, receptor, I2C de la OLED).

COMPILACIÓN Y PRUEBAS
---------------------
- El código debe compilar sin errores en una Raspberry Pi (simulado o real) usando `make`.
- Prueba básica: conectar el LED IR al GPIO 22 (a través del transistor), ejecutar `sudo ./bin/App --send POWER` apuntando a un TV LG, y verificar que responde.
- Prueba avanzada (opcional): usar el receptor VS1838B en GPIO 17 para aprender un código nuevo con `sudo ./bin/App --learn POWER`.

COMENTARIOS Y DOCUMENTACIÓN DEL CÓDIGO
--------------------------------------
- Todo el código fuente (`.cpp` y `.hpp`) debe estar ampliamente comentado en español o inglés (consistente), explicando:
  * El propósito de cada clase, método y función.
  * Los parámetros y retornos.
  * La lógica importante (especialmente la generación de la portadora de 38 kHz y los tiempos NEC).
  * Las decisiones de diseño.
- También se deben incluir comentarios de tipo Doxygen (si se desea) para facilitar la generación de documentación.
- El código debe ser legible y seguir buenas prácticas de programación (nombres descriptivos, const-correctness, RAII, etc.).

ENTREGABLE FINAL
----------------
El asistente debe generar todos los archivos y carpetas con el contenido adecuado. El código debe compilar sin errores en una Raspberry Pi usando `make`. La documentación debe estar completa y bien redactada. Todo debe ser funcional, coherente y seguir las especificaciones dadas, incluyendo la configuración del repositorio Git mediante interacción con el usuario.

NOTA: Este prompt fusiona las instrucciones previas (estructura de proyecto C++ con bcm2835, versión en tiempo de compilación, documentación exhaustiva, creación de repositorio Git) con el nuevo objetivo funcional: **emular un control remoto LG mediante un LED infrarrojo emisor conectado a la Raspberry Pi**. Ejecuta cada paso y genera el proyecto completo.
```
