#include "Empire.h"



void Empire::update(float dt)
{
	itimer += dt;

	for (size_t i = 0; i < buildings.size(); i++)
	{
		buildings[i]->pretick(dt, this);
	}

	for (size_t i = 0; i < entities.size(); i++)
	{
		entities[i]->pretick(dt);
	}

	for (size_t i = 0; i < buildings.size(); i++)
	{
		buildings[i]->baseUpdate(dt, this);
		buildings[i]->tick(dt, this);
		buildings[i]->build(50.0f * dt);
	}

	for (size_t i = 0; i < entities.size(); i++)
	{
		entities[i]->updateMovement(dt);
		entities[i]->updatePathing(dt);
		entities[i]->tick(dt);
	}
}

void Empire::draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide)
{


	for (size_t i = 0; i < entities.size(); i++)
	{
		entities[i]->draw(target, spriteSheet, tileSide);
	}

	for (size_t i = 0; i < buildings.size(); i++)
	{
		buildings[i]->draw(target, spriteSheet, tileSide);
		buildings[i]->drawDamage(target, spriteSheet, tileSide);
	}

}

void Empire::renderWholeView()
{
	if (game->board == NULL)
	{
		return;
	}

	viewJustFinishing = true;

	renderedBuffer.create(game->board->width, game->board->height);

	for (size_t x = 0; x < game->board->width; x++)
	{
		for (size_t y = 0; y < game->board->height; y++)
		{
			size_t i = y * game->board->width + x;

			// Safety
			if (i > view.size())
			{
				return;
			}

			if (view[i] == 2)
			{
				view[i] = 1;
			}
			else if (view[i] == 1)
			{
				view[i] = 1;
			}
			else
			{
				view[i] = 0;
			}
			
		}
	}
	
	// Call all buildings
	for (size_t i = 0; i < buildings.size(); i++)
	{
		buildings[i]->fillView(this);
	}

	// Call all entities
	for (size_t i = 0; i < entities.size(); i++)
	{
		entities[i]->fillView();
	}

	for (size_t x = 0; x < game->board->width; x++)
	{
		for (size_t y = 0; y < game->board->height; y++)
		{
			size_t i = y * game->board->width + x;
			sf::Color c;

			// Safety
			if (i > view.size())
			{
				return;
			}

			if (view[i] == 0)
			{
				// Never-viewed
				c = sf::Color(0, 0, 0, 255);
			}
			else if (view[i] == 1)
			{
				// Viewed, not in view currently
				c = sf::Color(0, 0, 0, 128);
			}
			else
			{
				// Viewing currently
				c = sf::Color(0, 0, 0, 0);
			}
			

			renderedBuffer.setPixel(x, y, c);
		}
	}

	viewFinal = view;

	rendered = renderedBuffer;

	viewJustFinishing = false;
	needsReload = true;


}

void Empire::drawView(sf::RenderTarget* target)
{
	// We must upload the texture to the GPU
	// from the main thread for some reason
	// The perfomance hit is almost 
	// not existing so dont worry much
	// about it

	if (needsReload)
	{
		prevRenderedT = renderedT;


		renderedT.loadFromImage(rendered);
		// Disable for horribly ugly FOV
		renderedT.setSmooth(true);
		needsReload = false;
	}

	sf::Sprite spr;

	spr.setTexture(renderedT);

	/*if (viewJustFinishing)
	{
		spr.setTexture(prevRenderedT);
	}
	else
	{
		spr.setTexture(renderedT);
	}*/

	spr.setScale(game->board->spriteSide, game->board->spriteSide);

	target->draw(spr);
}

void Empire::launchViewThread()
{
	viewThreadRun = true;
	viewThread = new std::thread(viewRendererWorker, this, &viewThreadRun);
}

Empire::Empire(GameState* s)
{
	view = std::vector<uint8_t>();
	view.resize(s->board->width * s->board->height);
	viewFinal = std::vector<uint8_t>();
	viewFinal.resize(s->board->width * s->board->height);
	game = s;
}


Empire::~Empire()
{
	end();
}

void Empire::end()
{
	// Finish the thread
	if (viewThread != NULL)
	{
		viewThreadRun = false;
		viewThread->join();

		delete viewThread;
		viewThread = NULL;
	}
}

void viewRendererWorker(Empire* empire, bool* run)
{
	while (*run)
	{
		empire->renderWholeView();
	}
}
