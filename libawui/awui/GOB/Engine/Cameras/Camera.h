#pragma once

#include <awui/GOB/Engine/Core/Object3D.h>
#include <awui/GOB/Engine/Math/Vector3.h>

namespace awui::GOB::Engine::Cameras {
	class Camera : public Object3D {
	  protected:
		Vector3 m_target;
		Vector3 m_upVector;

	  public:
		Camera();
		virtual void SetProjectionMatrix() = 0;
		virtual void SetViewMatrix() = 0;

		void LookAt(float x, float y, float z);

		Vector3 GetTarget() const;
		void SetUpVector(const Vector3 &newUpVector);
		Vector3 GetUpVector() const;
	};
} // namespace awui::GOB::Engine::Cameras
