#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Listeners/IButtonListener.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class SettingsWidget;

namespace awui::Windows::Forms {
	class Label;

	namespace Station {
		namespace Browser {
			class Browser;
			class Page;
		}

		namespace Settings {
			class SettingsUI : public Control, Listeners::IButtonListener {
				private:
					Browser::Browser * m_browser;
					Label * m_title;

					Browser::Page * ProcessJson(const json& j, int depth = 0);

				public:
					SettingsUI();

					virtual bool IsClass(Classes objectClass) const override;
					
					void InitializeComponent();

					virtual void OnTick(float deltaSeconds) override;

					virtual void OnClick(Control* sender) override;
			};
		}
	}
}
