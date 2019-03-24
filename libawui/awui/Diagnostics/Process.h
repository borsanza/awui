/*
#pragma once

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
*/