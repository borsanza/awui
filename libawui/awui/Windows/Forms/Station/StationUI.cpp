/**
 * awui/Windows/Forms/Station/StationUI.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "StationUI.h"

#include <awui/Console.h>
#include <awui/Emulation/Common/AudioSettings.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>
#include <awui/Localization.h>
#include <awui/Math.h>
#include <awui/Windows/Emulators/ArcadeContainer.h>
#include <awui/Windows/Forms/Bitmap.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/ImageFader.h>
#include <awui/Windows/Forms/Station/Browser.h>
#include <awui/Windows/Forms/Station/MenuButton.h>
#include <awui/Windows/Forms/Station/Page.h>
#include <awui/Windows/Forms/Station/Settings/SettingsStore.h>
#include <awui/Windows/Forms/Station/Settings/SettingsUI.h>
#include <awui/Windows/Forms/Statistics/Stats.h>
#include <awui/Windows/Forms/Station/SettingsWidget.h>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

#define BORDERMARGIN 50
#define MENUBUTTONHEIGHT 70

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Station::Settings;

StationUI::StationUI() {

	m_controlBase = new Control();

	m_actual = 0;
	m_fade.SetStationUI(this);
	m_arcade = nullptr;
	m_root = nullptr;
	m_settingsUI = nullptr;
	m_closeSettings = false;
	m_inGame = false;
	m_clock24 = true;
	m_showClock = true;
	m_noRoms = nullptr;

	m_backgroundFader = new ImageFader();
	m_backgroundFader->SetDock(DockStyle::Fill);
	m_backgroundFader->SetColor(ColorF::FromArgb(0.25f, 1.0f, 1.0f, 1.0f));

	m_controlBase->AddWidget(m_backgroundFader);

	Font font = Font("Liberation Sans", 38, FontStyle::Bold);
	Font font2 = Font("Liberation Sans", 22, FontStyle::Bold);
	Font fontClock = Font("Liberation Sans", 26, FontStyle::Bold);
	m_title = new Label();
	m_title->SetText("StationTV");
	m_title->SetTextAlign(ContentAlignment::BottomCenter);
	m_title->SetFont(font);
	m_title->SetDock(DockStyle::None);

	m_browser = new Browser();
	m_controlBase->AddWidget(m_title);
	m_controlBase->AddWidget(m_browser);

	m_settings = new SettingsWidget();
	m_settings->AddOnClickListener(this);
	m_settings->AddOnExitListener(this);
	m_settings->SetDock(DockStyle::None);
	m_settings->SetFont(font2);
	m_settings->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	m_settings->SetSize(44, 46);
	m_controlBase->AddWidget(m_settings);

	m_clock = new Label();
	m_clock->SetDock(DockStyle::None);
	m_clock->SetFont(fontClock);
	m_clock->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	m_clock->SetForeColor(Color::FromArgb(151, 151, 151));
	m_clock->SetTextAlign(ContentAlignment::TopCenter);
	m_clock->SetText("11:59");

	AddWidget(m_controlBase);
	m_controlBase->SetDock(DockStyle::Fill);

	// El reloj va fuera de m_controlBase para que se siga viendo con los ajustes abiertos (que ocultan el menú)
	AddWidget(m_clock);
}

StationUI::~StationUI() {
	// Los nodos son dueños de sus botones, páginas y emuladores: al borrarlos se sueltan solos del árbol
	m_arcade = nullptr;
	m_backgroundFader->Clear();
	if (m_root) {
		delete m_root;
		m_root = nullptr;
	}
}

void StationUI::SetPath(const String path) {
	m_path = path;
}

void StationUI::Clear() {
	// Lo que se muestra es de los nodos que se van a borrar
	SetArcade(nullptr);
	m_backgroundFader->Clear();
	m_noRoms = nullptr;
	if (m_root) {
		delete m_root;
		m_root = 0;
	}
}

void StationUI::SetBackground(Bitmap *background) {
	m_backgroundFader->SetImage(background);
}

void StationUI::RecursiveSearch(NodeFile *parent) {
	DIR *d;
	struct dirent *dir;
	d = opendir(parent->m_path.ToCharArray());
	if (d) {
		while ((dir = readdir(d)) != nullptr) {
			if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0)) {
				continue;
			}

			NodeFile *child = new NodeFile();

			String newFile = parent->m_path;

			if (!parent->m_path.EndsWith("/")) {
				newFile += "/";
			}

			newFile += dir->d_name;
			child->m_name = dir->d_name;
			child->m_background = nullptr;

			child->m_button = new MenuButton(this);
			child->m_button->SetNodeFile(child);
			// Sin la extensión, si la tiene (una carpeta como "48" o un ".oculto" se quedan como están)
			String name = child->m_name;
			int dot = name.LastIndexOf('.');
			if (dot > 0)
				name = name.Substring(0, dot);
			child->m_button->SetText(name);

			if (parent->m_emulator == Types::Undefined) {
				if (child->m_name == "chip8") {
					child->m_emulator = Types::Chip8;
					child->m_button->SetText("CHIP-8");
					child->m_background = new Bitmap("./images/chip8.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}

				if (child->m_name == "gamegear") {
					child->m_emulator = Types::GameGear;
					child->m_button->SetText("Game Gear");
					child->m_background = new Bitmap("./images/gamegear.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}

				if (child->m_name == "mastersystem") {
					child->m_emulator = Types::MasterSystem;
					child->m_button->SetText("Master System");
					child->m_background = new Bitmap("./images/mastersystem.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}

				// SG-1000: lo ejecuta el emulador de Master System (su VDP incluye los modos del TMS9918)
				if (child->m_name == "sg1000") {
					child->m_emulator = Types::MasterSystem;
					child->m_button->SetText("SG-1000");
					child->m_background = new Bitmap("./images/mastersystem.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}

				if (child->m_name == "zxspectrum") {
					child->m_emulator = Types::Spectrum;
					child->m_button->SetText("ZX Spectrum");
					child->m_background = new Bitmap("./images/zxspectrum.jpg");
					child->m_background->SetStretchMode(StretchMode::AspectFill);
				}
			} else {
				child->m_emulator = parent->m_emulator;
			}


			child->m_path = newFile;
			child->m_parent = parent;

			bool isDir = false;
			struct stat statbuf;
			if (stat(newFile.ToCharArray(), &statbuf) != -1) {
				isDir = S_ISDIR(statbuf.st_mode);
			}

			child->m_directory = isDir;

			child->m_key = String::Concat((child->m_directory ? "1" : "2"), child->m_name);
			parent->AddChild(child);

			if (child->m_directory) {
				RecursiveSearch(child);
			}
		}

		closedir(d);
	}
}

bool StationUI::Minimize(NodeFile *parent) {
	int r = false;

	for (int i = (int) parent->m_children.size() - 1; i >= 0; i--) {
		NodeFile *child = parent->m_children[i];

		if (child->m_directory) {
			r |= Minimize(child);

			if (!child->m_children.empty()) {
				continue;
			}
		} else {
			String path = child->m_path.ToLower();
			switch (child->m_emulator) {
				case Types::Chip8:
					if (path.EndsWith(".ch8") || path.EndsWith(".c8x")) {
						continue;
					}
					break;
				case Types::GameGear:
				case Types::MasterSystem:
					if (path.EndsWith(".sms") || path.EndsWith(".sg") || path.EndsWith(".gg")) {
						continue;
					}
					break;
				case Types::Spectrum:
					if (path.EndsWith(".rom") || path.EndsWith(".tap")) {
						continue;
					}
					break;
			}
		}

		delete child;
		parent->m_children.erase(parent->m_children.begin() + i);
		r = true;
	}

	return r;
}

void StationUI::Refresh() {
	Clear();
	m_root = new NodeFile();
	m_actual = m_root;
	m_root->m_path = m_path;
	m_root->m_emulator = Types::Undefined;
	RecursiveSearch(m_root);
	while (Minimize(m_root))
		;

	// Sin ROMs (carpeta vacía o sin ficheros válidos): un aviso en la lista
	if (m_root->m_children.empty()) {
		NodeFile *child = new NodeFile();
		child->m_name = Localization::Tr("station.noRoms");
		child->m_key = child->m_name;
		m_noRoms = child;
		child->m_directory = false;
		child->m_button = new MenuButton(this);
		child->m_button->SetNodeFile(child);
		child->m_button->SetText(child->m_name);
		m_root->AddChild(child);
	}

	RefreshList();
}

void StationUI::RefreshList() {
	if (m_actual->m_page == nullptr) {
		int y = 25;
		m_actual->m_page = new Page();
		for (int i = 0; i < (int) m_actual->m_children.size(); i++) {
			NodeFile *child = m_actual->m_children[i];
			child->m_button->SetHeight(MENUBUTTONHEIGHT);
			child->m_button->SetLocation(40, y);
			y += MENUBUTTONHEIGHT;
			m_actual->m_page->AddWidget(child->m_button, WidgetOwnership::Borrowed);
			if (i == 0) {
				child->m_button->SetFocus();
			}
		}

		m_actual->m_page->SetHeight(y + 25);
	}

	m_browser->SetPage(m_actual->m_page);
}

void StationUI::OnTick(float deltaSeconds) {
	static Control *lastFocused = nullptr;
	Control *c = m_actual->m_page->GetFocused();
	if (lastFocused != c) {
		lastFocused = c;
		CheckArcade();
	}

	// Se cierra aquí y no en OnExit: OnExit llega desde un botón del propio menú, que se borra con él
	if (m_closeSettings) {
		m_closeSettings = false;
		CloseSettings();
	}

	time_t t;
	time(&t);
	struct tm *tm;
	tm = localtime(&t);
	String horaS;
	if (m_clock24) {
		horaS = String("%02d:%02d", tm->tm_hour, tm->tm_min);
	} else {
		int hour = tm->tm_hour % 12;
		horaS = String("%d:%02d %s", (hour == 0) ? 12 : hour, tm->tm_min, (tm->tm_hour < 12) ? "AM" : "PM");
	}

	if (m_clock->GetText().CompareTo(horaS) != 0) {
		m_clock->SetText(horaS);
	}

	// Se ve con el menú o con los ajustes, pero no con un juego a pantalla completa
	bool clockVisible = m_showClock && (m_controlBase->GetVisible() || m_settingsUI);
	if (m_clock->GetVisible() != clockVisible) {
		m_clock->SetVisible(clockVisible);
	}

	// Reloj pegado a la derecha y el botón de ajustes a su izquierda (o en su sitio si el reloj está oculto)
	int clockLeft = GetWidth() - 10 - m_clock->GetLabelWidth();
	m_clock->SetLocation(clockLeft, 16);
	m_clock->SetSize(m_clock->GetLabelWidth(), 45);

	m_settings->SetLocation((m_showClock ? clockLeft : GetWidth() - 10) - 70, 8);

	m_title->SetLocation(GetWidth() >> 1, 0);
	m_title->SetSize(GetWidth() >> 1, 69);
	m_browser->SetLocation(GetWidth() >> 1, 69);
	m_browser->SetSize(GetWidth() >> 1, GetHeight() - (69 + 25));
	m_actual->m_page->SetWidth(m_browser->GetWidth());

	for (int i = 0; i < m_actual->m_page->GetCount(); i++) {
		Control *child = m_actual->m_page->Get(i);
		child->SetWidth(m_browser->GetWidth() - 100);
	}

	m_fade.SetBounds(0, 0, GetWidth(), GetHeight());

	if (m_arcade) {
		if (m_fade.IsFullScreen()) {
			m_arcade->SetBounds(0, 0, GetWidth(), GetHeight());
		} else {
			m_arcade->SetLocation(BORDERMARGIN, m_browser->GetTop());
			m_arcade->SetSize((GetWidth() >> 1) - BORDERMARGIN, m_browser->GetHeight());
		}
	}
}

void StationUI::SelectChild(NodeFile *node) {
	if (node->m_directory) {
		m_actual = node;
		RefreshList();
		UpdateTitle();
	} else {
		if (node->m_emulator != Types::Undefined) {
			if (!m_fade.IsStopped()) {
				return;
			}

			if (IndexOf(&m_fade) == -1) {
				AddWidget(&m_fade, WidgetOwnership::Borrowed);
			}

			MoveToEnd(&m_fade);
			m_fade.ShowFade();
		}
	}
}

void StationUI::SelectParent() {
	if (m_actual != nullptr) {
		if (m_actual->m_parent != nullptr) {
			m_actual = m_actual->m_parent;
			m_browser->SetPage(m_actual->m_page);
			UpdateTitle();
		}
	}
}

void StationUI::UpdateTitle() {
	if (m_actual == m_root) {
		m_title->SetText("StationTV");
	} else {
		m_title->SetText(m_actual->m_button->GetText());
	}
}

void StationUI::CheckArcade() {
	MenuButton *c = (MenuButton *) m_actual->m_page->GetFocused();
	if (c) {
		c->CheckArcade();
	}
}

void StationUI::SetArcade(Emulators::ArcadeContainer *arcade) {
	if (m_arcade == arcade) {
		return;
	}

	if (m_arcade && arcade) {
		ReplaceWidget(m_arcade, arcade, WidgetOwnership::Borrowed);
	} else {
		if (m_arcade) {
			RemoveWidget(m_arcade);
		}

		if (arcade) {
			AddWidget(arcade, WidgetOwnership::Borrowed);
		}
	}

	if (m_arcade) {
		m_arcade->SetSoundEnabled(false);
		m_arcade = nullptr;
	}

	if (arcade) {
		m_arcade = arcade;
		m_arcade->SetSoundEnabled(true);
	}
}

void StationUI::SetArcadeFullScreen() {
	RemoveWidget(&m_fade);
	m_controlBase->SetVisible(false);
	m_arcade->SetFocusable(true);
}

// Se ha entrado en el juego (a pantalla completa): se continúa la partida guardada si el ajuste lo pide
void StationUI::EnteringArcade() {
	if (m_inGame || !m_arcade) {
		return;
	}

	m_inGame = true;
	if (SettingsStore::Instance().GetBool("resumeGames")) {
		m_arcade->LoadAutoState();
	}
}

// Se sale del juego (al menú o cerrando el programa): se guarda la partida para continuarla
void StationUI::SaveGame() {
	if (m_inGame && m_arcade) {
		m_arcade->SaveAutoState();
	}

	m_inGame = false;
}

void StationUI::OnClosing() {
	SaveGame();
}

void StationUI::ExitingArcade() {
	if (!m_fade.IsStopped()) {
		return;
	}

	SaveGame();

	m_controlBase->SetVisible(true);

	m_fade.HideFade();
	AddWidget(&m_fade, WidgetOwnership::Borrowed);
	m_arcade->SetFocusable(false);
	m_browser->SetFocus();
}

void StationUI::ExitArcade() {
	RemoveWidget(&m_fade);
}

void StationUI::OnOk(Control *sender) {
	if (m_settingsUI) {
		return;
	}

	m_controlBase->SetVisible(false);

	m_settingsUI = new SettingsUI();
	m_settingsUI->SetDock(DockStyle::Fill);
	m_settingsUI->SetExitListener(this);
	m_settingsUI->SetOnChanged([this]() { ApplySettings(); });
	AddWidget(m_settingsUI);
	m_settingsUI->InitializeComponent();
	MoveToEnd(m_clock);
	CheckMouseControl();
}

void StationUI::OnMenu(Control *sender) {
}

void StationUI::OnExit(Control *sender) {
	if (m_settingsUI && (sender == m_settingsUI)) {
		m_closeSettings = true;
	}
}

void StationUI::CloseSettings() {
	if (!m_settingsUI) {
		return;
	}

	RemoveWidget(m_settingsUI);
	delete m_settingsUI;
	m_settingsUI = nullptr;

	m_controlBase->SetVisible(true);
	m_settings->SetFocus();
	CheckMouseControl();
}

// Aplica los ajustes guardados. Se llama al arrancar y cada vez que cambia uno en el menú
void StationUI::ApplySettings() {
	SettingsStore &settings = SettingsStore::Instance();

	Localization::SetLanguage(settings.GetString("language"));
	if (m_noRoms) {
		m_noRoms->m_name = Localization::Tr("station.noRoms");
		m_noRoms->m_button->SetText(m_noRoms->m_name);
	}

	Form *form = GetForm();
	if (form) {
		form->SetFullscreen(settings.GetBool("fullScreen") ? 1 : 0);

		bool vsync = settings.GetBool("vsync");
		if (form->GetSwapInterval() != vsync) {
			form->SetSwapInterval(vsync);
		}
	}

	Statistics::Stats::Instance()->SetVisible(settings.GetBool("fps"));
	m_showClock = settings.GetBool("clock");
	m_clock24 = settings.GetString("timeFormat") != "12";

	Emulation::Common::AudioSettings::SetEnabled(settings.GetBool("sound"));
	Emulation::Common::AudioSettings::SetVolume(atoi(settings.GetString("volume").c_str()));

	Emulation::MasterSystem::Sound::SetFMEnabled(settings.GetBool("fmSound"));

	for (int i = 0; i < 4; i++) {
		Emulation::MasterSystem::SoundSDL::SetChannelEnabled(i, settings.GetBool(String("channel%d", i + 1).ToCharArray()));
	}
}

/********************************* FadePanel **********************************/

