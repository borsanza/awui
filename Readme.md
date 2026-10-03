# 🕹️ awui y StationTV

Experimentos personales en C++, hechos como hobby para aprender y probar ideas: una librería de interfaz en OpenGL (**awui**), un menú para la tele con emuladores retro (**StationTV**) y un pequeño motor de vóxeles al estilo Minecraft.

## 📦 Qué hay

### StationTV

Un menú a pantalla completa pensado para la tele y el mando (al estilo del Apple TV), desde el que se eligen y se juegan las ROMs. Emuladores:

- **CHIP-8**, con SuperChip, MegaChip y los programas del ETI-660. Las flechas y el botón OK del mando se ajustan solos a las teclas que usa cada juego.
- **Sega Master System**, **Game Gear** y **SG-1000**: jugables, con sonido (también el FM de la Master System japonesa), rebobinado y partida automática.
- **ZX Spectrum**: carga cintas `.tap` y arranca la ROM de cada modelo. Funciona, pero está poco probado.

### awui

La librería en la que está hecho todo: ventanas y controles en OpenGL con animaciones suaves, texto con pango (también japonés y chino), mando de juegos y el mando a distancia de Apple (con un Arduino como receptor, en `arduino/`).

### Otros

- **gameOfBlocks**: el inicio de un motor de vóxeles, pasado a C++ desde uno que tenía en Three.js para ganar rendimiento y aprender sobre mundos infinitos.
- **awuiDemo**, **awSlider** y **awTest**: pruebas de la librería.
- **tools/chip8**: desensamblador y ensamblador de CHIP-8/MegaChip, y la versión mejorada de MegaBlinky (ver su README).

## 🚀 Empezar

Dependencias en Debian/Ubuntu:

```bash
sudo apt-get install cmake ninja-build libsdl2-dev libsdl2-image-dev libcairo2-dev libpango1.0-dev nlohmann-json3-dev libgl-dev libopengl-dev whiptail
```

Para la versión de Windows hace falta además Docker (no se instala nada más: todo va en contenedores).

Lo más cómodo es el menú:

```bash
./menu.sh
```

Desde él se compila (Linux en Release, Debug o con sanitizers, Windows, el instalador de Windows, el paquete para Ubuntu, o todo), se lanza cualquier programa de cualquiera de las compilaciones y se lanza la versión de Windows con Wine. Recuerda lo último que elegiste, y Ctrl+C cierra el programa que esté en marcha y vuelve al menú. Sin `whiptail` sale un menú de texto.

También vale sin menú (`./menu.sh help`):

```bash
./menu.sh build all                # release, debug, sanitize, windows, installer, deb o all
./menu.sh run release stationTV    # release, debug o sanitize
./menu.sh wine stationTV
./menu.sh clean                    # borra lo compilado; no toca imágenes ni ROMs
```

## 🔨 Compilar a mano

Requiere CMake ≥ 3.21, Ninja y un compilador con C++20. Cada compilación tiene su preset y su carpeta:

| Preset     | Carpeta           | Para qué                                                         |
|------------|-------------------|------------------------------------------------------------------|
| `release`  | `build/`          | Jugar                                                            |
| `debug`    | `build-debug/`    | Depurar                                                          |
| `sanitize` | `build-sanitize/` | Desarrollo: AddressSanitizer + UndefinedBehaviorSanitizer        |
| `windows`  | `build-windows/`  | Windows, desde Linux (con `scripts/build-windows.sh`, ver abajo) |
| `package`  | `build-package/`  | El paquete .deb (con `scripts/build-deb.sh`, ver abajo)          |

```bash
cmake --preset release
cmake --build --preset release
```

Los programas se lanzan desde su carpeta, porque cargan `images/`, `roms/`, `lang/` y `fonts/` con rutas relativas:

```bash
cd build/samples/stationTV && ./stationTV
```

Las imágenes y las ROMs están versionadas dentro de `build/samples/`. `./menu.sh run` las enlaza la primera vez en las otras compilaciones, así que se pueden lanzar desde su propia carpeta.

La compilación con sanitizers para el programa y enseña la pila en cuanto accede fuera de memoria, usa algo ya liberado, hace un `delete` doble, etc. Va 2-3 veces más lenta. Al cerrar informa también de la memoria sin liberar; para ver solo los errores, `ASAN_OPTIONS=detect_leaks=0` (el menú ya lo pone).

`-DAWUI_WARNINGS=ON` activa los avisos del compilador.

