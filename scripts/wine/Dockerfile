# Wine para lanzar la compilación de Windows en Linux (lo usa scripts/run-windows.sh). Debian: el Wine de Fedora se
# queda colgado al preparar su configuración dentro de un contenedor
FROM debian:trixie

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		wine wine64 libpulse0 libgl1-mesa-dri mesa-utils xvfb xdotool imagemagick procps \
	&& rm -rf /var/lib/apt/lists/*
