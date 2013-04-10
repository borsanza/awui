// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUILINEJOIN_H__
#define __AWUILINEJOIN_H__

namespace awui {
	namespace Drawing {
		namespace Drawing2D {
			struct LineJoin {
				enum Enum {
					Miter = 0,
					Bevel = 1,
					Round = 2,
				};
			};
		}
	}
}

#endif
