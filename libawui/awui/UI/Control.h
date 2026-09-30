#pragma once

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Rectangle.h>
#include <awui/String.h>
#include <awui/UI/Input/Keys.h>
#include <awui/UI/Input/MouseButtons.h>
#include <awui/UI/Input/RemoteButtons.h>

#include <vector>

namespace awui {
	namespace Drawing {
		class Font;
	}

	namespace OpenGL {
		class GL;
	}

	namespace UI {
		class Bitmap;
		class Form;
		namespace Events {
			class MouseEventArgs;
			class JoystickButtonEventArgs;
			class JoystickAxisMotionEventArgs;
		} // namespace Events

		// Quién libera un control hijo:
		//   Owned: el padre (lo borra en su destructor). Para controles creados con new para ese padre.
		//   Borrowed: otro (un miembro, un objeto compartido, algo de un modelo de datos). El padre solo lo suelta.
		// Además, un control se suelta solo de su padre al destruirse, así que borrarlo nunca deja punteros colgando.
		enum class WidgetOwnership {
			Owned,
			Borrowed,
		};

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

		  private:
			std::vector<Control *> m_controls;
			Control *m_parent;
			bool m_ownedByParent; // Lo borra su padre al destruirse (WidgetOwnership::Owned)
			static Bitmap *s_selectedBitmap;

			// Sirve para saber en que orden se han insertado los componentes
			static int32_t s_lastTabIndex;
			// Sirve para saber en que orden se hizo un focus
			static int32_t s_countFocused;

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
			Drawing::Color m_backColor;
			Drawing::Color m_foreColor;
			Events::MouseEventArgs *m_mouseEventArgs;
			Control *m_mouseControl;
			String m_name;

			void OnResizePre();
			void SetPosition(bool setX, int x, bool setY, int y);
			void SetDimensions(bool setWidth, int width, bool setHeight, int height);
			int OnPaintPre(int x, int y, int width, int height, OpenGL::GL *gl, bool first = false);
			void ChangeControlOnMouseOver(Control *control);
			bool IsVisible(bool checkInside = true) const;
			void CleanMouseControl();
			void ForgetMouse();

			Control *GetChildFocusedImp(Control *focused);
			void SetFocusImpl(bool forced, int32_t time);
			Control *FindNextFocusableControl();
			void FixFocusImpl();

		  protected:
			void OnTickPre(float deltaSeconds);

		  public:
			Control();
			virtual ~Control();

			const virtual Drawing::Size GetMinimumSize() const;
			void SetMinimumSize(Drawing::Size size);

			DockStyle GetDock() const;
			void SetDock(DockStyle dock);

			String GetName() const;
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
			inline bool GetDrawShadow() const { return m_drawShadow; }

			int GetWidth() const;
			void SetWidth(int width);

			int GetHeight() const;
			void SetHeight(int height);

			const Drawing::Size GetSize() const;
			void SetSize(int width, int height);
			void SetSize(const Drawing::Size size);
			inline void SetSizeGo(int w, int h) { m_boundsTo.SetSize(w, h); }
			inline void SetLocationGo(int x, int y) { m_boundsTo.SetLocation(x, y); }
			inline int GetLeftGo() const { return m_boundsTo.GetLeft(); }
			inline int GetRightGo() const { return m_boundsTo.GetRight(); }
			inline int GetTopGo() const { return m_boundsTo.GetTop(); }
			inline int GetBottomGo() const { return m_boundsTo.GetBottom(); }

			const Drawing::Rectangle GetBounds() const;
			void SetBounds(int x, int y, int width, int height);

			int GetRight() const;
			int GetBottom() const;

			void AddWidget(Control *control, WidgetOwnership ownership = WidgetOwnership::Owned);
			void RemoveWidget(Control *control);
			int GetCount() const { return (int) m_controls.size(); }
			int IndexOf(Control *control) const;
			// Fuera de rango devuelve nullptr
			Control *Get(int index) const { return ((index >= 0) && (index < GetCount())) ? m_controls[index] : nullptr; }
			void MoveToEnd(Control *item);
			void ReplaceWidget(Control *oldItem, Control *newItem, WidgetOwnership ownership = WidgetOwnership::Owned);

