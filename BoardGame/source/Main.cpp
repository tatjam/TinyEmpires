#include <iostream>

#include "SFML/Graphics.hpp"

#include "Board/GameState.h"
#include "Other/SettingsManager.h"
#include "Board/Buildings/Buildings.h"
#include "Board/Entities/Entities.h"


int main(void)
{    

	std::cout << "RTS - GAME: ";

	SettingsManager settings = SettingsManager();
	settings.load("res/settings.yaml");

	sf::RenderWindow* win = new sf::RenderWindow(
		sf::VideoMode(settings.wSets.width, settings.wSets.height), "HA-RTS");

	sf::Event ev;

	sf::Image spriteImage = sf::Image();
	spriteImage.loadFromFile("res/sprites8x8.png");
	sf::Texture spriteSheet = sf::Texture();
	spriteSheet.loadFromImage(spriteImage);

	GameState game = GameState(&settings);
	



	
	game.start(&spriteSheet, 8);

	game.addEmpire();
	Empire* empire = &game.empires[0];
	Windmill house = Windmill(game.board);
	DemoEntity entity = DemoEntity(empire);
	entity.setPosition({ 7, 8 });

	house.setPosition({ 4, 4 });
	empire->buildings.push_back(&house);
	empire->entities.push_back(&entity);

	house.start();

	game.rerender();

	float t = 0.0f;

	sf::Clock dtc;
	sf::Time dtt;
	float dt = 0.0f;

	win->setFramerateLimit(120);

	sf::View view = sf::View();
	view.reset(sf::FloatRect(0, 0, 512.0f, 512.0f));

	empire->launchViewThread();

	while (win->isOpen())
	{
		while (win->pollEvent(ev))
		{
			if (ev.type == sf::Event::Closed)
			{
				win->close();
			}

			if (ev.type == sf::Event::Resized)
			{
				view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
				view.reset(sf::FloatRect(0, 0, (float)ev.size.width, (float)ev.size.height));
			}
		}

		//empire.renderWholeView();
		game.update(dt, win);

		win->clear();
	
		game.draw(win);
		game.drawUI(win);

		win->display();
		


		dtt = dtc.restart();
		dt = dtt.asSeconds();

		t += dt;

		win->setTitle(std::string("FPS: " + std::to_string(1.0f / dt)));
	}

	return 0;
}