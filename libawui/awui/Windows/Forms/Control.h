#pragma once

#include <awui/Core/Color.h>
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

	namespace Windows::Forms {
		class Bitmap;
		class Form;
		class MouseEventArgs;
		class JoystickButtonEventArgs;
		class JoystickAxisMotionEventArgs;

		enum class DockStyle {
			None,
			Top,
			Bottom,
			Left,
			Right,
			Fill,
		};

		class Control : public Object {
			friend class Form;

		  protected:
			int _positionInWindowX;
			int _positionInWindowY;

		  private:
			ArrayList *m_controls;
			Control *m_parent;
			static Bitmap *m_selectedBitmap;

			// Sirve para saber en que orden se han insertado los componentes
			static int32_t lastTabIndex;
			// Sirve para saber en que orden se hizo un focus
			static int32_t countFocused;

			int32_t m_tabIndex;
			Control *m_focused;
			int32_t m_focusedTime;
			bool m_focusable;

			bool m_drawShadow;
			bool m_preventChangeControl;
			bool m_visible;
			Drawing::Rectangle m_bounds;

			float m_lastRight;
			float m_lastBottom;
			float m_lastTop;
			float m_lastLeft;
			Drawing::Rectangle m_boundsTo;

			Drawing::Size m_minimumSize;
			int m_needRefresh;
			int m_refreshed;
			bool m_scissorEnabled;
			Drawing::Font *m_font;
			DockStyle m_dock;
			Color m_backColor;
			Color m_foreColor;
			MouseEventArgs *m_mouseEventArgs;
			Control *m_mouseControl;
			String m_name;

			float m_deltaSeconds;

			void OnResizePre();
			int OnPaintPre(int x, int y, int width, int height, OpenGL::GL *gl, bool first = false);
			void ChangeControlOnMouseOver(Control *control);
			bool IsVisible(bool checkInside = true) const;
			void CleanMouseControl();

			Control *GetChildFocusedImp(Control *focused);
			void SetFocusImpl(bool forced, int32_t time);
			Control *FindNextFocusableControl();
			void FixFocusImpl();

		  protected:
			void OnTickPre(float deltaSeconds);

		  public:
			Control();
			virtual ~Control();

			virtual bool IsClass(Classes objectClass) const override;

			const virtual Drawing::Size GetMinimumSize() const;
			void SetMinimumSize(Drawing::Size size);

			DockStyle GetDock() const;
			void SetDock(DockStyle dock);

			const String GetName();
			void SetName(const String str);

			int GetLeft() const;
			int GetAbsoluteLeft() const;
			int GetAbsoluteRight() const;
			void SetLeft(int x);

			int GetTop() const;
			int GetAbsoluteTop() const;
			int GetAbsoluteBottom() const;
			void SetTop(int y);

			const Drawing::Point GetLocation() const;
			void SetLocation(int x, int y);

			inline void SetDrawShadow(bool mode) { m_drawShadow = mode; }
			inline bool GetDrawShadow() { return m_drawShadow; }

			int GetWidth() const;
			void SetWidth(int width);

			int GetHeight() const;
			void SetHeight(int height);

			const Drawing::Size GetSize() const;
			void SetSize(int width, int height);
			void SetSize(const Drawing::Size size);
			inline void SetSizeGo(int w, int h) { m_boundsTo.SetSize(w, h); }
			inline void SetLocationGo(int x, int y) { m_boundsTo.SetLocation(x, y); }
			inline int GetLeftGo() { return m_boundsTo.GetLeft(); }
			inline int GetRightGo() { return m_boundsTo.GetRight(); }
			inline int GetTopGo() { return m_boundsTo.GetTop(); }
			inline int GetBottomGo() { return m_boundsTo.GetBottom(); }

			const Drawing::Rectangle GetBounds() const;
			void SetBounds(int x, int y, int width, int height);

			int GetRight() const;
			int GetBottom() const;

			void AddWidget(Control *control);
			void RemoveWidget(Control *control);
			int GetCount() const { return m_controls->GetCount(); }
			int IndexOf(Control *control) const { return m_controls->IndexOf(control); }
			Control *Get(int index) const { return (Control *) m_controls->Get(index); }
			void MoveToEnd(Control *item);
			void ReplaceWidget(Control *oldItem, Control *newItem);

			Color GetBackColor();
			void SetBackColor(const Color color);

			Color GetForeColor();
			virtual void SetForeColor(const Color color);

			Drawing::Font *GetFont();
			virtual void SetFont(const Drawing::Font font);

			void Refresh();

			Control *GetParent() const;
			void SetParent(Control *parent);

			void OnMouseMovePre(int x, int y, int buttons);
			void OnMouseUpPre(MouseButtons::Enum button, int buttons);
			void OnMouseDownPre(int x, int y, MouseButtons::Enum button, int buttons);
			void OnRemoteKeyPressPre(int which, RemoteButtons::Enum button);
			void OnRemoteKeyUpPre(int which, RemoteButtons::Enum button);
			void OnJoystickButtonDownPre(int which, int button, uint32_t buttons, uint32_t prevButtons);
			void OnJoystickButtonUpPre(int which, int button, uint32_t buttons, uint32_t prevButtons);
			void OnJoystickAxisMotionPre(int which, int16_t axisX, int16_t axisY);
			void OnKeyPressPre(Keys::Enum key);
			void OnKeyUpPre(Keys::Enum key);

			virtual void Layout();
			virtual void OnMouseDown(MouseEventArgs *e) {}
			virtual void OnMouseMove(MouseEventArgs *e) {}
			virtual void OnMouseUp(MouseEventArgs *e) {}
			virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
			virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
			virtual bool OnJoystickButtonDown(JoystickButtonEventArgs *e);
			virtual bool OnJoystickButtonUp(JoystickButtonEventArgs *e);
			virtual bool OnJoystickAxisMotion(JoystickAxisMotionEventArgs *e);
			virtual bool OnKeyPress(Keys::Enum key);
			virtual bool OnKeyUp(Keys::Enum key);
			virtual void OnRemoteHeartbeat() {}
			virtual void OnMouseEnter();
			virtual void OnMouseLeave();
			virtual void OnPaint(OpenGL::GL *gl);
			virtual void OnResize() {}
			virtual void OnTick(float deltaSeconds) {}
			void SetScissorEnabled(bool mode);
			bool GetScissorEnabled();

			bool IsFocusable();
			void SetFocusable(bool focusable);

			void SetFocus(bool forced = true);
			inline Control *GetFocused() const { return m_focused; }
			Control *GetChildFocused();
			bool IsFocused() const;

			Control *GetRoot();
			Form *GetForm();

			void GetControlsSelectables(Collections::ArrayList *list);

			inline void SetPreventChangeControl(bool mode) { m_preventChangeControl = mode; }

			void SetVisible(bool mode);
			void CheckMouseControl();

			static Bitmap *GetSelectedBitmap();

			virtual String ToString() const override;
		};
	} // namespace Windows::Forms
} // namespace awui
