// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUITEXTWRITER_H__
#define __AWUITEXTWRITER_H__

namespace awui {
	class String;

	namespace IO {
		class TextWriter {
		public:
			virtual void Flush() = 0;
			virtual String GetNewLine() = 0;
			virtual void Write(char value) = 0;
			virtual void Write(String value) = 0;
			virtual void WriteLine();
			virtual void WriteLine(char value);
			virtual void WriteLine(String value);
		};
	}
}

#endif
