#include "DodgeFootPlayer.h"

#include <blib/math/Rectangle.h>
#include <blib/Texture.h>


DodgeFootPlayer::DodgeFootPlayer( int index ) : AlivePlayer(index)
{

}

blib::math::Rectangle DodgeFootPlayer::getBounds( blib::Texture* sprite )
{
	return blib::math::Rectangle((int)position, 1000, sprite->originalWidth, sprite->originalHeight);
}