			Drawing::Color GetBackColor() const;
			void SetBackColor(const Drawing::Color color);

			Drawing::Color GetForeColor() const;
			virtual void SetForeColor(const Drawing::Color color);

			Drawing::Font *GetFont();
			virtual void SetFont(const Drawing::Font font);

			void Refresh();

			Control *GetParent() const;
			void SetParent(Control *parent);

			void OnMouseMovePre(int x, int y, int buttons);
			void OnMouseUpPre(Input::MouseButtons::Enum button, int buttons, int clicks = 1);
			void OnMouseDownPre(int x, int y, Input::MouseButtons::Enum button, int buttons, int clicks = 1);
			// Rueda: delta > 0 hacia arriba, < 0 hacia abajo (una unidad por muesca). Devuelve si algún control la ha usado
			bool OnMouseWheelPre(int x, int y, int delta);
			void OnRemoteKeyPressPre(int which, Input::RemoteButtons::Enum button);
			void OnRemoteKeyUpPre(int which, Input::RemoteButtons::Enum button);
			void OnJoystickButtonDownPre(int which, int button, uint32_t buttons, uint32_t prevButtons);
			void OnJoystickButtonUpPre(int which, int button, uint32_t buttons, uint32_t prevButtons);
			void OnJoystickAxisMotionPre(int which, int16_t axisX, int16_t axisY);
			void OnKeyPressPre(Input::Keys::Enum key);
			void OnKeyUpPre(Input::Keys::Enum key);

			virtual void Layout();
			virtual void OnMouseDown(Events::MouseEventArgs *e) {}
			virtual void OnMouseMove(Events::MouseEventArgs *e) {}
			virtual void OnMouseUp(Events::MouseEventArgs *e) {}
			// La rueda le llega al control que está bajo el ratón; si devuelve false, sube a su padre
			virtual bool OnMouseWheel(Events::MouseEventArgs *e) { return false; }
			virtual bool OnRemoteKeyPress(int which, Input::RemoteButtons::Enum button);
			virtual bool OnRemoteKeyUp(int which, Input::RemoteButtons::Enum button);
			virtual bool OnJoystickButtonDown(Events::JoystickButtonEventArgs *e);
			virtual bool OnJoystickButtonUp(Events::JoystickButtonEventArgs *e);
			virtual bool OnJoystickAxisMotion(Events::JoystickAxisMotionEventArgs *e);
			virtual bool OnKeyPress(Input::Keys::Enum key);
			virtual bool OnKeyUp(Input::Keys::Enum key);
			virtual void OnRemoteHeartbeat() {}
			virtual void OnMouseEnter();
			virtual void OnMouseLeave();
			virtual void OnPaint(OpenGL::GL *gl);
			virtual void OnResize() {}
			virtual void OnTick(float deltaSeconds) {}
			// Un hijo acaba de salir (RemoveWidget, ReplaceWidget o porque se ha destruido)
			virtual void OnWidgetRemoved(Control *control) {}
			void SetScissorEnabled(bool mode);
			bool GetScissorEnabled() const;

			bool IsFocusable() const;
			void SetFocusable(bool focusable);

			void SetFocus(bool forced = true);
			inline Control *GetFocused() const { return m_focused; }
			Control *GetChildFocused();
			bool IsFocused() const;

			Control *GetRoot();
			Form *GetForm();

			// Aviso breve en pantalla del formulario de este control (abajo a la izquierda; sustituye al que se esté
			// viendo). Sin formulario no hace nada
			void ShowNotification(const String &text);

			void GetControlsSelectables(std::vector<Control *> &list);

			inline void SetPreventChangeControl(bool mode) { m_preventChangeControl = mode; }

			void SetVisible(bool mode);
			inline bool GetVisible() const { return m_visible; }
			void CheckMouseControl();

			static Bitmap *GetSelectedBitmap();

			virtual String ToString() const override;
		};
	} // namespace UI
} // namespace awui
