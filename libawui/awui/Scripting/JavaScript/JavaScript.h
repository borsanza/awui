#pragma once

#include <v8.h>

namespace awui {
	namespace Scripting {
		namespace JavaScript {
			class JavaScript {
				private:
					v8::Handle<v8::Context> context;
					static int instances;

					void Initialize();
					void Shutdown();

				public:
					JavaScript();
					virtual ~JavaScript();

					void Run(const char * code);
			};
		}
	}
}

