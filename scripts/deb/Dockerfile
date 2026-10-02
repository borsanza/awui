# Entorno para generar el paquete .deb de StationTV (lo usa scripts/build-deb.sh). La versión de Ubuntu decide de qué
# versiones de las librerías depende el paquete: tiene que ser la del Ubuntu donde se va a instalar
ARG UBUNTU=26.04
FROM ubuntu:${UBUNTU}

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		cmake ninja-build g++ pkg-config dpkg-dev file \
		libsdl2-dev libsdl2-image-dev libcairo2-dev libpango1.0-dev nlohmann-json3-dev libgl-dev libopengl-dev \
	&& rm -rf /var/lib/apt/lists/*
