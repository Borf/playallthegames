#include "ZombieSurvival.h"

#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/Animation.h>
#include <blib/math/Polygon.h>
#include <blib/math/Line.h>
#include <blib/math/Rectangle.h>
#include <blib/Renderer.h>
#include <blib/RenderState.h>
#include <blib/VBO.h>
#include <blib/FBO.h>
#include <blib/Shader.h>
#include <clipper/clipper.hpp>


#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

std::string ZombieSurvival::getName()
{
	return "ZombieSurvival";
}

std::string ZombieSurvival::getInstructions()
{
	return "Reach the exit";
}

void ZombieSurvival::loadResources()
{
	backSprite = resourceManager->getResource<blib::Texture>("assets/games/ZombieSurvival/back.png");
	wallSprites = resourceManager->getResource<blib::Texture>("assets/games/ZombieSurvival/walls2.png");
	finishSprite = resourceManager->getResource<blib::Texture>("assets/games/ZombieSurvival/finish.png");

	visionFbo = resourceManager->getResource<blib::FBO>();
	visionFbo->setSize(settings->resX, settings->resY);
	visionFbo->stencil = true;
	visionFbo->depth = false;
	visionFbo->textureCount = 1;

	combineShader = resourceManager->getResource<blib::Shader>("ZombieSurvival");
	combineShader->bindAttributeLocation("a_position", 0);
	combineShader->bindAttributeLocation("a_texture", 1);
	combineShader->bindAttributeLocation("a_color", 2);
	combineShader->setUniformName(ProjectionMatrix, "projectionmatrix", blib::Shader::Mat4);
	combineShader->setUniformName(Matrix, "matrix", blib::Shader::Mat4);
	combineShader->setUniformName(s_texture, "s_texture", blib::Shader::Int);
	combineShader->setUniformName(s_visionTexture, "s_visionTexture", blib::Shader::Int);
	combineShader->setUniformName(zombieFactor, "zombieFactor", blib::Shader::Float);
	combineShader->finishUniformSetup();

	combineShader->setUniform(s_texture, 0);
	combineShader->setUniform(s_visionTexture, 1);
	combineShader->setUniform(ProjectionMatrix, glm::ortho(0.0f, (float)settings->resX, (float)settings->resY, 0.0f, -1000.0f, 1.0f));
	combineShader->setUniform(Matrix, glm::mat4());


}
int level[32][60];
void ZombieSurvival::start(Difficulty difficulty)
{
	for (auto p : players)
	{
		p->position = glm::vec2(100, 750) + 25.0f * blib::util::fromAngle(p->index / (float)players.size() * 2 * blib::math::pif);
		p->rotation = p->index / (float)players.size() * 360.0f;
		p->playerAnimation = new blib::Animation("assets/games/ZombieSurvival/player.png.json", resourceManager);
		p->playerAnimation->setState("idle");

	}


	static int newLevel[32][60] = {
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 4, 4, 4, 4, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 4, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 4, 1, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 4, 4, 4, 1, 1, 4, 4, 4, 1, 1, 4, 4, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 4, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 4, 1, 1, 1, 4, 4, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 4, 4, 4, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 4, 4, 4, 4, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 4, 4, 4, 1, 2, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 4, 4, 4, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 4, 4, 1, 4, 1, 2, 1, 1, 1, 2, 1, 4, 4, 4, 1, 0, 0, 1, 4, 4, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 4, 4, 1, 1, 4, 4, 4, 1, 1, 4, 4, 4, 1, 2, 2, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	memcpy(level, newLevel, sizeof(newLevel));


	doors.clear();
	for (int x = 0; x < 60; x++)
		for (int y = 0; y < 32; y++)
			if (level[y][x] == 2)
				doors.push_back(glm::ivec2(x,y));

			buildLevelCache();


	objects.clear();


	ClipperLib::Clipper clipper;
	ClipperLib::Polygons subject;
	ClipperLib::Polygons solution;
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 60; x++)
		{
		/*	if (level[y][x] == 1 || level[y][x] == 3 || level[y][x] == 4)
				subject.push_back(blib::math::Polygon({ 
					glm::vec2(x * 32 - 1, y * 32 - 1), 
					glm::vec2(x * 32 + 33, y * 32 - 1), 
					glm::vec2(x * 32 + 33 + 32, y * 32 + 32),
					glm::vec2(x * 32 + 33 + 32, y * 32 + 65),
					glm::vec2(x * 32 - 1 + 32, y * 32 + 65),
					glm::vec2(x * 32 - 1, y * 32 + 65) }).toClipperPolygon());*/
			if (level[y][x] == 1 || level[y][x] == 2)
			{
				subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 24),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 24) }).toClipperPolygon());
					
				int id = 0;
				if (x > 0 && (level[y][x - 1] == 1 || level[y][x - 1] == 2 || level[y][x - 1] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 0, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 9, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 9, y * 32 + 24),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 0, y * 32 + 24) }).toClipperPolygon());
				if (x < 59 && (level[y][x + 1] == 1 || level[y][x + 1] == 2 || level[y][x + 1] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 23, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 33, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 33, y * 32 + 24),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 23, y * 32 + 24) }).toClipperPolygon());
				if (y > 0 && (level[y - 1][x] == 1 || level[y - 1][x] == 2 || level[y - 1][x] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 0),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 0),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 9),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 9) }).toClipperPolygon());
				if (y < 31 && (level[y + 1][x] == 1 || level[y + 1][x] == 2 || level[y + 1][x] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 23),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 23),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 33),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 33) }).toClipperPolygon());
			}
		}
	}
	clipper.AddPolygons(subject, ClipperLib::ptClip);
	clipper.Execute(ClipperLib::ctUnion, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	objects.clear();
	for (size_t i = 0; i < solution.size(); i++)
		objects.push_back(blib::math::Polygon(solution[i]));
	
	subject.clear();

	/*for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 60; x++)
		{
			if (level[y][x] == 1 || level[y][x] == 3 || level[y][x] == 4)
				subject.push_back(blib::math::Polygon({
				glm::vec2(x * 32 - 1, y * 32 - 1),
				glm::vec2(x * 32 + 33, y * 32 - 1),
				glm::vec2(x * 32 + 33 + 32, y * 32 + 32),
				glm::vec2(x * 32 + 33 + 32, y * 32 + 65),
				glm::vec2(x * 32 - 1 + 32, y * 32 + 65),
				glm::vec2(x * 32 - 1, y * 32 + 65) }).toClipperPolygon());
	
		}
	}*/


	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 60; x++)
		{

			if (level[y][x] == 1 || level[y][x] == 4)
			{
				subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 24),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 24) }).toClipperPolygon());
					
				int id = 0;
				if (x > 0 && (level[y][x - 1] == 1 || level[y][x - 1] == 2 || level[y][x - 1] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 0, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 9, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 9, y * 32 + 24),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 0, y * 32 + 24) }).toClipperPolygon());
				if (x < 59 && (level[y][x + 1] == 1 || level[y][x + 1] == 2 || level[y][x + 1] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 23, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 33, y * 32 + 8),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 33, y * 32 + 24),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 23, y * 32 + 24) }).toClipperPolygon());
				if (y > 0 && (level[y - 1][x] == 1 || level[y - 1][x] == 2 || level[y - 1][x] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 0),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 0),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 9),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 9) }).toClipperPolygon());
				if (y < 31 && (level[y + 1][x] == 1 || level[y + 1][x] == 2 || level[y + 1][x] == 4))
					subject.push_back(blib::math::Polygon({
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 23),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 23),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 24, y * 32 + 33),
					glm::vec2(16, 32) + glm::vec2(x * 32 + 8, y * 32 + 33) }).toClipperPolygon());
			}
		}
	}


	solution.clear();
	ClipperLib::OffsetPolygons(subject, solution, 7500);
	collisionObjects.clear();
	for (size_t i = 0; i < solution.size(); i++)
		collisionObjects.push_back(blib::math::Polygon(solution[i]));
	
	collisionAabb.clear();
	for (size_t i = 0; i < solution.size(); i++)
		collisionAabb.push_back(collisionObjects[i].getBoundingBox());




	blib::linq::deleteall(zombies);
	for (int i = 0; i < 50; i++)
	{
		spawnZombie();
	}
}


