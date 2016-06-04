#pragma once

#include <awui/Collections/SortedList.h>
#include <awui/Windows/Forms/Gradient.h>
#include <awui/Windows/Forms/Station/MenuButton.h>

namespace awui {
	namespace Effects {
		class Effect;
	}

	namespace Windows {
		namespace Forms {
			namespace Station {
				namespace Browser {
					class Browser;
					class Page;
				}

				class NodeFile : public awui::Object {
					public:
						NodeFile * _parent;
						String _name;
						String _path;
						bool _directory;
						int _selectedChild;
						SortedList * _childList;
						int _emulator;
						MenuButton * _label;
						Browser::Page * _page;

					public:
						NodeFile();
						virtual ~NodeFile();
				};

				class StationUI : public Control {
					private:
						String _path;
						NodeFile * _root;
						NodeFile * _actual;
						int _selected;
						int _margin;
						Control * _lastControl;
						float _lastTime;
						int _initPos;

						Label * _label;
						Browser::Browser * _browser;

						void RecursiveSearch(NodeFile * parent);
						bool Minimize(NodeFile * parent);

					public:
						StationUI();
						virtual ~StationUI();

						void Clear();
						void SetPath(const String path);

						void Refresh();
						void GetList(ArrayList * list, NodeFile * parent);
						virtual void OnTick();
				};
			}
		}
	}
}
