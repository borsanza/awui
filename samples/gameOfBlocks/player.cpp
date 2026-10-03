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

void Player::Update(float deltaSeconds) {
	float speed = (m_running ? RunSpeed : WalkSpeed) * deltaSeconds;

	// Adelante y atrás, en la dirección a la que mira
	float step = -((m_moveForward ? speed : 0.0f) - (m_moveBackward ? speed : 0.0f));
	bool moving = step != 0.0f;
	m_x += sinf(m_rotationY) * step;
	m_z += cosf(m_rotationY) * step;

	// A los lados
	step = -((m_moveLeft ? speed : 0.0f) - (m_moveRight ? speed : 0.0f));
	moving = moving || (step != 0.0f);
	m_x += sinf(m_rotationY + Pi / 2) * step;
	m_z += cosf(m_rotationY + Pi / 2) * step;

	if (m_jump && m_onGround) {
		m_onGround = false;
		m_verticalSpeed = JumpForce;
	}

	// Gravedad: si no está quieto sobre un bloque, sube o cae; al pasar a un bloque sólido por abajo, se posa
	double newY = m_y + m_verticalSpeed * deltaSeconds;
	uint8_t blockBelow = Chunk::GetGlobalBlock(m_x, (int) floor(m_y - 1), m_z);
	if (!(m_onGround && (m_verticalSpeed == 0.0f) && (blockBelow != 0))) {
		uint8_t blockAtNewY = Chunk::GetGlobalBlock(m_x, (int) floor(newY), m_z);
		m_onGround = (floor(newY) < floor(m_y)) && (blockAtNewY != 0);
		if (m_onGround) {
			m_y = floor(m_y);
			m_verticalSpeed = 0.0f;
		} else {
			m_verticalSpeed -= Gravity * deltaSeconds;
			m_y = newY;
		}
	}

	if (m_onGround && moving) {
		m_eyeTime += deltaSeconds * 15.0f;
		m_eyeOffset = sinf(m_eyeTime) * 0.05f;
	}

	Chunk::SetPlayerPosition(m_x, m_z);

	m_model->SetRotation(0.0f, m_rotationY, 0.0f);
	m_model->SetPosition((float) m_x, (float) m_y - 0.5f, (float) m_z);
}
