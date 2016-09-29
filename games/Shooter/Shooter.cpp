#include "Shooter.h"
#include "../../playallthegames/Participant.h"

#include <blib/LineBatch.h>
#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Color.h>
#include <blib/VBO.h>
#include <blib/Math.h>
#include <blib/math/Line.h>
#include <blib/util/Profiler.h>
#include <blib/util/Log.h>
#include <blib/ParticleSystem.h>

#include <poly2tri/poly2tri.h>
#include <clipper/clipper.hpp>

using blib::util::Log;

namespace shooter
{
	Shooter::Shooter()
	{
		controls[ControllerButton::ButtonA] = "Shoot";
		controls[ControllerButton::ThumbstickLeft] = "Move";
		controls[ControllerButton::ThumbstickRight] = "Aim";
	}


	std::string Shooter::getName()
	{
		return "Shooter";
	}

	std::string Shooter::getInstructions()
	{
		return "Shoot other players";
	}

	std::pair<int, int> Shooter::getPlayerCount()
	{
		return std::pair<int, int>(2, 99);
	}

	void Shooter::loadResources()
	{
		backgroundVbo = resourceManager->getResource<blib::VBO>();
		backgroundVbo->setVertexFormat<blib::VertexP2T2C4>();

		backSprite = resourceManager->getResource<blib::Texture>("assets/games/Shooter/back.png");
		stoneSprite = resourceManager->getResource<blib::Texture>("assets/games/Shooter/stone.png");
		playerSprite = resourceManager->getResource<blib::Texture>("assets/games/Shooter/player.png");
		rocketSprite = resourceManager->getResource<blib::Texture>("assets/games/Shooter/rocketbullet.png");
		cursorSprite = resourceManager->getResource<blib::Texture>("assets/games/Shooter/cursor.png");
		healthBar = resourceManager->getResource<blib::Texture>("assets/games/Shooter/bar.png");

		stoneSprite->setTextureRepeat(true);

		particleSystem = new blib::ParticleSystem(renderer, resourceManager, spriteBatch);
		particleSystem->setTextureFolder("assets/games/Shooter/particles/");

	}

