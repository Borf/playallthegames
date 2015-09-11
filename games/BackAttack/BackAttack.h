#include "../../PlayAllTheGames/AliveGame.h"
#include "Player.h"
#include <blib/RenderState.h>


namespace blib
{
	class StaticModel;
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
		Bullet(const glm::vec2& position, const glm::vec2& direction)
		{
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

		std::vector<Bullet> bullets;


		Level* level;

		BackAttack();
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