#include "Map.h"

Map::Map() {
	InitMap();
}

void Map::LoadMap(std::string path)
{
}

void Map::PrintMap(std::vector<Player*>& players)
{
	int N = map.size();
	int half = N / 2;

	for (int i = 0; i < half; ++i) {
		std::cout << "[" << i << ". " << names[i] << "] ";
	}
	std::cout << std::endl;

	for (int i = N - 1; i >= half; --i) {
		std::cout << "[" << i << ". " << names[i] << "] ";
	}
	std::cout << std::endl;
}

std::pair<std::vector<Tile*>, std::vector<std::string>> Map::getMap()
{
	return std::make_pair(map, names);
}

void Map::addTile(Tile* tile, std::string name)
{
	map.push_back(tile);
	names.push_back(name);
}

Tile* Map::GetTileAt(int pos)
{
	return map[pos];
}

int Map::getSize()
{
	return map.size();
}

void Map::InitMap()
{
	addTile(new StartTile(), "�_�I");
	addTile(new PropertyTile(0, 200, "�x�_"), "�x�_");
	addTile(new ShopTile(), "�ө�");
	addTile(new PropertyTile(0, 200, "�s�_"), "�s�_");
	addTile(new PropertyTile(0, 200, "���"), "���");
	addTile(new HospitalTile(), "��|");
	addTile(new PropertyTile(0, 200, "�s��"), "�s��");
	addTile(new PropertyTile(0, 200, "�]��"), "�]��");
	addTile(new PropertyTile(0, 200, "�x��"), "�x��");
	addTile(new MiniGameTile(), "�C��");
	addTile(new PropertyTile(0, 200, "����"), "����");
	addTile(new PropertyTile(0, 200, "�n��"), "�n��");
	addTile(new PropertyTile(0, 200, "���L"), "���L");
	addTile(new FateTile(), "�R�B");
	addTile(new PropertyTile(0, 200, "�Ÿq"), "�Ÿq");
	addTile(new PropertyTile(0, 200, "�x�n"), "�x�n");
	addTile(new ChanceTile(), "���|");
	addTile(new PropertyTile(0, 200, "����"), "����");
	addTile(new PropertyTile(0, 200, "�̪F"), "�̪F");
	addTile(new MiniGameTile(), "�C��");
	addTile(new PropertyTile(0, 200, "�x�F"), "�x�F");
	addTile(new PropertyTile(0, 200, "�Ὤ"), "�Ὤ");
	addTile(new PropertyTile(0, 200, "�y��"), "�y��");
}
