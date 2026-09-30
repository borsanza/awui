# Prompt

Revisa bien Core, Drawing, Effects, IO, OpenGL y Windows, ademas de las clases que hay en la raiz de awi, quiero que lo revises bien y me digas mejoras, problemas y si faltan componentes logicos. Replantea tambien si los ficheros y directorios estan como te gustarian. Con lo que saques, actualiza el TODO.md

# Pendiente

Cosas vistas en las revisiones que quedan por arreglar. Al hacer una, se borra de aquí (el historial queda en git).

## Fallos con efecto hoy

- **Teclas mantenidas:** `Form::ProcessEvents` no mira `event.key.repeat`, así que al mantener pulsada una tecla llegan pulsaciones repetidas. En el Spectrum, mantener F8 activa y desactiva el modo rápido sin parar, y F2 guarda el estado varias veces. La repetición sí conviene para moverse por los menús (botones del mando a distancia), pero no para las teclas de función ni para los emuladores.
- **Pérdida de foco de la ventana:** no se atiende `SDL_WINDOWEVENT_FOCUS_LOST`. Si se cambia de ventana (Alt+Tab) con una tecla o un botón pulsados, se quedan pulsados (`Form::s_buttonsPad1/2`, las teclas del Spectrum). Tampoco se pausa nada al minimizar.
- **Bucle sin límite:** no hay `SDL_Delay` en ningún sitio. Sin vsync, o con la ventana minimizada (donde el vsync no frena), el programa usa el 100 % de un núcleo.

## Fallos latentes

Nadie los usa hoy, pero fallarán en cuanto se usen.

- **`FileStream`:**
  - **Modos `Create`, `CreateNew` y `OpenOrCreate`** ([FileStream.cpp](libawui/awui/IO/FileStream.cpp)): los dos primeros abren con `"rb"` (solo lectura, sin crear nada) y el tercero con `"r+b"`, que falla si el fichero no existe. Deberían usar `"w+b"` y, para `CreateNew`, `"w+bx"` (falla si ya existe).
  - **Cambiar entre leer y escribir** sin un `fseek` en medio es comportamiento indefinido en C.
  - **La posición avanza aunque la lectura o escritura falle,** y en modo `Append` no refleja dónde se escribe de verdad.
- **Herencia privada:** `FileStream` y `MemoryStream` heredan de `Stream` en privado (`class FileStream : Stream`), así que no se pueden usar como `Stream *`.
- **Clases copiables que poseen recursos:** `Image`, `MemoryStream`, `FileStream` y `Control` se pueden copiar, y la copia liberaría dos veces el buffer, el `FILE *` o los hijos. Hay que borrar el constructor de copia y la asignación (`= delete`).
- **Bucles infinitos:**
  - **`String::Split("")`:** con un delimitador vacío nunca avanza. Además, trata distinto los vacíos: `"a,b,"` da `[a, b]` pero `",a"` da `["", a]`.
  - **`EffectBounce::Calculate`** con `p < -0.09`. Un porcentaje que se pasa de rango por un frame lento colgaría el programa.
  - **`Bitmap` en modo `Tile`** si los márgenes fijos suman el ancho o el alto de la textura o más: el paso es 0. Además, la última baldosa de cada fila se estira en vez de recortarse (no se recalculan las coordenadas de textura).
- **`Graphics::DrawImage` con tamaño escala al revés** ([Graphics.cpp](libawui/awui/Drawing/Graphics.cpp)): usa `imagen/destino` en vez de `destino/imagen`, así que pedir el doble dibuja a la mitad.
- **`Graphics::FromImage`:** devuelve un `new Graphics` que usa el contexto de cairo de la imagen. No está claro quién lo libera, y si la imagen se borra antes el `Graphics` queda colgando.
- **`Color`:**
  - **Dos interpretaciones del mismo número:** `Color(uint32_t)` lo lee como RGBA y `FromArgb(uint32_t)` como ARGB ([Color.cpp](libawui/awui/Drawing/Color.cpp)).
  - **`Color(float…)` trunca en vez de redondear:** 0.5 da 127 y 0.999 da 254.
