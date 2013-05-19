// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIREMOTEBUTTONS_H__
#define __AWUIREMOTEBUTTONS_H__

namespace awui {
	namespace Windows {
		namespace Forms {
			struct RemoteButtons {
				enum Enum {
					None = 0,
					Menu = 1,
					Ok = 2,
					Play = 4,
					Right = 8,
					Left = 16,
					Up = 32,
					Down = 64,
				};
			};
		}
	}
}

#endif
