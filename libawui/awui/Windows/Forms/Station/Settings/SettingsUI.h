#pragma once

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Listeners/IExitListener.h>
#include <awui/Windows/Forms/Listeners/IRemoteListener.h>

#include <functional>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace awui::Windows::Forms {
	class Bitmap;
	class ImageFader;
	class Label;

	namespace Station {
		class Browser;
		class Page;

		namespace Settings {
			class ConfigButton;

			// Menú de ajustes. Atrás (Esc, botón Menu o clic derecho) vuelve al grupo anterior y, desde la
			// raíz, avisa al IExitListener para que lo cierre. Cada cambio se guarda al momento y se avisa con onChanged.
			class SettingsUI : public Control, Listeners::IRemoteListener {
			  private:
				static const int DescriptionLines = 4;

				Browser *m_browser;
				Label *m_title;
				ImageFader *m_backgroundFader;
				Bitmap *m_background; // No es hijo de ningún control: se borra en el destructor
				Label *m_description[DescriptionLines];
				String m_lastDescription;

				Page *m_rootPage;
				std::vector<Page *> m_pages;		// Todas las páginas creadas (son de SettingsUI)
				std::vector<ConfigButton *> m_path; // Grupos abiertos, del primero al actual
				bool m_rebuild;

				Listeners::IExitListener *m_exitListener;
				std::function<void()> m_onChanged;

				Page *ProcessJson(const json &j);
				void Build();
				void DeletePages();
				void ShowPage(Page *page);
				void UpdateTitle();
				void UpdateDescription();
				void Rebuild();
				void OnValueChanged(ConfigButton *button);

			  public:
				SettingsUI();
				virtual ~SettingsUI();

				virtual bool IsClass(Classes objectClass) const override;

				void InitializeComponent();

				inline void SetExitListener(Listeners::IExitListener *listener) { m_exitListener = listener; }
				inline void SetOnChanged(std::function<void()> onChanged) { m_onChanged = onChanged; }

				virtual void OnTick(float deltaSeconds) override;

				virtual void OnOk(Control *sender) override;
				virtual void OnMenu(Control *sender) override;
			};
		} // namespace Settings
	}	  // namespace Station
} // namespace awui::Windows::Forms
