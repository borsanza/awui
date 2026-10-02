#!/bin/bash
# Genera el paquete .deb de StationTV dentro de un contenedor de Ubuntu (scripts/deb/Dockerfile). Solo hace falta
# Docker.
#
#   scripts/build-deb.sh                  para el mismo Ubuntu que este (o 26.04 si no es Ubuntu)
#   scripts/build-deb.sh 24.04            para otra versión de Ubuntu
#
# El paquete queda en build-package/ (o build-package-<versión>/ si es otra) y se instala con
#   sudo apt install ./build-package/stationtv_*.deb
set -euo pipefail

cd "$(dirname "$0")/.."

host=""
if [ -f /etc/os-release ]; then
	host=$(. /etc/os-release && [ "${ID:-}" = ubuntu ] && echo "${VERSION_ID:-}") || true
fi
ubuntu=${1:-${host:-26.04}}

# Para el Ubuntu de esta máquina, en build-package/ (el preset "package"); para otro, en su propia carpeta
build=build-package
[ "$ubuntu" != "$host" ] && build="build-package-$ubuntu"

image="awui-deb:$ubuntu"
docker build -q --build-arg "UBUNTU=$ubuntu" -t "$image" scripts/deb > /dev/null

docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/src" -w /src "$image" bash -c "
	cmake --preset package -B $build > /dev/null &&
	cmake --build $build &&
	cd $build && cpack"

echo
echo "Paquete: $(ls "$build"/stationtv_*.deb)"
echo "Para instalarlo: sudo apt install ./$(ls "$build"/stationtv_*.deb)"
