#pragma once

#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Label.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				class NodeFile;
				class StationUI;

				class LabelButton : public Label {
					private:
						bool _lastSelected;
						int _frames;

					public:
						LabelButton();
						virtual ~LabelButton();

						virtual void OnMouseDown(MouseEventArgs* e);
						virtual void OnTick();
				};

				class MenuButton : public Control {
					private:
						LabelButton _label;
						NodeFile * _node;
						StationUI * _station;

					public:
						MenuButton(StationUI * station);
						virtual ~MenuButton();

						virtual bool IsClass(Classes objectClass) const override;

						const String GetText();
						void SetText(const String str);

						virtual void OnMouseLeave();
						virtual void OnMouseDown(MouseEventArgs* e);
						virtual void OnPaint(OpenGL::GL* gl);
						virtual void SetForeColor(const Drawing::Color color);
						virtual void SetFont(const Drawing::Font font);
						int GetLabelWidth() const;
						virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

						void SetNodeFile(NodeFile * node);

						void CheckArcade();

						virtual void OnResize();
				};
			}
		}
	}
}
