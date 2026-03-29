#!/bin/bash
#
# openfpgaOS SDK — App Packager
#
# Usage:
#   ./package_app.sh                 Package the default app (APP ?= en Makefile)
#   ./package_app.sh myapp           Package src/myapp/
#   APP=myapp ./package_app.sh       Same, via environment variable
#
set -e

GREEN='\033[92m'
CYAN='\033[96m'
YELLOW='\033[93m'
RED='\033[91m'
RESET='\033[0m'

SDK_DIR="$(cd "$(dirname "$0")" && pwd)"

# ── Resolver APP: argumento > variable de entorno > default del Makefile ──
APP_NAME="${1:-${APP:-}}"

if [ -z "$APP_NAME" ]; then
    APP_NAME=$(grep -E '^APP\s*\?=' "$SDK_DIR/Makefile" 2>/dev/null \
        | head -1 | sed 's/.*=\s*//' | tr -d '[:space:]')
    [ -z "$APP_NAME" ] && {
        echo -e "${RED}Error: no se encuentra APP ?= en el Makefile.${RESET}"
        exit 1
    }
fi

BUILD="$SDK_DIR/build/sdk"
RELEASES="$SDK_DIR/releases"

echo -e "${CYAN}=== App Packager (APP=$APP_NAME) ===${RESET}"

# ── Verificar que existe src/<app>/ ───────────────────────────────
if [ ! -d "$SDK_DIR/src/$APP_NAME" ]; then
    echo -e "${RED}Error: no existe src/$APP_NAME/${RESET}"
    exit 1
fi

# ── Build ─────────────────────────────────────────────────────────
echo "  Building release..."
make -C "$SDK_DIR" release APP="$APP_NAME"

# ── Verificar build ───────────────────────────────────────────────
if [ ! -d "$BUILD/Cores" ]; then
    echo -e "${RED}Error: build/sdk/ no encontrado tras make release.${RESET}"
    exit 1
fi

# ── Leer metadatos del core.json ──────────────────────────────────
CORE_NAME=$(ls "$BUILD/Cores/" 2>/dev/null | head -1)
[ -z "$CORE_NAME" ] && {
    echo -e "${RED}Error: no se encuentra ningún core en build/sdk/Cores/.${RESET}"
    exit 1
}

CORE_JSON="$BUILD/Cores/$CORE_NAME/core.json"
[ -f "$CORE_JSON" ] || {
    echo -e "${RED}Error: $CORE_JSON no encontrado.${RESET}"
    exit 1
}

GAME_NAME=$(python3 -c "
import json
with open('$CORE_JSON') as f:
    d = json.load(f)
print(d['core']['metadata']['description'])
" 2>/dev/null)
[ -z "$GAME_NAME" ] && {
    echo -e "${YELLOW}Warning: no se pudo leer description, usando '$APP_NAME'${RESET}"
    GAME_NAME="$APP_NAME"
}

CORE_VERSION=$(python3 -c "
import json
with open('$CORE_JSON') as f:
    d = json.load(f)
print(d['core']['metadata']['version'])
" 2>/dev/null)
[ -z "$CORE_VERSION" ] && {
    echo -e "${YELLOW}Warning: no se pudo leer version, usando '1.0.0'${RESET}"
    CORE_VERSION="1.0.0"
}

OUTPUT_ZIP="$RELEASES/${APP_NAME}-v${CORE_VERSION}.zip"
echo "  Version : $CORE_VERSION"
echo "  Output  : $OUTPUT_ZIP"
echo

# ── Verificar ELF ─────────────────────────────────────────────────
REL_ASSETS="$BUILD/Assets/raytracertnw/common"
if [ ! -f "$REL_ASSETS/${APP_NAME}.elf" ]; then
    echo -e "${RED}Error: ${APP_NAME}.elf no encontrado en $REL_ASSETS/${RESET}"
    exit 1
fi

# ── Generar INSTALL.txt ───────────────────────────────────────────
cat > "$BUILD/INSTALL.txt" << EOF
$GAME_NAME
$(printf '=%.0s' $(seq 1 ${#GAME_NAME}))
Version: $CORE_VERSION

Installation:
1. Extract this ZIP to your Analogue Pocket SD card root
2. Merge with existing folders if prompted
3. The app will appear in the Pocket menu

Save files are created automatically on first use.
EOF

# ── Crear ZIP ─────────────────────────────────────────────────────
mkdir -p "$RELEASES"
rm -f "$OUTPUT_ZIP" 2>/dev/null || true

cd "$BUILD"
zip -r "$OUTPUT_ZIP" \
    Cores/ Assets/ Platforms/ INSTALL.txt \
    -x "*.DS_Store" "Thumbs.db" 2>/dev/null
cd "$SDK_DIR"

echo -e "${GREEN}Package created: $OUTPUT_ZIP${RESET}"
echo "  Size: $(du -h "$OUTPUT_ZIP" | cut -f1)"
