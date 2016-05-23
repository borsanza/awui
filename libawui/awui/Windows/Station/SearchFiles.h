#pragma once

#ifndef _AWUI_WINDOWS_STATION_SEARCHFILES_H
#define _AWUI_WINDOWS_STATION_SEARCHFILES_H

#include <awui/String.h>
#include <awui/Collections/SortedList.h>

using namespace awui;

namespace awui {
	namespace Windows {
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

			class SearchFiles {
				private:
					String _path;
					NodeFile * _root;

					void RecursiveSearch(NodeFile * parent);
					bool Minimize(NodeFile * parent);

				public:
					SearchFiles();
					virtual ~SearchFiles();

					void Clear();
					void SetPath(const String path);

					void Refresh();
			};
		}
	}
}

#endif
