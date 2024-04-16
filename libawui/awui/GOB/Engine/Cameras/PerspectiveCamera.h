
#pragma once

#include <awui/GOB/Engine/Cameras/Camera.h>
#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Math/Vector3.h>

namespace awui::GOB::Engine::Cameras {
	class PerspectiveCamera : public Camera {
	  private:
		float m_fov;
		float m_aspect;
		float m_near;
		float m_far;

		void MakePerspectiveMatrix(Matrix4 &matrix, float fovY, float aspect, float near, float far);
		void MakeLookAtMatrix(Matrix4 &matrix, const Vector3 &eye, const Vector3 &target, const Vector3 &up);

	  public:
		PerspectiveCamera(float fov, float aspect, float near, float far);

		virtual void SetProjectionMatrix() override;
		virtual void SetViewMatrix() override;
	};
} // namespace awui::GOB::Engine::Cameras
