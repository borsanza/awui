#pragma once

namespace awui {
	class String;

	class Object {
	  public:
		Object();
		virtual ~Object() = default;

		virtual String ToString() const;
	};
} // namespace awui
