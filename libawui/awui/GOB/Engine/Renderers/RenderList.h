#pragma once

#include <cstdint>
#include <vector>

namespace awui::GOB::Engine {
	class Texture;

	// Los triángulos de la escena, ya en coordenadas del mundo, seguidos y en tramos por textura: el Renderer los sube
	// a OpenGL de una vez y pinta cada tramo con una llamada. Y, aparte, las líneas (ejes, guías), sin textura
	class RenderList {
	  public:
		struct Vertex {
			float x, y, z;		// Posición en el mundo
			float u, v;			// Coordenadas de textura (0-1)
			uint8_t r, g, b, a; // Color, que multiplica a la textura
		};

		struct Batch {
			Texture *texture; // nullptr: solo color
			int first;		  // Primer vértice
			int count;
		};

	  private:
		std::vector<Vertex> m_vertices;
		std::vector<Batch> m_batches;
		std::vector<Vertex> m_lines; // De dos en dos

	  public:
		// Vacía la lista sin soltar la memoria
		void Clear() {
			m_vertices.clear();
			m_batches.clear();
			m_lines.clear();
		}

		void Add(Texture *texture, const Vertex &vertex) {
			if (m_batches.empty() || (m_batches.back().texture != texture))
				m_batches.push_back({texture, (int) m_vertices.size(), 0});
			m_batches.back().count++;
			m_vertices.push_back(vertex);
		}

		void AddLine(const Vertex &from, const Vertex &to) {
			m_lines.push_back(from);
			m_lines.push_back(to);
		}

		const std::vector<Vertex> &GetLines() const { return m_lines; }
		const std::vector<Vertex> &GetVertices() const { return m_vertices; }
		const std::vector<Batch> &GetBatches() const { return m_batches; }
	};
} // namespace awui::GOB::Engine