void ZombieSurvival::spawnZombie()
{
	Zombie* z = new Zombie();
	z->zombieSprite = new blib::Animation("assets/games/ZombieSurvival/zombie.json", resourceManager);
	z->zombieSprite->setState("idle");
	z->zombieSprite->update(blib::math::randomDouble(0, 10));
	z->direction = (float)(rand() % 360);

	std::vector<std::pair<glm::vec2, blib::math::Line> > collisions;
	bool ok = false;
	while (!ok)
	{
		z->position = glm::vec2(rand() % 1920, rand() % 1080);
		for (auto p : players)
		{
			blib::math::Line ray(p->position, z->position);
			ok = false;
			for (auto o : collisionObjects)
				if (o.intersects(ray, &collisions))
				{
					if (collisions.size() % 2 == 0)
					{
						collisions.clear();
						ok = true;
						break;
					}
				}
			if (!ok)
				break;
		}
	}

	z->target = NULL;

	zombies.push_back(z);
}



void ZombieSurvival::update(float elapsedTime)
{
	static float spawnTimer = 0;
	spawnTimer += elapsedTime;
	while (spawnTimer > 0.25f)
	{
		if (blib::linq::count(zombies, [](Zombie* z) { return z->hp > 0; }) < 40)
			spawnZombie();
		spawnTimer -= 0.25f;
	}

	for (auto p : players)
	{
		p->playerAnimation->update(elapsedTime);
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
				if (!collisionAabb[i].intersect(ray))
					continue;
				const blib::math::Polygon& o = collisionObjects[i];
				if (o.intersects(ray, &collisions))
				{
					for (size_t ii = 0; ii < collisions.size(); ii++)
					{
						glm::vec2 newPos = collisions[ii].second.project(p->position);
						p->position = newPos + 1.0001f * collisions[ii].second.normal();
						ray.p2 = p->position;
						collided = true;
					}
					break;
				}
				if (!collided)
					for (size_t i = 0; i < doors.size(); i++)
					{
						if (glm::distance(p->position, glm::vec2(32 * doors[i]) + glm::vec2(16, 32)) < 32)
						{
							glm::ivec2 d = doors[i] * 32 * 1000;

							ClipperLib::Clipper clipper;
							ClipperLib::Polygons solution;

							clipper.AddPolygons(blib::linq::select<ClipperLib::Polygons>(objects, [](const blib::math::Polygon& p) { return p.toClipperPolygon(); }), ClipperLib::ptSubject);
							clipper.AddPolygon({ ClipperLib::IntPoint(d.x + 16000, d.y), ClipperLib::IntPoint(d.x + 48000, d.y), ClipperLib::IntPoint(d.x + 48000, d.y + 64000), ClipperLib::IntPoint(d.x + 16000, d.y + 64000) }, ClipperLib::ptClip);
							clipper.Execute(ClipperLib::ctDifference, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
							objects.clear();
							for (size_t ii = 0; ii < solution.size(); ii++)
								objects.push_back(blib::math::Polygon(solution[ii]));
							level[doors[i].y][doors[i].x] = 0;
							buildLevelCache();
							doors.erase(doors.begin() + i);
							break;
						}
					}
			}
		}


		if (glm::length(p->joystick.leftStick) > 0.1f)
		{
			p->rotation = glm::degrees(atan2(p->joystick.leftStick.y, p->joystick.leftStick.x));
			p->accuracy = glm::max(1.0f, p->accuracy);
			p->accuracy /= glm::pow(0.01f * glm::length(p->joystick.leftStick), elapsedTime);
			p->accuracy = glm::min(10.0f, p->accuracy);
			p->playerAnimation->setState("walk");
		}
		else
		{
			p->playerAnimation->setState("idle");
			p->accuracy *= glm::pow(0.25f, elapsedTime);
		}

		if (p->joystick.a == 1 && p->prevJoystick.a == 0)
		{
			blib::math::Line shootRay(p->position, p->position + 1000.0f * blib::util::fromAngle(glm::radians(p->rotation + blib::math::randomFloat(-p->accuracy, p->accuracy))));
			std::vector<std::pair<glm::vec2, blib::math::Line> > collisions;
			for (auto o : objects)
				if (o.intersects(shootRay, &collisions))
					for (auto c : collisions)
						if (glm::distance(p->position, c.first) < shootRay.length())
							shootRay.p2 = c.first;
			for (Zombie* z : zombies)
				if (z->hp > 0 && glm::distance(shootRay.project(z->position), z->position) < 30)
					shootRay.p2 = shootRay.project(z->position);


			for (auto z : zombies)
				if (glm::distance(z->position, shootRay.project(z->position)) < 25 && z->hp > 0)
				{
					z->hp--;
					if (z->hp <= 0)
					{
						z->zombieSprite->setState("dead");
						z->zombieSprite->update(elapsedTime * 0.1f);
					}
				}

			shootLines.push_back(std::pair<blib::math::Line, float>(shootRay, 1.0f));
		}


	}

	for (auto z : zombies)
	{
		if (z->hp <= 0)
			continue;

		glm::vec2 oldPos = z->position;
		z->position += blib::util::fromAngle(glm::radians(z->direction)) * (z->target == NULL ? 50.0f : 150)* elapsedTime;
		blib::math::Line ray(oldPos, z->position);
		std::vector<std::pair<glm::vec2, blib::math::Line> > collisions;
		glm::vec2 point;
		blib::math::Line hitLine;
		bool collided = true;
		int iterations = 0;
		while (collided && iterations < 4)
		{
			iterations++;
			collided = false;
			for (const blib::math::Polygon& o : collisionObjects)
				if (o.intersects(ray, &collisions))
				{
					for (size_t ii = 0; ii < collisions.size(); ii++)
					{
						glm::vec2 newPos = collisions[ii].second.project(z->position);
						z->position = newPos + 1.0001f * collisions[ii].second.normal();
						z->direction += 360 * elapsedTime;
						z->rotation = 90;
						z->timer = blib::math::randomFloat(0, 1);
						ray.p2 = z->position;
						collided = true;
					}
					break;
				}
			if (!collided)
				for (auto d : doors)
				{
					if (glm::distance(z->position, glm::vec2(32 * d) + glm::vec2(16, 32)) < 32)
					{
						z->position = glm::vec2(32 * d) + glm::vec2(16, 32) + 32.01f * glm::normalize(z->position - (glm::vec2(32 * d) + glm::vec2(16, 32)));
						ray.p2 = z->position;
						collided = true;
					}
				}
		}

		if (z->target && z->target->alive)
		{
			z->direction = glm::degrees(atan2(z->target->position.y - z->position.y, z->target->position.x - z->position.x));
			blib::math::Line l(z->position, z->target->position);
			for (const blib::math::Polygon& o : collisionObjects)
				if (o.intersects(l))
					z->target = NULL;
		}
		else if (z->target && !z->target->alive)
			z->target = NULL;
		else
		{
			for (auto p : players)
			{
				if (glm::distance(p->position, z->position) < 300 && p->alive)
				{
					bool canSee = true;
					blib::math::Line l(z->position, p->position);
					for (const blib::math::Polygon& o : collisionObjects)
						if (o.intersects(l))
							canSee = false;
					if (canSee)
						z->target = p;
				}
			}

			z->direction += z->rotation * elapsedTime;
			z->timer -= elapsedTime;
			if (z->timer <= 0)
			{
				z->rotation = blib::math::randomFloat(-2, 2);
				z->timer = blib::math::randomFloat(1, 3);
			}
		}




		for (auto p : players)
		{
			if (glm::distance(p->position, z->position) < 10)
				p->alive = false;
		}


		if (z->hp > 0)
			z->zombieSprite->setState("walk");
		if(z->target)
			z->zombieSprite->update(elapsedTime * 1.5f);
		else
			z->zombieSprite->update(elapsedTime * 0.1f);
	}


	for (int i = 0; i < (int)shootLines.size(); i++)
	{
		shootLines[i].second -= elapsedTime * 2;
		if (shootLines[i].second < 0)
		{
			shootLines.erase(shootLines.begin() + i);
			i--;
		}
	}



}

