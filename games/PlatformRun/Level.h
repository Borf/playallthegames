#pragma once


namespace blib
{
	class SpriteBatch;
	class Texture;
	class ResourceManager;
}


namespace platformrun
{
	class Level
	{
		int* levelData;
		blib::Texture* texturemap; //todo: static

		int lastOffset = 0;
	public:
		Level(blib::ResourceManager* resourceManager);
		
		void draw(blib::SpriteBatch& spriteBatch, float offset);


		void buildSection(int offset, int screen);
		void buildHeight(int x, int height, int tileset);

		int& l(int x, int y);
		bool blocking(int x, int y);

	};

}