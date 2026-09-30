#pragma once

#include <awui/UI/Control.h>
#include <awui/UI/Events/IExitListener.h>
#include <awui/UI/Events/IRemoteListener.h>

#include <vector>

namespace awui {
	namespace UI {
		namespace Emulators {
			class ArcadeContainer;
		}

		class Bitmap;
		class ImageFader;
		class Label;

		namespace Station {
			namespace Settings {
				class SettingsUI;
			}

			class Browser;
			class MenuButton;
			class Page;
			class SettingsWidget;
			class StationUI;

			class FadePanel : public Control {
			  private:
				float m_status;
				bool m_showing;
				StationUI *m_station;

			  public:
				FadePanel();
				~FadePanel();

				void ShowFade();
				void HideFade();
				inline bool IsShowing() const { return m_status > 0.0f; }
				inline bool IsFullScreen() const { return m_status >= 100.0f; }
				inline bool IsStopped() const { return ((m_status == 0.0f) || (m_status == 200.0f)); }
				virtual void OnTick(float deltaSeconds);

				inline void SetStationUI(StationUI *station) { m_station = station; }
			};

			class NodeFile : public awui::Object {
			  public:
				NodeFile *m_parent;
				bool m_directory;
				int m_emulator;
				String m_key;
				String m_name;
				String m_path;
				MenuButton *m_button;
				Page *m_page;
				Emulators::ArcadeContainer *m_arcade;
				std::vector<NodeFile *> m_children; // Ordenados por m_key; son suyos
				Bitmap *m_background;

			  public:
				NodeFile();
				virtual ~NodeFile();

				// Inserta en su sitio según m_key (con claves iguales, detrás de los que ya estaban)
				void AddChild(NodeFile *child);
			};

			class StationUI : public Control, public Events::IRemoteListener, public Events::IExitListener {
			  private:
				FadePanel m_fade;
				String m_path;
				NodeFile *m_root;
				NodeFile *m_actual;
				NodeFile *m_noRoms; // Aviso de lista vacía (su texto depende del idioma)
				Emulators::ArcadeContainer *m_arcade;

				Label *m_title;
				Browser *m_browser;
				SettingsWidget *m_settings;
				Label *m_clock;
				ImageFader *m_backgroundFader;
				Settings::SettingsUI *m_settingsUI;
				Control *m_controlBase;
				bool m_closeSettings;
				bool m_inGame; // Se ha entrado en el juego (no solo la vista previa del menú)
				bool m_clock24;
				bool m_showClock;

				void RecursiveSearch(NodeFile *parent);
				bool Minimize(NodeFile *parent);

				void UpdateTitle();

				void CheckArcade();
				void RefreshList();
				void CloseSettings();
				void SaveGame();

			  public:
				StationUI();
				virtual ~StationUI();

				void Clear();
				void SetPath(const String path);

				void Refresh();
				virtual void OnTick(float deltaSeconds);

				void SelectChild(NodeFile *node);
				void SelectParent();

				void SetArcade(Emulators::ArcadeContainer *arcade);
				void SetArcadeFullScreen();
				void EnteringArcade();
				void ExitingArcade();
				void OnClosing();
				void ExitArcade();

				void SetBackground(Bitmap *background);
				void ApplySettings();

				virtual void OnOk(Control *sender) override;
				virtual void OnMenu(Control *sender) override;
				virtual void OnExit(Control *sender) override;
			};
		} // namespace Station
	} // namespace UI
} // namespace awui
