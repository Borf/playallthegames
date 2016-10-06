#include "../../PlayAllTheGames/AliveGame.h"

#include "BouncyPlayer.h"
#include "BouncyObject.h"

namespace blib { class AudioSample; }

class Bouncy : public AliveGame<BouncyPlayer>
{
	blib::Texture* playerSprite;
	blib::Texture* objectsSprite;

	std::vector<BouncyObject> objects;

public:
	blib::AudioSample* bumpSound;
	blib::AudioSample* dieSound;
	Bouncy();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start();
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};


