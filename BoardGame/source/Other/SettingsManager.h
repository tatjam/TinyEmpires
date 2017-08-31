#pragma once
#include "yaml-cpp/yaml.h"
#include <string>
#include <iostream>
#include "SFML/Window.hpp"


#define ST_KEY sf::Keyboard
typedef sf::Keyboard::Key Key;

struct WindowSettings
{
	size_t width = 512, height = 512;
	bool fullscreen = false;
};

struct ControlSettings
{
	

	float triggers[4] = { 0.2f, 0.8f, 0.2f, 0.8f };
	bool allowMouseScroll;
	bool offscreenScroll;
	float scrollSpeed[2] = { 40.0f, 120.0f };

	float cameraZoomSpeed[2] = { 0.4f, 1.2f };
	std::vector<Key> speedKeys = { ST_KEY::LControl, ST_KEY::V };
	std::vector<Key> multiSelectKeys = { ST_KEY::LShift };
	std::vector<Key> upKeys = { ST_KEY::W, ST_KEY::Up };
	std::vector<Key> rightKeys = { ST_KEY::D, ST_KEY::Right };
	std::vector<Key> downKeys = { ST_KEY::S, ST_KEY::Down };
	std::vector<Key> leftKeys = { ST_KEY::A, ST_KEY::Left };
	std::vector<Key> incZoomKeys = { ST_KEY::R };
	std::vector<Key> decZoomKeys = { ST_KEY::F };
};

class SettingsManager
{
public:

	// Settings
	WindowSettings wSets;
	ControlSettings cSets;


	void load(std::string path);
	void write(std::string path);

	std::vector<Key> loadKeys(YAML::Node* root);

	SettingsManager();
	~SettingsManager();
};

