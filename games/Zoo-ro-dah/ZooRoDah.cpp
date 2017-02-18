#include "ZooRoDah.h"

#include <blib/SpriteBatch.h>
#include <blib/SpriteSheet.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

namespace zoorodah
{

	ZooRoDah::ZooRoDah()
	{
		controls[ControllerButton::ThumbstickLeft] = "Walk";
	}

	std::string ZooRoDah::getName()
	{
		return "Zoo-Ro-Dah";
	}

	std::string ZooRoDah::getInstructions()
	{
		return "Hug all the animals";
	}

	void ZooRoDah::loadResources()
	{
		backSprite = resourceManager->getResource<blib::Texture>("assets/games/ZooRoDah/back.png");
		playerSprite = resourceManager->getResource<blib::Texture>("assets/games/ZooRoDah/vacuum.png");
		trashSprite = resourceManager->getResource<blib::Texture>("assets/games/ZooRoDah/trash.png");
		font = resourceManager->getResource<blib::Font>("menu");

		elephantSprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/ZooRoDah/elephant");
		giraffeSprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/ZooRoDah/giraffe");
		pandaSprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/ZooRoDah/panda");
	}

	void ZooRoDah::start()
	{
		maxPlayerScore = 10;

		for (auto p : players)
		{
			p->position = glm::vec2(1920 / 2, 1080 / 2) + 400.0f * blib::util::fromAngle(p->index / (float)players.size() * 2 * (float)M_PI);
			p->rotation = 0;
		}




		spawns.push_back(glm::vec2(400, 200));
		spawns.push_back(glm::vec2(400, 600));
		spawns.push_back(glm::vec2(400, 950));

		spawns.push_back(glm::vec2(1600, 200));
		spawns.push_back(glm::vec2(1600, 600));
		spawns.push_back(glm::vec2(1600, 950));

		positions.clear();
		for (int i = 0; i < 100; i++)
		{
			int spawn = rand() % 6;
			positions.push_back(glm::vec3(spawns[spawn] + blib::math::randomFloat(10,200) * blib::math::fromAngle(blib::math::randomFloat(0, 2*blib::math::pif)), rand() % 3));
		}

		std::sort(positions.begin(), positions.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return a.y < b.y; });

	}

	void ZooRoDah::update(float elapsedTime)
	{
		blib::math::Rectangle screenRect(0, 0, 1920, 1080);
		for (auto p : players)
		{
			
		}

		for (auto &p : positions)
		{
			p.x -= elapsedTime * 10;
		}

	}

	void ZooRoDah::draw()
	{
		spriteBatch->begin(settings->scaleMatrix);
		spriteBatch->draw(backSprite, glm::mat4());

		int frame = (int)(gameTime * 10);

		blib::SpriteSheet* sprites[] = { pandaSprite, giraffeSprite, elephantSprite };

		for (size_t i = 0; i < positions.size(); i++)
		{
			blib::SpriteSheet* sprite = sprites[(int)positions[i].z];
			spriteBatch->draw(sprite->getFrame((frame+i)%(sprite->spriteCountX), 0), blib::math::easyMatrix(glm::vec2(positions[i])));
		}

		spriteBatch->end();
	}

	blib::Texture* ZooRoDah::getTitleImage()
	{
		return NULL;
	}
}