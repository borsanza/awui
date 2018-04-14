#pragma once

namespace awui {
	namespace Scripting {
		class JavaScript {
			public:
				JavaScript();
				virtual ~JavaScript();

				void Initialize();
				void Run(const char * code);
				void Shutdown();
		};
	}
}

