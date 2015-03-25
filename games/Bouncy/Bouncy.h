#include "../../PlayAllTheGames/AliveGame.h"

#include "BouncyPlayer.h"
#include "BouncyObject.h"


class Bouncy : public AliveGame<BouncyPlayer>
{
	blib::Texture* playerSprite;
	blib::Texture* objectsSprite;

	std::vector<BouncyObject> objects;

public:
	Bouncy();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};


