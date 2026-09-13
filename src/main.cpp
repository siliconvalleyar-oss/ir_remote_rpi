/**
 * @file main.cpp
 * @brief Punto de entrada principal de IR-LG-Remote.
 *
 * @details Crea una instancia de Device::Device_t pasando los argumentos de
 * la línea de comandos, y ejecuta la lógica principal con run(). La memoria
 * se libera automáticamente al salir del main gracias al unique_ptr.
 *
 * Ejemplos de uso:
 *   sudo ./bin/App --send POWER
 *   sudo ./bin/App --macro encender_tv
 *   sudo ./bin/App --interactive
 *   sudo ./bin/App --learn VOLUME_UP
 *   sudo ./bin/App --list
 *   sudo ./bin/App --version
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#include <memory>
#include "core/Device_t.hpp"

int main(int argc, char** argv) {
    auto device = std::make_unique<Device::Device_t>(argc, argv);
    device->run();
    return 0;
}
