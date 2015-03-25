#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"
#include "MacroMachinesPlayer.h"

#include <blib/math/ArcPath.h>
#include <blib/math/BiArc.h>

#include <Box2D/Box2D.h>


namespace blib { class Texture; class FBO; class Font;  }


class TrackPart
{
public:
	glm::vec2 pos;
	glm::vec2 direction;

	TrackPart(const glm::vec2& pos, const glm::vec2 &direction)
	{
		this->pos = pos;
		this->direction = direction;
	}
};

class MacroMachines : public ScoreGame<MacroMachinesPlayer>, public b2ContactListener
{
	blib::Texture* backgroundSprite;
	blib::Texture* asphaltSprite;
	blib::Texture* carSprite;
	blib::Texture* slipSprite;
	blib::Texture* finishSprite;
	blib::Font* font;
	b2World* world;

	blib::FBO* trackSprite;

	float lastPhysicsTick;

	blib::math::BiArc road;

	float zoom;
public:
	std::vector<b2Body*> progressLines;

	MacroMachines();

	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 8); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update(float elapsedTime);
	virtual void draw();
	virtual blib::Texture* getTitleImage();

	virtual void BeginContact(b2Contact* contact);

};

