# 🕹️ Colección de Emuladores y Experimentos en C++

Este proyecto reúne varios experimentos personales que he ido desarrollando como hobby. Incluye desde emuladores retro hasta un sistema de widgets en OpenGL y un pequeño motor estilo Minecraft. Todo el código está escrito en C++ y pensado como un espacio para aprender, probar ideas y divertirme programando.

## 📼 Emuladores incluidos

### **Chip-8**

* Totalmente funcional.
* Compatible con la mayoría de ROMs clásicas.

### **Sega Master System**

* Implementación completa y jugable.
* Soporta gráficos, sonido (mejorable...) y controles básicos.

### **ZX Spectrum**

* Implementación parcial.
* Llega a ser funcional, pero poco testeado...

## 🎛️ Entorno de widgets en OpenGL (Apple TV-style)

Desarrollé un pequeño framework de interfaz inspirado en el diseño del Apple TV.
Incluye:

* Navegación con animaciones suaves en OpenGL.
* Sistema de widgets personalizable.
* Control mediante mando Apple IR, utilizando Arduino como receptor.

Lo utilicé para cargar mis emuladores y jugar desde un entorno más cómodo y visual.

## ⛏️ Proyecto estilo Minecraft

Un experimento inicial para crear mi propio “voxel engine”.
Se trata de una conversión a C++ de un motor que ya tenía en Three.js, con el objetivo de conseguir más rendimiento y aprender sobre estructuras para mundos infinitos.

## Anotaciones antiguas

Windows:
  winget install --id=TortoiseHg.TortoiseHg  -e
  winget install --id=Kitware.CMake  -e
  winget install --id=Ninja-build.Ninja  -e
  winget install --id=MSYS2.MSYS2  -e

  Desde terminal de msys64
    pacman -Syu
    pacman -S vim

    pacman -S mingw-w64-i686-toolchain
    pacman -S mingw-w64-i686-glew
    pacman -S mingw-w64-i686-SDL2
    pacman -S mingw-w64-i686-SDL2_image
    pacman -S mingw-w64-i686-cairo
    pacman -S mingw-w64-i686-nlohmann-json

    pacman -S mingw-w64-x86_64-toolchain
    pacman -S mingw-w64-x86_64-glew
    pacman -S mingw-w64-x86_64-SDL2
    pacman -S mingw-w64-x86_64-SDL2_image
    pacman -S mingw-w64-x86_64-cairo
    pacman -S mingw-w64-x86_64-nlohmann-json

    cd /c/awui/ext/
    ./generate-libs.sh

  Command:
    bbr.bat

Actualizar Paquetes:
  pacman -Syu


find -type f -exec wc -l {} + | sort -n
