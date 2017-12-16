#pragma once

namespace awui {
	class Object;
	class String;

	namespace IO {
		class TextWriter {
			public:
				virtual ~TextWriter();

				virtual void Flush() = 0;
				virtual String GetNewLine() = 0;
				virtual void Write(char value) = 0;
				virtual void Write(Object *value);
				virtual void Write(String value) = 0;
				virtual void WriteLine();
				virtual void WriteLine(char value);
				virtual void WriteLine(Object *value);
				virtual void WriteLine(String value);
		};
	}
}
