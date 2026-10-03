#pragma once

#include <awui/GOB/Engine/Math/Euler.h>
#include <awui/GOB/Engine/Math/Vector3.h>
#include <vector>

namespace awui::GOB::Engine {
	class RenderList;

	class Object3D {
	  private:
		static unsigned int s_changes;
		std::vector<Object3D *> m_children;

	  protected:
		Vector3 m_position;
		Euler m_rotation;
		Vector3 m_scale;

	  public:
		Object3D();
		~Object3D();

		// Cuenta las veces que se ha añadido o quitado un objeto (en cualquier sitio): el Renderer rehace la escena
		// cuando cambia
		static unsigned int GetChanges();

		void Add(Object3D *object);
		void Remove(Object3D *object);

		void SetPosition(const Vector3 &newPosition);
		void SetPosition(float x, float y, float z);
		void SetScale(float x, float y, float z);
		void SetRotation(float x, float y, float z);
		Vector3 GetPosition() const;

		// Añade a la lista los triángulos del objeto y de sus hijos
		void PreRender(const Matrix4 &parentMatrix, RenderList &list);
		virtual void Render(const Matrix4 &transform, RenderList &list);
	};
} // namespace awui::GOB::Engine
