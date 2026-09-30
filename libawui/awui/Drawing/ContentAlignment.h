#pragma once

namespace awui::Drawing {
	// Posición del contenido (texto, imagen) dentro de su espacio: vertical (Top, Middle, Bottom) y horizontal
	// (Left, Center, Right)
	struct ContentAlignment {
		enum Enum {
			TopLeft = 1,
			TopCenter = 2,
			TopRight = 4,
			MiddleLeft = 16,
			MiddleCenter = 32,
			MiddleRight = 64,
			BottomLeft = 256,
			BottomCenter = 512,
			BottomRight = 1024,
		};
	};
} // namespace awui::Drawing