- **`ColorF` (además de la escala, ver "Mejoras"):**
  - **`GetBrightness`/`GetHue`/`GetSaturation`** pasan las componentes a `int`, y con valores de 0 a 1 salen siempre 0.
  - **`ToArgb`** desborda un `int` con alfa 255.
  - **`FromArgb(int)`** falla con alfa de 128 o más, porque el número es negativo y `%` da restos negativos.
- **`Random::Next(min, max)`** con `max <= min` es comportamiento indefinido (`uniform_int_distribution` con `a > b`).
- **Eventos del mando con varios formularios** ([Application.cpp](libawui/awui/UI/Application.cpp)): se procesan dentro del bucle de formularios. Con más de uno, cada pulsación llegaría varias veces y `Controller::Refresh` se ejecutaría una vez por formulario.
- **`Stats` con varios formularios:** es un único control que cada `Form` añade como hijo. El segundo formulario falla en el `assert` de `AddWidget`, porque ya tiene padre.
- **`Application::Run(NULL)`:** el valor por defecto `NULL` hace que falle en `form->Init()`. Además, `SDL_Quit` se llama dos veces (con `atexit` y al final).
- **`GL::FillRectangle` toma el extremo como incluido y `Bitmap` como excluido.** `Control::OnPaintPre` rellena el fondo con `FillRectangle(0, 0, ancho, alto)`, que pinta un píxel de más; ahora mismo lo tapa el recorte (scissor). Lo mismo pasa con `Rectangle::GetRight`/`GetBottom` (incluidos, `x + ancho - 1`) usados con coordenadas `float`. Hay que elegir un criterio (lo normal es el extremo excluido) y usarlo en todas partes.
- **Texturas borradas sin contexto:** los destructores de `Image` y `Bitmap` llaman a `glDeleteTextures`. Si mueren después de destruir el contexto de OpenGL (al salir), esa llamada no tiene contexto.
- **`Image`:**
  - `SetPixel` no comprueba los límites.
  - `Clear` pinta negro opaco, no transparente.
- **`Point::GetRight` / `GetBottom`** devuelven `x` / `y`, lo que no tiene sentido en un punto.
- **`Button`:** los "exit listeners" se guardan pero nunca se llaman, y `m_text` y `m_nextId` no se usan.
- **`DateTime` en UTC:** `GetHour`/`GetMinute` salen del reloj del sistema sin zona horaria, así que dan la hora UTC. Ahora no los usa nadie (el reloj de StationUI usa `localtime`).
- **`settings.json` roto:** si no se puede leer, se trata como vacío y el siguiente guardado lo sobrescribe. Sería mejor renombrarlo a `settings.json.bad` antes de empezar de cero.

## Mejoras de diseño

### Estructura

- **Recursos relativos al directorio de trabajo:** `images/button.png` (en `Control::GetSelectedBitmap`), `./images/*.jpg`, `roms/zxspectrum/48.rom`, `lang/`… Si se arranca el programa desde otra carpeta, no encuentra nada. Hace falta una carpeta de recursos (con `SDL_GetBasePath` o configurable).
- **`Object` donde no hace falta:** `Convert`, `Pen`, `Graphics`, `Effect`, `Application` y `Controller` heredan de `Object` (vtable) sin usarlo. `Convert` y `Application` solo tienen métodos estáticos.
- **Escalas de `ColorF`:** `FromArgb` recorta de 0 a 255, pero `Bitmap` e `ImageFader` lo usan de 0 a 1 (`glColor4f`), y `Gradient` de 0 a 255 (`glColor4ub`). Hay que unificar el rango (lo natural es 0 a 1).
- **Tres formas de avisar de eventos:** interfaces (`IRemoteListener`, `IExitListener`), `std::function` (en el código nuevo) y métodos virtuales, cada una en una parte del código. Conviene elegir una.

### Pintado

