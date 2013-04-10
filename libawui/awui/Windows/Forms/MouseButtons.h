// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIMOUSEBUTTONS_H__
#define __AWUIMOUSEBUTTONS_H__

namespace awui {
	namespace Windows {
		namespace Forms {
			struct MouseButtons {
				enum Enum {
					None = 0,
					Left = 1,
					Right = 2,
					Middle = 4,
					XButton1 = 8,
					XButton2 = 16,
				};
			};
		}
	}
}

#endif
