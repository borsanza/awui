// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIPANEL_H__
#define __AWUIPANEL_H__

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Panel : public Control {
				public:
					Panel();
					virtual ~Panel();

					virtual int IsClass(Classes::Enum objectClass) const;

					const virtual awui::Drawing::Size GetMinimumSize() const;
			};
		}
	}
}

#endif
