/**
 * samples/gameOfBlocks/chunk.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "chunk.h"

#include "blocks.h"
#include "noise.h"

#include <awui/Drawing/Color.h>
#include <awui/GOB/Engine/Objects/QuadMesh.h>
#include <awui/GOB/Engine/Scenes/Scene.h>

#include <algorithm>
#include <cmath>

using namespace awui::Drawing;
using namespace awui::GOB::Engine;

std::map<std::pair<int, int>, Chunk *> Chunk::s_chunks;
std::vector<Chunk *> Chunk::s_queue;
double Chunk::s_playerX = 0;
double Chunk::s_playerZ = 0;
Scene *Chunk::s_scene = nullptr;
SimplexNoise2D *Chunk::s_noise = nullptr;

namespace {
	const int Unassigned = -1;

	float SRGBToLinear(float value) {
		return (value <= 0.04045f) ? (value / 12.92f) : powf((value + 0.055f) / 1.055f, 2.4f);
	}

	float LinearToSRGB(float value) {
		return (value <= 0.0031308f) ? (value * 12.92f) : (1.055f * powf(value, 1.0f / 2.4f) - 0.055f);
	}

	// La luz de la versión web (setupLights.js) sobre una cara, como color que multiplica a la textura: una luz
	// ambiente (0xaabbcb, intensidad 2) y un sol (0xfff4d6, intensidad 2) que cae en vertical, así que solo ilumina las
	// caras de arriba. Como en three.js (MeshPhongMaterial, sin el brillo): la luz se suma en lineal, se divide entre
	// pi y el resultado se pasa a sRGB
	Color FaceLight(int side) {
		const float pi = 3.14159265f;
		const float ambient[3] = {0xaa / 255.0f, 0xbb / 255.0f, 0xcb / 255.0f};
		const float sun[3] = {0xff / 255.0f, 0xf4 / 255.0f, 0xd6 / 255.0f};
		float facing = (side == Blocks::Up) ? 1.0f : 0.0f;

		uint8_t color[3];
		for (int i = 0; i < 3; i++) {
			float light = (SRGBToLinear(ambient[i]) * 2.0f + SRGBToLinear(sun[i]) * 2.0f * facing) / pi;
			color[i] = (uint8_t) lroundf(LinearToSRGB(std::min(light, 1.0f)) * 255.0f);
		}

		return Color::FromArgb(255, color[0], color[1], color[2]);
	}
} // namespace

Chunk::Chunk(int x, int z) {
	m_chunkX = x;
	m_chunkZ = z;
	m_optimized = false;
	m_mesh = nullptr;
	m_blocks.assign(Size, Blocks::Air);

	FillChunk();
}

void Chunk::SetScene(Scene *scene) {
	s_scene = scene;
}

bool Chunk::IsInCircle(double px, double py, double cx, double cy, double radius) {
	return sqrt((px - cx) * (px - cx) + (py - cy) * (py - cy)) < radius;
}

// Entra en la cola si alguna de sus cuatro esquinas está dentro del radio del jugador
void Chunk::QueueIfNear() {
	if (m_optimized || (std::find(s_queue.begin(), s_queue.end(), this) != s_queue.end()))
		return;

	double radius = RenderDistance * Width;
	double x = m_chunkX * Width;
	double z = m_chunkZ * Depth;
	if (IsInCircle(x, z, s_playerX, s_playerZ, radius) || IsInCircle(x, z + 15, s_playerX, s_playerZ, radius) || IsInCircle(x + 15, z, s_playerX, s_playerZ, radius) ||
		IsInCircle(x + 15, z + 15, s_playerX, s_playerZ, radius))
		s_queue.push_back(this);
}

double Chunk::GetDistanceToPlayer() const {
	double x = m_chunkX * Width + Width / 2 - s_playerX;
	double z = m_chunkZ * Depth + Depth / 2 - s_playerZ;
	return sqrt(x * x + z * z);
}

void Chunk::OptimizeOneMore() {
	if (s_queue.empty())
		return;

	size_t selected = 0;
	double distance = s_queue[0]->GetDistanceToPlayer();
	for (size_t i = 1; i < s_queue.size(); i++) {
		double newDistance = s_queue[i]->GetDistanceToPlayer();
		if (newDistance < distance) {
			distance = newDistance;
			selected = i;
		}
	}

	Chunk *chunk = s_queue[selected];
	s_queue.erase(s_queue.begin() + selected);
	chunk->OptimizeAllGeometries();
}

void Chunk::SetPlayerPosition(double x, double z) {
	s_playerX = x;
	s_playerZ = z;

	for (double xp = -RenderDistance * Width + x; xp <= x + RenderDistance * Width; xp += Width) {
		for (double zp = -RenderDistance * Depth + z; zp <= z + RenderDistance * Depth; zp += Depth)
			GetChunk((int) floor(xp / Width), (int) floor(zp / Depth))->QueueIfNear();
	}
}

Chunk *Chunk::GetChunk(int x, int z) {
	auto found = s_chunks.find({x, z});
	if (found != s_chunks.end())
		return found->second;

	Chunk *chunk = new Chunk(x, z);
	s_chunks[{x, z}] = chunk;
	return chunk;
}

Chunk *Chunk::GetGlobalChunk(double x, double z) {
	return GetChunk((int) floor(floor(x) / Width), (int) floor(floor(z) / Depth));
}

uint8_t Chunk::GetGlobalBlock(double x, int y, double z) {
	int blockX = (int) floor(x);
	int blockZ = (int) floor(z);
	Chunk *chunk = GetGlobalChunk(x, z);
	return chunk->GetData(blockX - chunk->m_chunkX * Width, y, blockZ - chunk->m_chunkZ * Depth);
}

void Chunk::SetBlock(int x, int y, int z, uint8_t block) {
	m_blocks[GetIndex(x, y, z)] = block;
}

// Fuera del mundo por arriba o por abajo, aire
uint8_t Chunk::GetBlock(int x, int y, int z) const {
	if ((y < 0) || (y >= Height))
		return Blocks::Air;

	return m_blocks[GetIndex(x, y, z)];
}

uint8_t Chunk::GetData(int x, int y, int z) {
	if (x == -1)
		return GetChunk(m_chunkX - 1, m_chunkZ)->GetData(15, y, z);

	if (x == 16)
		return GetChunk(m_chunkX + 1, m_chunkZ)->GetData(0, y, z);

	if (z == -1)
		return GetChunk(m_chunkX, m_chunkZ - 1)->GetData(x, y, 15);

	if (z == 16)
		return GetChunk(m_chunkX, m_chunkZ + 1)->GetData(x, y, 0);

	return GetBlock(x, y, z);
}

// El terreno: llano de arena a la altura 63 y colinas de hierba encima, según el ruido. En el chunk del origen,
// además, una caseta de ladrillo con un pozo que baja a una cueva con un bloque de diamante, y dos bloques de prueba
void Chunk::FillChunk() {
	if (!s_noise) {
		Alea random(1);
		s_noise = new SimplexNoise2D(random);
	}

	const double scale = 0.02;
	const double maxHeight = 19;
	for (int x = 0; x < Width; x++) {
		int realX = x + m_chunkX * Width;
		for (int z = 0; z < Depth; z++) {
			int realZ = m_chunkZ * Depth + z;
			double noise = s_noise->Noise(realX * scale, realZ * scale);
			int height = (int) floor(62 - maxHeight / 2 + ((noise + 1) / 2) * maxHeight);
			if (height < 63)
				height = 63;

			for (int y = 0; y < height; y++)
				SetBlock(x, y, z, (y == height - 1) ? ((y >= 63) ? Blocks::Grass : Blocks::Sand) : Blocks::Dirt);
		}
	}

	if ((m_chunkX != 0) || (m_chunkZ != 0))
		return;

	const int base = 62;

	// La caseta: tres paredes, el techo y el suelo
	for (int i = base + 1; i <= base + 3; i++) {
		SetBlock(7, i, 9, Blocks::StoneBrick);
		SetBlock(7, i, 8, Blocks::StoneBrick);
		SetBlock(7, i, 7, Blocks::StoneBrick);
		SetBlock(8, i, 7, Blocks::StoneBrick);
		SetBlock(9, i, 7, Blocks::StoneBrick);
		SetBlock(9, i, 8, Blocks::StoneBrick);
		SetBlock(9, i, 9, Blocks::StoneBrick);
	}

	SetBlock(8, base + 3, 8, Blocks::StoneBrick);
	SetBlock(8, base + 3, 9, Blocks::StoneBrick);

	SetBlock(7, base + 3, 10, Blocks::StoneBrick);
	SetBlock(8, base + 3, 10, Blocks::StoneBrick);
	SetBlock(9, base + 3, 10, Blocks::StoneBrick);

	SetBlock(7, base, 11, Blocks::StoneBrick);
	SetBlock(8, base, 11, Blocks::StoneBrick);
	SetBlock(9, base, 11, Blocks::StoneBrick);

	for (int i = 6; i <= 10; i++) {
		for (int j = 6; j <= 10; j++)
			SetBlock(i, base, j, Blocks::StoneBrick);
	}

	SetBlock(0, base + 2, 0, Blocks::Pattern);
	SetBlock(11, base + 2, 9, Blocks::Patito);

	// El pozo, dentro de la caseta, y la cueva
	for (int i = 1; i <= base; i++)
		SetBlock(8, i, 8, Blocks::Air);

	for (int i = 6; i <= 10; i++) {
		for (int j = 6; j <= 10; j++) {
			for (int k = base - 9; k <= base - 6; k++)
				SetBlock(i, k, j, Blocks::Air);
		}
	}

	SetBlock(7, base - 8, 7, Blocks::Diamond);
}

// Qué caras de un bloque dan al aire (un bit por cara). La de arriba del todo y la de abajo del todo no se pintan
uint8_t Chunk::BlockVisibleSides(int x, int y, int z) {
	uint8_t sides = 0;

	if (GetData(x + 1, y, z) == Blocks::Air)
		sides |= 1 << Blocks::Right;

	if (GetData(x - 1, y, z) == Blocks::Air)
		sides |= 1 << Blocks::Left;

	if ((y != Height - 1) && (GetData(x, y + 1, z) == Blocks::Air))
		sides |= 1 << Blocks::Up;

	if ((y != 0) && (GetData(x, y - 1, z) == Blocks::Air))
		sides |= 1 << Blocks::Down;

	if (GetData(x, y, z + 1) == Blocks::Air)
		sides |= 1 << Blocks::Front;

	if (GetData(x, y, z - 1) == Blocks::Air)
		sides |= 1 << Blocks::Back;

	return sides;
}

void Chunk::OptimizeAllGeometries() {
	if (m_optimized)
		return;

	m_visibleSides.assign(Size, 0);
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {
			for (int z = 0; z < Depth; z++) {
				if (GetBlock(x, y, z) != Blocks::Air)
					m_visibleSides[GetIndex(x, y, z)] = BlockVisibleSides(x, y, z);
			}
		}
	}

	m_mesh = new QuadMesh();
	for (int side = Blocks::Right; side <= Blocks::Back; side++)
		OptimizeGeometries(side);

	m_optimized = true;
	m_visibleSides.clear();
	m_visibleSides.shrink_to_fit();

	if (s_scene && !m_mesh->IsEmpty()) {
		s_scene->Add(m_mesh);
	} else {
		delete m_mesh;
		m_mesh = nullptr;
	}
}

// Si la cara de ese bloque puede unirse al rectángulo que se está formando: sin asignar a otro, del mismo bloque y
// con esa cara visible
bool Chunk::CanExpand(int x, int y, int z, int id, uint8_t block, uint8_t sideBit) const {
	return (id == Unassigned) && (GetBlock(x, y, z) == block) && ((m_visibleSides[GetIndex(x, y, z)] & sideBit) != 0);
}

// Las caras de un lado, capa a capa. En cada capa se recorren las caras visibles: cada una que no esté ya en un
// rectángulo empieza uno, que se estira primero a lo ancho y luego a lo alto mientras las caras sean del mismo bloque
void Chunk::OptimizeGeometries(int side) {
	uint8_t sideBit = (uint8_t) (1 << side);

	// Los tres ejes según el lado: el de las capas, el del ancho del rectángulo y el del alto
	int layers, across, along;
	switch (side) {
		case Blocks::Right:
		case Blocks::Left:
			layers = Width; // x
			across = Depth; // z
			along = Height; // y
			break;
		case Blocks::Up:
		case Blocks::Down:
			layers = Height; // y
			across = Width;	 // x
			along = Depth;	 // z
			break;
		default:
			layers = Depth;	 // z
			across = Width;	 // x
			along = Height;	 // y
			break;
	}

	// De (capa, ancho, alto) a (x, y, z)
	auto toBlock = [side](int layer, int a, int b, int &x, int &y, int &z) {
		switch (side) {
			case Blocks::Right:
			case Blocks::Left:
				x = layer;
				z = a;
				y = b;
				break;
			case Blocks::Up:
			case Blocks::Down:
				y = layer;
				x = a;
				z = b;
				break;
			default:
				z = layer;
				x = a;
				y = b;
				break;
		}
	};

	std::vector<int> ids(across * along);
	for (int layer = 0; layer < layers; layer++) {
		std::fill(ids.begin(), ids.end(), Unassigned);
		int next = 1;

		for (int b = 0; b < along; b++) {
			for (int a = 0; a < across; a++) {
				int x, y, z;
				toBlock(layer, a, b, x, y, z);
				uint8_t block = GetBlock(x, y, z);
				if ((block == Blocks::Air) || !CanExpand(x, y, z, ids[b * across + a], block, sideBit))
					continue;

				// A lo ancho
				ids[b * across + a] = next;
				int endA = a;
				for (int a2 = a + 1; a2 < across; a2++) {
					int x2, y2, z2;
					toBlock(layer, a2, b, x2, y2, z2);
					if (!CanExpand(x2, y2, z2, ids[b * across + a2], block, sideBit))
						break;

					ids[b * across + a2] = next;
					endA = a2;
				}

				// A lo alto: la fila entera tiene que poder unirse
				int endB = b;
				for (int b2 = b + 1; b2 < along; b2++) {
					bool canExpand = true;
					for (int a2 = a; (a2 <= endA) && canExpand; a2++) {
						int x2, y2, z2;
						toBlock(layer, a2, b2, x2, y2, z2);
						canExpand = CanExpand(x2, y2, z2, ids[b2 * across + a2], block, sideBit);
					}

					if (!canExpand)
						break;

					for (int a2 = a; a2 <= endA; a2++)
						ids[b2 * across + a2] = next;
					endB = b2;
				}

				int endX, endY, endZ;
				toBlock(layer, endA, endB, endX, endY, endZ);
				AddQuad(side, block, x, y, z, endX, endY, endZ);
				next++;
			}
		}
	}
}

// El rectángulo de una cara que va del bloque start al end (incluidos), con la textura repetida una vez por bloque
void Chunk::AddQuad(int side, uint8_t block, int startX, int startY, int startZ, int endX, int endY, int endZ) {
	float worldX = m_chunkX * Width;
	float worldZ = m_chunkZ * Depth;

	// Centro, ancho y alto del rectángulo, y hacia dónde van su derecha y su arriba (mirándolo de frente), como los
	// planos de la versión web (un PlaneGeometry girado según la cara)
	float center[3] = {worldX + (startX + endX) / 2.0f + 0.5f, (startY + endY) / 2.0f + 0.5f, worldZ + (startZ + endZ) / 2.0f + 0.5f};
	float right[3] = {0, 0, 0};
	float up[3] = {0, 0, 0};
	float width, height;

	switch (side) {
		case Blocks::Right:
			center[0] = worldX + startX + 1.0f;
			right[2] = -1;
			up[1] = 1;
			width = endZ - startZ + 1;
			height = endY - startY + 1;
			break;
		case Blocks::Left:
			center[0] = worldX + startX;
			right[2] = 1;
			up[1] = 1;
			width = endZ - startZ + 1;
			height = endY - startY + 1;
			break;
		case Blocks::Up:
			center[1] = startY + 1.0f;
			right[0] = 1;
			up[2] = -1;
			width = endX - startX + 1;
			height = endZ - startZ + 1;
			break;
		case Blocks::Down:
			center[1] = startY;
			right[0] = 1;
			up[2] = 1;
			width = endX - startX + 1;
			height = endZ - startZ + 1;
			break;
		case Blocks::Front:
			center[2] = worldZ + startZ + 1.0f;
			right[0] = 1;
			up[1] = 1;
			width = endX - startX + 1;
			height = endY - startY + 1;
			break;
		default: // Back
			center[2] = worldZ + startZ;
			right[0] = -1;
			up[1] = 1;
			width = endX - startX + 1;
			height = endY - startY + 1;
			break;
	}

	// Arriba izquierda, arriba derecha, abajo izquierda y abajo derecha: x, y, z, u, v
	float corners[4][5];
	for (int i = 0; i < 4; i++) {
		float horizontal = ((i & 1) ? 0.5f : -0.5f) * width;
		float vertical = ((i & 2) ? -0.5f : 0.5f) * height;
		for (int axis = 0; axis < 3; axis++)
			corners[i][axis] = center[axis] + right[axis] * horizontal + up[axis] * vertical;

		corners[i][3] = (i & 1) ? width : 0.0f;
		corners[i][4] = (i & 2) ? height : 0.0f;
	}

	m_mesh->AddQuad(Blocks::GetTexture(block, side), corners, FaceLight(side));
}
