#pragma once

#include <awui/GOB/Engine/Math/Vector3.h>
#include <vector>

namespace awui::GOB::Engine {

	class Object3D {
	  private:
		std::vector<Object3D *> _children;

	  protected:
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;

	  public:
		Object3D();
		~Object3D();

		void Add(Object3D *object);
		void Remove(Object3D *object);

		void SetPosition(const Vector3 &newPosition);
		void SetPosition(float x, float y, float z);
		void SetScale(float x, float y, float z);
		void SetRotation(float x, float y, float z);
		Vector3 GetPosition() const;

		virtual void Render();
	};
} // namespace awui::GOB::Engine