- **OpenGL antiguo:** todo usa el modo inmediato (`glBegin`/`glEnd`, sin shaders), que no existe en perfiles modernos ni en OpenGL ES. Importa si algún día stationTV va a una Raspberry Pi o a una tele Android. [Shader.cpp](libawui/awui/OpenGL/Shader.cpp) es un experimento sin usar: llama a `glewInit` en el constructor y carga un `shader.glfs` fijo.
- **Estado de OpenGL a mano:** cada `DrawImageGL` y `Bitmap::OnPaint` consulta y restaura `GL_TEXTURE_2D`, `GL_BLEND` y `GL_DEPTH_TEST` con `glIsEnabled`.
- **Dos formas de mezclar:** `Image` (cairo) sube el alfa premultiplicado y `Bitmap` (SDL_image) sin premultiplicar, cada uno con su `glBlendFunc`.
- **`OnPaint(OpenGL::GL *gl)`** recibe siempre `NULL`: el parámetro no sirve.
- **`Refresh()` y `m_needRefresh`** no se usan para nada: se repinta todo en cada frame. O se quitan, o se usan para no repintar si nada cambia (ahorra consumo en la tele).
- **Carga de imágenes en el pintado:** `Bitmap::Load` hace `IMG_Load` dentro de `OnPaint`. Al pasar a una página con muchas carátulas, se decodifican todas en el mismo frame y hay un tirón. Se podrían decodificar en un hilo y subir la textura cuando esté lista. Además, sin *mipmaps* las carátulas reducidas se ven con dientes de sierra.
- **`Bitmap` es un `Control` completo,** pero se usa sobre todo como recurso de imagen (fondos, marco de selección, `ImageFader`): mezcla widget y recurso.
- **Texturas repetidas:** dos `Bitmap` del mismo fichero lo cargan dos veces.
- **`Label`:** crea una `Image` y una textura nuevas en cada `SetText`, `SetForeColor` o `SetFont`, aunque el valor no cambie.
- **Memoria por control:** cada `Control` reserva con `new` su `Font` y su `MouseEventArgs`. Podrían ser miembros normales.

### Animación y entrada

- **`Effects` sin usar en la interfaz:**
  - Hay curvas de animación (lineal, cuádrica, cúbica…), pero `Control`, `SelectionFrame`, `Gradient` y `LabelButton` usan cada uno su propio `Interpolate` con constantes distintas. Solo las usan `SliderBrowser` y un sample.
  - `EffectExpo` es `p⁶`, no una exponencial.
  - `EffectIn`/`Out`/`InOut` son clases sin estado que podrían ser funciones.
  - Cada `Effect` guarda un `String` con su nombre.
- **Suavizado dependiente de los frames:** `Control::OnTickPre` y `SelectionFrame` usan `percent = 10 * deltaSeconds`, que es una aproximación lineal. Con frames lentos cambia la curva. Lo correcto es `1 - exp(-10 * deltaSeconds)` (como ya hace `Gradient` con `powf`).
- **Animaciones congeladas:** un control invisible no recibe `OnTick`, así que su animación se congela y salta al volver a mostrarse.
- **Navegación con las flechas:** el código de las cuatro direcciones en `Control::OnRemoteKeyPress` está repetido casi igual, y en cada pulsación recorre todo el árbol para recoger los controles seleccionables.
- **Teclas por código de tecla (`SDL_Keycode`) y no por posición (`SDL_Scancode`):** en un teclado no inglés, las teclas del emulador cambian de sitio (por ejemplo, las comillas del Spectrum con teclado español).
- **Mando:**
  - Solo se lee el stick izquierdo, sin zona muerta, y los gatillos se ignoran.
  - Los mandos que SDL no reconoce como *GameController* (sin entrada en su base de datos) no funcionan: se descartan los eventos `SDL_JOY*`.
  - Solo hay dos mandos (`Form::s_buttonsPad1/2`, estáticos).
  - `RemoteButtons` mezcla el mando a distancia y el de SNES en los mismos bits (`SNES_Y == Ok`, `SNES_SELECT == Menu`).

### Otros

- **`Configuration`:** cada `Write` reescribe el fichero entero, y cada `Read` de una clave que falta también escribe. Al arrancar se guarda el fichero una vez por ajuste. Mejor marcarlo como modificado y guardar al final o tras un rato.
- **`Console` y `TextWriter` con formato de `printf` sin comprobar:** `Console::WriteLine("50%")` interpreta el `%` como formato. Con `__attribute__((format(printf, …)))` el compilador avisaría de los errores de formato.
- **`String` trabaja en bytes, no en caracteres:** `GetLength` y `Substring` pueden cortar una letra UTF-8 por la mitad. Además, `operator[]` no comprueba límites, `IndexOf(String)` con inicio negativo devuelve -1 (el de `char` empieza en 0), y el constructor implícito desde `char` permite conversiones inesperadas.
- **`Stats`:** se configura con `#define` en la cabecera (`SHOW_FPS`…), así que los FPS se ven siempre. Debería ser un ajuste. Además, es un singleton que nunca se libera.
- **`ImageFader`:** guarda `Bitmap *` sin ser su dueño. Si alguien borra la imagen sin llamar antes a `Clear`, queda colgando.
- **Sample `awTest/test2`:** mide tiempos con `DateTime::GetNow` (reloj del sistema, que puede saltar). Debería usar `ChronoLap`.

