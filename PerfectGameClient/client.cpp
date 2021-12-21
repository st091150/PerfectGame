#include <iostream>
#include <memory>
#include <chrono>
#include<vector>
#include "UdpSocket.h"
#include "../GameState/GameState.h"
#include "SFML/Graphics.hpp"
const bool condition = 0;
using namespace sf;
//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
//The port on which to listen for incoming data
u_short const kPort = 8888;
size_t const kBufferSize = 512;
char buffer[kBufferSize] = "0";

void sleep(unsigned long us)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	while (microseconds.count() < us)
	{
		finish = std::chrono::high_resolution_clock::now();
		microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	}
}


class DrawCharacter{
protected:
	std::vector<Image> _imageTextures;

	std::map<std::string, Texture> _textureOfPlayer;
	std::map<std::string, Sprite> _spriteOfPlayer;
public:
	DrawCharacter() {}
	DrawCharacter(GameState &state,size_t countOfCharacter, std::string Texture_Folder){
		_imageTextures.resize(countOfCharacter);

		for (size_t i = 0; i < countOfCharacter; i++) {
			_imageTextures[i].loadFromFile(std::string(Texture_Folder) +
											   std::string("character_") +
											   std::to_string(i) +
											   std::string(".png"));
			_imageTextures[i].createMaskFromColor(Color::White);
		}
		srand(time(NULL));
		for (auto& map_pair : state.getListOfPlayers()) {
			Texture texture;
			srand(time(NULL));
			texture.loadFromImage(_imageTextures[rand() % countOfCharacter]);
			_textureOfPlayer.insert(std::make_pair(map_pair.second.getName(),texture));
			_spriteOfPlayer.insert(std::make_pair(map_pair.second.getName(), setSprite(_textureOfPlayer[map_pair.first])));
		}

	}


	Sprite setSprite(Texture &texture, size_t x = 0, size_t y = 0) {
		Sprite sprite;
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(x, y, 75, 100));
		return sprite;
	}

	void setTexture(std::string nameOfPlayer, std::string pathToTexture) {
		Texture texture;
		texture.loadFromFile(pathToTexture);
		_textureOfPlayer[nameOfPlayer] = texture;
	}

	void Draw(RenderWindow& window,GameState &state, size_t move = 0) {
		for (auto& map_pair : state.getListOfPlayers()) {
			if (map_pair.second.getStatus() == PlayerStatus::Active) {
				/*std::cout << '\n' <<  map_pair.second.getName() << " : x = " <<
					int(map_pair.second.getPos().first) << " y = " << int(map_pair.second.getPos().second);*/
				_spriteOfPlayer[map_pair.second.getName()].
					setPosition(map_pair.second.getPos().first - 20, map_pair.second.getPos().second - 60); //!!!

				window.draw(_spriteOfPlayer[map_pair.first]);
			}
		}
	}

	void updateDC(GameState& state, size_t countOfCharacter, std::string Texture_Folder) {
		for (auto& map_pair : state.getListOfPlayers()) {
			std::string tmp = map_pair.second.getName();
			if (!_textureOfPlayer.count(tmp)) {
				Texture texture;
				srand(time(NULL));
				texture.loadFromImage(_imageTextures[rand() % countOfCharacter]);
				_textureOfPlayer.insert(std::make_pair(tmp, texture));
				_spriteOfPlayer.insert(std::make_pair(tmp, setSprite(_textureOfPlayer[map_pair.first])));
			}
		}
	}
};



class DrawMap{
protected:
	std::vector<Texture> _blockTextures;
	std::vector<Sprite> _blockSprites;

	size_t _sizeOfBlock;
public:
	DrawMap() {}
	DrawMap(size_t sizeOfBlock,std::string Texture_Folder){

		const int countOfBlocks = int(GameState::Block::Count);

		_sizeOfBlock = sizeOfBlock;
		_blockTextures.resize(countOfBlocks);
		_blockSprites.resize(countOfBlocks);


		for (size_t i = 0; i < countOfBlocks; i++) {
			_blockTextures[i].loadFromFile(std::string(Texture_Folder) +
										   std::string("texture_") + 
										   std::to_string(i) +
										   std::string(".png"));

			_blockSprites[i].setTexture(_blockTextures[i]);
			_blockSprites[i].setTextureRect(IntRect(0, 0, sizeOfBlock, sizeOfBlock));
		}
	}

	void Draw(RenderWindow &window,GameState &state){
		for (size_t i = 0; i < state.getCol(); i++) {
			for (size_t j = 0; j < state.getRow(); j++) {
				size_t sup = size_t(state.getMap()[i][j]);
				//std::cout << sup << std::endl;
				_blockSprites[sup].setPosition(j * _sizeOfBlock, i * _sizeOfBlock);
				window.draw(_blockSprites[sup]);
			}
		}
	}

	void update() {

	}
};


class State:public DrawMap,public DrawCharacter {
	RenderWindow *_window;
	GameState *_state;

	DrawMap DM;
	DrawCharacter DC;

	size_t _sizeOfBlock;
	std::string _Texture_Folder;
	size_t _countOfCharacter;

	size_t speedMove = 0.01;
public:
	State(GameState &state, std::string Texture_Folder,
		  size_t countOfCharacter, RenderWindow& window,
		  size_t sizeOfBlock = 50)
	{
		_state = &state;
		_window = &window;

		DM = DrawMap(sizeOfBlock, Texture_Folder);

		DC = DrawCharacter(*_state, countOfCharacter, Texture_Folder);

		_sizeOfBlock = sizeOfBlock;
		_Texture_Folder = Texture_Folder;
		_countOfCharacter = countOfCharacter;
	}

