#include "MicroRacePlayer.h"
#include <blib/math/Rectangle.h>
#include <blib/Texture.h>
#include "Car.h"

MicroRacePlayer::MicroRacePlayer( int index ) : AlivePlayer(index)
{
	moved = false;
	rot = 0;
}

void MicroRacePlayer::update( const std::vector<MicroRacePlayer*> &players, float elapsedTime, float speed, const std::vector<Car*> &otherCars )
{
	if(alive)
	{
		float fac = glm::pow(0.9f, elapsedTime*60);
		position = position * fac + targetPosition * (1-fac); //todo: framerate this
		handleControllerMovement(players);
		testCarCollision(otherCars);
	}
	else
	{
		rot += 0.1f * elapsedTime*60;
		position.x -= 9 * speed * elapsedTime*60;
	}

}

void MicroRacePlayer::handleControllerMovement( const std::vector<MicroRacePlayer*> &players )
{
	int xoff = 0;
	int yoff = 0;
	if(joystick.leftStick.x < -0.6)
		xoff = -1;
	if(joystick.leftStick.x > 0.6)
		xoff =  1;
	if(joystick.leftStick.y < -0.6)
		yoff = -1;
	if(joystick.leftStick.y >  0.6)
		yoff = 1;

	if(!moved)
	{
		if(xoff != 0 || yoff != 0)
			moved = true;

		glm::vec2 oldTargetPosition = targetPosition;
		if(players.size() <= 4)
		{
			if(targetPosition.x > 100 && xoff < 0)
				targetPosition.x -= 250;
			if(targetPosition.x < 350 && xoff > 0)
				targetPosition.x += 250;
		}
		else
		{
			if(targetPosition.x > 100 && xoff < 0)
				targetPosition.x -= 250;
			if(targetPosition.x < 600 && xoff > 0)
				targetPosition.x += 250;
		}
		if(targetPosition.y > 150 && yoff < 0)
			targetPosition.y -= 150;
		if(targetPosition.y < 900 && yoff > 0)
			targetPosition.y += 150;


		for(auto pp : players)
		{
			if(pp != this && glm::length(targetPosition - pp->targetPosition) < 10 && pp->alive)
			{
				targetPosition = oldTargetPosition;
				if(xoff != 0 || yoff != 0)
					moved = false;
			}
		}
	}

	if(xoff == 0 && yoff == 0)
		moved = false;
}

void MicroRacePlayer::testCarCollision( const std::vector<Car*> & otherCars )
{
	blib::math::Rectangle r1(position, (float)sprite->originalWidth, (float)sprite->originalHeight);
	r1.inflate(1,1);	//if all of the lines on one axis are the same on r1 and r2, problem...so solve it this way :D
	for(auto car : otherCars)
	{
		blib::math::Rectangle r2(car->position, car->sprite->originalWidth, car->sprite->originalHeight);
		if(r1.intersect(r2))
			alive = false;
	}
}

