#!/bin/bash
# Lanza con Wine un sample de la compilación de Windows (build-windows/, ver scripts/build-windows.sh), dentro de un
# contenedor de Debian (scripts/wine/Dockerfile). Solo hace falta Docker.
#
#   scripts/run-windows.sh [sample]              en tu escritorio, con sonido (por defecto, stationTV)
#   scripts/run-windows.sh --headless [sample]   sin ventana: lo deja 30 s en una pantalla virtual y guarda una
#                                                captura en build-windows/wine-<sample>.png
#
# La configuración de Wine se guarda en ~/.cache/awui/wine: la primera vez tarda un poco en crearse
set -euo pipefail

cd "$(dirname "$0")/.."
IMAGE=awui-wine

headless=0
if [ "${1:-}" = "--headless" ]; then
	headless=1
	shift
fi
sample=${1:-stationTV}

if [ ! -f "build-windows/samples/$sample/$sample.exe" ]; then
	echo "No está build-windows/samples/$sample/$sample.exe: compílalo antes con scripts/build-windows.sh" >&2
	exit 1
fi

docker build -q -t "$IMAGE" scripts/wine > /dev/null

prefix="${XDG_CACHE_HOME:-$HOME/.cache}/awui/wine"
mkdir -p "$prefix"

# Docker no pasa Ctrl+C al contenedor: al salir el script (por lo que sea) se para, y con él el programa
container="awui-wine-$$"
trap 'docker kill "$container" > /dev/null 2>&1 || true' EXIT INT TERM

args=(--rm --name "$container" -u "$(id -u):$(id -g)"
	-v "$PWD/build-windows:/app"
	-v "$prefix:/wine"
	-e HOME=/wine -e WINEPREFIX=/wine/prefix -e "WINEDEBUG=${WINEDEBUG:--all}"
	-w "/app/samples/$sample")

# Prepara la configuración de Wine la primera vez, esperando a que termine (si se corta a medias queda rota: sin
# sonido, por ejemplo). La marca .awui-ready dice que está completa; si no está, se rehace
init='if [ ! -f /wine/prefix/.awui-ready ]; then
		rm -rf /wine/prefix
		echo "Preparando Wine (solo la primera vez)..."
		wine wineboot -i > /dev/null 2>&1 && wineserver -w && touch /wine/prefix/.awui-ready
	fi'

if [ $headless = 1 ]; then
	docker run "${args[@]}" "$IMAGE" bash -c "
		Xvfb :99 -screen 0 1280x720x24 > /dev/null 2>&1 &
		export DISPLAY=:99
		sleep 1
		$init
		wine $sample.exe > /app/wine-$sample.log 2>&1 &
		sleep 30
		import -window root /app/wine-$sample.png"
	echo "Captura: build-windows/wine-$sample.png (salida del programa: build-windows/wine-$sample.log)"
	exit 0
fi

# En el escritorio: la pantalla (X11, también XWayland), el sonido (PulseAudio o PipeWire) y la tarjeta gráfica
display_args=(-e "DISPLAY=$DISPLAY" -v /tmp/.X11-unix:/tmp/.X11-unix)
if [ -n "${XAUTHORITY:-}" ] && [ -f "$XAUTHORITY" ]; then
	display_args+=(-v "$XAUTHORITY:/tmp/xauthority:ro" -e XAUTHORITY=/tmp/xauthority)
fi
runtime="${XDG_RUNTIME_DIR:-/run/user/$(id -u)}"
if [ -S "$runtime/pulse/native" ]; then
	display_args+=(-v "$runtime/pulse/native:/tmp/pulse-native" -e PULSE_SERVER=unix:/tmp/pulse-native)
fi
# Driver propietario de NVIDIA: el contenedor solo trae Mesa, que no lo sabe usar (OpenGL iría por software). Se
# montan las librerías de OpenGL del sistema, de la misma versión que el driver, y sus dispositivos
if [ -f /proc/driver/nvidia/version ]; then
	version=$(head -1 /proc/driver/nvidia/version | grep -oE '[0-9]+\.[0-9]+(\.[0-9]+)?' | head -1)
	glx=$(ldconfig -p | awk '/libGLX_nvidia\.so\.0 .*x86-64/ { print $NF; exit }')
	if [ -n "$version" ] && [ -n "$glx" ]; then
		libdir=$(dirname "$glx")
		display_args+=(-v "$libdir/libGLX_nvidia.so.$version:$libdir/libGLX_nvidia.so.0:ro")
		for lib in "$libdir"/libnvidia-*.so."$version"; do
			display_args+=(-v "$lib:$lib:ro")
		done
		for node in /dev/nvidia0 /dev/nvidiactl /dev/nvidia-modeset /dev/nvidia-uvm; do
			[ -c "$node" ] && display_args+=(--device "$node")
		done
	fi
fi
if [ -d /dev/dri ]; then
	display_args+=(--device /dev/dri)
	for node in /dev/dri/*; do
		[ -c "$node" ] && display_args+=(--group-add "$(stat -c %g "$node")")
	done
fi

# En segundo plano y con wait: así Ctrl+C llega enseguida al trap (con docker en primer plano, bash esperaría a que
# terminase)
docker run "${args[@]}" "${display_args[@]}" "$IMAGE" bash -c "$init
	wine $sample.exe" &
wait $!
