#include "PlayAllTheGames.h"

#include  "../games/Asteroids/Asteroids.h"
#include "../games/BackAttack/BackAttack.h"
#include "../games/Battle/Battle.h"
#include "../games/BomberMan/BomberMan.h"
#include "../games/Bouncy/Bouncy.h"
#include "../games/BreakOut/BreakOut.h"
#include "../games/BugSwat/BugSwat.h"
#include "../games/BulletHell/BulletHell.h"
#include "../games/Chicken/Chicken.h"
#include "../games/CircleShooter/CircleShooter.h"
#include "../games/CurveFever/CurveFever.h"
#include "../games/DodgeFoot/DodgeFoot.h"
#include "../games/FallDown/FallDown.h"
#include "../games/FlyFar/FlyFar.h"
#include "../games/Herding/Herding.h"
#include "../games/HillClimber/HillClimbGame.h"
#include "../games/JumpMan/JumpMan.h"
#include "../games/MacroMachines/MacroMachines.h"
#include "../games/MicroRace/MicroRace.h"
#include "../games/MazeEscape/MazeEscape.h"
#include "../games/NippleRub/NippleRub.h"
#include "../games/Osmos/Osmos.h"
#include "../games/PacMan/PacMan.h"
#include "../games/PewPew/PewPew.h"
#include "../games/PokerSteps/PokerSteps.h"
#include "../games/Pong/Pong.h"
#include "../games/Pool/Pool.h"
#include "../games/PushOut/PushOut.h"
#include "../games/RushHour/RushHour.h"
#include "../games/Shooter/Shooter.h"
#include "../games/ShowDown/ShowDown.h"
#include "../games/SkipRope/SkipRope.h"
#include "../games/Soccer/Soccer.h"
#include "../games/Tanks/Tanks.h"
#include "../games/TowerTopple/TowerTopple.h"
#include "../games/Tron/Tron.h"
#include "../games/VacuumSucker/VacuumSucker.h"
#include "../games/ZombieSurvival/ZombieSurvival.h"

void PlayAllTheGames::loadGames()
{
	games.push_back(new asteroids::Asteroids());
	games.push_back(new backattack::BackAttack());
	games.push_back(new Battle());
	games.push_back(new BomberMan());
	games.push_back(new Bouncy());
	games.push_back(new BreakOut());
	games.push_back(new bugswat::BugSwat());
	games.push_back(new BulletHell());
	games.push_back(new Chicken());
	games.push_back(new CircleShooter());
	games.push_back(new CurveFever());
	games.push_back(new DodgeFoot());
	games.push_back(new FallDown());
	games.push_back(new FlyFar());
	games.push_back(new Herding());
	games.push_back(new hillclimb::HillClimb());
	games.push_back(new jumpman::JumpMan());
	games.push_back(new MicroRace());
	games.push_back(new MacroMachines());
	games.push_back(new mazeescape::MazeEscape());
	games.push_back(new nipplerub::NippleRub());
	games.push_back(new Osmos());
	games.push_back(new pacman::PacMan());
	games.push_back(new PewPew());
	games.push_back(new PokerSteps());
	games.push_back(new Pong());
	games.push_back(new Pool());
	games.push_back(new PushOut());
	games.push_back(new RushHour());
	games.push_back(new shooter::Shooter());
	games.push_back(new ShowDown());
	games.push_back(new SkipRope());
	games.push_back(new Soccer());
	games.push_back(new Tanks());
	games.push_back(new TowerTopple());
	games.push_back(new Tron());
	games.push_back(new VacuumSucker());
	games.push_back(new ZombieSurvival());


}
