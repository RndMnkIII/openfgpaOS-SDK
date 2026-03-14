#!/bin/bash
#
# openfpgaOS SDK Updater
#
# Downloads the latest SDK release and replaces sdk/, app.ld, and os.bin.
# Your source files (main.c, Makefile, dist/input.json, instances) are untouched.
#
# Usage:
#   ./update.sh                     # latest release
#   ./update.sh v0.2                # specific version tag
#

set -e

REPO="ThinkElastic/openfpgaOS-SDK"
TAG="${1:-latest}"

if [ "$TAG" = "latest" ]; then
    URL="https://github.com/${REPO}/releases/latest/download/openfpgaOS-SDK.zip"
else
    URL="https://github.com/${REPO}/releases/download/${TAG}/openfpgaOS-SDK.zip"
fi

TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

echo "openfpgaOS SDK Updater"
echo "================"
echo
echo "Downloading ${TAG} from ${REPO}..."
curl -fsSL "$URL" -o "$TMPDIR/sdk.zip"

echo "Extracting..."
unzip -qo "$TMPDIR/sdk.zip" -d "$TMPDIR/extracted"

# Find the root of the extracted archive (may be nested in a directory)
SDK_ROOT="$TMPDIR/extracted"
if [ ! -d "$SDK_ROOT/sdk" ]; then
    SDK_ROOT=$(find "$SDK_ROOT" -maxdepth 1 -type d ! -name extracted | head -1)
fi

if [ ! -d "$SDK_ROOT/sdk" ]; then
    echo "ERROR: Downloaded archive does not contain an sdk/ directory"
    exit 1
fi

# Back up current SDK version
if [ -f sdk/of.h ]; then
    OLD_VER=$(grep -o 'OF_LIBC_VERSION [0-9]*' sdk/of_libc.h 2>/dev/null | awk '{print $2}' || echo "?")
    echo "Current SDK libc version: $OLD_VER"
fi

# Replace SDK-owned files
echo "Updating sdk/..."
rm -rf sdk
cp -r "$SDK_ROOT/sdk" .

echo "Updating app.ld..."
cp "$SDK_ROOT/app.ld" .

if [ -f "$SDK_ROOT/os.bin" ]; then
    echo "Updating os.bin..."
    cp "$SDK_ROOT/os.bin" .
fi

# Update dist infrastructure (but NOT user-customized files)
for f in core.json data.json audio.json video.json variants.json interact.json; do
    if [ -f "$SDK_ROOT/dist/$f" ]; then
        cp "$SDK_ROOT/dist/$f" dist/
    fi
done

# Platform files
if [ -d "$SDK_ROOT/dist/platforms" ]; then
    cp -r "$SDK_ROOT/dist/platforms" dist/
fi
if [ -f "$SDK_ROOT/dist/icon.bin" ]; then
    cp "$SDK_ROOT/dist/icon.bin" dist/
fi

# Show what changed
NEW_VER=$(grep -o 'OF_LIBC_VERSION [0-9]*' sdk/of_libc.h 2>/dev/null | awk '{print $2}' || echo "?")
echo
echo "Updated to SDK libc version: $NEW_VER"
echo
echo "Files preserved (not overwritten):"
echo "  main.c, Makefile, dist/input.json, dist/instances/*"
echo
echo "Done. Run 'make clean && make' to rebuild."
