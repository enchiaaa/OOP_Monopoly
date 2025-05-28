#include "Map.h"
#include "Monopoly.h"

#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>

Map::Map() {
	InitMap();
}

void Map::LoadMap(std::string path)
{
	std::ifstream inputFile(path);
	if (!inputFile.is_open()) {
		std::cerr << "���~�G�L�k�}�Ҧa�ϳ]�w�� '" << path << "'�C�N�ϥιw�]�a�ϡC\n";
		InitMap();
		return;
	}

	// �M���즳���a�Ϯ�l
	for (Tile* tile : map) {
		delete tile;
	}
	map.clear();
	names.clear();

	// Ū�� JSON �ɮפ��e
	std::string jsonContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	inputFile.close();

	try {
		// �ѪR JSON ���
		// �`�N�G���B�ϥ�²���� JSON �ѪR��k�A��ڱM�פ���ĳ�ϥ� JSON �ѪR�w
		// JSON �榡�w���p�U�G
		/*
		{
			"tiles": [
				{
					"type": "START",
					"name": "�_�I",
					"price": 0,
					"level": 0
				},
				{
					"type": "PROPERTY",
					"name": "�x�_",
					"price": 200,
					"level": 0
				},
				...
			]
		}
		*/

		// ²���ѪR - �o�̶ȧ@�ܽd�A������ϥ� JSON �w�p nlohmann/json
		// �ϥγv��Ū���覡���� JSON �ѪR
		std::stringstream ss(jsonContent);
		std::string line;
		bool inTilesArray = false;

		int num = 0;

		while (std::getline(ss, line)) {
			// ���L�ťզ�M����
			if (line.empty() || line.find("//") == 0) continue;

			// �}�l�ѪR tiles �}�C
			if (line.find("\"tiles\"") != std::string::npos) {
				inTilesArray = true;
				continue;
			}

			// ���b tiles �}�C���A���L
			if (!inTilesArray) continue;

			// ���� tiles �}�C
			if (line.find("]") != std::string::npos) {
				inTilesArray = false;
				continue;
			}

			// �ѪR tile ����
			if (line.find("{") != std::string::npos) {
				std::string typeStr = "";
				std::string nameStr = "";
				int price = 0;
				int level = 0;

				// Ū�� tile �����ݩʪ���o�{ }
				std::string objLine;
				while (std::getline(ss, objLine) && objLine.find("}") == std::string::npos) {
					// �ѪR type
					if (objLine.find("\"type\"") != std::string::npos) {
						size_t start = objLine.find(":") + 1;
						size_t end = objLine.find(",", start);
						if (end == std::string::npos) end = objLine.length();
						typeStr = objLine.substr(start, end - start);
						// �����޸��M�ť�
						typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), '\"'), typeStr.end());
						typeStr.erase(std::remove(typeStr.begin(), typeStr.end(), ' '), typeStr.end());
					}

					// �ѪR name
					if (objLine.find("\"name\"") != std::string::npos) {
						size_t start = objLine.find(":") + 1;
						size_t end = objLine.find(",", start);
						if (end == std::string::npos) end = objLine.length();
						nameStr = objLine.substr(start, end - start);
						// �����޸��M�ť�
						nameStr.erase(std::remove(nameStr.begin(), nameStr.end(), '\"'), nameStr.end());
						nameStr.erase(std::remove(nameStr.begin(), nameStr.end(), ' '), nameStr.end());
					}

					// �ѪR price
					if (objLine.find("\"price\"") != std::string::npos) {
						size_t start = objLine.find(":") + 1;
						size_t end = objLine.find(",", start);
						if (end == std::string::npos) end = objLine.length();
						std::string priceStr = objLine.substr(start, end - start);
						// �����ť�
						priceStr.erase(std::remove(priceStr.begin(), priceStr.end(), ' '), priceStr.end());
						try {
							price = std::stoi(priceStr);
						}
						catch (...) {
							std::cerr << "ĵ�i�G����榡���~ '" << priceStr << "'�A�ϥιw�]�� 0�C\n";
						}
					}

					// �ѪR level
					if (objLine.find("\"level\"") != std::string::npos) {
						size_t start = objLine.find(":") + 1;
						size_t end = objLine.find(",", start);
						if (end == std::string::npos) end = objLine.length();
						std::string levelStr = objLine.substr(start, end - start);
						// �����ť�
						levelStr.erase(std::remove(levelStr.begin(), levelStr.end(), ' '), levelStr.end());
						try {
							level = std::stoi(levelStr);
						}
						catch (...) {
							std::cerr << "ĵ�i�G���Ů榡���~ '" << levelStr << "'�A�ϥιw�]�� 0�C\n";
						}
					}
				}

				// �ھڸѪR�X����ƫإߦX�A�� Tile ����
				Tile* newTile = nullptr;
				if (typeStr == "START") {
					newTile = new StartTile(num);
				}
				else if (typeStr == "PROPERTY") {
					newTile = new PropertyTile(level, price, nameStr, num);
				}
				else if (typeStr == "SHOP") {
					newTile = new ShopTile(num);
				}
				else if (typeStr == "HOSPITAL") {
					newTile = new HospitalTile(num);
				}
				else if (typeStr == "CHANCE") {
					newTile = new ChanceTile(num);
				}
				else if (typeStr == "FATE") {
					newTile = new FateTile(num);
				}
				else if (typeStr == "GAME") {
					newTile = new MiniGameTile(num);
				}
				else {
					std::cerr << "ĵ�i�G��������l���� '" << typeStr << "'�A�w���L�C\n";
					continue;
				}

				// �N�إߪ���l�K�[��a�Ϥ�
				if (newTile) {
					addTile(newTile, nameStr);
				}

				num++;
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "���~�G�ѪR�a���ɮɵo�Ϳ��~�G" << e.what() << "�C�N�ϥιw�]�a�ϡC\n";
		// �M���i�ೡ�����J���a��
		for (Tile* tile : map) {
			delete tile;
		}
		map.clear();
		names.clear();
		InitMap();
		return;
	}

	// �p�G�S�����\���J�����l�A�ϥιw�]�a��
	if (map.empty()) {
		std::cerr << "ĵ�i�G�a���ɥ����\���J�����l�A�N�ϥιw�]�a�ϡC\n";
		InitMap();
	}
}