	size_t getSpeedMove() { return speedMove; }
	void setSpeedMove(size_t X) { speedMove = X; }
	size_t getCountOfCharacter() { return _countOfCharacter; }
	size_t setCountOfCharacter(size_t x) { _countOfCharacter = x; }
	size_t getSizeOFBlock() { return _sizeOfBlock; }

	void update() {
		DM.update();
		DC.updateDC(*_state, _countOfCharacter, _Texture_Folder);
	}

	void drawMap(size_t moveCharacter = 0) {
		_window->clear(Color(107, 136, 230)); // закрасить область
		DM.Draw(*_window, *_state);
		DC.Draw(*_window,*_state, moveCharacter);
		_window->display();
	}

};

int main(int argc, const char* argv[])
{
	size_t sizeOfBlocks;
	std::cout << "Enter size of blocks (max_size = 200) : ";
	std::cin >> sizeOfBlocks;

	std::cout << "PEFECTGAME" << '\n';
	std::unique_ptr<UdpSocket> sock_ptr;
	std::string name = argv[1];
	GameState state;

	RenderWindow window(VideoMode(800,600),"PerfectGame!");
	window.setVerticalSyncEnabled(true);
	
	try
	{
		sock_ptr = std::make_unique<UdpSocket>(kIpAddr, kPort);
	}
	catch (std::exception const& err)
	{
		std::cout << "Couldn't init socket: " << err.what() << "\n";
		exit(EXIT_FAILURE);
	}

	//start communication
	//send the message
	if (sock_ptr->send(name.c_str(), name.length()) != 0)
	{
		std::cout << "Failed to send\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "request sent\n";

	while (1) {
		size_t sz = kBufferSize;
		if (sock_ptr->recv(buffer, sz) != 0)
		{
			std::cout << "No data to recv\n";
			sleep(1e5);
			continue;
			exit(EXIT_FAILURE);
		}
		state.deserialize(buffer, sz);
		break;
	}
	State DState(state,"content/",1,window,sizeOfBlocks);
	
	while (window.isOpen())
	{
		//receive a reply and print it
		size_t sz = kBufferSize;
		size_t sup = state.getListOfPlayers().size();
		if (sock_ptr->recv(buffer, sz) != 0)
		{
			std::cout << "No data to recv\n";
			sleep(1e5);
			continue;
			exit(EXIT_FAILURE);
		}

		std::cout << "Received game state: " << sz << "\n";
		state.deserialize(buffer, sz);
		Player* p = state.getPlayer(name);
		PlayerPos curr_pos = p->getPos();
		std::cout << "Received player pos: (";
		std::cout << (int)curr_pos.first << "," << (int)curr_pos.second << ")\n";
		
		if (state.getListOfPlayers().size() > sup) {
			DState.update();
		}

		Event event;
		sz = kBufferSize;
		bool sup1 = true;
		if(window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			int tmp1 = round(double(p->getPos().first) / DState.getSizeOFBlock());
			int tmp2 = round(double(p->getPos().second) / DState.getSizeOFBlock());

			std::cout << tmp1 << " !!!!!!!!!!!!! " << tmp2 << '\n';

			sf::Vector2i vec = sf::Mouse::getPosition(window);

			size_t x = vec.x / DState.getSizeOFBlock();
			size_t y = vec.y / DState.getSizeOFBlock();

			std::cout << x << " !!!!!!!!!!!!! " << y << '\n';


			//moving
			if (Keyboard::isKeyPressed(Keyboard::D)) {
				state.updatePos(p->getName(), sizeOfBlocks,curr_pos.first + 7, curr_pos.second);
			}
			if (Keyboard::isKeyPressed(Keyboard::A)) {
				state.updatePos(p->getName(), sizeOfBlocks, curr_pos.first - 7, curr_pos.second);
			}
			if (Keyboard::isKeyPressed(Keyboard::S)) {
				state.updatePos(p->getName(), sizeOfBlocks, curr_pos.first, curr_pos.second + 7);
			}
			if (Keyboard::isKeyPressed(Keyboard::W)) {
				state.updatePos(p->getName(), sizeOfBlocks, curr_pos.first, curr_pos.second - 7);
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

				sf::Vector2i vec = sf::Mouse::getPosition(window);

				size_t x = vec.x / DState.getSizeOFBlock();
				size_t y = vec.y / DState.getSizeOFBlock();

				int tmp1 = round((p->getPos().first) / DState.getSizeOFBlock());
				int tmp2 = round((p->getPos().second) / DState.getSizeOFBlock());


				GameState::Block block = GameState::Block::Count;
				if (x < state.getRow() && y < state.getCol()) {
					block = state.getMap()[y][x];
				}

				if (std::pair<GameIdx, GameIdx>(x, y) == std::pair<GameIdx, GameIdx>(tmp1, tmp2))goto con;

				for (size_t i = tmp1 - 1; i <= tmp1 + 1; ++i) {
					for (size_t j = tmp2 - 1; j <= tmp2 + 1; ++j) {
						if (i == x && j == y) {
							if (block == GameState::Block::Background) {
								p->serialize(buffer, sz, y, x, Block::Ground);
							}
							else {
								p->serialize(buffer, sz, y, x, Block::Background);
							}
							sup1 = false;
							goto con;
						}
					}
				}
			}
		}
		con:
		for (int i = 0; i < state.getCol(); i++) {
			for (int j = 0; j < state.getRow(); j++) {
				std::cout << int(state.getMap()[i][j]);
			}
			std::cout << '\n';
		}

		DState.drawMap();
		
		if (sup1) {
			p->serialize(buffer, sz);
		}

		if (sock_ptr->send(buffer, sz) != 0)
		{
			std::cout << "Failed to send pos\n";
			exit(EXIT_FAILURE);
		}

	}

	return 0;
}