#pragma once

#include <awui/IO/TextWriter.h>

using namespace awui;

namespace awui {
	class Console {
		private:
			class outClass : public IO::TextWriter {
			public:
				virtual void Flush();
				virtual String GetNewLine();
				virtual void Write(char value);
				virtual void Write(String value);
			};

		public:
			static IO::TextWriter * GetOut();
			static void Write(String value);
			static void WriteLine(String value);
			static void WriteLine(Object * value);
	};
}
