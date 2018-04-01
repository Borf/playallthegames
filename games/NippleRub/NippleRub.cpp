#include "NippleRub.h"

#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/gl/Vertex.h>
#include <blib/Renderer.h>
#include <blib/RenderState.h>
#include <blib/Math.h>
#include <blib/Color.h>

#include "../../playallthegames/Participant.h"


namespace nipplerub
{

	NippleRub::NippleRub()
	{
		controls[ControllerButton::ThumbstickLeft] = "Rub";
		controls[ControllerButton::ThumbstickRight] = "Rub";
		maxPlayerScore = 50;
	}

	std::string NippleRub::getName()
	{
		return "NippleRub";
	}

	std::string NippleRub::getInstructions()
	{
		return "Rub it";
	}

	std::pair<int, int> NippleRub::getPlayerCount()
	{
		return std::pair<int,int>(2, 999);
	}

	void NippleRub::loadResources()
	{
		tex = resourceManager->getResource<blib::Texture>("assets/games/NippleRub/nipples.png");
		finger = resourceManager->getResource<blib::Texture>("assets/games/NippleRub/finger.png");
		fingerOverlay = resourceManager->getResource<blib::Texture>("assets/games/NippleRub/finger_overlay.png");
		whitePixel = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");
		font = resourceManager->getResource<blib::Font>("lindsey");
	}

	void NippleRub::start()
	{
	}

	void NippleRub::update(float elapsedTime)
	{
		for (auto p : players)
		{
			for (int i = 0; i < 2; i++)
			{
				glm::vec2 cur = i == 0 ? p->joystick.leftStick : p->joystick.rightStick;

				float angle1 = (float)atan2(cur.y, cur.x);
				float angle2 = (float)atan2(p->lastPos[i].y, p->lastPos[i].x);
				float diff = angle2 - angle1;
				while(diff > 2 * blib::math::pif)
					diff -= 2 * blib::math::pif;
				while (diff < -2 * blib::math::pif)
					diff += 2 * blib::math::pif;

				if (fabs(diff) < 0.7)
				{
					p->counter[i] += diff;
					if (fabs(p->counter[i]) > 2 * blib::math::pif)
					{
						p->scores[i]++;
						p->counter[i] = 0;
					}
				}
				p->lastPos[i] = cur;
			}
			p->score = p->scores[0] + p->scores[1];
		}

	}


	glm::vec2 transform(glm::vec2 pos, glm::vec2 nip1, glm::vec2 nip2)
	{
		{
			glm::vec2 diff(pos - nip1);
			float dist = glm::length(diff);
			if (glm::length(glm::vec2(12, 35)-pos) < 5)
			{
				pos -= (5 - glm::length(glm::vec2(12, 35) - pos)) * 0.1f * diff;
			}
		}
		{
			glm::vec2 diff(pos - nip2);
			if (glm::length(glm::vec2(50, 25) - pos) < 5)
			{
				pos -= (5 - glm::length(glm::vec2(50, 25) - pos)) * 0.1f * diff;
			}
		}

		return pos;
	}

	void NippleRub::draw()
	{


		blib::RenderState s = spriteBatch->renderState;
		s.activeVbo = nullptr;

		int playerCount = players.size();

		int xCount = (int)ceil(sqrt(playerCount));
		int yCount = (int)ceil(playerCount / (float)xCount);

		int w = 1920 / xCount;
		int h = 1080 / yCount;
		float scale = h / (float)tex->originalHeight;

		for (auto p : players)
		{
			int x = p->index%xCount;
			int y = p->index / xCount;

			spriteBatch->renderState.scissor = false;
			spriteBatch->renderState.scissorArea[0] = x*w;
			spriteBatch->renderState.scissorArea[1] = y*h;
			spriteBatch->renderState.scissorArea[2] = w;
			spriteBatch->renderState.scissorArea[3] = h;
			spriteBatch->begin();
			spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, blib::math::Rectangle(glm::vec2(x*w, y*h), w, h)), blib::Color::add(p->participant->color, .25f));
			spriteBatch->draw(tex, blib::math::easyMatrix(glm::vec2(x*w + (w - tex->originalWidth*scale) / 2, y*h), 0, scale));
			glm::vec2 nipPos1(x*w + w/2.0 - 160 * scale, y*h + 290*scale);
			glm::vec2 nipPos2(x*w + w/2.0 + 160 * scale, y*h + 210*scale);
			spriteBatch->end();
			spriteBatch->renderState.scissor = true;
			glm::vec2 scale2 = glm::vec2(tex->originalWidth / 64, tex->originalHeight / 64) * (h / (float)tex->originalHeight);


