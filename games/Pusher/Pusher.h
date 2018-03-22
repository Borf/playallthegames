#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include "PusherEnemy.h"
#include "PusherPlayer.h"

namespace blib
{
    class Texture;
    class Font;
}

class Pusher : public AliveGame<PusherPlayer>
{

    blib::Texture* playerSprite;
    blib::Texture* levelSprite;

    blib::Texture* truckSprite;
    blib::Texture* carSprite;

    /*
    @param carLength the width of the car is 1.5 times the length, so you dont have to worry about that ;)
    */
    int carLength = 100, enemyCount = 0;
    bool enemySpawnedLeft;
    float enemySpawnTime = 0.5f;
	float speed = 150.0f;
	float updateSpeedTimer = 5.0f;

    std::vector<PusherEnemy*> enemies;
    std::vector<bool> filledLanes;

public:
    Pusher();
    virtual std::string getName();
    virtual std::string getInstructions();
    virtual void update(float elapsedTime);
    virtual void start();
    virtual void draw();
    virtual void loadResources();
    blib::Texture* Pusher::getTitleImage();
    virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 1); }
};