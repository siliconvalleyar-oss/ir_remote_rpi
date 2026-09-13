/**
 * @file IR_Transmitter.hpp
 * @brief Driver del emisor IR (LED infrarrojo + transistor).
 *
 * @details Encapsula el motor IR_Engine y proporciona una interfaz de alto
 * nivel para enviar comandos infrarrojos. Gestiona la inicialización de
 * bcm2835 y la configuración del pin GPIO del LED emisor.
 *
 * El GPIO del emisor se puede configurar en `config/hardware.cfg`.
 * Por defecto se usa el GPIO 22, que es un pin accesible en el conector
 * de la Raspberry Pi y permitido por el SoC para PWM (opcional).
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#ifndef IR_TRANSMITTER_HPP
#define IR_TRANSMITTER_HPP

#include <cstdint>
#include <string>
#include <map>

#include "engine/IR_Engine.hpp"

namespace IR {

/**
 * @class IR_Transmitter
 * @brief Emisor IR de alto nivel.
 *
 * @details Inicializa bcm2835, configura el GPIO del LED emisor y delega
 * la generación de la portadora y tramas NEC al motor IR_Engine.
 */
class IR_Transmitter {
public:
    /**
     * @brief Constructor.
     * @param gpioPin Pin GPIO (numeración BCM) del LED IR. Por defecto GPIO 22.
     */
    explicit IR_Transmitter(uint8_t gpioPin = 22);

    /** @brief Destructor. Cierra bcm2835. */
    ~IR_Transmitter();

    /**
     * @brief Inicializa bcm2835 y configura el GPIO como salida.
     * @return true si la inicialización fue exitosa.
     */
    bool begin();

    /**
     * @brief Envía un comando NEC por dirección y código.
     * @param address Dirección NEC (ej. 0x04 para LG).
     * @param command Código de comando (8 bits).
     * @param repeat Número de repeticiones.
     */
    void sendCommand(uint8_t address, uint8_t command, uint8_t repeat = 0);

    /**
     * @brief Cierra bcm2835 y libera recursos.
     */
    void end();

    /**
     * @brief Obtiene el número de pin GPIO configurado.
     * @return Pin GPIO del emisor.
     */
    uint8_t getGpioPin() const;

private:
    /** @brief Motor de codificación NEC. */
    IR_Engine engine_;

    /** @brief Pin GPIO del LED IR. */
    uint8_t gpioPin_;

    /** @brief Indica si bcm2835 está inicializado. */
    bool initialized_;
};

} // namespace IR

#endif // IR_TRANSMITTER_HPP
