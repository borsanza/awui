
#pragma once

namespace awui::GOB::Engine {
	class Matrix4 {
		float m[16];

	  public:
		Matrix4();

		float *operator[](int index);
		const float *data() const;
	};
} // namespace awui::GOB::Engine
