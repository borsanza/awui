// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUISTREAM_H__
#define __AWUISTREAM_H__

#include <stdint.h>

namespace awui {
	namespace IO {
		class Stream {
			public:
				virtual void Close() = 0;

				virtual void SetPosition(int64_t value) = 0;
				virtual int64_t GetPosition() = 0;

				virtual int64_t GetLength() = 0;
		};
	}
}

#endif
