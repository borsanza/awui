#pragma once

#include <cstdarg>

namespace awui {
	class Object;
	class String;

	namespace IO {
		class TextWriter {
			public:
				virtual ~TextWriter();

				virtual void Flush() = 0;
				virtual String GetNewLine() = 0;
				virtual void Write(Object *value);
				virtual void Write(String value);
				virtual void Write(const char * value, ...);
				virtual void Write(const char * value, va_list args) = 0;
				virtual void WriteLine();
				virtual void WriteLine(Object *value);
				virtual void WriteLine(String value);
				virtual void WriteLine(const char * value, ...);
				virtual void WriteLine(const char * value, va_list args);
		};
	}
}
