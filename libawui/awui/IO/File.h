#pragma once

#include <awui/String.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace awui::IO {
	struct FileAccess {
		enum Enum {
			Read = 1,
			Write = 2,
			ReadWrite = 3,
		};
	};

	struct FileMode {
		enum Enum {
			CreateNew = 1,
			Create = 2,
			Open = 3,
			OpenOrCreate = 4,
			Truncate = 5,
			Append = 6,
		};
	};

	class File {
	  public:
		static bool Exists(String path);

		// Fichero entero de una vez. false si no se puede leer
		static bool ReadAllBytes(const String &path, std::vector<uint8_t> &data);
		// Atómica: se escribe en <path>.tmp y se renombra, así un corte a medias no deja el fichero roto
		static bool WriteAllBytes(const String &path, const uint8_t *data, size_t size);
		static bool WriteAllBytes(const String &path, const std::vector<uint8_t> &data);
	};
} // namespace awui::IO
