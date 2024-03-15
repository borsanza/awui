#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Station/LabelButton.h>
#include <awui/Windows/Forms/Station/Settings/TypeConfigButton.h>

#include <functional>
#include <vector>

namespace awui::Windows::Forms {
	namespace Listeners {
		class IButtonListener;
	}

	namespace Station {
		namespace Browser {
			class Page;
		}

		namespace Settings {
			enum class TypeButton;

			class ConfigButton : public Control {
			  private:
				LabelButton m_label;
				bool m_group;
				Browser::Page *m_subpage;
				std::vector<Listeners::IButtonListener *> m_listeners;
				TypeButton m_typeButton;

			  public:
				ConfigButton(TypeButton typeButton);
				virtual ~ConfigButton();

				virtual bool IsClass(Classes objectClass) const override;

				const String GetText() const;
				void SetText(const String str);

				virtual void OnPaint(OpenGL::GL *gl);
				virtual void SetForeColor(const Drawing::Color color);
				virtual void SetFont(const Drawing::Font font);
				int GetLabelWidth() const;

				inline void SetGroup(bool mode) { m_group = mode; }
				inline bool IsGroup() const { return m_group; }

				virtual void OnResize();

				void SetSubPage(Browser::Page *subpage) { m_subpage = subpage; }
				Browser::Page *GetSubPage() const { return m_subpage; }
				TypeButton GetTypeButton() { return m_typeButton; }

				void Click();
				void AddOnClickListener(Listeners::IButtonListener *listener);
				void RemoveOnClickListener(Listeners::IButtonListener *listener);
				void RemoveAllListeners();
				void OnMouseUp(MouseEventArgs *e);
				bool OnKeyPress(Keys::Enum key);

				String ToString() const override;
			};
		} // namespace Settings
	}	  // namespace Station
} // namespace awui::Windows::Forms