/**
 * @file IR_Receiver.hpp
 * @brief Driver del receptor IR (VS1838B) - modo aprendizaje.
 *
 * @details Implementa la recepción y decodificación de señales infrarrojas
 * NEC mediante un sensor VS1838B conectado a un GPIO. Se usa en el modo
 * `--learn` para capturar códigos de un control remoto real y guardarlos
 * en `config/ir_codes.cfg`.
 *
 * El GPIO del receptor se puede configurar en `config/hardware.cfg`.
 * Por defecto se usa el GPIO 17.
 *
 * @author Proyecto IR-LG-Remote
 * @version 0.1.0
 */

#ifndef IR_RECEIVER_HPP
#define IR_RECEIVER_HPP

#include <cstdint>
#include <string>
#include <functional>

namespace IR {

/**
 * @class IR_Receiver
 * @brief Receptor IR NEC con capacidad de aprendizaje.
 *
 * @details Lee un GPIO (VS1838B) y decodifica tramas NEC para extraer
 * la dirección y el comando. La captura se basa en polling de alta
 * frecuencia con medición de duraciones de pulso/espacio mediante
 * `bcm2835_delayMicroseconds` y contadores.
 */
class IR_Receiver {
public:
    /**
     * @brief Callback que se invoca al decodificar una trama completa.
     * @param address Dirección decodificada (16 bits NEC).
     * @param command Comando decodificado (8 bits).
     */
    using DecodeCallback = std::function<void(uint16_t address, uint8_t command)>;

    /**
     * @brief Constructor.
     * @param gpioPin Pin GPIO (numeración BCM) del sensor VS1838B. Por defecto GPIO 17.
     */
    explicit IR_Receiver(uint8_t gpioPin = 17);

    /** @brief Destructor. */
    ~IR_Receiver() = default;

    /**
     * @brief Inicializa bcm2835 y configura el GPIO como entrada.
     * @return true si la inicialización fue exitosa.
     */
    bool begin();

    /**
     * @brief Captura una única trama IR y la decodifica.
     * @param[out] address Dirección decodificada.
     * @param[out] command Comando decodificado.
     * @return true si se decodificó una trama válida.
     */
    bool learn(uint16_t& address, uint8_t& command);

    /**
     * @brief Obtiene el número de pin GPIO configurado.
     * @return Pin GPIO del receptor.
     */
    uint8_t getGpioPin() const;

private:
    /** @brief Lee el nivel lógico del GPIO y mide el tiempo de espera. */
    uint32_t awaitPinChange(bool expected, uint32_t maxUs);

    /** @brief Pin GPIO del sensor VS1838B. */
    uint8_t pin_;
};

} // namespace IR

#endif // IR_RECEIVER_HPP
