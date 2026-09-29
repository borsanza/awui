#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Station/LabelButton.h>
#include <awui/Windows/Forms/Station/Settings/TypeConfigButton.h>

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace awui::Windows::Forms {
	namespace Listeners {
		class IRemoteListener;
	}

	namespace Station {
		class Page;

		namespace Settings {
			enum class TypeButton;

			// Una fila del menú de ajustes: nombre a la izquierda y, si tiene, el valor a la derecha.
			// Grupo, lista y opción: OK avisa a los listeners con OnOk (abrir el grupo, abrir la página de opciones
			// de la lista o elegir la opción). Sí/No: OK cambia el valor. Sí/No y lista: izquierda/derecha cambian
			// el valor. Los cambios de valor se avisan con onValueChanged y atrás con OnMenu.
			class ConfigButton : public Control {
			  private:
				LabelButton m_label;
				LabelButton m_value;
				Page *m_subpage;
				std::vector<Listeners::IRemoteListener *> m_listeners;
				std::function<void(ConfigButton *)> m_onValueChanged;
				TypeButton m_typeButton;
				std::string m_key;
				String m_description;

				bool m_boolValue; // Sí/No: el valor. Opción: si es la elegida
				String m_onText;
				String m_offText;
				std::vector<std::pair<std::string, String>> m_options; // código, nombre
				int m_selected;

				void UpdateValueText();
				void Step(int direction);

			  public:
				ConfigButton(TypeButton typeButton);
				virtual ~ConfigButton();

				const String GetText() const;
				void SetText(const String str);

				virtual void OnPaint(OpenGL::GL *gl);
				virtual void SetForeColor(const Color color);
				virtual void SetFont(const Drawing::Font font);
				int GetLabelWidth() const;

				inline bool IsGroup() const { return m_typeButton == TypeButton::Group; }

				virtual void OnResize();

				void SetSubPage(Page *subpage) { m_subpage = subpage; }
				Page *GetSubPage() const { return m_subpage; }
				TypeButton GetTypeButton() { return m_typeButton; }

				inline void SetKey(const std::string &key) { m_key = key; }
				inline const std::string &GetKey() const { return m_key; }
				inline void SetDescription(const String &description) { m_description = description; }
				inline const String &GetDescription() const { return m_description; }

				void SetBoolValue(bool value, const String &onText, const String &offText);
				inline bool GetBoolValue() const { return m_boolValue; }
				void SetOptions(const std::vector<std::pair<std::string, String>> &options, const std::string &selected);
				std::string GetListValue() const;
				void SetListValue(const std::string &code);
				inline void SetChecked(bool checked) { m_boolValue = checked; }
				inline void SetOnValueChanged(std::function<void(ConfigButton *)> onValueChanged) { m_onValueChanged = onValueChanged; }
				void SetValueText(const String &text);

				void Click();
				void AddOnClickListener(Listeners::IRemoteListener *listener);
				void RemoveOnClickListener(Listeners::IRemoteListener *listener);
				void RemoveAllListeners();
				void OnMouseDown(MouseEventArgs *e);
				bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
				bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

				String ToString() const override;
			};
		} // namespace Settings
	}	  // namespace Station
} // namespace awui::Windows::Forms
