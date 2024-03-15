#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Listeners/IButtonListener.h>

namespace awui {
	namespace Collections {
		class SortedList;
	}

	namespace Windows {
		namespace Emulators {
			class ArcadeContainer;
		}

		namespace Forms {
			class Bitmap;
			class ImageFader;
			class Label;

			namespace Station {
				class MenuButton;
				class SettingsWidget;
				class StationUI;

				namespace Settings {
					class SettingsUI;
				}

				namespace Browser {
					class Browser;
					class Page;
				} // namespace Browser

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
					Browser::Page *m_page;
					Emulators::ArcadeContainer *m_arcade;
					SortedList *m_childList;
					Bitmap *m_background;

				  public:
					NodeFile();
					virtual ~NodeFile();
				};

				using namespace awui::Windows::Forms::Listeners;

				class StationUI : public Control, IButtonListener {
				  private:
					FadePanel m_fade;
					String m_path;
					NodeFile *m_root;
					NodeFile *m_actual;
					Emulators::ArcadeContainer *m_arcade;

					Label *m_title;
					Browser::Browser *m_browser;
					SettingsWidget *m_settings;
					Label *m_clock;
					ImageFader *m_backgroundFader;
					Settings::SettingsUI *m_settingsUI;
					Control *m_controlBase;

					void RecursiveSearch(NodeFile *parent);
					bool Minimize(NodeFile *parent);

					void UpdateTitle();

					void CheckArcade();
					void RefreshList();

				  public:
					StationUI();
					virtual ~StationUI();

					void Clear();
					void SetPath(const String path);

					void Refresh();
					// void GetList(ArrayList * list, NodeFile * parent);
					virtual void OnTick(float deltaSeconds);

					void SelectChild(NodeFile *node);
					void SelectParent();

					void SetArcade(Emulators::ArcadeContainer *arcade);
					void SetArcadeFullScreen();
					void ExitingArcade();
					void ExitArcade();

					void SetBackground(Bitmap *background);

					virtual void OnClick(Control *sender) override;
				};
			} // namespace Station
		}	  // namespace Forms
	}		  // namespace Windows
} // namespace awui
