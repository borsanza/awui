#pragma once

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

namespace awui::GOB::Engine {
	class QuadMesh;
	class Scene;
} // namespace awui::GOB::Engine

class SimplexNoise2D;

// Un trozo del mundo de 16x256x16 bloques (chunk.js en la versión web). El terreno sale del ruido, igual que en la
// web. Su geometría ("optimizar") junta las caras visibles vecinas del mismo bloque en rectángulos grandes, con la
// textura repetida: un chunk llano son un puñado de rectángulos en vez de miles de caras
class Chunk {
  public:
	static constexpr int Width = 16;
	static constexpr int Height = 256;
	static constexpr int Depth = 16;
	static constexpr int Size = Width * Depth * Height;

	// Distancia de visión, en chunks alrededor del jugador: los que se generan y se pintan. En la versión web se
	// midieron los triángulos del mundo con cada una, desde el punto de salida, y aquí salen los mismos: son los
	// calculados al arrancar (GetMeshedTriangles), que incluyen los de alrededor de (8, 8), donde se centra el mundo
	// antes de colocar al jugador:
	//    1: 1406     2: 3080     3: 5850     4: 9258     5: 12502
	//    6: 16448    7: 21944    8: 30130    9: 37864   10: 45006
	//   11: 53638   12: 62454   13: 72048   14: 81250   15: 95606
	//   16: 111142  17: 123798  18: 136616  19: 150294  20: 166380
	static constexpr int DefaultRenderDistance = 16;
	static constexpr int MaxRenderDistance = 20;

  private:
	static std::map<std::pair<int, int>, Chunk *> s_chunks;
	static std::vector<Chunk *> s_queue; // Los que esperan a que se calcule su geometría
	static double s_playerX;
	static double s_playerZ;
	static awui::GOB::Engine::Scene *s_scene;
	static SimplexNoise2D *s_noise;
	static int s_renderDistance;
	static int s_meshedTriangles;  // De todos los chunks con la geometría calculada
	static int s_visibleTriangles; // De los que están dentro de la distancia de visión (los que se pintan)

	int m_chunkX;
	int m_chunkZ;
	bool m_optimized;
	std::vector<uint8_t> m_blocks;
	std::vector<uint8_t> m_visibleSides; // Solo mientras se calcula la geometría
	awui::GOB::Engine::QuadMesh *m_mesh; // Es de la escena

	Chunk(int x, int z);

	static inline int GetIndex(int x, int y, int z) { return (y * Depth + z) * Width + x; }
	static bool IsInCircle(double px, double py, double cx, double cy, double radius);

	void FillChunk();
	bool IsNear() const;
	void QueueIfNear();
	double GetDistanceToPlayer() const;

	uint8_t BlockVisibleSides(int x, int y, int z);
	void OptimizeAllGeometries();
	void OptimizeGeometries(int side);
	bool CanExpand(int x, int y, int z, int id, uint8_t block, uint8_t sideBit) const;
	void AddQuad(int side, uint8_t block, int startX, int startY, int startZ, int endX, int endY, int endZ);

  public:
	// Antes de nada: la escena a la que se añaden las mallas
	static void SetScene(awui::GOB::Engine::Scene *scene);

	static Chunk *GetChunk(int x, int z);
	static Chunk *GetGlobalChunk(double x, double z);
	// El bloque en una posición del mundo (0: aire)
	static uint8_t GetGlobalBlock(double x, int y, double z);

	// Donde está el jugador: genera los chunks de alrededor y encola los que entran en el radio
	static void SetPlayerPosition(double x, double z);
	// Calcula la geometría de un chunk de la cola, el más cercano al jugador. Uno por frame para no dar tirones
	static void OptimizeOneMore();
	// Se puede cambiar en marcha: al subirla se generan los chunks que faltan, y al bajarla dejan de pintarse los que
	// quedan fuera (no se descargan: vuelven a verse al acercarse)
	static void SetRenderDistance(int distance);
	static inline int GetRenderDistance() { return s_renderDistance; }

	// Triángulos del mundo (dos por rectángulo): los que se pintan y todos los calculados
	static inline int GetVisibleTriangles() { return s_visibleTriangles; }
	static inline int GetMeshedTriangles() { return s_meshedTriangles; }

	static inline int GetQueueLength() { return (int) s_queue.size(); }
	static inline int GetChunkCount() { return (int) s_chunks.size(); }

	inline int GetChunkX() const { return m_chunkX; }
	inline int GetChunkZ() const { return m_chunkZ; }

	void SetBlock(int x, int y, int z, uint8_t block);
	uint8_t GetBlock(int x, int y, int z) const;
	// Como GetBlock, pero x y z pueden salirse un bloque: se mira en el chunk vecino
	uint8_t GetData(int x, int y, int z);
};
