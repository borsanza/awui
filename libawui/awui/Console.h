// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUICONSOLE_H__
#define __AWUICONSOLE_H__

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
	};
}

#endif