void Map::PrintMap(std::vector<Player*>& players)
{
	const int MAP_SIZE = map.size();
	if (MAP_SIZE != 28) {
		std::cerr << "���~�G�a�ϥ��L�޿�ثe�Ȥ䴩 28 ��a�� (��ڤj�p: " << MAP_SIZE << ")�C\n";
		for (size_t i = 0; i < map.size(); ++i) std::cout << i << ": " << names[i] << std::endl;
		return;
	}
	const int GRID_WIDTH = 8;      // ����/��������l�� 0�V7
	const int SIDE_HEIGHT_RIGHT = 6; // �k���䪺��l�� 8�V13
	const int SIDE_HEIGHT_LEFT = 6; // �����䪺��l�� 27�V22
	const int CELL_WIDTH = 16;    // �C�Ӯ�l���e��
	const std::string RESET_COLOR = "\033[0m";

	//���a��m�������a�Ÿ�
	std::map<int, std::vector<std::string>> playerSymbols;
	char symbol = 'A';
	for (auto* p : players) {
		// std::string sym = "[" + p->GetColor().substr(0,1) + "]";
		std::string sym = "[" + std::string(1, symbol) + "]";
		std::string coloredSym = Monopoly::GetColorCode(p->GetColor()) + sym + "\033[0m";
		playerSymbols[p->GetPosition()].push_back(coloredSym);
		symbol++;
	}

	//�C��
	auto get_color_code = [&](int idx) -> std::string {
		if (idx < 0 || idx >= MAP_SIZE) return RESET_COLOR;
		std::string c = map[idx]->getColor();
		if (c == "red")    return "\033[41;37m";
		if (c == "green")  return "\033[42;37m";
		if (c == "yellow") return "\033[43;30m";
		if (c == "blue")   return "\033[44;37m";
		if (c == "cyan")   return "\033[46;30m";
		if (c == "purple") return "\033[45;37m";
		if (c == "orange") return "\033[48;2;255;168;0;30m";
		if (c == "teal")   return "\033[46;30m";
		if (c == "brown")  return "\033[43;30m";
		if (c == "gray")   return "\033[100;37m";
		return RESET_COLOR;
	};

	//�榡��
	auto format_cell = [&](int idx) {
		std::stringstream ss;
		if (idx < 0 || idx >= MAP_SIZE) {
			ss << std::setw(CELL_WIDTH) << std::setfill(' ') << "";
			return ss.str();
		}
		std::string colorCode = get_color_code(idx);
		ss << colorCode;
		std::string content = std::to_string(idx) + " " + names[idx];
		if (map[idx]->getLevel() != 0)
			content += "(" + std::to_string(map[idx]->getLevel()) + ")";
		if ((int)content.length() > CELL_WIDTH - 1)
			content = content.substr(0, CELL_WIDTH - 1);
		ss << std::left << std::setw(CELL_WIDTH - 1) << std::setfill(' ') << content << " ";
		ss << RESET_COLOR;
		return ss.str();
	};

	//�榡�ƪ��a�Ÿ�
	auto format_players = [&](int idx) {
		std::stringstream ss;
		if (playerSymbols.count(idx)) {
			std::string pstr;
			int visible_len = 0;

			for (auto& s : playerSymbols[idx]) {
				pstr += s;
				visible_len += 3;
			}

			// �ۦ�ɨ��ťա]�`�e�� CELL_WIDTH�^
			int pad = CELL_WIDTH - visible_len;
			pstr += std::string(pad, ' ');
			ss << pstr;
		}
		else {
			ss << std::string(CELL_WIDTH, ' ');
		}
		return ss.str();
	};

	//------------------------------------ø�s�a��----------------------------------------------
	//�������
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
	std::cout << "\n";

	//������l�W�� (0�V7)
	std::cout << "|";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << format_cell(i) << "|";
	std::cout << "\n";

	//������l���a
	std::cout << "|";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << format_players(i) << "|";
	std::cout << "\n";

	//�����U�����
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
	std::cout << "\n";

	//������ (���� 27�V22 + �ť� + �k�� 8�V13)
	int left_idx = MAP_SIZE - 1; // 27
	int right_idx = GRID_WIDTH;   // 8
	for (int i = 0; i < SIDE_HEIGHT_LEFT; ++i) {
		bool draw_right = (i < SIDE_HEIGHT_RIGHT);

		// �W�٦�
		std::cout << "|" << format_cell(left_idx--) << "|";
		std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) - 1, ' ');
		if (draw_right) {
			std::cout << "|" << format_cell(right_idx++) << "|";
		}
		else {
			std::cout << std::string(CELL_WIDTH + 1, ' ');
		}
		std::cout << "\n";

		// ���a��
		std::cout << "|" << format_players(left_idx + 1) << "|";
		std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) - 1, ' ');
		if (draw_right) {
			std::cout << "|" << format_players(right_idx - 1) << "|";
		}
		else {
			std::cout << std::string(CELL_WIDTH + 1, ' ');
		}
		std::cout << "\n";

		// ������u
		if (i < SIDE_HEIGHT_LEFT - 1) {
			std::cout << "+" << std::string(CELL_WIDTH, '-') << "+";
			std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) - 1, ' ');
			if (draw_right && i < SIDE_HEIGHT_RIGHT - 1) {
				std::cout << "+" << std::string(CELL_WIDTH, '-') << "+";
			}
			else {
				std::cout << std::string(CELL_WIDTH + 1, ' ');
			}
			std::cout << "\n";
		}
	}

	// �����W�����
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
	std::cout << "\n";

	// ������l�W�� (21 �� 14)
	std::cout << "|";
	for (int i = 21; i >= 14; --i) {
		std::cout << format_cell(i) << "|";
	}
	std::cout << "\n";

	// ������l���a
	std::cout << "|";
	for (int i = 21; i >= 14; --i) {
		std::cout << format_players(i) << "|";
	}
	std::cout << "\n";

	// �������
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
	std::cout << "\n";
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
	// �M���i��s�b���¦a�ϸ�� (�p�G InitMap �Q�N�~�I�s�h��)
	for (Tile* tile : map) {
		delete tile;
	}
	map.clear();
	names.clear();

	// �̷ӹϤ����ǫإ� 28 �Ӯ�l
	addTile(new StartTile(0), "START");               // 0
	addTile(new PropertyTile(0, 200, "USA", 1), "USA"); // 1
	addTile(new FateTile(2), "Fate");                // 2
	addTile(new PropertyTile(0, 200, "France", 3), "France"); // 3
	addTile(new ShopTile(4), "Item Shop");           // 4
	addTile(new PropertyTile(0, 200, "Japan", 5), "Japan"); // 5
	addTile(new PropertyTile(0, 200, "Mexico", 6), "Mexico"); // 6
	addTile(new PropertyTile(0, 200, "Sweden", 7), "Sweden"); // 7

	addTile(new PropertyTile(0, 200, "Germany", 8), "Germany"); // 8
	addTile(new PropertyTile(0, 200, "UK", 9), "UK");       // 9
	addTile(new ChanceTile(10), "Chance");             // 10
	addTile(new PropertyTile(0, 200, "Canada", 11), "Canada"); // 11
	addTile(new PropertyTile(0, 200, "Australia", 12), "Australia"); // 12
	addTile(new PropertyTile(0, 200, "Egypt", 13), "Egypt");   // 13

	addTile(new PropertyTile(0, 200, "Italy", 14), "Italy");   // 14
	addTile(new HospitalTile(15), "Hospital");         // 15
	addTile(new FateTile(16), "Fate");                // 16
	addTile(new PropertyTile(0, 200, "Spain", 17), "Spain");   // 17
	addTile(new PropertyTile(0, 200, "China", 18), "China");   // 18
	addTile(new PropertyTile(0, 200, "Turkey", 19), "Turkey"); // 19
	addTile(new PropertyTile(0, 200, "Korea", 20), "Korea");   // 20

	addTile(new PropertyTile(0, 200, "Brazil", 21), "Brazil"); // 21
	addTile(new PropertyTile(0, 200, "Uganda", 22), "Uganda"); // 22
	addTile(new PropertyTile(0, 200, "Russia", 23), "Russia"); // 23
	addTile(new ChanceTile(24), "Chance");             // 24
	addTile(new PropertyTile(0, 200, "India", 25), "India");   // 25
	addTile(new PropertyTile(0, 200, "England", 26), "England"); // 26
	addTile(new PropertyTile(0, 200, "Africa", 27), "Africa"); // 27
}
