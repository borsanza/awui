#pragma once

#include <awui/String.h>
#include <awui/Collections/SortedList.h>

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				class NodeFile : public awui::Object {
					public:
						NodeFile * _parent;
						String _name;
						String _path;
						bool _directory;
						int _selectedChild;
						SortedList * _childList;
						int _emulator;

					public:
						NodeFile();
						virtual ~NodeFile();
				};

				class StationUI : public Control {
					private:
						String _path;
						NodeFile * _root;

						void RecursiveSearch(NodeFile * parent);
						bool Minimize(NodeFile * parent);

					public:
						StationUI();
						virtual ~StationUI();

						void Clear();
						void SetPath(const String path);

						void Refresh();
						void GetList(ArrayList * list, NodeFile * parent);
				};
			}
		}
	}
}
