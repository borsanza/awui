# Pendiente de gameOfBlocks

Lo que queda por hacer en gameOfBlocks y en su motor 3D (`libawui/awui/GOB/Engine`). Al hacer una cosa, se borra de aquí (el historial queda en git). Lo del resto de awui (la librería de interfaz, los emuladores, stationTV) está en el [TODO.md](../../TODO.md) de la raíz.

Está a la par de la versión web (three.js, en el proyecto `topcon`): mismo terreno, misma luz y colores, mismas cámaras y misma física. Además, aquí el jugador choca con los bloques por todos los lados (en la web solo con el suelo), la distancia de visión se cambia en marcha y lo que queda fuera deja de pintarse.

## Prioridad alta

- **Pruebas automáticas.** Todo esto se ha comprobado con arneses sin ventana que no están guardados: hay que volver a escribirlos cada vez. Irían con las del resto del repositorio (`tests/`, con `ctest` y los sanitizers; ver el TODO de la raíz). Ninguna necesita nada externo:
  1. **El ruido,** contra unas alturas de referencia sacadas de la versión web (con `alea(1)` y `simplex-noise`): tienen que salir idénticas.
  2. **La tabla de triángulos** por distancia de visión, de 1 a 20 (está en `chunk.h`): comprueba el mallado entero.
  3. **Las colisiones:** pararse en una pared, deslizarse por ella, no subir un escalón andando y sí saltando, darse con el techo, y caer por el pozo sin atravesar nada aunque vaya a tirones.
  4. **El motor contra three.js:** la escena de referencia de `index2.html` (el cubo con una letra en cada cara), comparando la imagen. Hoy coincide salvo los bordes.

## Carga del mundo

A distancia 16 el mundo tarda unos segundos en completarse, y al andar puede dar tirones:

- **El arranque genera de golpe más de mil chunks:** `Chunk::SetPlayerPosition` crea los datos de todo el cuadrado alrededor del jugador en la primera llamada. Habría que generarlos poco a poco, del centro hacia fuera.
- **Las mallas van de una en una por frame** (`Chunk::OptimizeOneMore`): unos 800 chunks son 13 segundos a 60 fps. Mejor dedicar unos milisegundos de cada frame a mallar los que quepan.
- **Cada chunk nuevo rehace toda la geometría fija:** el `Renderer` vuelve a copiar y subir todos los triángulos de la escena cada vez que un chunk entra, sale o se oculta. Con un buffer de OpenGL por chunk solo se subiría el suyo; a cambio habría una llamada de pintado por chunk (hoy son 17 en total), salvo que las texturas de los bloques vayan en un atlas. Hay que medirlo.
- **Los chunks nunca se descargan:** los que quedan fuera de la distancia de visión dejan de pintarse, pero sus datos y su malla siguen en memoria (64 KB de bloques por chunk, más la malla): crece al andar.
- Si con lo anterior no basta, generar y mallar en un hilo aparte.

## El juego

- **Poner y quitar bloques.** Al cambiar un bloque habría que rehacer la malla de su chunk (y la de los vecinos si está en el borde): hoy cada chunk se calcula una sola vez.
- **Cámara en tercera persona:** atraviesa el terreno (si hay una colina entre la cámara y el jugador, se ve por dentro). Habría que acercarla al jugador cuando algo se interpone.
- **Subir escalones:** un bloque de alto solo se sube saltando (como en Minecraft). Si se quiere subir andando escalones bajos, haría falta medio bloque o un "paso automático".
- **Huecos de un bloque:** andando se cruzan sin caer (la caja del jugador siempre pisa algún borde); para caer por el pozo hay que pararse encima.
- **Mando:** solo teclado y ratón.
- **Texturas de los bloques:** son propias y salen de [make-textures.py](art/make-textures.py) (dibujadas por código, con semilla fija), menos las de prueba (`block-empty` y `block-pattern-*`), dibujadas a mano. Cada bloque tiene una sola: la grieta de los sillares se repite en todas las piedras. Para evitarlo harían falta varias variantes por bloque.
- **`World` es también el control que pinta** (hereda de `Renderer`). Lo limpio sería separar el mundo (la escena, el jugador, las cámaras) del control.

## El motor (`GOB/Engine`)

Sigue la forma de three.js (`Object3D`, `Mesh`, `BoxGeometry`, `MeshBasicMaterial`, `PerspectiveCamera`…), para que quien conozca three sepa usarlo y se pueda comparar escena a escena.

- **De quién son las geometrías y los materiales.** `Mesh` guarda su `BufferGeometry *` y sus `Material *` sin liberarlos nunca, y las texturas de los bloques (`Blocks::GetTexture`) y los chunks tampoco se liberan: no está claro quién es el dueño. Hoy da igual porque viven hasta que se cierra el programa.
- **No hay luces:** la del mundo va calculada en el color de cada cara (`FaceLight`, en `chunk.cpp`), con los valores de la versión web. Para luces de verdad harían falta normales en los vértices y un shader que las use.
- **No hay `InstancedMesh`** (muchas copias de un mismo objeto con una sola llamada). El pintado ya se junta por textura en toda la escena, que resuelve el caso del terreno.
- **El modo malla** pinta todos los triángulos como líneas; en three solo afecta a los materiales que lo piden.
- **Está dentro de `libawui`** (`libawui/awui/GOB/`), aunque solo lo usa gameOfBlocks: cualquier programa de awui lo lleva dentro. Iría en una librería propia (`libgob/`), movido con `git mv` para no perder el historial.