void ZombieSurvival::draw()
{
	blib::RenderState state = lineBatch->renderState;
	state.activeVbo = NULL;
	state.activeFbo = visionFbo;
	state.stencilTestEnabled = true;
	renderer->clear(glm::vec4(0.25f, 0.25f, 0.25f, 1), blib::Renderer::Color | blib::Renderer::Stencil, state);
	for (auto p : players)
	{
		if (!p->alive)
			continue;
		glm::vec2 lightPoint(p->position);

		std::vector<blib::VertexP2C4> verts;
		for (const blib::math::Polygon& o : objects)
		{
			for (size_t i = 0; i < o.size(); i++)
			{
				const glm::vec2& v1 = o[i];
				const glm::vec2& v2 = o[(i + 1) % o.size()];

				blib::math::Line l(v1, v2);
				if (l.side(lightPoint))
					continue;


				const glm::vec2& v3 = v1 + 50.0f * (v1 - lightPoint);
				const glm::vec2& v4 = v2 + 50.0f * (v2 - lightPoint);;

				glm::vec4 c(0.25f, 0.25f, 0.25f, 1.0f);
				verts.push_back(blib::VertexP2C4(v1, c));
				verts.push_back(blib::VertexP2C4(v2, c));
				verts.push_back(blib::VertexP2C4(v3, c));

				verts.push_back(blib::VertexP2C4(v3, c));
				verts.push_back(blib::VertexP2C4(v4, c));
				verts.push_back(blib::VertexP2C4(v2, c));
			}
		}


		renderer->clear(glm::vec4(1, 1, 1, 0.5f), blib::Renderer::Stencil, state);
		state.stencilWrite = true;
		renderer->drawTriangles(verts, state);
		state.stencilWrite = false;
		state.blendEnabled = false;
		
		glm::vec4 c(1.0f, 1.0f, 1.0f, 1.0f);

		//for more difficulty, use cone here instead of full screen
		std::vector<blib::VertexP2C4> verts2{
			blib::VertexP2C4(glm::vec2(0, 0), c),
			blib::VertexP2C4(glm::vec2(1920, 0), c),
			blib::VertexP2C4(glm::vec2(0, 1080), c),

			blib::VertexP2C4(glm::vec2(1920, 1080), c),
			blib::VertexP2C4(glm::vec2(1920, 0), c),
			blib::VertexP2C4(glm::vec2(0, 1080), c)
		};

		renderer->drawTriangles(verts2, state);
	}

	blib::Shader* s = spriteBatch->renderState.activeShader;
	spriteBatch->renderState.activeShader = combineShader;
	spriteBatch->renderState.activeTexture[1] = visionFbo;

	spriteBatch->begin();
	lineBatch->begin();
	spriteBatch->draw(backSprite, glm::mat4());

	spriteBatch->end();
	combineShader->setUniform(zombieFactor, 1.0f);
	spriteBatch->begin();
	for (auto z : zombies)
		z->zombieSprite->draw(*spriteBatch, blib::math::easyMatrix(z->position, z->direction-90, 0.5f));
	spriteBatch->end();
	combineShader->setUniform(zombieFactor, 0.0f);
	spriteBatch->begin();

	//spriteBatch->draw(visionFbo, blib::math::easyMatrix(glm::vec2(0,1080), 0, glm::vec2(1,-1)));

	for (auto p : players)
	{
		if (!p->alive)
			continue;
		p->playerAnimation->draw(*spriteBatch, blib::math::easyMatrix(p->position, p->rotation - 90, 0.5f), p->participant->color);
		lineBatch->draw(p->position, p->position + 200.0f * blib::util::fromAngle(glm::radians(p->rotation + p->accuracy)), p->participant->color);
		lineBatch->draw(p->position, p->position + 200.0f * blib::util::fromAngle(glm::radians(p->rotation - p->accuracy)), p->participant->color);
	}
	
	spriteBatch->drawCache(levelCache);

	spriteBatch->draw(finishSprite, blib::math::easyMatrix(glm::vec2(100, 250)), finishSprite->center);

	
	for (const std::pair<blib::math::Line, float>& l : shootLines)
		lineBatch->draw(l.first, glm::vec4(1, 1, 1, l.second));

	/*for (auto p : objects)
		lineBatch->draw(p, glm::vec4(0, 0, 1, 1));
	for (auto p : collisionObjects)
		lineBatch->draw(p, glm::vec4(0, 1, 1, 1));*/

	spriteBatch->end();
	lineBatch->end();


	spriteBatch->renderState.activeShader = s;
	spriteBatch->renderState.activeTexture[1] = NULL;



}