## Ficheros y directorios

Cómo lo organizaría. Son cambios de sitio y de nombre, sin tocar el comportamiento, y conviene hacerlos cada uno en su propio commit (con `git mv`) para no perder el historial.

### Separar librería, emuladores y aplicación

Hoy todo está en una única `libawui.so`: la interfaz, los emuladores, el motor 3D de gameOfBlocks y los menús de stationTV. Cualquier sample enlaza con todo.

```text
libawui/awui/          librería de interfaz: String, IO, Drawing, OpenGL, UI...
libemulation/          núcleos de los emuladores (hoy Emulation/): Z80, Master System, Spectrum, Chip-8, Common
libgob/                motor 3D (hoy awui/GOB/), solo lo usa gameOfBlocks
samples/stationTV/     la aplicación: menús (hoy UI/Station), controles de los emuladores
                       (hoy UI/Emulators), formArcade, main, lang, menu-settings.json
third_party/emu2413/   código de terceros sin modificar (hoy Emulation/MasterSystem/emu2413)
```

- **Dependencias en su sitio:** la librería de interfaz no sabría nada de ROMs, partidas ni ajustes de stationTV, y los emuladores se podrían probar sin ventana (como ya hacen los arneses).
- **Código de terceros aparte:** con emu2413 en `third_party/`, la excepción de UBSan de [libawui/CMakeLists.txt](libawui/CMakeLists.txt) apunta a una carpeta en vez de a un fichero dentro de nuestro código.

### Raíz del repositorio

- **Assets dentro de `build/`:** las imágenes de los samples y las ROMs de Chip-8 están versionadas en `build/samples/*/images` y `build/samples/stationTV/roms`, con un `.gitignore` enrevesado para excluir lo demás de `build/`. Por eso `build-debug/` y `build-sanitize/` no tienen imágenes (las pruebas con sanitizers tienen que ejecutarse desde `build/`). Irían en `samples/<nombre>/images` y `samples/stationTV/roms`, copiados junto al ejecutable con el `FILES` de `awui_add_sample`, como ya se hace con `lang` y `menu-settings.json`.
- **Scripts de Windows sueltos:** `bbd.bat`, `bbr.bat`, sus versiones de 32 bits, `buildvars*.bat`, `clean*.bat`, `stationTV.bat` y `gameOfBlocks.bat` irían a `scripts/windows/` (o se sustituyen por los presets de CMake, que ya existen). Habría que ver si la versión de 32 bits sigue haciendo falta.
- **`ext/`** mezcla cosas distintas: las DLL de Windows (`lib32`/`lib64`), `generate-libs.sh`, los fuentes de GIMP (`button.xcf`, `settings.xcf`) y `Cursors/` (56 ficheros que no usa nadie). Las DLL irían a `third_party/`, los `.xcf` a `art/` (junto a `samples/stationTV/art`), y `Cursors/` se borraría si no hace falta.
- **`arduino/`** (el receptor del mando de Apple) es un proyecto aparte: iría a `tools/arduino-remote/`.
- **`doc/obsolete`:** si ya no sirve, se borra (queda en git).

### Estilo y normas del repositorio

- **Cabeceras de copyright:** conviven tres formatos (`/** awui/... Copyright */`, `// (c) Copyright ... (BSD License)` y ninguno). Uno solo, igual en todos.

## Componentes que faltan

