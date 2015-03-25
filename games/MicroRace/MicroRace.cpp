#include "MicroRace.h"

#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/Math.h>
#include <blib/Color.h>

#include "Car.h"
#include "Foot.h"

#include <glm/gtc/matrix_transform.hpp>

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

std::string MicroRace::getName()
{
	return "MicroRace";
}

std::string MicroRace::getInstructions()
{
	return "Dodge the grey cars";
}

void MicroRace::loadResources()
{
	carSprite = resourceManager->getResource<blib::Texture>("assets/games/MicroRace/car.png");
	back = resourceManager->getResource<blib::Texture>("assets/games/MicroRace/back.png");
	lineSprite = resourceManager->getResource<blib::Texture>("assets/games/MicroRace/line.png");
	footSprite = resourceManager->getResource<blib::Texture>("assets/games/MicroRace/foot.png");
}

void MicroRace::start(Difficulty difficulty)
{
	spawnFactor = 1;
	for(auto p : players)
	{
		p->sprite = carSprite;
		p->position = glm::vec2(100 + 250 * (p->index/4), 300+ 150*(p->index%4));
		p->targetPosition = p->position;
	}
	backPosition = 0;
	speed = 1;
	gameTime = 0;
	blib::linq::deleteall(cars);
}

void MicroRace::update( float elapsedTime )
{
	speed += 0.002f * elapsedTime * 60.0f;
	gameTime += elapsedTime;
	scrollBackground(elapsedTime);
	for(auto p : players) { p->update(players, elapsedTime, speed, cars); };
	for(auto c : cars) { c->update(elapsedTime, speed); };
	if(blib::math::randomDouble() < spawnFactor * 0.01 * (1+(speed-1) * 0.9f) * elapsedTime*60)
		spawnCar(gameTime);

	blib::linq::deletewhere(cars, [] (Car* c) { return c->position.x < 0; });
}

void MicroRace::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	drawBackground();
	for(auto p : players) { spriteBatch->draw(p->sprite, glm::translate(glm::mat4(), glm::vec3(p->position + p->sprite->center,0)), p->sprite->center, blib::math::Rectangle(0,0,1,1), p->alive ? p->participant->color : blib::Color::black); };
	for(auto c : cars) { spriteBatch->draw(c->sprite, glm::translate(glm::mat4(), glm::vec3(c->position,0)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), c->color); };
	spriteBatch->end();
}

blib::Texture* MicroRace::getTitleImage()
{
	return NULL;
}

void MicroRace::scrollBackground( float elapsedTime )
{
	backPosition -= 10 * speed * 60 * elapsedTime;
	while(backPosition < -1920)
		backPosition += 1920;
}

void MicroRace::spawnCar( float gameTime )
{
	if(blib::math::randomDouble() < 0.05 && gameTime > 20)
		cars.push_back(new Foot(glm::vec2(1920, 150 + 150 * (rand()%6)), footSprite, blib::Color::white));
	else
		cars.push_back(new Car(glm::vec2(1920, 150 + 150 * (rand()%6)), carSprite, blib::Color::grey));
}

void MicroRace::drawBackground()
{
	spriteBatch->draw(back, glm::translate(glm::mat4(), glm::vec3(backPosition - 1920, 0, 0)));
	spriteBatch->draw(back, glm::translate(glm::mat4(), glm::vec3(backPosition, 0, 0)));
	spriteBatch->draw(back, glm::translate(glm::mat4(), glm::vec3(backPosition + 1920, 0, 0)));
	drawLines();
}

void MicroRace::drawLines()
{
	for(int ii = 0; ii < 7; ii++)
		for(float i = -1920 + backPosition; i < 2*1920 + backPosition; i += 1920 / ((ii == 0 || ii == 6) ? 10.0f : 5.0f))
			spriteBatch->draw(lineSprite, glm::translate(glm::mat4(), glm::vec3(i, 150*ii+115, 0)));
}