Todo pinta con shaders y pide OpenGL 3.3 *core*; si la máquina no lo tiene (una Raspberry Pi), usa OpenGL ES 3.0. Para probar uno u otro: `AWUI_GL_PROFILE=es ./stationTV` (`core` o `es`). Al arrancar dice cuál ha conseguido ("OpenGL: ...").

## 🪟 Windows

Se compila desde Linux. Solo hace falta Docker: el script compila dentro de un contenedor de Fedora, que trae MinGW-w64 y las librerías (SDL2, cairo, pango) ya compiladas para Windows.

```bash
scripts/build-windows.sh            # o --clean para empezar de cero
```

La primera vez tarda unos minutos en preparar el contenedor. Cada programa queda en `build-windows/samples/<programa>/` con su `.exe`, las DLL que necesita, la configuración de fuentes, las imágenes y las ROMs: se copia esa carpeta entera a Windows y se lanza el `.exe` desde ella.

Para repartirlo, el instalador:

```bash
scripts/build-windows.sh --installer    # build-windows/StationTV-<versión>-installer.exe
```

Instala StationTV en *Archivos de programa*, con accesos directos en el menú Inicio y en el escritorio, y se desinstala desde *Aplicaciones*. Lleva el programa, sus DLL, los recursos y los juegos de CHIP-8 del Community Archive (CC0). Instalado, guarda los ajustes en `%APPDATA%\StationTV`, y las ROMs y las partidas van en `Documentos\StationTV` (`roms`, `saves` y `states`), donde se ven y entran en las copias de seguridad.

Para probarlo sin Windows, con Wine (en otro contenedor, de Debian):

```bash
scripts/run-windows.sh [programa]              # en el escritorio, con sonido y la tarjeta gráfica (también NVIDIA)
scripts/run-windows.sh --headless [programa]   # sin ventana: deja una captura en build-windows/wine-<programa>.png
```

La configuración de Wine se guarda en `~/.cache/awui/wine`. Para ver los mensajes de Wine: `WINEDEBUG=err+all scripts/run-windows.sh`.

En Windows también se puede compilar directamente con MSYS2 (paquetes `mingw-w64-x86_64-` de `toolchain`, `SDL2`, `SDL2_image`, `cairo`, `pango` y `nlohmann-json`) y los presets `release` o `debug`: las DLL se copian solas junto a cada ejecutable.

## 📦 Paquete para Ubuntu

```bash
scripts/build-deb.sh            # para el Ubuntu de esta máquina
scripts/build-deb.sh 24.04      # para otra versión
sudo apt install ./build-package/stationtv_*.deb
```

