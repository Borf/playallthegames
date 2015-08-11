#include "BackAttack.h"
#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/StaticModel.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../PlayAllTheGames/Settings.h"

std::string backattack::BackAttack::getName()
{
	return "BackAttack";
}

std::string backattack::BackAttack::getInstructions()
{
	return "Shoot from behind";
}

std::pair<int, int> backattack::BackAttack::getPlayerCount()
{
	return std::pair<int, int>(2, 20);
}

void backattack::BackAttack::loadResources()
{
	renderState = spriteBatch->renderState;
	renderState.depthTest = true;
	renderState.activeShader = resourceManager->getResource<blib::Shader>("TowerTopple");
	renderState.activeShader->bindAttributeLocation("a_position", 0);
	renderState.activeShader->bindAttributeLocation("a_texcoord", 1);
	renderState.activeShader->bindAttributeLocation("a_normal", 2);
	renderState.activeShader->setUniformName(Uniforms::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	renderState.activeShader->setUniformName(Uniforms::ModelMatrix, "modelMatrix", blib::Shader::Mat4);
	renderState.activeShader->setUniformName(Uniforms::CameraMatrix, "cameraMatrix", blib::Shader::Mat4);
	renderState.activeShader->setUniformName(Uniforms::s_texture, "s_texture", blib::Shader::Int);
	renderState.activeShader->setUniformName(Uniforms::color, "color", blib::Shader::Vec4);
	renderState.activeShader->finishUniformSetup();
	renderState.activeShader->setUniform(Uniforms::s_texture, 0);
	renderState.cullFaces = blib::RenderState::CullFaces::NONE;
	renderState.activeVbo = NULL;
	renderState.activeVio = NULL;

	trackStraight = new blib::StaticModel("assets/games/BackAttack/rail_cross2.fbx.json", resourceManager, renderer);
	trackTurn = new blib::StaticModel("assets/games/BackAttack/rail_turn.fbx.json", resourceManager, renderer);
}

void backattack::BackAttack::start(Difficulty difficulty)
{

}

void backattack::BackAttack::update(float elapsedTime)
{

}

float bla = 0;

void backattack::BackAttack::draw()
{
	bla += 0.0025f;
	glm::mat4 projectionMatrix = glm::perspective(70.0f, (float)settings->resX / settings->resY, 0.1f, 500.0f);
	glm::mat4 cameraMatrix = glm::lookAt(glm::vec3(5*8-4,-30,10*8), glm::vec3(5*8-4, -16.5, 5 * 8+30), glm::vec3(0, -1, 0));
	renderState.activeShader->setUniform(Uniforms::color, glm::vec4(1, 1, 1, 1));


	renderState.activeShader->setUniform(Uniforms::CameraMatrix, cameraMatrix);
	renderState.activeShader->setUniform(Uniforms::ProjectionMatrix, projectionMatrix);
	renderState.activeShader->setUniform(Uniforms::ModelMatrix, glm::mat4());
	renderState.activeFbo = spriteBatch->renderState.activeFbo;




	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			glm::mat4 mat;
			mat = glm::translate(mat, glm::vec3(8 * x, 0, 8 * y));
			mat = glm::rotate(mat, 90.0f * (int)gameTime, glm::vec3(0, 1, 0));
			renderState.activeShader->setUniform(Uniforms::ModelMatrix, mat);
			trackStraight->draw(renderState, renderer, [this](const blib::Material& material)
			{
				renderState.activeTexture[0] = material.texture;
			});
		}
	}


}

blib::Texture* backattack::BackAttack::getTitleImage()
{
	throw std::logic_error("The method or operation is not implemented.");
}
