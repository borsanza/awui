#pragma once

#include <cstdint>
#include <vector>

namespace awui::GOB::Engine {
	class Texture;

	// Los triángulos de la escena, ya en coordenadas del mundo. Los objetos los van añadiendo (Add) y al final se
	// juntan por textura (Finish), sean del objeto que sean: el Renderer los sube a OpenGL de una vez y pinta cada
	// textura con una sola llamada, haya los objetos que haya. Y, aparte, las líneas (ejes, guías), sin textura
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
		// Los vértices de una textura, mientras se añaden
		struct Bucket {
			Texture *texture;
			std::vector<Vertex> vertices;
		};

		std::vector<Bucket> m_buckets;
		int m_last = -1; // El último usado: casi siempre el siguiente vértice es de la misma textura

		std::vector<Vertex> m_vertices; // Tras Finish: todos seguidos, por textura
		std::vector<Batch> m_batches;
		std::vector<Vertex> m_lines; // De dos en dos

	  public:
		// Vacía la lista sin soltar la memoria
		void Clear() {
			for (Bucket &bucket : m_buckets)
				bucket.vertices.clear();

			m_last = -1;
			m_vertices.clear();
			m_batches.clear();
			m_lines.clear();
		}

		void Add(Texture *texture, const Vertex &vertex) {
			if ((m_last < 0) || (m_buckets[m_last].texture != texture)) {
				m_last = -1;
				for (int i = 0; i < (int) m_buckets.size(); i++) {
					if (m_buckets[i].texture == texture) {
						m_last = i;
						break;
					}
				}

				if (m_last < 0) {
					m_buckets.push_back({texture, {}});
					m_last = (int) m_buckets.size() - 1;
				}
			}

			m_buckets[m_last].vertices.push_back(vertex);
		}

		void AddLine(const Vertex &from, const Vertex &to) {
			m_lines.push_back(from);
			m_lines.push_back(to);
		}

		// Cuando ya está todo añadido: deja los vértices seguidos y un tramo por textura
		void Finish() {
			m_vertices.clear();
			m_batches.clear();
			for (const Bucket &bucket : m_buckets) {
				if (bucket.vertices.empty())
					continue;

				m_batches.push_back({bucket.texture, (int) m_vertices.size(), (int) bucket.vertices.size()});
				m_vertices.insert(m_vertices.end(), bucket.vertices.begin(), bucket.vertices.end());
			}
		}

		// Tras Finish
		const std::vector<Vertex> &GetVertices() const { return m_vertices; }
		const std::vector<Batch> &GetBatches() const { return m_batches; }
		const std::vector<Vertex> &GetLines() const { return m_lines; }
	};
} // namespace awui::GOB::Engine
