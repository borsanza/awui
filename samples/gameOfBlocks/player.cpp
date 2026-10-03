/**
 * samples/gameOfBlocks/player.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "player.h"

#include "chunk.h"

#include <awui/GOB/Engine/Core/Object3D.h>
#include <awui/GOB/Engine/Geometries/BoxGeometry.h>
#include <awui/GOB/Engine/Materials/MeshBasicMaterial.h>
#include <awui/GOB/Engine/Objects/Mesh.h>
#include <awui/GOB/Engine/Scenes/Scene.h>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace awui::GOB::Engine;

namespace {
	const float Pi = 3.14159265f;

	// Velocidad al saltar para subir algo más de un bloque (1,2522)
	const float JumpForce = sqrtf(Player::Gravity * 2.0f * 1.2522f);

	// Una caja de un color, con su centro en (x, y, z) respecto al muñeco
	Mesh *Box(float width, float height, float depth, uint32_t color, float x, float y, float z) {
		Material *material = new MeshBasicMaterial(color);
		Mesh *mesh = new Mesh(new BoxGeometry(width, height, depth), std::vector<Material *>(6, material));
		mesh->SetPosition(x, y, z);
		return mesh;
	}
} // namespace

Player::Player(Scene *scene, double x, double y, double z) {
	m_x = x;
	m_y = y;
	m_z = z;
	m_rotationY = 0.0f;
	m_verticalSpeed = 0.0f;
	m_eyeTime = 0.0f;
	m_eyeOffset = 0.0f;
	m_onGround = true;
	m_moveForward = false;
	m_moveBackward = false;
	m_moveLeft = false;
	m_moveRight = false;
	m_running = false;
	m_jump = false;

	BuildModel(scene);
}

void Player::BuildModel(Scene *scene) {
	// Se mueve en cada frame: dinámico, para no rehacer la geometría del mundo
	m_model = new Object3D();
	m_model->SetDynamic(true);
	m_model->SetVisible(false);

	const uint32_t red = 0xff0000ff;
	const uint32_t green = 0x00ff00ff;
	const uint32_t blue = 0x0000ffff;

	// Cabeza, cuerpo, pierna izquierda y derecha, brazo izquierdo y derecho. Las alturas, desde medio bloque por
	// debajo de los pies (el muñeco se coloca ahí)
	m_model->Add(Box(0.5f, 0.5f, 0.5f, red, 0.0f, -0.5f / 2 + 0.65f + 0.65f + 0.5f + 0.5f, 0.0f));
	m_model->Add(Box(0.44f, 0.65f, 0.25f, green, 0.0f, -0.65f / 2 + 0.65f + 0.65f + 0.5f, 0.0f));
	m_model->Add(Box(0.215f, 0.65f, 0.25f, red, 0.215f / 2, -0.65f / 2 + 0.65f + 0.5f, 0.0f));
	m_model->Add(Box(0.215f, 0.65f, 0.25f, blue, -0.215f / 2, -0.65f / 2 + 0.65f + 0.5f, 0.0f));
	m_model->Add(Box(0.19f, 0.74f, 0.25f, red, 0.19f / 2 + 0.22f, -0.74f / 2 + 0.65f + 0.65f + 0.5f, 0.0f));
	m_model->Add(Box(0.19f, 0.74f, 0.25f, blue, -0.19f / 2 - 0.22f, -0.74f / 2 + 0.65f + 0.65f + 0.5f, 0.0f));

	scene->Add(m_model);
}

void Player::SetModelVisible(bool visible) {
	m_model->SetVisible(visible);
}

bool Player::Collides(double x, double y, double z) {
	// Tocar justo la cara de un bloque no es chocar: se quita un pelo por cada lado
	const double skin = 1e-6;
	double half = Width / 2.0;

	int minX = (int) floor(x - half + skin);
	int maxX = (int) floor(x + half - skin);
	int minY = (int) floor(y + skin);
	int maxY = (int) floor(y + Height - skin);
	int minZ = (int) floor(z - half + skin);
	int maxZ = (int) floor(z + half - skin);

	for (int blockY = minY; blockY <= maxY; blockY++) {
		for (int blockX = minX; blockX <= maxX; blockX++) {
			for (int blockZ = minZ; blockZ <= maxZ; blockZ++) {
				if (Chunk::GetGlobalBlock(blockX, blockY, blockZ) != 0)
					return true;
			}
		}
	}

	return false;
}

// Un eje cada vez: si en uno choca, se queda pegado a la cara del bloque y en el otro sigue (se desliza por la pared)
void Player::MoveHorizontal(double dx, double dz) {
	double half = Width / 2.0;

	if (dx != 0.0) {
		double newX = m_x + dx;
		if (Collides(newX, m_y, m_z))
			newX = (dx > 0.0) ? (floor(newX + half) - half) : (floor(newX - half) + 1.0 + half);
		if (!Collides(newX, m_y, m_z))
			m_x = newX;
	}

	if (dz != 0.0) {
		double newZ = m_z + dz;
		if (Collides(m_x, m_y, newZ))
			newZ = (dz > 0.0) ? (floor(newZ + half) - half) : (floor(newZ - half) + 1.0 + half);
		if (!Collides(m_x, m_y, newZ))
			m_z = newZ;
	}
}

// Gravedad y salto. Cayendo deprisa se avanza en pasos de menos de un bloque, para no atravesar el suelo
void Player::MoveVertical(float deltaSeconds) {
	// Ha dejado de tener suelo debajo (se ha salido de un borde)
	if (m_onGround && !Collides(m_x, m_y - 0.001, m_z))
		m_onGround = false;

	if (m_jump && m_onGround) {
		m_onGround = false;
		m_verticalSpeed = JumpForce;
	}

	if (m_onGround)
		return;

	double remaining = m_verticalSpeed * deltaSeconds;
	while (remaining != 0.0) {
		double step = std::clamp(remaining, -0.5, 0.5);
		remaining -= step;

		double newY = m_y + step;
		if (!Collides(m_x, newY, m_z)) {
			m_y = newY;
			continue;
		}

		if (step < 0.0) {
			// Se posa sobre el bloque
			m_y = floor(newY) + 1.0;
			m_onGround = true;
		} else {
			// Se da con la cabeza
			m_y = floor(newY + Height) - Height;
		}

		m_verticalSpeed = 0.0f;
		return;
	}

	m_verticalSpeed = std::max(m_verticalSpeed - Gravity * deltaSeconds, -MaxFallSpeed);
}

void Player::Update(float deltaSeconds) {
	float speed = (m_running ? RunSpeed : WalkSpeed) * deltaSeconds;

	// Adelante y atrás, en la dirección a la que mira, y a los lados
	float forward = -((m_moveForward ? speed : 0.0f) - (m_moveBackward ? speed : 0.0f));
	float sideways = -((m_moveLeft ? speed : 0.0f) - (m_moveRight ? speed : 0.0f));
	bool moving = (forward != 0.0f) || (sideways != 0.0f);
	double oldX = m_x;
	double oldZ = m_z;
	MoveHorizontal(sinf(m_rotationY) * forward + sinf(m_rotationY + Pi / 2) * sideways, cosf(m_rotationY) * forward + cosf(m_rotationY + Pi / 2) * sideways);
	moving = moving && ((m_x != oldX) || (m_z != oldZ)); // Contra una pared no se balancea la vista

	MoveVertical(deltaSeconds);

	if (m_onGround && moving) {
		m_eyeTime += deltaSeconds * 15.0f;
		m_eyeOffset = sinf(m_eyeTime) * 0.05f;
	}

	Chunk::SetPlayerPosition(m_x, m_z);

	m_model->SetRotation(0.0f, m_rotationY, 0.0f);
	m_model->SetPosition((float) m_x, (float) m_y - 0.5f, (float) m_z);
}
