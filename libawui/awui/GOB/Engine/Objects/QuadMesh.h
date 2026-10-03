#pragma once

#include <awui/Drawing/Color.h>
#include <awui/GOB/Engine/Core/Object3D.h>
#include <awui/GOB/Engine/Renderers/RenderList.h>

#include <vector>

namespace awui::GOB::Engine {
	class Texture;

	// Malla de rectángulos ya calculados, agrupados por textura: para geometría que se genera de una vez (el terreno
	// de un chunk), sin pasar por BufferGeometry y un material por cara. Ocupa mucho menos que un Mesh por rectángulo
	// y sus rectángulos salen seguidos por textura (menos llamadas de pintado)
	class QuadMesh : public Object3D {
	  private:
		struct Group {
			Texture *texture; // No es suya
			std::vector<RenderList::Vertex> vertices;
		};

		std::vector<Group> m_groups;

	  public:
		// Las cuatro esquinas en el orden de un PlaneGeometry: arriba izquierda, arriba derecha, abajo izquierda y
		// abajo derecha (mirándolo de frente), cada una con x, y, z, u, v. texture: nullptr para solo color
		void AddQuad(Texture *texture, const float corners[4][5], const Drawing::Color &color);

		inline bool IsEmpty() const { return m_groups.empty(); }
		int GetQuadCount() const;

		virtual void Render(const Matrix4 &transform, RenderList &list) override;
	};
} // namespace awui::GOB::Engine
