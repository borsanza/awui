#pragma once

namespace awui::Drawing {
	// Cómo acaba un trazo abierto
	enum class LineCap {
		Butt = 0,
		Square = 1,
		Round = 2,
	};

	// Cómo se unen dos segmentos del mismo trazo (esquinas de DrawRectangle, por ejemplo)
	enum class LineJoin {
		Miter = 0,
		Bevel = 1,
		Round = 2,
	};
} // namespace awui::Drawing
