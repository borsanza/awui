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

## 🔨 Compilar

Requiere CMake ≥ 3.21, Ninja y un compilador con C++20.

Dependencias en Debian/Ubuntu:

```bash
sudo apt-get install cmake ninja-build libsdl2-dev libsdl2-image-dev libglew-dev libcairo2-dev nlohmann-json3-dev libgl-dev
```

En Windows, con MSYS2 (ver paquetes más abajo).

```bash
cmake --preset release          # o: cmake --preset debug
cmake --build --preset release
```

Los ejecutables quedan en `build/samples/<sample>/` y se lanzan desde ese directorio (cargan `images/` y `roms/` con rutas relativas):

```bash
cd build/samples/stationTV && ./stationTV
```

La compilación Debug va a `build-debug/`; para ejecutarla, lánzala igualmente desde `build/samples/<sample>` (ahí están las imágenes y ROMs).

Opción `-DAWUI_WARNINGS=ON` para activar los warnings del compilador.

### Con sanitizers (desarrollo)

`-DAWUI_SANITIZE=ON` compila con AddressSanitizer y UndefinedBehaviorSanitizer: el programa se para y muestra la pila en cuanto accede fuera de memoria, usa algo ya liberado, hace un `delete` doble, etc. Va 2-3 veces más lento, así que es para desarrollo:

```bash
cmake --preset sanitize && cmake --build --preset sanitize
cd build/samples/stationTV && ../../../build-sanitize/samples/stationTV/stationTV
```

Al cerrar informa también de la memoria que no se ha liberado; para ver solo los errores: `ASAN_OPTIONS=detect_leaks=0`.

### Desde Visual Studio Code

Con la extensión C/C++ (`ms-vscode.cpptools`) y `gdb`:

* **F5** → *Depurar (Debug)*: compila en Debug y lo lanza con el depurador (puntos de ruptura, etc.).
* *Ejecutar (Release)* (en el desplegable de Ejecutar y depurar): compila en Release y lo lanza a velocidad real.
* Ambas preguntan qué programa lanzar (stationTV por defecto).
* **Ctrl+Shift+B** solo compila (Debug).

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
