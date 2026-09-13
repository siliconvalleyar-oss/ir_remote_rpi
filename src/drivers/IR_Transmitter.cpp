/**
 * @file IR_Transmitter.cpp
 * @brief Implementación del driver de emisor IR (LED infrarrojo + transistor).
 *
 * @details Inicializa bcm2835 (inicialización idempotente), configura el
 * GPIO del LED emisor como salida y delega la generación de tramas NEC
 * al motor IR_Engine.
 *
 * El propietario del ciclo de vida de bcm2835 (init/cierra) es este driver.
 * Si se usan múltiples componentes (transmisor + receptor), el transmisor
 * inicializa bcm2835 y el receptor asume que ya está inicializado.
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#include "drivers/IR_Transmitter.hpp"

#include <bcm2835.h>
#include <cstdio>

namespace IR {

IR_Transmitter::IR_Transmitter(uint8_t gpioPin)
    : engine_(gpioPin),
      gpioPin_(gpioPin),
      initialized_(false) {
}

IR_Transmitter::~IR_Transmitter() {
    end();
}

bool IR_Transmitter::begin() {
    // bcm2835_init() es idempotente: múltiples llamadas no son problemáticas.
    if (!bcm2835_init()) {
        std::printf("Error: No se pudo inicializar bcm2835 para el emisor IR\n");
        std::printf("Ejecuta con sudo: sudo ./bin/App --send POWER\n");
        return false;
    }

    // Configura el pin GPIO del LED IR como salida.
    bcm2835_gpio_fsel(gpioPin_, BCM2835_GPIO_FSEL_OUTP);
    // Inicia con el LED apagado (nivel bajo).
    bcm2835_gpio_write(gpioPin_, LOW);

    initialized_ = true;
    std::printf("Emisor IR inicializado en GPIO %d\n", gpioPin_);
    return true;
}

void IR_Transmitter::sendCommand(uint8_t address, uint8_t command, uint8_t repeat) {
    if (!initialized_) {
        std::printf("Error: Emisor IR no inicializado. Llama a begin() primero.\n");
        return;
    }
    engine_.sendNEC(address, command, repeat);
}

void IR_Transmitter::end() {
    if (initialized_) {
        // Apaga el LED IR.
        bcm2835_gpio_write(gpioPin_, LOW);
        // Cierra bcm2835 (inicializado por este driver).
        bcm2835_close();
        initialized_ = false;
    }
}

uint8_t IR_Transmitter::getGpioPin() const {
    return gpioPin_;
}

} // namespace IR
