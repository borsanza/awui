#pragma once

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/String.h>
#include <awui/Windows/Forms/Keys.h>
#include <awui/Windows/Forms/MouseButtons.h>
#include <awui/Windows/Forms/RemoteButtons.h>

namespace awui {
	namespace Drawing {
		class Font;
	}

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
							None = 0,
							Top = 1,
							Bottom = 2,
							Left = 3,
							Right = 4,
							Fill = 5,
						};
					};

				private:
					Control * focused;
					bool tabStop;
					bool _drawShadow;
					bool _preventChangeControl;
					Drawing::Rectangle bounds;

					float _lastWidth;
					float _lastHeight;
					float _lastX;
					float _lastY;
					Drawing::Rectangle boundsTo;

					Drawing::Size minimumSize;
					int needRefresh;
					int refreshed;
					bool scissorEnabled;
					Drawing::Font * font;
					DockStyle::Enum dock;
					ControlCollection * controls;
					Drawing::Color backColor;
					Drawing::Color foreColor;
					Control * parent;
					MouseEventArgs * mouseEventArgs;
					Control * mouseControl;
					String name;

					void OnResizePre();
					int OnPaintPre(int x, int y, int width, int height, OpenGL::GL * gl, bool first = false);
					void ChangeControlOnMouseOver(Control * control);
					bool IsVisible() const;

				protected:
					void OnTickPre();

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
					int GetAbsoluteLeft() const;
					void SetLeft(int x);

					int GetTop() const;
					int GetAbsoluteTop() const;
					void SetTop(int y);

					const Drawing::Point GetLocation() const;
					void SetLocation(int x, int y);

					inline void SetDrawShadow(bool mode) { this->_drawShadow = mode; }
					inline bool GetDrawShadow() { return this->_drawShadow; }

					int GetWidth() const;
					void SetWidth(int width);

					int GetHeight() const;
					void SetHeight(int height);

					const Drawing::Size GetSize() const;
					void SetSize(int width, int height);
					void SetSize(const Drawing::Size size);
					inline void SetSizeGo(int w, int h) { this->boundsTo.SetSize(w, h); }
					inline void SetLocationGo(int x, int y) { this->boundsTo.SetLocation(x, y); }
					inline int GetLeftGo() { return this->boundsTo.GetLeft(); }
					inline int GetRightGo() { return this->boundsTo.GetRight(); }
					inline int GetTopGo() { return this->boundsTo.GetTop(); }
					inline int GetBottomGo() { return this->boundsTo.GetBottom(); }

					const Drawing::Rectangle GetBounds() const;
					void SetBounds(int x, int y, int width, int height);

					int GetRight() const;
					int GetBottom() const;

					ControlCollection * GetControls() const;

					Drawing::Color GetBackColor();
					void SetBackColor(const Drawing::Color color);

					Drawing::Color GetForeColor();
					virtual void SetForeColor(const Drawing::Color color);

					Drawing::Font * GetFont();
					virtual void SetFont(const Drawing::Font font);

					void Refresh();

					Control * GetParent() const;
					void SetParent(Control * parent);

					void OnMouseMovePre(int x, int y, int buttons);
					void OnMouseUpPre(MouseButtons::Enum button, int buttons);
					void OnMouseDownPre(int x, int y, MouseButtons::Enum button, int buttons);
					void OnRemoteKeyPressPre(int which, RemoteButtons::Enum button);
					void OnRemoteKeyUpPre(int which, RemoteButtons::Enum button);
					void OnKeyPressPre(Keys::Enum key);
					void OnKeyUpPre(Keys::Enum key);

					virtual void Layout();
					virtual void OnMouseDown(MouseEventArgs* e) {}
					virtual void OnMouseMove(MouseEventArgs* e) {}
					virtual void OnMouseUp(MouseEventArgs* e) {}
					virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
					virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
					virtual bool OnKeyPress(Keys::Enum key);
					virtual bool OnKeyUp(Keys::Enum key);
					virtual void OnRemoteHeartbeat() {}
					virtual void OnMouseEnter();
					virtual void OnMouseLeave();
					virtual void OnPaint(OpenGL::GL * gl);
					virtual void OnResize() {}
					virtual void OnTick() {}
					void SetScissorEnabled(bool mode);
					bool GetScissorEnabled();

					bool GetTabStop();
					void SetTabStop(bool tabStop);

					void SetFocus(bool selectControl = true);
					inline Control * GetFocused() const { return this->focused; }

					Control * GetTopParent();

					void GetControlsSelectables(Collections::ArrayList * list);
					Control * GetNextControl(Control * ommitControl, Drawing::Point * pCenter, Drawing::Point * p1, Drawing::Point * p2);

					float Interpolate(float from, int to, float percent);

					inline void SetPreventChangeControl(bool mode) { this->_preventChangeControl = mode; }
			};
		}
	}
}
