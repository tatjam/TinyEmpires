#include "SettingsManager.h"



void SettingsManager::load(std::string path)
{
	try
	{


		std::cout << "Loading settings from: " << path << std::endl;
		YAML::Node config = YAML::LoadFile(path);

		wSets.width = config["window"]["width"].as<size_t>();
		wSets.height = config["window"]["height"].as<size_t>();
		wSets.fullscreen = config["window"]["fullscreen"].as<bool>();

		cSets.allowMouseScroll = config["controls"]["allow_mouse_scroll"].as<bool>();
		cSets.offscreenScroll = config["controls"]["offscreen_scroll"].as<bool>();
		cSets.triggers[0] = config["controls"]["triggers"]["left"].as<float>();
		cSets.triggers[1] = config["controls"]["triggers"]["right"].as<float>();
		cSets.triggers[2] = config["controls"]["triggers"]["top"].as<float>();
		cSets.triggers[3] = config["controls"]["triggers"]["down"].as<float>();

		cSets.scrollSpeed[0] = config["controls"]["scroll_speed"]["slow"].as<float>();
		cSets.scrollSpeed[1] = config["controls"]["scroll_speed"]["fast"].as<float>();

		cSets.upKeys = loadKeys(&config["controls"]["up_keys"]);
		cSets.rightKeys = loadKeys(&config["controls"]["right_keys"]);
		cSets.downKeys = loadKeys(&config["controls"]["down_keys"]);
		cSets.leftKeys = loadKeys(&config["controls"]["left_keys"]);

	}
	catch (YAML::Exception e)
	{
		std::cout << "Error in the " << path << " config file. Default settings used" << std::endl;
		std::cout << "Error (line: " << e.mark.line << ", col: " << e.mark.column << "): " << e.msg << std::endl;
	}
}

std::vector<Key> SettingsManager::loadKeys(YAML::Node* root)
{
	std::vector<Key> out;

	YAML::Node sub = *root;

	for (size_t i = 0; i < sub.size(); i++)
	{
		int asInt = sub[i].as<int>();
		Key asKey = (Key)asInt;
		out.push_back(asKey);
	}

	return out;
}

SettingsManager::SettingsManager()
{
}


SettingsManager::~SettingsManager()
{
}
