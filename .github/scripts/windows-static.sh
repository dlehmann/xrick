#!/bin/sh
#
# Cross-compiles a static xrick.exe (SDL 1.2 and zlib linked in) with
# MinGW-w64 in a Debian container. Called by the release workflow; run it
# from the repository root:
#
#   docker run --rm -v "$PWD:/src" -w /src debian:trixie \
#       sh .github/scripts/windows-static.sh <version>
#
# The result is dist/xrick-<version>-windows-x86_64.zip.
#
set -eu

VERSION=${1:?version missing}

SDL_REF=9c868c5ecc3e183615a4e3e5a2445f858026dec5  # SDL-1.2 main
ZLIB_VERSION=1.3.1

HOST=x86_64-w64-mingw32
SRC=$(pwd)
WORK=/tmp/xrick-deps
PREFIX=/opt/win

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y --no-install-recommends gcc-mingw-w64-x86-64 binutils-mingw-w64-x86-64 \
    cmake make curl ca-certificates zip automake

mkdir -p "$WORK" "$PREFIX"
cd "$WORK"

# zlib
curl -fsSL "https://zlib.net/fossils/zlib-$ZLIB_VERSION.tar.gz" | tar xz
cd "zlib-$ZLIB_VERSION"
make -f win32/Makefile.gcc PREFIX="$HOST-" libz.a
mkdir -p "$PREFIX/include" "$PREFIX/lib"
cp zlib.h zconf.h "$PREFIX/include/"
cp libz.a "$PREFIX/lib/"
cd "$WORK"

# SDL 1.2
curl -fsSL "https://github.com/libsdl-org/SDL-1.2/archive/$SDL_REF.tar.gz" | tar xz
cd "SDL-1.2-$SDL_REF"
cp /usr/share/automake-*/config.guess /usr/share/automake-*/config.sub build-scripts/
CFLAGS="-O2 -std=gnu11 -Wno-error=implicit-function-declaration -Wno-error=incompatible-pointer-types -Wno-error=int-conversion" \
./configure --host="$HOST" --prefix="$PREFIX" --enable-static --disable-shared
make -j"$(nproc)"
make install
cd "$SRC"

# xrick
cat > "$WORK/toolchain.cmake" <<EOT
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER $HOST-gcc)
set(CMAKE_RC_COMPILER $HOST-windres)
set(CMAKE_FIND_ROOT_PATH /usr/$HOST $PREFIX)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOT

cmake -S source/xrick/projects/cmake -B build-release -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$WORK/toolchain.cmake" \
    -DSDL_PREFIX="$PREFIX" \
    -DZLIB_LIBRARY="$PREFIX/lib/libz.a" -DZLIB_INCLUDE_DIR="$PREFIX/include" \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc" \
    -DCMAKE_C_STANDARD_LIBRARIES="-luser32 -lgdi32 -lwinmm -ldxguid -lkernel32"
cmake --build build-release -j"$(nproc)"
"$HOST-strip" build-release/xrick.exe
"$HOST-objdump" -p build-release/xrick.exe | grep 'DLL Name' || true

PKG="xrick-$VERSION-windows-x86_64"
mkdir -p "dist/$PKG"
cp build-release/xrick.exe "dist/$PKG/"
cp game/data.zip README.md CHANGELOG.md "dist/$PKG/"
cp -r game/lang "dist/$PKG/"
(cd dist && zip -r "$PKG.zip" "$PKG")
rm -rf "dist/$PKG"
