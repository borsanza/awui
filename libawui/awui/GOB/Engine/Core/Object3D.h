#pragma once

#include <awui/GOB/Engine/Math/Euler.h>
#include <awui/GOB/Engine/Math/Vector3.h>
#include <unordered_set>
#include <vector>

namespace awui::GOB::Engine {
	class Material;

	class Object3D {
	  private:
		std::vector<Object3D *> _children;

	  protected:
		Vector3 position;
		Euler rotation;
		Vector3 scale;

	  public:
		Object3D();
		~Object3D();

		virtual void Add(Object3D *object);
		void Remove(Object3D *object);

		void SetPosition(const Vector3 &newPosition);
		void SetPosition(float x, float y, float z);
		void SetScale(float x, float y, float z);
		void SetRotation(float x, float y, float z);
		Vector3 GetPosition() const;

		void PreRender(const Material &material, const Matrix4 &parentMatrix);
		virtual void Render(const Material &material, const Matrix4 &transform);

		void PreFillMaterialList(std::unordered_set<Material *> &materials);
		virtual void FillMaterialList(std::unordered_set<Material *> &materials);
	};
} // namespace awui::GOB::Engine
