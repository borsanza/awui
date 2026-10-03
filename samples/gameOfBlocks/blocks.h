#pragma once

#include <awui/GOB/Engine/Textures/Texture.h>

#include <map>
#include <string>

// Los tipos de bloque y la textura de cada una de sus seis caras, como en la versión web (block.js)
namespace Blocks {
	enum Id {
		Air = 0,
		Dirt = 1,
		Grass = 2,
		Diamond = 3,
		StoneBrick = 4,
		YellowWool = 5,
		Stone = 6,
		Cobblestone = 7,
		Patito = 8,
		Pattern = 9,
		Sand = 10,
		Count
	};

	// El orden de las caras, que es también el bit de cada una en las máscaras de caras visibles
	enum Side {
		Right = 0, // +X
		Left = 1,  // -X
		Up = 2,	   // +Y
		Down = 3,  // -Y
		Front = 4, // +Z
		Back = 5,  // -Z
	};

	// La textura de una cara de un bloque (compartida: se carga una vez por fichero). Píxel a píxel, repetida, y con
	// los colores como los deja three.js (ver Texture::SetEncodeSRGB)
	awui::GOB::Engine::Texture *GetTexture(int block, int side);
} // namespace Blocks
