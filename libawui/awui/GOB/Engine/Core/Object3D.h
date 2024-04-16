#pragma once

#include <awui/GOB/Engine/Math/Vector3.h>
#include <vector>

namespace awui::GOB::Engine {

	class Object3D {
	  private:
		std::vector<Object3D *> _children;

	  protected:
		Vector3 position;

	  public:
		~Object3D();

		void Add(Object3D *object);
		void Remove(Object3D *object);

		void SetPosition(const Vector3 &newPosition);
		Vector3 GetPosition() const;
	};
} // namespace awui::GOB::Engine
