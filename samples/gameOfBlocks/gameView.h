#pragma once

#include <awui/GOB/Engine/Renderers/Renderer.h>
#include <awui/String.h>
#include <awui/UI/Input/Keys.h>

#include <vector>

namespace awui::GOB::Engine {
	class AxesHelper;
} // namespace awui::GOB::Engine

class Player;

// El mundo de bloques (world.js en la versión web): la escena, el jugador y sus dos cámaras.
//
//  - En primera persona se mira con el ratón (un clic lo captura, Escape lo suelta).
//  - En tercera persona la cámara gira alrededor del jugador arrastrando con el botón izquierdo.
//
// Teclas: W A S D para moverse, Ctrl para correr, Espacio para saltar, 5 para cambiar de cámara, 6 para ver la malla
// y 7 para ver los ejes
class GameView : public awui::GOB::Engine::Renderer {
  private:
	awui::GOB::Engine::Scene *m_scene;
	awui::GOB::Engine::Cameras::PerspectiveCamera *m_cameraPointer; // Primera persona
	awui::GOB::Engine::Cameras::PerspectiveCamera *m_cameraOrbit;	// Tercera persona
	awui::GOB::Engine::AxesHelper *m_axes;
	Player *m_player;

	bool m_firstPerson;
	bool m_mouseCaptured;

	// Hacia dónde mira la cámara en primera persona (radianes)
	float m_yaw;
	float m_pitch;

	// Cámara orbital: ángulos alrededor del jugador y lo que le queda por girar (se frena poco a poco)
	float m_orbitTheta;
	float m_orbitPhi;
	float m_orbitDeltaTheta;
	float m_orbitDeltaPhi;

	bool m_ctrlLeft;
	bool m_ctrlRight;

	void SetMouseCaptured(bool captured);
	void UpdateCameras(float deltaSeconds, int mouseX, int mouseY, bool dragging);

  public:
	static constexpr float OrbitDistance = 4.0f;

	GameView();
	virtual ~GameView();

	// Las teclas llegan al formulario, que las pasa. Devuelven true si son del juego
	bool KeyDown(awui::UI::Input::Keys::Enum key);
	bool KeyUp(awui::UI::Input::Keys::Enum key);

	void SetFirstPerson(bool firstPerson);
	inline bool IsFirstPerson() const { return m_firstPerson; }
	inline bool IsMouseCaptured() const { return m_mouseCaptured; }
	// Escape: devuelve true si había algo que soltar
	bool ReleaseMouse();

	// Las líneas del panel de información
	std::vector<awui::String> GetInfo() const;

	virtual void OnTick(float deltaSeconds) override;
	virtual void OnMouseDown(awui::UI::Events::MouseEventArgs *e) override;
};
