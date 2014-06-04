// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIMEMORYSTREAM_H__
#define __AWUIMEMORYSTREAM_H__

#include <awui/IO/Stream.h>

namespace awui {
	namespace IO {
		class MemoryStream : Stream {
			public:
				virtual void Close();

				virtual void SetPosition(int64_t value);
				virtual int64_t GetPosition();

				virtual int64_t GetLength();
		};
	}
}

#endif