blib::Texture* ZombieSurvival::getTitleImage()
{
	return NULL;
}

void ZombieSurvival::buildLevelCache()
{
	spriteBatch->begin();
	spriteBatch->startCache();
	for (int x = 0; x < 60; x++)
	{
		for (int y = 0; y < 32; y++)
		{
			if (level[y][x] == 1)
			{
				int id = 0;
				if (x > 0 && (level[y][x - 1] == 1 || level[y][x - 1] == 2 || level[y][x - 1] == 4))
					id |= 1;
				if (x < 59 && (level[y][x + 1] == 1 || level[y][x + 1] == 2 || level[y][x + 1] == 4))
					id |= 2;
				if (y > 0 && (level[y - 1][x] == 1 || level[y - 1][x] == 2 || level[y - 1][x] == 4))
					id |= 4;
				if (y < 31 && (level[y + 1][x] == 1 || level[y + 1][x] == 2 || level[y + 1][x] == 4))
					id |= 8;
				//						0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
				static int lookup[] = { 2, 0, 0, 0, 1, 6, 7, 10, 1, 5, 4, 8, 1, 9, 11, 2 };

				id = lookup[id];
				spriteBatch->draw(wallSprites, blib::math::easyMatrix(glm::vec2(32 * x, 32 * y)), glm::vec2(0, 0), blib::math::Rectangle(glm::vec2((id % 4)*0.25f, (id / 4)*0.25f), 0.25f, 0.25f));
			}
			else if (level[y][x] == 2)
			{
				int id = 0;
				if (level[y - 1][x] == 0 && level[y + 1][x] == 0)
					id = 13;
				else
					id = 14;
				spriteBatch->draw(wallSprites, blib::math::easyMatrix(glm::vec2(32 * x, 32 * y)), glm::vec2(0, 0), blib::math::Rectangle(glm::vec2((id % 4)*0.25f, (id / 4)*0.25f), 0.25f, 0.25f));
			}
			else if (level[y][x] == 4)
			{
				int id = 0;
				if (level[y - 1][x] == 0 && level[y + 1][x] == 0)
					id = 3;
				else
					id = 15;
				spriteBatch->draw(wallSprites, blib::math::easyMatrix(glm::vec2(32 * x, 32 * y)), glm::vec2(0, 0), blib::math::Rectangle(glm::vec2((id % 4)*0.25f, (id / 4)*0.25f), 0.25f, 0.25f));
			}
			else if (level[y][x] == 3)
				spriteBatch->draw(wallSprites, blib::math::easyMatrix(glm::vec2(32 * x, 32 * y)), glm::vec2(0, 0), blib::math::Rectangle(glm::vec2((12 % 4)*0.25f, (12 / 4)*0.25f), 0.25f, 0.25f));

		}
	}
	levelCache = spriteBatch->getCache();
	spriteBatch->end();
}

bool ZombieSurvival::hasWinner()
{
	if (!blib::linq::any(players, [](ZombieSurvivalPlayer* p) { return p->alive; }))
		return true;
	if (blib::linq::any(players, [](ZombieSurvivalPlayer* p) { return p->alive && glm::distance(p->position, glm::vec2(100, 250)) < 50; }))
		return true;
	return false;
}

std::list<Player*> ZombieSurvival::getWinners()
{
	return blib::linq::where<std::list<Player*>>(players, [](ZombieSurvivalPlayer* p) { return p->alive && glm::distance(p->position, glm::vec2(100, 250)) < 50; });
}
