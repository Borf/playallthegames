#include "../../PlayAllTheGames/AliveGame.h"
#include "Player.h"
#include <blib/RenderState.h>


namespace blib
{
	class StaticModel;
	class AudioSample;
}

namespace backattack
{
	class Level;

	class Bullet
	{
	public:
		glm::vec2 position;
		glm::vec2 direction;
		bool alive;
		Player* player;
		Bullet(const glm::vec2& position, const glm::vec2& direction, Player* player)
		{
			this->player = player;
			this->position = position;
			this->direction = direction;
			alive = true;
		}
	};

	class BackAttack : public AliveGame<Player>
	{
	public:
		enum class Uniforms
		{
			ProjectionMatrix,
			CameraMatrix,
			ModelMatrix,
			s_texture,
			color
		};

		blib::RenderState renderState;
		blib::StaticModel* cart;
		blib::StaticModel* cube;
		blib::StaticModel* bullet;
		blib::StaticModel* powerup;

		blib::AudioSample* shootSound;
		blib::AudioSample* bumpSound;
		blib::AudioSample* pickupSound;
		blib::AudioSample* explodeSound;

		std::vector<Bullet> bullets;

		int nextSpawn;


		Level* level;

		BackAttack();
		virtual std::string getName() override;
		virtual std::string getInstructions() override;
		virtual std::pair<int, int> getPlayerCount() override;
		virtual void loadResources() override;
		virtual void start() override;
		virtual void update(float elapsedTime) override;
		virtual void draw() override;
		virtual blib::Texture* getTitleImage() override;

	};
}