#!/usr/bin/env bash
# =============================================================================
# setup_git.sh - Configura el repositorio Git de IR-LG-Remote
# =============================================================================
# Script interactivo que:
#   1. Inicializa el repositorio local (si no existe) y crea el commit inicial.
#   2. Pregunta el usuario, nombre y visibilidad del repositorio.
#   3. Crea el repositorio remoto con GitHub CLI (gh) o muestra los pasos
#      manuales si no está disponible.
#   4. Hace push de main y crea el tag v<VERSION>.
#
# Requiere que las credenciales de GitHub estén configuradas:
#   - GitHub CLI:  gh auth login   (o token en $GH_TOKEN)
#   - Git:         git config --global credential.helper store
#
# Uso:
#   bash scripts/setup_git.sh
# =============================================================================

set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'
ok()  { echo -e "${GREEN}[git]${NC} $*"; }
warn(){ echo -e "${YELLOW}[git]${NC} $*"; }
die() { echo -e "${RED}[git]${NC} $*" >&2; exit 1; }

VERSION="$(cat VERSION 2>/dev/null || echo 0.0.0)"
REPO_NAME="ir_remote_rpi"
PROJECT_NAME="IR-LG-Remote"

echo "============================================================"
echo "  Configuración de Git para ${PROJECT_NAME} (${REPO_NAME})"
echo "============================================================"

# ---- 1. Inicializar el repositorio local ------------------------------------
if [[ -d .git ]]; then
    ok "Repositorio local ya inicializado. Se conserva el historial."
else
    ok "Inicializando repositorio local..."
    git init -b main
fi

# ---- 2. Recoger datos del usuario --------------------------------------------
read -rp "Plataforma (github/gitlab) [github]: " PLATFORM
[[ -z "${PLATFORM}" ]] && PLATFORM="github"

GIT_CONFIG_USER="$(git config user.name 2>/dev/null || true)"
[[ -z "${GIT_CONFIG_USER}" ]] && GIT_CONFIG_USER="${USER:-$(whoami)}"
read -rp "Nombre de usuario de ${PLATFORM} [${GIT_CONFIG_USER}]: " USERNAME
[[ -z "${USERNAME}" ]] && USERNAME="${GIT_CONFIG_USER}"
[[ -z "${USERNAME}" ]] && die "Se necesita el nombre de usuario."

read -rp "Nombre del repositorio [${REPO_NAME}]: " REPO
[[ -z "${REPO}" ]] && REPO="${REPO_NAME}"

read -rp "Visibilidad (public/private) [private]: " VISIBILITY
[[ -z "${VISIBILITY}" ]] && VISIBILITY="private"

# ---- 3. Configurar identidad de Git si falta ---------------------------------
[[ -z "$(git config user.name 2>/dev/null || true)" ]] && git config user.name "${USERNAME}"
if [[ -z "$(git config user.email 2>/dev/null || true)" ]]; then
    read -rp "Email para Git (p.ej. you@example.com) [${USERNAME}@users.noreply.github.com]: " EMAIL
    [[ -z "${EMAIL}" ]] && EMAIL="${USERNAME}@users.noreply.github.com"
    git config user.email "${EMAIL}"
fi

# ---- 4. Commit inicial (si el índice está vacío) ------------------------------
if git rev-parse -q --verify HEAD >/dev/null 2>&1; then
    ok "Ya existe el commit inicial."
else
    ok "Creando commit inicial..."
    git add .
    git commit -m "Initial commit: IR-LG-Remote" || warn "Nada que añadir en el commit inicial."
fi

# ---- 5. Crear el repositorio remoto -------------------------------------------
REMOTE_URL="https://${PLATFORM}.com/${USERNAME}/${REPO}.git"

if command -v gh >/dev/null 2>&1 && [[ "${PLATFORM}" == "github" ]]; then
    ok "GitHub CLI disponible. Creando repositorio '${REPO}' (${VISIBILITY})..."
    if gh repo create "${REPO}" --"${VISIBILITY}" --source=. --remote=origin --push; then
        ok "Repositorio creado y contenido subido."
        # La URL del remote la fija gh; la mostramos para referencia.
        ok "Remote: $(git remote get-url origin)"
    else
        warn "gh repo create falló. Compruebe la autenticación:"
        warn "    gh auth login   (o export GH_TOKEN=...)"
        warn "También puede crear el repo manualmente y volver a ejecutar este script."
    fi
elif ! git remote get-url origin >/dev/null 2>&1; then
    warn "gh no está disponible o la plataforma es ${PLATFORM}. Configure el remoto manualmente:"
    echo ""
    echo "   1) Cree el repositorio ${USERNAME}/${REPO} en ${PLATFORM^}.com"
    echo "      (visibilidad: ${VISIBILITY})"
    echo "   2) Añada el remoto y suba el contenido:"
    echo "        git remote add origin ${REMOTE_URL}"
    echo "        git push -u origin main"
    echo ""
    read -rp "¿Quiere que añada ahora el remote origin? [y/N]: " ADD_REMOTE
    if [[ "${ADD_REMOTE}" =~ ^[yY]$ ]]; then
        git remote add origin "${REMOTE_URL}"
        ok "Remote añadido: ${REMOTE_URL}"
    fi
fi

# ---- 6. Push de main y tag de versión -----------------------------------------
if git remote get-url origin >/dev/null 2>&1; then
    ok "Haciendo push de main..."
    git push -u origin main || warn "El push falló. Revise las credenciales o el remote."
    if [[ -z "$(git tag -l "v${VERSION}")" ]]; then
        ok "Creando y subiendo el tag v${VERSION}..."
        git tag "v${VERSION}"
        git push origin "v${VERSION}" || true
    fi
    ok "Listo: https://${PLATFORM}.com/${USERNAME}/${REPO}"
else
    warn "No hay remote configurado; el repositorio queda local."
    warn "Siga los pasos manuales de arriba para publicarlo."
fi

ok "Proceso de configuración Git completado."