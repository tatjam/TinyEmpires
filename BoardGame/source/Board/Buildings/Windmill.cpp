#include "Windmill.h"

void Windmill::tick(float dt, Empire* owner)
{
	wingRotation -= 20.0f * dt;
}

void Windmill::draw(sf::RenderTarget* target, sf::Texture* spriteSheet, size_t tileSide)
{
	Building::draw(target, spriteSheet, tileSide);

	// Draw wing
	if (isBuilt())
	{

		sf::Sprite wing = sf::Sprite();
		wing.setTexture(*spriteSheet);
		wing.setTextureRect(sf::IntRect(11 * tileSide, 3 * tileSide, 3 * tileSide, 3 * tileSide));
		wing.setOrigin(tileSide * 1.5f, tileSide * 1.5f);
		wing.setRotation(wingRotation);

		wing.setPosition(((float)getPosition().x + 1.0f) * (float)tileSide, ((float)getPosition().y + 1.5f) * (float)tileSide);

		target->draw(wing);

	}
}
