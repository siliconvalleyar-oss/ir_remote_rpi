#!/usr/bin/env bash
# =============================================================================
# generate_basic_src.sh - Script auxiliar de generación de código fuente
# =============================================================================
# Script auxiliar del proyecto IR-LG-Remote. Su función principal es recrear
# el esqueleto mínimo de los archivos fuente (main.cpp + Device_t) cuando se
# parta de cero, evitando retipear el formato obligatorio del prompt.md.
#
# No es necesario ejecutarlo para compilar: el repositorio ya incluye todo el
# código fuente. Se ofrece como utilidad de mantenimiento/generación.
#
# Uso:
#   bash generate_basic_src.sh [--force]
#   --force  sobreescribe los archivos que ya existan (¡cuidado!).
# =============================================================================

set -euo pipefail

FORCE=0
[[ $# -ge 1 && "$1" == "--force" ]] && FORCE=1

# --- Formato obligatorio del main.cpp (ver prompt.md) --------------------------
write_file() {
    local path="$1" content="$2"
    if [[ -f "${path}" && "${FORCE}" -ne 1 ]]; then
        echo "[skip] ${path} ya existe (use --force para sobreescribir)"
        return
    fi
    mkdir -p "$(dirname "${path}")"
    printf '%s' "${content}" > "${path}"
    echo "[ok]   ${path} generado"
}

write_file "src/main.cpp" \
"#include <memory>
#include \"core/Device_t.hpp\"

int main(int argc, char** argv) {
    auto device = std::make_unique<Device::Device_t>(argc, argv);
    device->run();
    return 0;
}
"

write_file "include/core/Device_t.hpp" \
"#ifndef DEVICE_T_HPP
#define DEVICE_T_HPP

#include <memory>

namespace Device {
class Device_t {
public:
    Device_t(int argc, char** argv);
    ~Device_t();
    void run();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
} // namespace Device

#endif // DEVICE_T_HPP
"

echo "Generación de código fuente base completada."
echo "Nota: este esqueleto es mínimo; el proyecto real ya está completo en src/."