	void Shooter::start()
	{
		objects.clear();
		objects.push_back(blib::math::Polygon({ glm::vec2(1545, 1063), glm::vec2(1423, 987), glm::vec2(1407, 1007), glm::vec2(1291, 966), glm::vec2(1235, 927), glm::vec2(1220, 914), glm::vec2(987, 980), glm::vec2(881, 1018), glm::vec2(729, 958), glm::vec2(715, 937), glm::vec2(537, 920), glm::vec2(405, 980), glm::vec2(293, 910), glm::vec2(306, 716), glm::vec2(210, 716), glm::vec2(204, 571), glm::vec2(54, 572), glm::vec2(51, 226), glm::vec2(167, 232), glm::vec2(166, 205), glm::vec2(104, 158), glm::vec2(54, 96), glm::vec2(107, 38), glm::vec2(300, 57), glm::vec2(301, 109), glm::vec2(398, 105), glm::vec2(400, 82), glm::vec2(542, 34), glm::vec2(640, 49), glm::vec2(682, 158), glm::vec2(635, 185), glm::vec2(674, 265), glm::vec2(825, 195), glm::vec2(823, 171), glm::vec2(880, 168), glm::vec2(908, 149), glm::vec2(920, 124), glm::vec2(995, 82), glm::vec2(1063, 120), glm::vec2(1160, 135), glm::vec2(1163, 187), glm::vec2(1361, 261), glm::vec2(1358, 227), glm::vec2(1697, 121), glm::vec2(1697, 195), glm::vec2(1732, 193), glm::vec2(1763, 124), glm::vec2(1866, 139), glm::vec2(1862, 236), glm::vec2(1878, 235), glm::vec2(1889, 406), glm::vec2(1878, 405), glm::vec2(1874, 463), glm::vec2(1852, 475), glm::vec2(1870, 601), glm::vec2(1863, 602), glm::vec2(1878, 623), glm::vec2(1724, 694), glm::vec2(1726, 672), glm::vec2(1698, 695), glm::vec2(1698, 702), glm::vec2(1761, 724), glm::vec2(1699, 762), glm::vec2(1699, 773), glm::vec2(1767, 812), glm::vec2(1699, 843), glm::vec2(1699, 856), glm::vec2(1765, 895), glm::vec2(1699, 922), glm::vec2(1699, 959), glm::vec2(1650, 933), glm::vec2(1622, 1017), glm::vec2(1600, 1007), glm::vec2(1614, 1046), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1546, 969), glm::vec2(1581, 896), glm::vec2(1520, 863), glm::vec2(1453, 948), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1380, 946), glm::vec2(1464, 833), glm::vec2(1402, 809), glm::vec2(1394, 777), glm::vec2(1415, 696), glm::vec2(1352, 692), glm::vec2(1353, 605), glm::vec2(1177, 604), glm::vec2(1175, 648), glm::vec2(1202, 652), glm::vec2(1194, 755), glm::vec2(1229, 834), glm::vec2(1312, 906), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1014, 914), glm::vec2(1175, 868), glm::vec2(1136, 760), glm::vec2(1132, 740), glm::vec2(1012, 819), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(833, 896), glm::vec2(910, 888), glm::vec2(903, 828), glm::vec2(824, 844), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(411, 889), glm::vec2(541, 846), glm::vec2(673, 859), glm::vec2(663, 768), glm::vec2(777, 710), glm::vec2(825, 721), glm::vec2(845, 601), glm::vec2(794, 601), glm::vec2(803, 548), glm::vec2(749, 540), glm::vec2(747, 601), glm::vec2(516, 599), glm::vec2(516, 717), glm::vec2(376, 717), glm::vec2(360, 856), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(988, 758), glm::vec2(1016, 711), glm::vec2(1120, 677), glm::vec2(1113, 640), glm::vec2(1127, 642), glm::vec2(1131, 603), glm::vec2(1062, 603), glm::vec2(1030, 544), glm::vec2(1012, 603), glm::vec2(883, 602), glm::vec2(865, 730), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1571, 743), glm::vec2(1638, 690), glm::vec2(1572, 620), glm::vec2(1603, 520), glm::vec2(1538, 470), glm::vec2(1512, 527), glm::vec2(1411, 526), glm::vec2(1500, 591), glm::vec2(1465, 648), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1744, 617), glm::vec2(1818, 558), glm::vec2(1810, 477), glm::vec2(1771, 437), glm::vec2(1772, 420), glm::vec2(1698, 435), glm::vec2(1698, 450), glm::vec2(1788, 477), glm::vec2(1698, 516), glm::vec2(1698, 520), glm::vec2(1772, 549), glm::vec2(1698, 597), glm::vec2(1698, 602), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(402, 636), glm::vec2(382, 609), glm::vec2(425, 611), glm::vec2(427, 546), glm::vec2(467, 531), glm::vec2(428, 510), glm::vec2(429, 455), glm::vec2(384, 455), glm::vec2(361, 422), glm::vec2(346, 456), glm::vec2(290, 456), glm::vec2(286, 506), glm::vec2(322, 503), glm::vec2(329, 559), glm::vec2(290, 556), glm::vec2(288, 606), glm::vec2(343, 608), glm::vec2(326, 627), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(211, 487), glm::vec2(211, 360), glm::vec2(355, 360), glm::vec2(354, 261), glm::vec2(422, 195), glm::vec2(383, 167), glm::vec2(386, 154), glm::vec2(303, 166), glm::vec2(304, 203), glm::vec2(228, 207), glm::vec2(218, 290), glm::vec2(127, 289), glm::vec2(129, 484), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(516, 471), glm::vec2(749, 466), glm::vec2(691, 312), glm::vec2(516, 413), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1415, 467), glm::vec2(1471, 460), glm::vec2(1484, 401), glm::vec2(1481, 353), glm::vec2(1536, 332), glm::vec2(1559, 370), glm::vec2(1597, 410), glm::vec2(1644, 376), glm::vec2(1623, 266), glm::vec2(1571, 242), glm::vec2(1433, 299), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1263, 530), glm::vec2(1268, 456), glm::vec2(1355, 454), glm::vec2(1357, 301), glm::vec2(1165, 229), glm::vec2(1116, 340), glm::vec2(1043, 345), glm::vec2(1054, 460), glm::vec2(1212, 457), glm::vec2(1205, 528), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(1772, 394), glm::vec2(1773, 377), glm::vec2(1814, 374), glm::vec2(1814, 259), glm::vec2(1769, 237), glm::vec2(1697, 270), glm::vec2(1697, 283), glm::vec2(1794, 324), glm::vec2(1698, 355), glm::vec2(1698, 366), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(514, 362), glm::vec2(618, 276), glm::vec2(579, 219), glm::vec2(519, 264), glm::vec2(462, 223), glm::vec2(405, 283), glm::vec2(407, 359), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(882, 550), glm::vec2(925, 544), glm::vec2(912, 463), glm::vec2(997, 461), glm::vec2(974, 349), glm::vec2(933, 352), glm::vec2(842, 280), glm::vec2(833, 231), glm::vec2(710, 302), glm::vec2(768, 466), glm::vec2(879, 464), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(978, 255), glm::vec2(1044, 249), glm::vec2(1039, 195), glm::vec2(975, 202), }));
		objects.push_back(blib::math::Polygon({ glm::vec2(515, 179), glm::vec2(512, 150), glm::vec2(557, 143), glm::vec2(559, 113), glm::vec2(495, 121), glm::vec2(492, 177), }));


		{
			ClipperLib::Polygons out;
			ClipperLib::OffsetPolygons(blib::linq::select<ClipperLib::Polygons>(objects, [](const blib::math::Polygon& p) { return blib::linq::select<ClipperLib::Polygon>(p, [](const glm::vec2& pp) { return pp; }); }), out, -16000);
			collisionObjects.clear();
			for (const ClipperLib::Polygon& p : out)
				collisionObjects.push_back(p);
		}



		for (auto p : players)
		{
			bool ok = false;
			while (!ok)
			{
				p->position = glm::vec2(rand() % 1920, rand() % 1080);
				if (!collisionObjects[0].contains(p->position))
					continue;
				ok = true;
				for (size_t i = 1; i < collisionObjects.size(); i++)
					if (collisionObjects[i].contains(p->position))
						ok = false;
				for (auto pp : players)
					if (pp != p && glm::distance(p->position, pp->position) < 400)
						ok = false;
			}

		}
		particleSystem->clear();
		bullets.clear();

		buildTriangles();

	}


	void Shooter::buildTriangles()
	{
		std::vector<blib::VertexP2T2C4> verts;
		{
			std::vector<p2t::Point*> outline;
			outline.push_back(new p2t::Point(-100, -100));
			outline.push_back(new p2t::Point(1920 + 100, -100));
			outline.push_back(new p2t::Point(1920 + 100, 1080 + 100));
			outline.push_back(new p2t::Point(0 - 100, 1080 + 100));
			p2t::CDT cdt(outline);

			const blib::math::Polygon& p = objects[0];
			cdt.AddHole(blib::linq::select<std::vector<p2t::Point*>>(p, [](const glm::vec2& p) { return new p2t::Point(p.x, p.y); }));
			cdt.Triangulate();

			std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();
			for (auto t : triangles)
			{
				for (int i = 0; i < 3; i++)
					verts.push_back(blib::VertexP2T2C4(glm::vec2(t->GetPoint(i)->x, t->GetPoint(i)->y), glm::vec2(t->GetPoint(i)->x, t->GetPoint(i)->y) / 100.0f, blib::Color::white));
			}
		}
		for (const blib::math::Polygon& p : objects)
		{
			if (p == objects[0])
				continue;

			p2t::CDT cdt(blib::linq::select<std::vector<p2t::Point*>>(p, [](const glm::vec2& p) { return new p2t::Point(p.x, p.y); }));
			cdt.Triangulate();

			std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();
			for (auto t : triangles)
			{
				for (int i = 0; i < 3; i++)
					verts.push_back(blib::VertexP2T2C4(glm::vec2(t->GetPoint(i)->x, t->GetPoint(i)->y), glm::vec2(t->GetPoint(i)->x, t->GetPoint(i)->y) / 100.0f, blib::Color::white));
			}

		}
		renderer->setVbo(backgroundVbo, verts);
	}


	void Shooter::addHole(const glm::vec2 &position)
	{
		ClipperLib::Clipper clipper;
		clipper.AddPolygons(blib::linq::select<ClipperLib::Polygons>(objects, [](const blib::math::Polygon& p) { return blib::linq::select<ClipperLib::Polygon>(p, [](const glm::vec2& pp) { return pp; }); }), ClipperLib::ptSubject);

		ClipperLib::Polygon circle;

		for (float f = 0; f < 2 * blib::math::pif; f += blib::math::pif / 8)
			circle.push_back(position + 60.0f * blib::math::fromAngle(f));


		clipper.AddPolygon(circle, ClipperLib::ptClip);

		ClipperLib::Polygons out;
		clipper.Execute(ClipperLib::ctUnion, out);

		objects.clear();
		for (const ClipperLib::Polygon& p : out)
			objects.push_back(p);
		
		{
			ClipperLib::Polygons out;
			ClipperLib::OffsetPolygons(blib::linq::select<ClipperLib::Polygons>(objects, [](const blib::math::Polygon& p) { return blib::linq::select<ClipperLib::Polygon>(p, [](const glm::vec2& pp) { return pp; }); }), out, -16000);
			collisionObjects.clear();
			for (const ClipperLib::Polygon& p : out)
				collisionObjects.push_back(p);
		}


		buildTriangles();
	}


	void Shooter::update(float elapsedTime)
	{
		particleSystem->update(elapsedTime);

		for (auto p : players)
		{
			if (!p->alive)
				continue;

			glm::vec2 oldPos = p->position;
			if (glm::length(p->joystick.leftStick) > 0.1f && glm::length(p->joystick.leftStick) < 2)
				p->position += p->joystick.leftStick * elapsedTime * 200.0f;
			blib::math::Line ray(oldPos, p->position);
			glm::vec2 point;
			blib::math::Line hitLine;
			std::vector<std::pair<glm::vec2, blib::math::Line> > collisions;

			bool collided = true;
			while (collided)
			{
				collided = false;
				for (size_t i = 0; i < collisionObjects.size(); i++)
				{
					const blib::math::Polygon& o = collisionObjects[i];
					if (o.intersects(ray, &collisions))
					{
						for (size_t ii = 0; ii < collisions.size(); ii++)
						{
							glm::vec2 newPos = collisions[ii].second.project(p->position);
							p->position = newPos + 1.0001f * -collisions[ii].second.normal();
							ray.p2 = p->position;
							collided = true;
						}
						break;
					}
				}
			}

			if(glm::length(p->joystick.rightStick) > 0.75f)
				p->direction = glm::normalize(p->joystick.rightStick);

			if (p->joystick.a == 1 && p->shootTime < 0)
			{
				bullets.push_back(std::pair<glm::vec2, glm::vec2>(p->position + 16.0f * p->direction, p->direction + 0.5f * p->joystick.leftStick));
				p->shootTime = 1.5f;
			}
			p->shootTime -= elapsedTime;

		}

		for (int i = 0; i < (int)bullets.size(); i++)
		{
			glm::vec2 oldPos = bullets[i].first;
			bullets[i].first += bullets[i].second * elapsedTime * 300.0f;

			if (bullets[i].first.x < 0 || bullets[i].first.y < 0 || bullets[i].first.x > 1920 || bullets[i].first.y > 1080)
			{
				bullets.erase(bullets.begin() + i);
				i--;
				continue;
			}

			bool exploded = false;

			blib::math::Line ray(oldPos, bullets[i].first);
			for (size_t ii = 0; ii < objects.size(); ii++)
			{
				const blib::math::Polygon& o = objects[ii];
				if (o.intersects(ray))
				{
					exploded = true;
					break;
				}
			}
			for (auto p : players)
			{
				if (glm::distance(p->position, bullets[i].first) < 16)
				{
					exploded = true;
					break;
				}
			}

			if (exploded)
			{
				blib::Emitter* emitter = particleSystem->addEmitter("assets/games/Shooter/particles/explosion.json");
				emitter->position = emitter->prevPosition = bullets[i].first;
				emitter->life = 0.25;


				for (auto p : players)
				{
					if (!p->alive)
						continue;
					float dmg = 120 - 2 * glm::distance(p->position, bullets[i].first);
					if (dmg > 0)
						p->health -= dmg;
					if (p->health < 0)
						p->alive = false;
				}
				addHole(bullets[i].first);
				bullets.erase(bullets.begin() + i);
				i--;

			}

			

		}


	}

	void Shooter::draw()
	{
		spriteBatch->begin();
		spriteBatch->draw(backSprite, glm::mat4());
		
		spriteBatch->end();
		blib::RenderState rs = spriteBatch->renderState;
		rs.activeVbo = backgroundVbo;
		rs.activeTexture[0] = stoneSprite;
		renderer->drawTriangles<blib::VertexP2T2C4>(backgroundVbo->getLength(), rs);

		spriteBatch->begin();

		for (auto p : players)
			if (p->alive)
			{
				spriteBatch->draw(playerSprite, blib::math::easyMatrix(p->position), playerSprite->center, p->participant->color);
				spriteBatch->draw(cursorSprite, blib::math::easyMatrix(p->position + 32.0f * p->direction, (float)blib::util::Profiler::getAppTime()*180, glm::vec2(0.5f, 0.5f)), cursorSprite->center, p->participant->color * glm::vec4(1,1,1,p->shootTime > 0 ? 0.1f : 1.0f));
				spriteBatch->draw(healthBar, blib::math::easyMatrix(p->position + glm::vec2(0,-20), 0, glm::vec2(0.5f, 10)), healthBar->center, blib::math::Rectangle(glm::vec2(0,0), glm::vec2(p->health/100, 1)));
			}
		
		
		
		for (auto b : bullets)
		{
			spriteBatch->draw(rocketSprite, blib::math::easyMatrix(b.first, glm::degrees(atan2(b.second.y, b.second.x))), rocketSprite->center);
		}
		particleSystem->draw(glm::mat4());
		spriteBatch->end();




		lineBatch->begin();
		for (const blib::math::Polygon& p : objects)
			lineBatch->draw(p);

	//	for (const blib::math::Polygon& p : collisionObjects)
	//		lineBatch->draw(p, blib::Color::green);
		lineBatch->end();

		


	}

	blib::Texture* Shooter::getTitleImage()
	{
		return NULL;
	}

}