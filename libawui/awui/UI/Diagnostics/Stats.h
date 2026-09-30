#pragma once

#include <awui/Time/ChronoLap.h>
#include <awui/UI/Label.h>
#include <awui/UI/Panel.h>

namespace awui::UI::Diagnostics {
	class Heartbeat;
	class Spinner;

	// Barra de depuración al pie del formulario (cada Form tiene la suya: Form::GetStats). Qué indicadores muestra se
	// elige en tiempo de ejecución; por defecto, solo los FPS. La barra entera se oculta con SetVisible
	class Stats : public Panel {
	  private:
		Label *m_labelFps;
		Label *m_labelWidgets;
		Heartbeat *m_heartbeat;
		Spinner *m_spinner;

		Time::ChronoLap m_fpsChronoLap;
		int m_frames;
		float m_fpsPreviousElapsedTime;
		int m_drawedControls;

		void ShowIndicator(Control *indicator, bool show);

	  public:
		// Cada cuánto se recalculan los FPS
		static constexpr float TimeToMeasure = 1.0f;

		Stats();
		virtual ~Stats() = default;

		// Fotogramas por segundo
		void SetShowFps(bool show);
		// Controles pintados en el último frame
		void SetShowWidgetCount(bool show);
		// Parpadea con cada latido del mando a distancia
		void SetShowHeartbeat(bool show);
		// Gira mientras el programa sigue vivo (para ver si se ha colgado)
		void SetShowSpinner(bool show);

		void SetTimeBeforeIddle();
		void SetTimeAfterIddle();
		virtual void OnRemoteHeartbeat() override;
		void SetDrawedControls(int drawedControls);
	};
} // namespace awui::UI::Diagnostics
