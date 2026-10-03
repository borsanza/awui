/**
 * samples/gameOfBlocks/blocks.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "blocks.h"

using namespace awui::GOB::Engine;

namespace {
	// Fichero de cada cara: derecha, izquierda, arriba, abajo, delante y detrás
	const char *Files[Blocks::Count][6] = {
		{"block-empty.png", "block-empty.png", "block-empty.png", "block-empty.png", "block-empty.png", "block-empty.png"},
		{"block-dirt.png", "block-dirt.png", "block-dirt.png", "block-dirt.png", "block-dirt.png", "block-dirt.png"},
		{"block-grass_block-0145.png", "block-grass_block-0145.png", "block-grass_block-2.png", "block-dirt.png", "block-grass_block-0145.png", "block-grass_block-0145.png"},
		{"block-diamond_block.png", "block-diamond_block.png", "block-diamond_block.png", "block-diamond_block.png", "block-diamond_block.png", "block-diamond_block.png"},
		{"block-stonebrick.png", "block-stonebrick.png", "block-stonebrick.png", "block-stonebrick.png", "block-stonebrick.png", "block-stonebrick.png"},
		{"block-yellow_wool.png", "block-yellow_wool.png", "block-yellow_wool.png", "block-yellow_wool.png", "block-yellow_wool.png", "block-yellow_wool.png"},
		{"block-stone.png", "block-stone.png", "block-stone.png", "block-stone.png", "block-stone.png", "block-stone.png"},
		{"block-cobblestone.png", "block-cobblestone.png", "block-cobblestone.png", "block-cobblestone.png", "block-cobblestone.png", "block-cobblestone.png"},
		{"block-patito-015.png", "block-patito-015.png", "block-patito-2.png", "block-patito-3.png", "block-patito-4.png", "block-patito-015.png"},
		{"block-pattern-0.png", "block-pattern-1.png", "block-pattern-2.png", "block-pattern-3.png", "block-pattern-4.png", "block-pattern-5.png"},
		{"block-sand.png", "block-sand.png", "block-sand.png", "block-sand.png", "block-sand.png", "block-sand.png"},
	};

	std::map<std::string, Texture *> s_textures;
} // namespace

Texture *Blocks::GetTexture(int block, int side) {
	if ((block <= 0) || (block >= Count))
		block = 0;

	const char *file = Files[block][side];
	auto found = s_textures.find(file);
	if (found != s_textures.end())
		return found->second;

	Texture *texture = new Texture(awui::String("images/") + file, Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST);
	texture->SetWrap(Texture::WRAP_REPEAT);
	texture->SetEncodeSRGB(true);
	s_textures[file] = texture;
	return texture;
}
