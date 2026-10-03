#pragma once

namespace awui::GOB::Engine {
	class Object3D;
	class Scene;
} // namespace awui::GOB::Engine

// El jugador (elsa.js en la versión web): se mueve con las teclas en la dirección a la que mira, corre, salta y cae
// con gravedad. Choca con los bloques por los lados, por abajo y por arriba (en la versión web solo con el suelo):
// ocupa una caja de Width x Height con los pies en su posición. Su muñeco (cabeza, cuerpo, brazos y piernas, de cajas
// de colores) solo se ve con la cámara en tercera persona
class Player {
  public:
	static constexpr float Gravity = 30.0f;
	static constexpr float EyeHeight = 1.62f;
	static constexpr float WalkSpeed = 4.317f; // Bloques por segundo
	static constexpr float RunSpeed = 5.612f;
	static constexpr float Width = 0.6f;		// De lado a lado y de delante a atrás
	static constexpr float Height = 1.8f;
	static constexpr float MaxFallSpeed = 78.4f; // Bloques por segundo

  private:
	awui::GOB::Engine::Object3D *m_model; // Es de la escena

	double m_x, m_y, m_z; // Los pies
	float m_rotationY;
	float m_verticalSpeed;
	float m_eyeTime;
	float m_eyeOffset;
	bool m_onGround;

	bool m_moveForward;
	bool m_moveBackward;
	bool m_moveLeft;
	bool m_moveRight;
	bool m_running;
	bool m_jump;

	void BuildModel(awui::GOB::Engine::Scene *scene);

	// Si la caja del jugador, con los pies en (x, y, z), toca algún bloque
	static bool Collides(double x, double y, double z);
	void MoveHorizontal(double dx, double dz);
	void MoveVertical(float deltaSeconds);

  public:
	Player(awui::GOB::Engine::Scene *scene, double x, double y, double z);

	inline void SetMoveForward(bool mode) { m_moveForward = mode; }
	inline void SetMoveBackward(bool mode) { m_moveBackward = mode; }
	inline void SetMoveLeft(bool mode) { m_moveLeft = mode; }
	inline void SetMoveRight(bool mode) { m_moveRight = mode; }
	inline void SetRunning(bool mode) { m_running = mode; }
	inline void SetJump(bool mode) { m_jump = mode; }

	// Hacia dónde mira (radianes alrededor del eje vertical): lo da la cámara en primera persona
	inline void SetRotation(float rotationY) { m_rotationY = rotationY; }
	void SetModelVisible(bool visible);

	void Update(float deltaSeconds);

	inline double GetX() const { return m_x; }
	inline double GetY() const { return m_y; }
	inline double GetZ() const { return m_z; }
	inline float GetVerticalSpeed() const { return m_verticalSpeed; }
	inline bool IsOnGround() const { return m_onGround; }
	// Balanceo de la vista al andar
	inline float GetEyeOffset() const { return m_eyeOffset; }
};
