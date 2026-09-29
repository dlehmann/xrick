#!/bin/sh
#
# Builds xrick.app for macOS as a universal binary (Apple Silicon and
# Intel). Called by the release workflow on a macOS runner; run it from
# the repository root:
#
#   sh .github/scripts/macos-app.sh <version>
#
# SDL 1.2 does not run on current macOS, so sdl12-compat is linked in
# statically. It loads SDL 2 at runtime, whose library goes into the app
# bundle next to the binary. The result is
# dist/xrick-<version>-macos-universal.zip.
#
set -eu

VERSION=${1:?version missing}

SDL2_TAG=release-2.32.10
SDL12COMPAT_TAG=release-1.2.76

export MACOSX_DEPLOYMENT_TARGET=11.0
ARCHS="arm64;x86_64"

SRC=$(pwd)
WORK="$SRC/.deps"
PREFIX="$WORK/prefix"
# keep Homebrew's libraries (e.g. its arm64-only zlib) out of the build
IGNORE="/opt/homebrew;/usr/local"
JOBS=$(sysctl -n hw.ncpu)

mkdir -p "$WORK" "$PREFIX"
cd "$WORK"

# SDL 2
curl -fsSL "https://github.com/libsdl-org/SDL/archive/refs/tags/$SDL2_TAG.tar.gz" | tar xz
cmake -S "SDL-$SDL2_TAG" -B sdl2-build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHS" -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_IGNORE_PREFIX_PATH="$IGNORE" \
    -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST=OFF
cmake --build sdl2-build -j"$JOBS"
cmake --install sdl2-build

# sdl12-compat, as a static library
curl -fsSL "https://github.com/libsdl-org/sdl12-compat/archive/refs/tags/$SDL12COMPAT_TAG.tar.gz" | tar xz
cmake -S "sdl12-compat-$SDL12COMPAT_TAG" -B sdl12-build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHS" -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_PREFIX_PATH="$PREFIX" -DCMAKE_IGNORE_PREFIX_PATH="$IGNORE" \
    -DSDL12TESTS=OFF -DSDL12DEVEL=ON -DSTATICDEVEL=ON
cmake --build sdl12-build -j"$JOBS"
cmake --install sdl12-build
# make FindSDL pick the static library
rm -f "$PREFIX"/lib/libSDL-*.dylib "$PREFIX"/lib/libSDL.dylib
cd "$SRC"

# xrick
cmake -S source/xrick/projects/cmake -B build-release -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHS" \
    -DCMAKE_IGNORE_PREFIX_PATH="$IGNORE" -DCMAKE_FIND_FRAMEWORK=NEVER \
    -DSDL_PREFIX="$PREFIX"
cmake --build build-release -j"$JOBS"
lipo -info build-release/xrick.app/Contents/MacOS/xrick
otool -L build-release/xrick.app/Contents/MacOS/xrick

# app bundle: a launcher script copies the data to Application Support,
# where the high scores can be written, and starts the game from there
APP=dist/xrick.app
rm -rf dist
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
cp build-release/xrick.app/Contents/MacOS/xrick "$APP/Contents/MacOS/xrick-bin"
cp "$PREFIX/lib/libSDL2-2.0.0.dylib" "$APP/Contents/MacOS/"
cp game/data.zip "$APP/Contents/Resources/"
cp -R game/lang "$APP/Contents/Resources/"
sips -s format icns assets/images/xrickST.ico --out "$APP/Contents/Resources/xrick.icns" \
    || echo "no icon, continuing"

cat > "$APP/Contents/MacOS/xrick" <<'EOT'
#!/bin/sh
HERE=$(cd "$(dirname "$0")" && pwd)
RES="$HERE/../Resources"
DATA="$HOME/Library/Application Support/xrick"
mkdir -p "$DATA"
cp -f "$RES/data.zip" "$DATA/"
cp -Rf "$RES/lang" "$DATA/"
cd "$DATA"
exec "$HERE/xrick-bin" --data "$DATA/data.zip" "$@"
EOT
chmod +x "$APP/Contents/MacOS/xrick"

cat > "$APP/Contents/Info.plist" <<EOT
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key><string>en</string>
    <key>CFBundleExecutable</key><string>xrick</string>
    <key>CFBundleIconFile</key><string>xrick</string>
    <key>CFBundleIdentifier</key><string>io.github.dlehmann.xrick</string>
    <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
    <key>CFBundleName</key><string>xrick</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleShortVersionString</key><string>$VERSION</string>
    <key>CFBundleVersion</key><string>$VERSION</string>
    <key>LSMinimumSystemVersion</key><string>$MACOSX_DEPLOYMENT_TARGET</string>
    <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
EOT

# ad-hoc signature, Apple Silicon does not start unsigned code
codesign --force --sign - "$APP/Contents/MacOS/libSDL2-2.0.0.dylib"
codesign --force --sign - "$APP/Contents/MacOS/xrick-bin"
codesign --force --sign - "$APP"
codesign --verify --verbose "$APP"

PKG="xrick-$VERSION-macos-universal"
mkdir -p "dist/$PKG"
mv "$APP" "dist/$PKG/"
cp README.md CHANGELOG.md "dist/$PKG/"
(cd dist && ditto -c -k --keepParent "$PKG" "$PKG.zip")
rm -rf "dist/$PKG"
