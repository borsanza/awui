#!/bin/bash
# Compila awui y los samples para Windows (64 bits) desde Linux, dentro de un contenedor de Fedora con MinGW-w64 y las
# librerías ya compiladas para Windows (scripts/windows/Dockerfile). Solo hace falta Docker.
#
#   scripts/build-windows.sh                compila en build-windows/ (preset "windows", Release)
#   scripts/build-windows.sh --clean        borra build-windows/ antes
#   scripts/build-windows.sh --installer    además, genera el instalador de StationTV (NSIS):
#                                           build-windows/StationTV-<versión>-instalador.exe
#
# Cada sample queda en build-windows/samples/<sample>/ con su .exe, las DLL que necesita y sus imágenes y ROMs, listo
# para copiar a un Windows.
set -euo pipefail

cd "$(dirname "$0")/.."
IMAGE=awui-mingw

installer=0
for arg in "$@"; do
	case "$arg" in
		--clean) rm -rf build-windows ;;
		--installer) installer=1 ;;
		*)
			echo "Opción desconocida: $arg" >&2
			exit 1
			;;
	esac
done

# La imagen se construye la primera vez (unos minutos); después Docker la reutiliza
docker build -q -t "$IMAGE" scripts/windows > /dev/null

# Con el usuario de fuera, para que build-windows/ no quede de root
docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/src" -w /src "$IMAGE" \
	bash -c 'cmake --preset windows > /dev/null && cmake --build --preset windows'

# Imágenes y ROMs: están versionadas en build/samples/ (las mismas que usa la compilación de Linux)
for dir in build/samples/*/; do
	sample=$(basename "$dir")
	[ -d "build-windows/samples/$sample" ] || continue
	for assets in images roms; do
		if [ -d "$dir$assets" ]; then
			cp -ru "$dir$assets" "build-windows/samples/$sample/"
		fi
	done
done

if [ $installer = 1 ]; then
	docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/src" -w /src/build-windows "$IMAGE" cpack
	echo "Instalador: $(ls build-windows/StationTV-*-instalador.exe)"
else
	echo "Listo: build-windows/samples/<sample>/<sample>.exe"
fi
