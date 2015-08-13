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