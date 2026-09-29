#!/bin/sh
#
# Builds an AppImage of xrick in two steps, called by the release
# workflow from the repository root:
#
#   docker run --rm -v "$PWD:/src" -w /src ubuntu:22.04 \
#       sh .github/scripts/linux-appimage.sh build <version> <arch>
#   sh .github/scripts/linux-appimage.sh package <version> <arch>
#
# "build" compiles xrick in an Ubuntu 22.04 container, so that it runs on
# distributions with glibc 2.35 or newer, and fills dist/AppDir-<arch>.
# "package" turns that into the AppImage, outside the container: the
# appimagetool for ARM can not run under QEMU, the one for the machine
# can pack for any architecture. <arch> is x86_64, aarch64 or armhf. SDL 1.2 comes from sdl12-compat on
# top of SDL 2; both go into the AppImage. SDL 2 loads X11, Wayland, KMSDRM
# and PipeWire, PulseAudio or ALSA from the system at runtime. The result
# is dist/xrick-<version>-<arch>.AppImage.
#
set -eu

STEP=${1:?step missing}
VERSION=${2:?version missing}
ARCH=${3:?arch missing}

SDL2_TAG=release-2.32.10
SDL12COMPAT_TAG=release-1.2.76

SRC=$(pwd)
APPDIR="$SRC/dist/AppDir-$ARCH"

if [ "$STEP" = package ]; then
    TOOL="$SRC/dist/appimagetool"
    curl -fsSL -o "$TOOL" \
        "https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-$(uname -m).AppImage"
    chmod +x "$TOOL"
    # appimagetool is itself an AppImage, run it without FUSE
    ARCH="$ARCH" APPIMAGE_EXTRACT_AND_RUN=1 "$TOOL" --no-appstream \
        "$APPDIR" "dist/xrick-$VERSION-$ARCH.AppImage"
    rm -rf "$TOOL" "$APPDIR"
    exit 0
fi
[ "$STEP" = build ] || { echo "unknown step $STEP" >&2; exit 1; }
WORK=/tmp/xrick-deps
PREFIX=/opt/sdl
JOBS=$(nproc)

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y --no-install-recommends build-essential cmake pkg-config \
    curl ca-certificates file imagemagick zlib1g-dev \
    libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxfixes-dev \
    libxss-dev libxkbcommon-dev libwayland-dev wayland-protocols libdecor-0-dev \
    libegl1-mesa-dev libgles2-mesa-dev libgl1-mesa-dev libgbm-dev libdrm-dev \
    libasound2-dev libpulse-dev libpipewire-0.3-dev libudev-dev libdbus-1-dev

mkdir -p "$WORK" "$PREFIX"
cd "$WORK"

# SDL 2; the system libraries for video and sound are loaded at runtime
curl -fsSL "https://github.com/libsdl-org/SDL/archive/refs/tags/$SDL2_TAG.tar.gz" | tar xz
cmake -S "SDL-$SDL2_TAG" -B sdl2-build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_INSTALL_LIBDIR=lib \
    -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST=OFF
cmake --build sdl2-build -j"$JOBS"
cmake --install sdl2-build

# sdl12-compat
curl -fsSL "https://github.com/libsdl-org/sdl12-compat/archive/refs/tags/$SDL12COMPAT_TAG.tar.gz" | tar xz
cmake -S "sdl12-compat-$SDL12COMPAT_TAG" -B sdl12-build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_INSTALL_LIBDIR=lib \
    -DCMAKE_PREFIX_PATH="$PREFIX" -DSDL12TESTS=OFF
cmake --build sdl12-build -j"$JOBS"
cmake --install sdl12-build
cd "$SRC"

# xrick, with zlib linked in
cmake -S source/xrick/projects/cmake -B build-release -DCMAKE_BUILD_TYPE=Release \
    -DSDL_PREFIX="$PREFIX" -DZLIB_USE_STATIC_LIBS=ON \
    -DZLIB_LIBRARY="$(gcc -print-file-name=libz.a)"
cmake --build build-release -j"$JOBS"
strip build-release/xrick

# AppDir
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/lib" "$APPDIR/usr/share/xrick"
cp build-release/xrick "$APPDIR/usr/bin/"
cp -L "$PREFIX/lib/libSDL-1.2.so.0" "$PREFIX/lib/libSDL2-2.0.so.0" "$APPDIR/usr/lib/"
strip --strip-unneeded "$APPDIR"/usr/lib/*.so.*
cp game/data.zip "$APPDIR/usr/share/xrick/"
cp -r game/lang "$APPDIR/usr/share/xrick/"
convert assets/images/xrickST.ico "$APPDIR/xrick.png"

cat > "$APPDIR/xrick.desktop" <<EOT
[Desktop Entry]
Type=Application
Name=xrick
Comment=Clone of the platform game Rick Dangerous
Exec=xrick
Icon=xrick
Categories=Game;ArcadeGame;
Terminal=false
X-AppImage-Version=$VERSION
EOT

# The AppImage is read-only: copy the data to the user's data directory,
# where the high scores can be written, and start the game from there
cat > "$APPDIR/AppRun" <<'EOT'
#!/bin/sh
HERE=$(dirname "$(readlink -f "$0")")
DATA="${XDG_DATA_HOME:-$HOME/.local/share}/xrick"
mkdir -p "$DATA"
cp -f "$HERE/usr/share/xrick/data.zip" "$DATA/"
cp -rf "$HERE/usr/share/xrick/lang" "$DATA/"
export LD_LIBRARY_PATH="$HERE/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
cd "$DATA"
exec "$HERE/usr/bin/xrick" --data "$DATA/data.zip" "$@"
EOT
chmod +x "$APPDIR/AppRun"

echo "Libraries needed by the bundled files:"
for f in "$APPDIR/usr/bin/xrick" "$APPDIR"/usr/lib/*.so.*; do
    echo "$f:"; readelf -d "$f" | grep NEEDED || true
done

# hand the results to the user outside the container
chown -R "$(stat -c %u:%g "$SRC")" "$SRC/dist" "$SRC/build-release"
