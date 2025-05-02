#include "Map.h"
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <sstream>

Map::Map() {
	InitMap();
}

void Map::LoadMap(std::string path)
{
}

void Map::PrintMap(std::vector<Player*>& players)
{
	//�t�m
	const int MAP_SIZE = map.size();
	if (MAP_SIZE != 24) { //�i�H�A��
		std::cerr << "���~�G�a�ϥ��L�޿�ثe�Ȥ䴩 24 ��a�ϡC" << std::endl;
		return; 
	}
	const int GRID_WIDTH = 7;  // ����/��������l��
	const int GRID_HEIGHT = 5; // ���䪺��l��
	const int CELL_WIDTH = 12; // �C�Ӯ�l���e��
	const std::string RESET_COLOR = "\\033[0m";

	//���a��m ���� ���a
	std::map<int, std::vector<std::string>> playerSymbols;
	for (const auto& player : players) {
		std::string symbol = "[" + player->GetColor().substr(0, 1) + "]"; //�p[R]
		playerSymbols[player->GetPosition()].push_back(symbol);
	}

	//�����l�C��
	auto get_color_code = [&](int index) -> std::string {
		if (index < 0 || index >= MAP_SIZE) return RESET_COLOR;

		// �ȥ�Tile��color
		std::string color = map[index]->getColor();
		// �qGame::GetBackgroundColorCode�Ө�
		if (color == "red")        return "\\033[41;37m";
		if (color == "green")      return "\\033[42;37m";
		if (color == "yellow")     return "\\033[43;30m";
		if (color == "blue")       return "\\033[44;37m";
		if (color == "cyan")       return "\\033[46;30m";
		if (color == "purple")     return "\\033[45;37m";
		if (color == "orange")     return "\\033[48;2;255;168;0;30m"; 
		if (color == "teal")       return "\\033[46;30m";
		if (color == "brown")      return "\\033[43;30m";
		if (color == "gray")       return "\\033[100;37m";
		return RESET_COLOR;
	};

	//���U��� : �榡�Ʈ�l���e
	auto format_cell = [&](int index) {
		std::stringstream ss;
		if (index < 0 || index >= MAP_SIZE) {
			ss << std::setw(CELL_WIDTH) << std::setfill(' ') << "";
			return ss.str();
		}

		std::string colorCode = get_color_code(index);
		ss << colorCode;

		//���e: �s���M�W��
		std::string content = std::to_string(index) + " " + names[index];
		if (static_cast<int>(content.length()) > CELL_WIDTH - 1) {
			content = content.substr(0, CELL_WIDTH - 1); // �W�Xcell width�N�I�_
		}
		ss << std::left << std::setw(CELL_WIDTH - 1) << std::setfill(' ') << content << " "; //�d�@�ӪŮ�

		ss << RESET_COLOR;
		return ss.str();
	};

	//���U��ơG�榡�ƪ��a�аO
	auto format_players = [&](int index) {
		std::stringstream ss;
		if (playerSymbols.count(index)) {
			std::string p_str;
			for(size_t i = 0; i < playerSymbols[index].size(); ++i) {
				const auto& sym = playerSymbols[index][i];
				p_str += sym;
			}
			if (static_cast<int>(p_str.length()) > CELL_WIDTH) {
				p_str = p_str.substr(0, CELL_WIDTH); //�I�_
			}
			ss << std::left << std::setw(CELL_WIDTH) << std::setfill(' ') << p_str;
		} else {
			ss << std::setw(CELL_WIDTH) << std::setfill(' ') << "";
		}
		return ss.str();
	};

	//------------------------------------ø�s�a��----------------------------------------------
	//�������
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
	std::cout << std::endl;

	// ������l�W��
	std::cout << "|";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << format_cell(i) << "|";
	std::cout << std::endl;

	// ������l���a
	std::cout << "|";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << format_players(i) << "|";
	std::cout << std::endl;

	// �����U����� & ������W���
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
	std::cout << std::endl;

	// ������ (���� + �ť� + �k��)
	int left_idx = MAP_SIZE - 1; // �����_�l���� �p�G�O24�N�O23
	int right_idx = GRID_WIDTH; // �k���_�l����  �P�W�A7
	for (int i = 0; i < GRID_HEIGHT - 2; ++i) {
		// ������l�W��
		std::cout << "|" << format_cell(left_idx) << "|";
		// �����ť�
		std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) -1 , ' ');
		// �k����l�W��
		std::cout << "|" << format_cell(right_idx) << "|" << std::endl;

		// ������l���a
		std::cout << "|" << format_players(left_idx--) << "|";
		// �����ť�
		std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1)-1, ' ');
		// �k����l���a
		std::cout << "|" << format_players(right_idx++) << "|" << std::endl;

		// ���������
        if (i < GRID_HEIGHT - 3) {
		    std::cout << "+" << std::string(CELL_WIDTH, '-') << "+";
            std::cout << std::string((GRID_WIDTH - 2) * (CELL_WIDTH + 1) - 1, ' ');
		    std::cout << "+" << std::string(CELL_WIDTH, '-') << "+" << std::endl;
        }
	}

    // �����W����� & ������U���
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
	std::cout << std::endl;


	// ������l�W�� 18~12
	std::cout << "|";
	for (int i = GRID_WIDTH + GRID_HEIGHT - 3; i >= GRID_WIDTH + GRID_HEIGHT - 2 - (GRID_WIDTH-1) ; --i) {
        std::cout << format_cell(i) << "|";
    }
	std::cout << std::endl;

	// ������l���a
	std::cout << "|";
	for (int i = GRID_WIDTH + GRID_HEIGHT - 3; i >= GRID_WIDTH + GRID_HEIGHT - 2 - (GRID_WIDTH-1) ; --i) {
        std::cout << format_players(i) << "|";
    }
	std::cout << std::endl;

	// �������
	std::cout << "+";
	for (int i = 0; i < GRID_WIDTH; ++i) std::cout << std::string(CELL_WIDTH, '-') << "+";
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
