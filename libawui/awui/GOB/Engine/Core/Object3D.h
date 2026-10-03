#pragma once

#include <awui/GOB/Engine/Math/Euler.h>
#include <awui/GOB/Engine/Math/Vector3.h>
#include <vector>

namespace awui::GOB::Engine {
	class RenderList;

	// Un objeto de la escena, con su posición, giro (en radianes, orden XYZ) y escala respecto a su padre, y sus
	// hijos (que son suyos: los borra).
	//
	// El Renderer guarda en OpenGL la geometría de lo que no se mueve y solo la rehace cuando cambia algo (se añade o
	// quita un objeto, o se mueve, gira, escala u oculta uno). Lo que se mueve a menudo (un personaje) se marca como
	// dinámico (SetDynamic, él y todo lo que cuelga de él): se calcula en cada frame y moverlo no rehace lo demás
	class Object3D {
	  private:
		static unsigned int s_changes;
		std::vector<Object3D *> m_children;
		bool m_visible;
		bool m_dynamic;

		void Changed();

	  protected:
		Vector3 m_position;
		Euler m_rotation;
		Vector3 m_scale;

	  public:
		Object3D();
		virtual ~Object3D();

		// Cuenta los cambios de lo que no es dinámico: el Renderer rehace la geometría fija cuando cambia
		static unsigned int GetChanges();

		void Add(Object3D *object);
		void Remove(Object3D *object);

		void SetPosition(const Vector3 &newPosition);
		void SetPosition(float x, float y, float z);
		void SetScale(float x, float y, float z);
		void SetRotation(float x, float y, float z);
		Vector3 GetPosition() const;

		// Oculto: no se pinta, ni sus hijos
		void SetVisible(bool visible);
		inline bool IsVisible() const { return m_visible; }

		void SetDynamic(bool dynamic);
		inline bool IsDynamic() const { return m_dynamic; }

		// Añade a la lista la geometría del objeto y de sus hijos: la de los dinámicos o la de los fijos, según se
		// pida. parentDynamic: cuelga de un objeto dinámico
		void Collect(const Matrix4 &parentMatrix, RenderList &list, bool dynamic, bool parentDynamic = false);

		// La geometría del objeto, ya con su transformación (transform lleva al mundo)
		virtual void Render(const Matrix4 &transform, RenderList &list);
	};
} // namespace awui::GOB::Engine