Se genera en un contenedor de Ubuntu (solo hace falta Docker), así que sus dependencias son las de esa versión: hay que hacerlo para la versión donde se va a instalar. Instala `stationtv` (también en el menú de aplicaciones) con los recursos en `/usr/share/stationtv` y los juegos de CHIP-8 del [Chip-8 Community Archive](https://github.com/JohnEarnest/chip8Archive), que son de dominio público (CC0). Las demás ROMs no se pueden distribuir: cada uno pone las suyas en `~/.local/share/stationtv/roms/<sistema>/` (ver "Dónde están las ROMs").

## 🧑‍💻 Visual Studio Code

Con la extensión C/C++ (`ms-vscode.cpptools`) y `gdb`:

- **F5** → *Depurar (Debug)*: compila en Debug y lo lanza con el depurador.
- *Ejecutar (Release)*, en el desplegable de Ejecutar y depurar: compila en Release y lo lanza a velocidad real.
- Las dos preguntan qué programa lanzar (stationTV por defecto).
- **Ctrl+Shift+B** solo compila (Debug).

## 🎮 Usar StationTV

### Teclas

En los menús, las flechas, Enter (OK) y Escape (volver; dentro de un juego, vuelve al menú). Re Pág/Av Pág, Inicio y Fin para moverse por las listas largas. La rueda del ratón también sirve.

| Dónde         | Tecla                                   | Qué hace                                     |
|---------------|-----------------------------------------|----------------------------------------------|
| Siempre       | F11                                     | Pantalla completa                            |
|               | F10                                     | Sincronización vertical (vsync)              |
| CHIP-8        | 1234 / QWER / ASDF / ZXCV               | El teclado hexadecimal del CHIP-8            |
|               | Flechas y Enter                         | Las teclas de cada juego (se detectan solas) |
|               | I                                       | Invertir los colores                         |
| Master System | WASD, G (botón 1) y H (botón 2)         | Mando 1                                      |
|               | Flechas; 1 o 3 y 2 del teclado numérico | Mando 2                                      |
|               | Espacio                                 | Pausa                                        |
|               | Retroceso                               | Reinicio                                     |
|               | Q / E                                   | Rebobinar / avanzar rápido                   |
|               | 1 a 4                                   | Silenciar o activar cada canal de sonido     |
| ZX Spectrum   | El teclado del ordenador                | El del Spectrum                              |
|               | F2 / F4                                 | Guardar / cargar el estado                   |
|               | F8                                      | Carga rápida de la cinta                     |
|               | F9                                      | Rebobinar la cinta                           |

### Dónde están las ROMs

En la lista se ven juntas dos carpetas, con una subcarpeta por sistema (`chip8`, `mastersystem`, `gamegear`, `sg1000`, `zxspectrum`):

- **La del usuario:** `~/.local/share/stationtv/roms/` (o `$XDG_DATA_HOME/stationtv/roms`; en Windows, `Documentos\StationTV\roms`). Para usar otra, añade `"romsDirectory": "/ruta"` a `settings.json`.
- **La del programa:** `roms/` junto a sus recursos (al compilar, `build/samples/stationTV/roms`). Solo trae los juegos de CHIP-8 del Community Archive, los únicos que se pueden redistribuir.

Al arrancar se crea en cada una la subcarpeta de cada sistema, para que se sepa dónde va cada juego (en la del programa, solo si se puede escribir en ella: no instalado). Las que estén vacías no salen en el menú. Si un juego está en las dos, vale el del usuario. Para el Spectrum, la ROM de cada modelo va en la carpeta `zxspectrum` (`48.rom`, `128.rom`...), y una cinta en `zxspectrum/<modelo>/` arranca con ese modelo.

### Ajustes

El engranaje de arriba a la derecha abre los ajustes: idioma, continuar partidas, reloj, pantalla completa, vsync, estadísticas en pantalla, sonido, volumen, el FM y los canales de la Master System, y si los juegos de CHIP-8 vuelven a empezar al terminar. Se guardan en `settings.json`, junto al programa si se puede escribir ahí (al compilarlo, o en una copia portable); si no (instalado), en `~/.config/stationtv/` (en Windows, `%APPDATA%\StationTV`).

### Partidas guardadas

StationTV guarda las partidas en la carpeta de datos del usuario, `~/.local/share/stationtv/` (o `$XDG_DATA_HOME/stationtv`; en Windows, `Documentos\StationTV`), por sistema y juego, en dos carpetas:

- **`saves/`:** lo que guarda el propio juego, la RAM del cartucho (`.sav`). Por ejemplo, `roms/mastersystem/Golvellius.sms` → `saves/mastersystem/Golvellius.sav`.
- **`states/`:** los estados del emulador, con la CPU, la memoria y todo lo demás: los que se guardan con una tecla (`.state`) y la partida automática al salir (`.autostate`).

Así las ROMs pueden estar en una carpeta de solo lectura (NAS, pendrive...).

Las partidas de antes se siguen encontrando: las que estén junto a la ROM se leen y al guardar pasan a la carpeta nueva, y las de versiones anteriores de StationTV se mueven solas a `saves/` y `states/` al arrancar. Para usar otra carpeta de datos, añade `"saveDirectory": "/ruta"` a `settings.json` (dentro irán `saves/` y `states/`).

## 🗂️ El repositorio

```text
libawui/awui/        la librería: interfaz, dibujo, emuladores (Emulation/) y los menús de StationTV (UI/Station)
samples/             los programas: stationTV, gameOfBlocks, awuiDemo, awSlider, awTest
third_party/         código de terceros sin modificar (emu2413, el chip FM de la Master System)
build/samples/       imágenes de los programas y los juegos CC0 de CHIP-8 (versionados aquí; el resto de build/ no)
cmake/               compilación para Windows (toolchain de MinGW y copia de las DLL)
scripts/             compilar para Windows, lanzarlo con Wine y generar el paquete .deb, con sus contenedores
tools/chip8/         herramientas de CHIP-8
arduino/             receptor del mando a distancia de Apple
doc/                 documentación de los sistemas emulados
menu.sh              el menú
TODO.md              lo que queda por hacer
```

## ✍️ Convenciones de código

- **Miembros de datos de una clase:** `m_nombre` (`m_width`, `m_saveData`). Estáticos: `s_nombre` (`s_formsList`). No se usa `this->` salvo que haga falta.
- **Campos de un `struct` de datos** (los `saveData` de los emuladores, por ejemplo): sin prefijo (`m_saveData.line`).
- **Constantes** (`static constexpr`): en PascalCase (`TicksPerSecond`).
- **Nada empieza por `_`:** los nombres con `_` y mayúscula, o con `__`, están reservados para el compilador.
