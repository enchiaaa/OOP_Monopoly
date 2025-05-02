#include "Map.h"
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

	for (Tile* tile : map) {
		delete tile;
	}
	map.clear();
	names.clear();

	std::string line;
	int lineNumber = 0;
	while (std::getline(inputFile, line)) {
		lineNumber++;
		if (lineNumber == 1) continue;
		if (line.empty() || line[0] == '#') continue;

		std::stringstream ss(line);
		std::string segment;
		std::vector<std::string> parts;

		while(std::getline(ss, segment, ',')) {
			parts.push_back(segment);
		}

		if (parts.size() < 2) {
			std::cerr << "ĵ�i�G�a���ɲ� " << lineNumber << " ��榡���~�A�w���L�C\n";
			continue;
		}

		std::string typeStr = parts[0];
		std::string nameStr = parts[1];
		int price = 0;
		int level = 0;

		if (parts.size() >= 3) {
			try {
				price = std::stoi(parts[2]);
			} catch (const std::invalid_argument&) {
				std::cerr << "ĵ�i�G�a���ɲ� " << lineNumber << " �����榡���~�A�ϥιw�]�� 0�C\n";
			} catch (const std::out_of_range&) {
				std::cerr << "ĵ�i�G�a���ɲ� " << lineNumber << " �����W�X�d��A�ϥιw�]�� 0�C\n";
			}
		}
		if (parts.size() >= 4) {
			try {
				level = std::stoi(parts[3]);
			} catch (const std::invalid_argument&) {
				std::cerr << "ĵ�i�G�a���ɲ� " << lineNumber << " �浥�Ů榡���~�A�ϥιw�]�� 0�C\n";
			} catch (const std::out_of_range&) {
				std::cerr << "ĵ�i�G�a���ɲ� " << lineNumber << " �浥�ŶW�X�d��A�ϥιw�]�� 0�C\n";
			}
		}

		Tile* newTile = nullptr;
		if (typeStr == "START") {
			newTile = new StartTile();
		} else if (typeStr == "PROPERTY") {
			newTile = new PropertyTile(level, price, nameStr);
		} else if (typeStr == "SHOP") {
			newTile = new ShopTile();
		} else if (typeStr == "HOSPITAL") {
			newTile = new HospitalTile();
		} else if (typeStr == "CHANCE") {
			newTile = new ChanceTile();
		} else if (typeStr == "FATE") {
			newTile = new FateTile();
		} else if (typeStr == "GAME") {
			newTile = new MiniGameTile();
		} else {
			std::cerr << "ĵ�i�G�a���ɲ� " << lineNumber << " ������ '/" << typeStr << "/' �L�ġA�w���L�C\n";
			continue;
		}

		if (newTile) {
			addTile(newTile, nameStr);
		}
	}

	inputFile.close();

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
    const int SIDE_HEIGHT_LEFT  = 6; // �����䪺��l�� 27�V22
    const int CELL_WIDTH = 12;    // �C�Ӯ�l���e��
    const std::string RESET_COLOR = "\033[0m";

    //���a��m�������a�Ÿ�
    std::map<int, std::vector<std::string>> playerSymbols;
    for (auto* p : players) {
        std::string sym = "[" + p->GetColor().substr(0,1) + "]";
        playerSymbols[p->GetPosition()].push_back(sym);
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
            for (auto& s : playerSymbols[idx]) pstr += s;
            if ((int)pstr.length() > CELL_WIDTH)
                pstr = pstr.substr(0, CELL_WIDTH);
            ss << std::left << std::setw(CELL_WIDTH) << std::setfill(' ') << pstr;
        } else {
            ss << std::setw(CELL_WIDTH) << std::setfill(' ') << "";
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
    int left_idx  = MAP_SIZE - 1; // 27
    int right_idx = GRID_WIDTH;   // 8
    for (int i = 0; i < SIDE_HEIGHT_LEFT; ++i) {
        bool draw_right = (i < SIDE_HEIGHT_RIGHT);

        // �W�٦�
        std::cout << "|" << format_cell(left_idx--) << "|";
        std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) - 1, ' ');
        if (draw_right) {
            std::cout << "|" << format_cell(right_idx++) << "|";
        } else {
            std::cout << std::string(CELL_WIDTH + 1, ' ');
        }
        std::cout << "\n";

        // ���a��
        std::cout << "|" << format_players(left_idx + 1) << "|";
        std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) - 1, ' ');
        if (draw_right) {
            std::cout << "|" << format_players(right_idx - 1) << "|";
        } else {
            std::cout << std::string(CELL_WIDTH + 1, ' ');
        }
        std::cout << "\n";

        // ������u
        if (i < SIDE_HEIGHT_LEFT - 1) {
            std::cout << "+" << std::string(CELL_WIDTH, '-') << "+";
            std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) - 1, ' ');
            if (draw_right && i < SIDE_HEIGHT_RIGHT - 1) {
                std::cout << "+" << std::string(CELL_WIDTH, '-') << "+";
            } else {
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
	addTile(new StartTile(), "START");               // 0
	addTile(new PropertyTile(0, 200, "USA"), "USA"); // 1
	addTile(new FateTile(), "Fate");                // 2
	addTile(new PropertyTile(0, 200, "France"), "France"); // 3
	addTile(new ShopTile(), "Item Shop");           // 4
	addTile(new PropertyTile(0, 200, "Japan"), "Japan"); // 5
	addTile(new PropertyTile(0, 200, "Mexico"), "Mexico"); // 6
	addTile(new PropertyTile(0, 200, "Sweden"), "Sweden"); // 7

	addTile(new PropertyTile(0, 200, "Germany"), "Germany"); // 8
	addTile(new PropertyTile(0, 200, "UK"), "UK");       // 9
	addTile(new ChanceTile(), "Chance");             // 10
	addTile(new PropertyTile(0, 200, "Canada"), "Canada"); // 11
	addTile(new PropertyTile(0, 200, "Australia"), "Australia"); // 12
	addTile(new PropertyTile(0, 200, "Egypt"), "Egypt");   // 13

	addTile(new PropertyTile(0, 200, "Italy"), "Italy");   // 14
	addTile(new HospitalTile(), "Hospital");         // 15
	addTile(new FateTile(), "Fate");                // 16
	addTile(new PropertyTile(0, 200, "Spain"), "Spain");   // 17
	addTile(new PropertyTile(0, 200, "China"), "China");   // 18
	addTile(new PropertyTile(0, 200, "Turkey"), "Turkey"); // 19
	addTile(new PropertyTile(0, 200, "Korea"), "Korea");   // 20

	addTile(new PropertyTile(0, 200, "Brazil"), "Brazil"); // 21
	addTile(new PropertyTile(0, 200, "Uganda"), "Uganda"); // 22
	addTile(new PropertyTile(0, 200, "Russia"), "Russia"); // 23
	addTile(new ChanceTile(), "Chance");             // 24
	addTile(new PropertyTile(0, 200, "India"), "India");   // 25
	addTile(new PropertyTile(0, 200, "England"), "England"); // 26
	addTile(new PropertyTile(0, 200, "Africa"), "Africa"); // 27
}
