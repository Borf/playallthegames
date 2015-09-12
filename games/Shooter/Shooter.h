#pragma once

#include "ShooterPlayer.h"
#include "../../playallthegames/AliveGame.h"

#include <blib/math/Polygon.h>
#include <blib/math/Rectangle.h>
#include <blib/gl/Vertex.h>

namespace blib { class VBO; class Texture; class ParticleSystem; }

namespace shooter
{
	class Shooter : public AliveGame<shooter::Player>
	{
	public:
		blib::Texture* backSprite;
		blib::Texture* stoneSprite;
		blib::Texture* playerSprite;
		blib::Texture* rocketSprite;
		blib::Texture* cursorSprite;
		blib::Texture* healthBar;
		blib::ParticleSystem* particleSystem;


		std::vector<blib::math::Polygon> objects;
		std::vector<blib::math::Polygon> collisionObjects;
		std::vector<blib::math::Rectangle> collisionAabb;
		blib::VBO* backgroundVbo;


		std::vector<std::pair<glm::vec2, glm::vec2>> bullets;

		void buildTriangles();
		void addHole(const glm::vec2& position);

		virtual std::string getName() override;
		virtual std::string getInstructions() override;
		virtual std::pair<int, int> getPlayerCount() override;
		virtual void loadResources() override;
		virtual void start(Difficulty difficulty) override;
		virtual void update(float elapsedTime) override;
		virtual void draw() override;
		virtual blib::Texture* getTitleImage() override;
	};
}