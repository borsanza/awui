// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIBINARYREADER_H__
#define __AWUIBINARYREADER_H__

namespace awui {
	namespace IO {
		class Stream;

		class BinaryReader {
			public:
				BinaryReader(Stream * input);

				virtual void Close();
				virtual unsigned char ReadByte();
		};
	}
}

#endif
