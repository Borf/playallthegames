#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include "RushHourPlayer.h"

namespace blib {
	class Texture; class Font; namespace math { class Rectangle; }
	class ParticleSystem;
}

class Car
{
public:
	float pos;
	bool ob;
	float deadTimer;

	float speed;
	enum
	{
		Driving,
		Stopped,
		SpedUp
	} state;
	bool operator != (Car& b) const
	{
		return this != &b;
	}
	float getSpeed()
	{
		if (state == Driving)
			return speed;
		else if (state == Stopped)
			return 0;
		else// if (state == SpedUp)
			return speed * 2;
	}
};


class RushHour : public AliveGame<RushHourPlayer>
{
	blib::Texture* backgroundTexture;
	blib::Texture* carSprite;
	blib::Texture* cursorSprite;
	blib::Texture* whitePixel;

	blib::ParticleSystem* particleSystem;

	class Lane : public std::vector<Car>
	{
	public:
		enum
		{
			Horizontal,
			Vertical,
		} orientation;
		int direction;
		glm::vec2 spawn;

		float spawnTime;

		glm::vec2 getPos(const Car &c);
		blib::math::Rectangle getRect(Car& car);
	};

	std::vector<Lane> lanes;

public:
	RushHour();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 4); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};


