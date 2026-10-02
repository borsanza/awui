#pragma once

#include <cstdint>
#include <vector>

typedef unsigned int GLuint;

namespace awui {
	namespace Drawing {
		class Color;
	}

	namespace OpenGL {
		// Pintado 2D con shaders, que funciona igual con OpenGL 3.3 (escritorio) y con OpenGL ES 3.0 (Raspberry Pi,
		// Android, WebGL 2): sustituye a glBegin/glEnd, glColor, glOrtho... que no existen en OpenGL ES.
		//
		// Todo se pinta como triángulos con color y, si se quiere, textura (el color multiplica a la textura). Las
		// coordenadas son en píxeles, como las del pintado antiguo: (0, 0) arriba a la izquierda del control.
		//
		// En un contexto de compatibilidad convive con el pintado antiguo (el 3D de gameOfBlocks) y deja OpenGL como
		// lo encuentra: sin programa, sin buffers y con la mezcla que hubiera. Necesita el contexto ya creado y activo
		class Painter {
		  public:
			struct Vertex {
				float x, y;			// Píxeles
				float u, v;			// Coordenadas de textura (0-1)
				uint8_t r, g, b, a; // Color, que multiplica a la textura
			};

			// Cómo se mezcla con lo que hay debajo
			enum class Blend {
				Normal,		   // Color sin premultiplicar (el habitual): SRC_ALPHA, ONE_MINUS_SRC_ALPHA
				Premultiplied, // Color ya multiplicado por el alfa (las imágenes de cairo): ONE, ONE_MINUS_SRC_ALPHA
			};

			// Orden de los bytes de la textura
			enum class TextureFormat {
				RGBA,
				BGRA, // El de cairo (ARGB32 en memoria little-endian). OpenGL ES no lo admite: se sube como RGBA y el
					  // shader cambia el orden
			};

		  private:
			bool m_initialized;
			bool m_failed;
			bool m_es;	   // Contexto OpenGL ES (si no, de escritorio)
			bool m_legacy; // Contexto de compatibilidad: convive con el pintado antiguo y deja OpenGL como estaba
			GLuint m_program;
			GLuint m_vertexArray;
			GLuint m_vertexBuffer;
			int m_projectionLocation;
			int m_modeLocation;
			int m_textureLocation;
			float m_projection[16];
			float m_offsetX;
			float m_offsetY;
			std::vector<Vertex> m_quad; // Para los rectángulos sueltos, sin reservar memoria cada vez

			Painter();
			bool Initialize();
			void Draw(const Vertex *vertices, int count, int mode, GLuint texture, Blend blend);
			GLuint CompileShader(unsigned int type, const char *source);
			static void AddSegment(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float width, float extendStart, float extendEnd, const Drawing::Color &color);

		  public:
			static Painter &Instance();

			// Proyección ortográfica, como glOrtho(left, right, bottom, top, -1, 1)
			void SetOrtho(float left, float right, float bottom, float top);

			// Desplazamiento que se suma a todos los vértices (como glTranslatef en 2D)
			void SetOffset(float x, float y);
			inline float GetOffsetX() const { return m_offsetX; }
			inline float GetOffsetY() const { return m_offsetY; }

			// Pinta triángulos sueltos (de tres en tres vértices). texture: 0 para solo color
			void DrawTriangles(const Vertex *vertices, int count, GLuint texture = 0, TextureFormat format = TextureFormat::RGBA, Blend blend = Blend::Normal);

			inline void DrawTriangles(const std::vector<Vertex> &vertices, GLuint texture = 0, TextureFormat format = TextureFormat::RGBA, Blend blend = Blend::Normal) {
				DrawTriangles(vertices.data(), (int) vertices.size(), texture, format, blend);
			}

			// Añade a una lista los dos triángulos de un rectángulo, de (x1, y1) a (x2, y2) sin incluir el borde
			// derecho ni el inferior, con las coordenadas de textura de (u1, v1) a (u2, v2)
			static void AddQuad(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2, const Drawing::Color &color);

			// Igual, con un color en cada esquina (degradados): arriba izquierda, arriba derecha, abajo izquierda,
			// abajo derecha
			static void AddQuad(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2, const Drawing::Color &topLeft, const Drawing::Color &topRight, const Drawing::Color &bottomLeft, const Drawing::Color &bottomRight);

			// Añade una línea de cierto grosor como un rectángulo girado (OpenGL ES no tiene líneas de más de un
			// píxel). Acaba justo en sus dos puntos. Se pinta con DrawLines, que suaviza los bordes
			static void AddLine(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float width, const Drawing::Color &color);

			// Añade una línea quebrada (x0, y0, x1, y1...): en las uniones los tramos se alargan medio grosor para
			// que no quede una muesca en la esquina
			static void AddPolyline(std::vector<Vertex> &vertices, const std::vector<float> &points, float width, const Drawing::Color &color);

			// Pinta las líneas de AddLine y AddPolyline con los bordes suavizados (como GL_LINE_SMOOTH): el shader calcula la
			// opacidad de cada píxel según su distancia al centro de la línea
			void DrawLines(const std::vector<Vertex> &vertices);

			// Rectángulo de color, de (x1, y1) a (x2, y2) incluidos: como GL::FillRectangle
			void FillRectangle(int x1, int y1, int x2, int y2, const Drawing::Color &color);

			// Textura en un rectángulo. opacity: 0 transparente, 1 opaca
			void DrawTexture(GLuint texture, TextureFormat format, Blend blend, float x, float y, float width, float height, float opacity = 1.0f);
		};
	} // namespace OpenGL
} // namespace awui