			{
				std::vector<blib::VertexP2T2C4C4> verts;
				std::vector<blib::VertexP2T2C4C4> points;

				glm::vec2 nip1 = glm::vec2(12, 35) + p->joystick.leftStick * 3.5f;
				glm::vec2 nip2 = glm::vec2(50, 25) + p->joystick.rightStick * 3.5f;

				glm::vec2 pos(x*w+(w - tex->originalWidth*(h / (float)tex->originalHeight)) / 2, y*h);
				glm::vec2 scale = glm::vec2(tex->originalWidth / 64, tex->originalHeight / 64) * (h / (float)tex->originalHeight);
				for (int x = 0; x < 64; x++)
				{
					for (int y = 0; y < 64; y++)
					{
						if (glm::length(glm::vec2(x, y) - nip1) > 10 &&
							glm::length(glm::vec2(x, y) - nip2) > 10)
							continue;
						verts.push_back(blib::VertexP2T2C4C4(pos + transform(glm::vec2(x, y), nip1, nip2)*scale, glm::vec2((x) / 64.0f, (y) / 64.0f), glm::vec4(1, 1, 1, 1), glm::vec4(1,1,1,0)));
						verts.push_back(blib::VertexP2T2C4C4(pos + transform(glm::vec2(x + 1, y), nip1, nip2)*scale, glm::vec2((x + 1) / 64.0f, (y) / 64.0f), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0)));
						verts.push_back(blib::VertexP2T2C4C4(pos + transform(glm::vec2(x + 1, y + 1), nip1, nip2)*scale, glm::vec2((x + 1) / 64.0f, (y + 1) / 64.0f), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0)));

						verts.push_back(blib::VertexP2T2C4C4(pos + transform(glm::vec2(x, y), nip1, nip2)*scale, glm::vec2((x) / 64.0f, (y) / 64.0f), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0)));
						verts.push_back(blib::VertexP2T2C4C4(pos + transform(glm::vec2(x, y + 1), nip1, nip2)*scale, glm::vec2((x) / 64.0f, (y + 1) / 64.0f), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0)));
						verts.push_back(blib::VertexP2T2C4C4(pos + transform(glm::vec2(x + 1, y + 1), nip1, nip2)*scale, glm::vec2((x + 1) / 64.0f, (y + 1) / 64.0f), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0)));

					}
				}
				s.activeTexture[0] = tex;

				renderer->drawTriangles(verts, s);
			}

			spriteBatch->begin();

			spriteBatch->draw(finger, blib::math::easyMatrix(nipPos1 + p->joystick.leftStick * 10.0f, 10, glm::vec2(-1.0f, 1.0f)), glm::vec2(29, 8));
			spriteBatch->draw(finger, blib::math::easyMatrix(nipPos2 + p->joystick.rightStick * 10.0f, -10, 1.0f), glm::vec2(29, 8));

			spriteBatch->draw(fingerOverlay, blib::math::easyMatrix(nipPos1 + p->joystick.leftStick * 10.0f, 10, glm::vec2(-1.0f, 1.0f)), glm::vec2(29, 8), p->participant->color);
			spriteBatch->draw(fingerOverlay, blib::math::easyMatrix(nipPos2 + p->joystick.rightStick * 10.0f, -10, 1.0f), glm::vec2(29, 8), p->participant->color);


			spriteBatch->draw(whitePixel, blib::math::easyMatrix(nipPos1 + glm::vec2(0, -100), 0, glm::vec2(p->counter[0] * 15, 10)), glm::mix(blib::Color::crimson, blib::Color::brightGreen, fabs(p->counter[0] / (2*blib::math::pif))));
			spriteBatch->draw(whitePixel, blib::math::easyMatrix(nipPos2 + glm::vec2(0, -100), 0, glm::vec2(p->counter[1] * 15, 10)), glm::mix(blib::Color::crimson, blib::Color::brightGreen, fabs(p->counter[1] / (2 * blib::math::pif))));


			spriteBatch->draw(font, std::to_string(p->scores[0]), blib::math::easyMatrix(nipPos1 + glm::vec2(0, -150)), blib::Color::grey);
			spriteBatch->draw(font, std::to_string(p->scores[1]), blib::math::easyMatrix(nipPos2 + glm::vec2(0, -150)), blib::Color::grey);

			spriteBatch->draw(font, std::to_string(p->score), blib::math::easyMatrix(glm::vec2(x*w + (w - tex->originalWidth) / 2 + 250*scale, y*h + h-50)));

			spriteBatch->end();
		}
		spriteBatch->renderState.scissor = false;

	


	}

	blib::Texture* NippleRub::getTitleImage()
	{
		return NULL;
	}

}