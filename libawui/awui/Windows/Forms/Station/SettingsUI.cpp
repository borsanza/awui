/**
 * awui/Windows/Forms/Station/SettingsUI.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "SettingsUI.h"

#include <awui/Drawing/Color.h>

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Station;
using json = nlohmann::json;

SettingsUI::SettingsUI() {
	SetBackColor(Color::FromArgb(0, 0, 0));

	std::ifstream i("menu-settings.json");

	json j;
	i >> j;
	for (const auto& section : j) {
		std::cout << "Section: " << section["section"] << std::endl;
		for (const auto& item : section["items"].items()) {
			std::cout << "  Item: " << item.value()["key"] << std::endl;
		}
	}
}