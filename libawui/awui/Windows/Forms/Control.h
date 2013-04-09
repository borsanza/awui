// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUICONTROL_H__
#define __AWUICONTROL_H__

#include <awui/Object.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Rectangle.h>
#include <string>

class awuiGL;

class MouseButtons {
public:
	enum Buttons {
		None = 0,
		Left = 1,
		Right = 2,
		Middle = 4,
		XButton1 = 8,
		XButton2 = 16,
	};
};

namespace awui {
	namespace Windows {
		namespace Forms {
			class ControlCollection;
			class MouseEventArgs;

			class Control : public Object {
				friend class Form;
			public:
				enum DockStyle {
					None,
					Top,
					Bottom,
					Left,
					Right,
					Fill,
				};

			protected:
				Drawing::Rectangle bounds;
				Drawing::Size minimumSize;

				int needRefresh;
				int refreshed;
				Control::DockStyle dock;
				ControlCollection * controls;
				Drawing::Color backColor;
				Control * parent;
				MouseEventArgs * mouseEventArgs;
				Control * mouseControl;
				std::string name;

				void OnTickPre();
				void OnResizePre();
				void OnPaintPre(int x, int y, int width, int height, awuiGL * gl);
				void ChangeControlOnMouseOver(Control * control);

			public:
				Control();
				~Control();

				virtual int IsClass(Classes objectClass) const;

				const virtual Drawing::Size GetMinimumSize() const;
				void SetMinimumSize(Drawing::Size size);

				Control::DockStyle GetDock() const;
				void SetDock(Control::DockStyle dock);

				const std::string& GetName();
				void SetName(const std::string& str);

				int GetLeft() const;
				void SetLeft(int x);

				int GetTop() const;
				void SetTop(int y);

				const Drawing::Point GetLocation() const;
				void SetLocation(int x, int y);

				int GetWidth() const;
				void SetWidth(int width);

				int GetHeight() const;
				void SetHeight(int height);

				const Drawing::Size GetSize() const;
				void SetSize(int width, int height);
				void SetSize(const Drawing::Size size);

				const Drawing::Rectangle GetBounds() const;
				void SetBounds(int x, int y, int width, int height);

				int GetRight() const;
				int GetBottom() const;

				ControlCollection * GetControls() const;

				Drawing::Color GetBackColor();
				void SetBackColor(const Drawing::Color color);

				void Refresh();

				Control * GetParent() const;
				void SetParent(Control * parent);

				void OnMouseMovePre(int x, int y, int buttons);
				void OnMouseUpPre(MouseButtons::Buttons button, int buttons);
				void OnMouseDownPre(int x, int y, MouseButtons::Buttons button, int buttons);

				virtual void Layout();
				virtual void OnMouseDown(MouseEventArgs* e) {}
				virtual void OnMouseMove(MouseEventArgs* e) {}
				virtual void OnMouseUp(MouseEventArgs* e) {}
				virtual void OnMouseEnter();
				virtual void OnMouseLeave();
				virtual void OnPaint(awuiGL * gl) {}
				virtual void OnResize() {}
				virtual void OnTick() {}
			};
		}
	}
}
#endif
