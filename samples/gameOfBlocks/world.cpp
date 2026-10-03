/**
 * samples/gameOfBlocks/world.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "world.h"

#include "chunk.h"
#include "player.h"

#include <awui/GOB/Engine/Geometries/BoxGeometry.h>
#include <awui/GOB/Engine/Materials/MeshBasicMaterial.h>
#include <awui/GOB/Engine/Objects/AxesHelper.h>
#include <awui/GOB/Engine/Objects/Mesh.h>

#include <SDL.h>

#include <algorithm>
#include <cmath>

using namespace awui;
using namespace awui::GOB::Engine;
using namespace awui::GOB::Engine::Cameras;
using namespace awui::UI::Input;

namespace {
	const float Pi = 3.14159265f;

	// Radianes por píxel de ratón en primera persona (como PointerLockControls)
	const float LookSpeed = 0.002f;

	// Cámara orbital (como OrbitControls con enableDamping): en cada sesentavo de segundo gira esta parte de lo que
	// le queda, que se va frenando
	const float OrbitDamping = 0.05f;

	// Una barra de color con su centro en (x, y, z)
	Mesh *Bar(float width, float height, float depth, uint32_t color, float x, float y, float z) {
		Material *material = new MeshBasicMaterial(color);
		Mesh *mesh = new Mesh(new BoxGeometry(width, height, depth), std::vector<Material *>(6, material));
		mesh->SetPosition(x, y, z);
		return mesh;
	}
} // namespace

World::World() {
	m_firstPerson = true;
	m_mouseCaptured = false;
	m_yaw = 0.0f;
	m_pitch = 0.0f;
	m_orbitTheta = 0.0f;
	m_orbitPhi = Pi / 3.0f; // Por detrás del jugador y algo por encima
	m_orbitDeltaTheta = 0.0f;
	m_orbitDeltaPhi = 0.0f;
	m_ctrlLeft = false;
	m_ctrlRight = false;

	m_scene = new Scene();
	m_cameraPointer = new PerspectiveCamera(60, 16.0f / 9.0f, 0.1f, 1000);
	m_cameraOrbit = new PerspectiveCamera(60, 16.0f / 9.0f, 0.1f, 1000);

	m_axes = new AxesHelper(8);
	m_axes->SetPosition(8, 64, 11);
	m_axes->SetVisible(false);
	m_scene->Add(m_axes);

	// Tres barras con los ejes sobre el bloque de prueba del origen: X roja, Y verde, Z azul
	const int base = 62;
	m_scene->Add(Bar(1.125f, 0.125f, 0.125f, 0xff0000ff, 0.5f, base + 2 + 0.5f, 0.5f));
	m_scene->Add(Bar(0.125f, 1.125f, 0.125f, 0x00ff00ff, 0.5f, base + 2 + 0.5f, 0.5f));
	m_scene->Add(Bar(0.125f, 0.125f, 1.125f, 0x0000ffff, 0.5f, base + 2 + 0.5f, 0.5f));

	Chunk::SetScene(m_scene);
	Chunk::SetPlayerPosition(8, 8);
	m_player = new Player(m_scene, 6, 63, 16);

	SetScene(m_scene);
	SetCamera(m_cameraPointer);
	SetClearColor(0xaad1feff);
	UpdateCameras(0.0f, 0, 0, false);
}

World::~World() {
	SetMouseCaptured(false);
	delete m_player;
	delete m_scene;
	delete m_cameraPointer;
	delete m_cameraOrbit;
}

// Capturado, el ratón no sale de la ventana ni se ve, y solo cuenta cuánto se mueve
void World::SetMouseCaptured(bool captured) {
	if (m_mouseCaptured == captured)
		return;

	m_mouseCaptured = captured;
	SDL_SetRelativeMouseMode(captured ? SDL_TRUE : SDL_FALSE);
	SDL_GetRelativeMouseState(nullptr, nullptr); // Lo movido hasta ahora no cuenta
}

bool World::ReleaseMouse() {
	if (!m_mouseCaptured)
		return false;

	SetMouseCaptured(false);
	return true;
}

void World::SetFirstPerson(bool firstPerson) {
	m_firstPerson = firstPerson;
	m_player->SetModelVisible(!firstPerson);
	SetCamera(firstPerson ? m_cameraPointer : m_cameraOrbit);
	SetMouseCaptured(firstPerson);

	// La cámara orbital empieza detrás del jugador
	if (!firstPerson) {
		m_orbitTheta = m_yaw;
		m_orbitDeltaTheta = 0.0f;
		m_orbitDeltaPhi = 0.0f;
	}
}

void World::OnMouseDown(UI::Events::MouseEventArgs *e) {
	if (m_firstPerson)
		SetMouseCaptured(true);
}

bool World::KeyDown(Keys::Enum key) {
	switch (key) {
		case Keys::Key_W:
			m_player->SetMoveForward(true);
			return true;
		case Keys::Key_S:
			m_player->SetMoveBackward(true);
			return true;
		case Keys::Key_A:
			m_player->SetMoveLeft(true);
			return true;
		case Keys::Key_D:
			m_player->SetMoveRight(true);
			return true;
		case Keys::Key_SPACE:
			m_player->SetJump(true);
			return true;
		case Keys::Key_LCTRL:
			m_ctrlLeft = true;
			m_player->SetRunning(true);
			return true;
		case Keys::Key_RCTRL:
			m_ctrlRight = true;
			m_player->SetRunning(true);
			return true;
		case Keys::Key_5:
			SetFirstPerson(!m_firstPerson);
			return true;
		case Keys::Key_6:
			SetWireframe(!GetWireframe());
			return true;
		case Keys::Key_7:
			m_axes->SetVisible(!m_axes->IsVisible());
			return true;
		case Keys::Key_8:
			Chunk::SetRenderDistance(Chunk::GetRenderDistance() - 1);
			return true;
		case Keys::Key_9:
			Chunk::SetRenderDistance(Chunk::GetRenderDistance() + 1);
			return true;
		default:
			return false;
	}
}

bool World::KeyUp(Keys::Enum key) {
	switch (key) {
		case Keys::Key_W:
			m_player->SetMoveForward(false);
			return true;
		case Keys::Key_S:
			m_player->SetMoveBackward(false);
			return true;
		case Keys::Key_A:
			m_player->SetMoveLeft(false);
			return true;
		case Keys::Key_D:
			m_player->SetMoveRight(false);
			return true;
		case Keys::Key_SPACE:
			m_player->SetJump(false);
			return true;
		case Keys::Key_LCTRL:
			m_ctrlLeft = false;
			m_player->SetRunning(m_ctrlRight);
			return true;
		case Keys::Key_RCTRL:
			m_ctrlRight = false;
			m_player->SetRunning(m_ctrlLeft);
			return true;
		default:
			return false;
	}
}

void World::OnTick(float deltaSeconds) {
	// Tras un parón (arrastrar la ventana, un tirón) no se da un salto: el jugador podría atravesar el suelo
	deltaSeconds = std::min(deltaSeconds, 0.05f);

	// La geometría de un chunk más de los que esperan
	Chunk::OptimizeOneMore();

	int mouseX = 0;
	int mouseY = 0;
	Uint32 buttons = SDL_GetRelativeMouseState(&mouseX, &mouseY);
	bool dragging = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;

	if (m_firstPerson && m_mouseCaptured) {
		m_yaw -= mouseX * LookSpeed;
		m_pitch -= mouseY * LookSpeed;
		m_pitch = std::clamp(m_pitch, -Pi / 2 + 0.001f, Pi / 2 - 0.001f);
	}

	m_player->SetRotation(m_yaw);
	m_player->Update(deltaSeconds);

	UpdateCameras(deltaSeconds, mouseX, mouseY, dragging);
}

void World::UpdateCameras(float deltaSeconds, int mouseX, int mouseY, bool dragging) {
	float x = (float) m_player->GetX();
	float y = (float) m_player->GetY() + Player::EyeHeight;
	float z = (float) m_player->GetZ();

	// Primera persona: en los ojos del jugador, con su balanceo al andar
	float eyeY = y + m_player->GetEyeOffset();
	m_cameraPointer->SetPosition(x, eyeY, z);
	m_cameraPointer->LookAt(x - sinf(m_yaw) * cosf(m_pitch), eyeY + sinf(m_pitch), z - cosf(m_yaw) * cosf(m_pitch));

	// Tercera persona: arrastrar una altura de ventana es una vuelta entera
	if (!m_firstPerson && dragging && (GetHeight() > 0)) {
		m_orbitDeltaTheta -= 2.0f * Pi * mouseX / GetHeight();
		m_orbitDeltaPhi -= 2.0f * Pi * mouseY / GetHeight();
	}

	float steps = deltaSeconds * 60.0f;
	float applied = 1.0f - powf(1.0f - OrbitDamping, steps);
	m_orbitTheta += m_orbitDeltaTheta * applied;
	m_orbitPhi = std::clamp(m_orbitPhi + m_orbitDeltaPhi * applied, 0.01f, Pi - 0.01f);
	m_orbitDeltaTheta *= 1.0f - applied;
	m_orbitDeltaPhi *= 1.0f - applied;

	m_cameraOrbit->SetPosition(x + OrbitDistance * sinf(m_orbitPhi) * sinf(m_orbitTheta), y + OrbitDistance * cosf(m_orbitPhi), z + OrbitDistance * sinf(m_orbitPhi) * cosf(m_orbitTheta));
	m_cameraOrbit->LookAt(x, y, z);
}

std::vector<String> World::GetInfo() const {
	double x = m_player->GetX();
	double y = m_player->GetY();
	double z = m_player->GetZ();
	int blockX = (int) floor(x);
	int blockZ = (int) floor(z);
	Chunk *chunk = Chunk::GetGlobalChunk(x, z);

	std::vector<String> lines;
	lines.push_back(String("OnGround: %s", m_player->IsOnGround() ? "true" : "false"));
	lines.push_back(String("Vertical Speed: %.2f", m_player->GetVerticalSpeed()));
	lines.push_back(String("XYZ: %.5f / %.5f / %.5f", x, y, z));
	lines.push_back(String("Block: %d / %d / %d [%d %d]", blockX, (int) floor(y), blockZ, blockX - chunk->GetChunkX() * Chunk::Width, blockZ - chunk->GetChunkZ() * Chunk::Depth));
	lines.push_back(String("Chunk: %d / %d", chunk->GetChunkX(), chunk->GetChunkZ()));
	// Los del mundo (dos por rectángulo): los que se pintan y, como en la versión web, todos los calculados (los de
	// su tabla: ver chunk.h)
	lines.push_back(String("Triangles: %d (meshed: %d)", Chunk::GetVisibleTriangles(), Chunk::GetMeshedTriangles()));
	lines.push_back(String("Distance: %d  Draw calls: %d  Chunks: %d (+%d)", Chunk::GetRenderDistance(), GetDrawCalls(), Chunk::GetChunkCount(), Chunk::GetQueueLength()));
	return lines;
}