FadePanel::FadePanel() {
	m_station = nullptr;
	m_showing = false;
	m_status = 0.0f;
}

FadePanel::~FadePanel() {
}

void FadePanel::ShowFade() {
	m_showing = true;
}

void FadePanel::HideFade() {
	m_showing = false;
}

void FadePanel::OnTick(float deltaSeconds) {
	// 200 unidades en 1/3 de segundo, sea cual sea la tasa de frames
	float step = 600.0f * deltaSeconds;
	if (m_showing) {
		// Con la pantalla en negro (mitad del fundido) se entra en el juego: si se continúa una partida, el
		// salto no se ve
		bool black = m_status >= 100.0f;
		m_status += step;
		if (!black && (m_status >= 100.0f))
			m_station->EnteringArcade();

		if (Math::Round(m_status) >= 200.0f) {
			m_status = 200.0f;
			m_station->SetArcadeFullScreen();
		}
	} else {
		m_status -= step;
		if (Math::Round(m_status) <= 0.0f) {
			m_status = 0.0f;
			m_station->ExitArcade();
		}
	}

	if (m_status <= 100.0f) {
		SetBackColor(Color::FromArgb(m_status * 2.55f, 0, 0, 0));
	} else {
		SetBackColor(Color::FromArgb((200.0f - m_status) * 2.55f, 0, 0, 0));
	}
}

/********************************** NodeFile **********************************/

NodeFile::NodeFile() {
	m_parent = 0;
	m_directory = true;
	m_emulator = Types::Undefined;
	m_button = nullptr;
	m_page = nullptr;
	m_arcade = nullptr;
	m_background = nullptr;
}

void NodeFile::AddChild(NodeFile *child) {
	auto pos = std::upper_bound(m_children.begin(), m_children.end(), child, [](const NodeFile *a, const NodeFile *b) { return a->m_key < b->m_key; });
	m_children.insert(pos, child);
}

// El nodo es dueño de su botón, de la página con los botones de sus hijos, de su emulador (en el árbol de
// controles están prestados) y de su imagen de fondo. Primero los hijos, que vacían la página al borrar sus botones
NodeFile::~NodeFile() {
	for (NodeFile *object : m_children) {
		delete object;
	}

	delete m_page;
	delete m_button;
	delete m_arcade;
	delete m_background;
}
