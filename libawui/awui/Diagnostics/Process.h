// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIPROCESS_H__
#define __AWUIPROCESS_H__

#include <awui/String.h>

namespace awui {
	namespace Diagnostics {
		class Process {
			private:
				FILE* pPipe;

				int DoSelect();

			public:
				Process();
				virtual ~Process();

				void Start(String process, String arguments);

				bool GetHasExited();
				bool GetHasString();

				String GetLine();
		};
	}
}

#endif
