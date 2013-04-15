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
			virtual void WriteLine() = 0;
			virtual void WriteLine(String value) = 0;
		};
	}
}

#endif
