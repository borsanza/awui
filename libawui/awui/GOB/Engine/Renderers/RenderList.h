#pragma once

#include <cstdint>
#include <vector>

namespace awui::GOB::Engine {
	class Texture;

	// Los triángulos de la escena, ya en coordenadas del mundo, seguidos y en tramos por textura: el Renderer los sube
	// a OpenGL de una vez y pinta cada tramo con una llamada
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

	  public:
		// Vacía la lista sin soltar la memoria
		void Clear() {
			m_vertices.clear();
			m_batches.clear();
		}

		void Add(Texture *texture, const Vertex &vertex) {
			if (m_batches.empty() || (m_batches.back().texture != texture))
				m_batches.push_back({texture, (int) m_vertices.size(), 0});
			m_batches.back().count++;
			m_vertices.push_back(vertex);
		}

		const std::vector<Vertex> &GetVertices() const { return m_vertices; }
		const std::vector<Batch> &GetBatches() const { return m_batches; }
	};
} // namespace awui::GOB::Engine
