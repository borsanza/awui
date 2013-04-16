// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUICONTROL_H__
#define __AWUICONTROL_H__

#include <awui/Object.h>
#include <awui/String.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/Windows/Forms/MouseButtons.h>

namespace awui {
	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
			class ControlCollection;
			class MouseEventArgs;

			class Control : public Object {
				friend class Form;
			public:
				struct DockStyle {
					enum Enum {
						None,
						Top,
						Bottom,
						Left,
						Right,
						Fill,
					};
				};

			protected:
				Drawing::Rectangle bounds;
				Drawing::Size minimumSize;

				int needRefresh;
				int refreshed;
				DockStyle::Enum dock;
				ControlCollection * controls;
				Drawing::Color backColor;
				Control * parent;
				MouseEventArgs * mouseEventArgs;
				Control * mouseControl;
				String name;

				void OnTickPre();
				void OnResizePre();
				void OnPaintPre(int x, int y, int width, int height, OpenGL::GL * gl);
				void ChangeControlOnMouseOver(Control * control);

			public:
				Control();
				virtual ~Control();

				virtual int IsClass(Classes::Enum objectClass) const;

				const virtual Drawing::Size GetMinimumSize() const;
				void SetMinimumSize(Drawing::Size size);

				DockStyle::Enum GetDock() const;
				void SetDock(DockStyle::Enum dock);

				const String GetName();
				void SetName(const String str);

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
				void OnMouseUpPre(MouseButtons::Enum button, int buttons);
				void OnMouseDownPre(int x, int y, MouseButtons::Enum button, int buttons);

				virtual void Layout();
				virtual void OnMouseDown(MouseEventArgs* e) {}
				virtual void OnMouseMove(MouseEventArgs* e) {}
				virtual void OnMouseUp(MouseEventArgs* e) {}
				virtual void OnMouseEnter();
				virtual void OnMouseLeave();
				virtual void OnPaint(OpenGL::GL * gl) {}
				virtual void OnResize() {}
				virtual void OnTick() {}
			};
		}
	}
}
#endif
