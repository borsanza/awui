#pragma once

#include <awui/Collections/SortedList.h>
#include <awui/Windows/Forms/Gradient.h>
#include <awui/Windows/Forms/Station/MenuButton.h>
#include <awui/Windows/Emulators/ArcadeContainer.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				namespace Browser {
					class Browser;
					class Page;
				}

				class FadePanel : public Control {
					private:
						float _status;
						bool _showing;
						StationUI * _station;

					public:
						FadePanel();
						~FadePanel();

						void ShowFade();
						void HideFade();
						inline bool IsShowing() const { return this->_status > 0.0f; }
						inline bool IsFullScreen() const { return this->_status >= 100.0f; }
						inline bool IsStopped() const { return ((this->_status == 0.0f) || (this->_status == 200.0f)); }
						virtual void OnTick();

						inline void SetStationUI(StationUI * station) { this->_station = station; }

				};

				class NodeFile : public awui::Object {
					public:
						NodeFile * _parent;
						String _name;
						String _path;
						bool _directory;
						SortedList * _childList;
						int _emulator;
						MenuButton * _button;
						Browser::Page * _page;

						Emulators::ArcadeContainer * _arcade;

					public:
						NodeFile();
						virtual ~NodeFile();
				};

				class StationUI : public Control {
					private:
						FadePanel _fade;
						String _path;
						NodeFile * _root;
						NodeFile * _actual;
						Emulators::ArcadeContainer * _arcade;

						Label * _title;
						Browser::Browser * _browser;

						void RecursiveSearch(NodeFile * parent);
						bool Minimize(NodeFile * parent);

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
						virtual void OnTick();

						void SelectChild(NodeFile * node);
						void SelectParent();

						void SetArcade(Emulators::ArcadeContainer * arcade);
						void SetArcadeFullScreen();
						void ExitingArcade();
						void ExitArcade();
				};
			}
		}
	}
}