1. **Entrada de texto:** no hay `TextBox` ni se atiende `SDL_TEXTINPUT`, así que no se puede escribir nada (buscar un juego, elegir la carpeta de partidas en los ajustes, las comillas en el Spectrum con teclado español).
2. **Teclado en pantalla:** [OnScreenKeyboard.cpp](libawui/awui/UI/OnScreenKeyboard.cpp) es un esqueleto: pinta botones con letras que no hacen nada. En una tele es la única forma de escribir con el mando, así que va junto con el `TextBox`.
3. **Navegar con el mando:** los botones del mando no se traducen a `RemoteButtons`, así que el mando no mueve los menús.
4. **Contenedores de maquetación:** una pila vertical u horizontal y una rejilla. StationUI y SettingsUI colocan todo a mano en cada `OnTick` con números fijos (`GetWidth() - 150`, `+ 42`, `- 66`…).
5. **Lista con desplazamiento reutilizable:** `ListBox` es un esqueleto sin pintado, y lo que funciona (`Browser` + `Page`) está dentro de Station.
6. **Sistema de animaciones:** una animación con duración, curva (las de `Effects`) y aviso al terminar, en vez de un `Interpolate` distinto en cada clase.
7. **Caché de recursos:** texturas por ruta, fuentes y texto ya dibujado (ver "Texturas repetidas").
8. **Carga en segundo plano:** un hilo o una cola de tareas para decodificar imágenes (carátulas) y leer ficheros sin parar el pintado.
9. **Carpeta de recursos:** saber dónde están `images/`, `lang/`… sin depender del directorio de trabajo (ver "Recursos relativos").
10. **Rutas y carpetas:** `Path::GetFileName` / `GetExtension` / `GetDirectoryName` y `Directory::Exists` / `Create` / `GetFiles`. Hoy `Directory` solo tiene `GetWorkingDirectory`, y se usan `opendir`, `std::filesystem` o `LastIndexOf("/")` según el sitio. `Path::Combine` no reconoce `/` como separador en Windows ni una segunda ruta absoluta.
11. **Controles deshabilitados:** no hay `Enabled`. Un control se puede ocultar, pero no dejarlo visible e inactivo.
12. **Orden de foco con el tabulador:** `m_tabIndex` se asigna pero no se usa; no hay navegación con Tab.
13. **Registro de mensajes con niveles:** hoy se mezclan `Console`, `printf` y `fprintf(stderr)`. Con niveles (depuración, aviso, error) se podrían silenciar mensajes como "Partida guardada cargada".
14. **`Label` de varias líneas:** el texto ya pasa por Pango y las descripciones de los ajustes ya se cortan con `TextRenderer::SplitLines` (también en japonés y chino). Falta un `Label` que haga eso solo (ancho máximo, alto según las líneas), para usarlo en otros sitios sin repetir la cuenta de `SettingsUI`.
15. **Pruebas automáticas en el repositorio**, con las pruebas sin ventana que ya existen como base (ajustes, paginación, estados, cintas, Chip-8…) y un `ctest` que las lance con los sanitizers.

## Windows y otras plataformas

La librería tiene que compilar y funcionar igual en Windows, aunque todavía no haya build.

- **Montar la build:** probar al menos a compilar con MinGW (`mingw-w64`) para detectar lo que no compila.
- **Rutas UTF-8:** awui pasa las rutas en UTF-8 como `char*` a `fopen`, `std::fstream` y `std::filesystem` (en `IO/File`, `IO/FileStream`, `Localization`, `SettingsStore` y `OpenGL/Shader`). En Windows esas funciones interpretan la ruta en la página de códigos ANSI, y una ruta con "ñ" fallaría. Lo más sencillo es un manifiesto con `activeCodePage = UTF-8` en el ejecutable (Windows 10 1903 o posterior). Si no, habría que convertir a UTF-16 dentro de `File`/`FileStream`.
- **`opendir`:** `Localization::GetLanguages` usa `opendir`/`readdir`, que no existen con MSVC. Mejor `std::filesystem::directory_iterator`.
- **`Directory.cpp`** solo contempla `__linux__` y `_WIN32`: en macOS no compila.
- **Contexto de OpenGL:** `Application::Run` pide un contexto 3.3 de compatibilidad. macOS no lo da (solo 2.1, o 3.2+ *core*); va unido a pasar a OpenGL moderno.

## Emuladores

- **Modo rápido del Spectrum (F8):** falta decidir si se queda así. Desde el cambio a `steady_clock` se emula durante 30 ms en cada tick, como dice el comentario; antes, por un error de unidades, era un frame por tick. Con un cargador propio la cinta va unas 3 veces más deprisa, pero la interfaz baja a unos 30 fps mientras dura.
