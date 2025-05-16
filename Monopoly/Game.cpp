#include "Game.h"
#include "Map.h"
#include "Monopoly.h"
#include "HorseRacing.h"
#include "SheLongMen.h"

#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include <chrono>
#include <conio.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <algorithm>
#include <sstream>
#include <iomanip>

// ����C����ҫ��СA�Ω󪱮a�}���B�z
Game* gameInstance = nullptr;

std::string colors[4] = { "red", "yellow", "green", "blue" };

std::vector<std::vector<std::string>> diceFaces = {
	{
		"�z�w�w�w�w�w�w�w�{",
		"�x       �x",
		"�x   ��   �x",
		"�x       �x",
		"�|�w�w�w�w�w�w�w�}"
	},
	{
		"�z�w�w�w�w�w�w�w�{",
		"�x ��     �x",
		"�x       �x",
		"�x     �� �x",
		"�|�w�w�w�w�w�w�w�}"
	},
	{
		"�z�w�w�w�w�w�w�w�{",
		"�x ��     �x",
		"�x   ��   �x",
		"�x     �� �x",
		"�|�w�w�w�w�w�w�w�}"
	},
	{
		"�z�w�w�w�w�w�w�w�{",
		"�x ��   �� �x",
		"�x       �x",
		"�x ��   �� �x",
		"�|�w�w�w�w�w�w�w�}"
	},
	{
		"�z�w�w�w�w�w�w�w�{",
		"�x ��   �� �x",
		"�x   ��   �x",
		"�x ��   �� �x",
		"�|�w�w�w�w�w�w�w�}"
	},
	{
		"�z�w�w�w�w�w�w�w�{",
		"�x ��   �� �x",
		"�x ��   �� �x",
		"�x ��   �� �x",
		"�|�w�w�w�w�w�w�w�}"
	}
};

Game::Game()
{
	currentPlayerIdx = 0;
	gameOver = false;
	gameMap = nullptr; // ��l�Ƭ� nullptr
	srand(static_cast<unsigned int>(time(nullptr)));

	// �]�w����C����ҫ���
	gameInstance = this;
}

void Game::InitGame()
{
	gameMap = new Map();

	int playerNum = 0;

	std::string question = "## ��ܹC���H�� ##";
	std::vector<std::string> options = {
		"2�H",
		"3�H",
		"4�H"
	};

	int choice = Monopoly::GetUserChoice(question, options, true, false);

	playerNum = choice + 2;

	Clear();
	std::cout << "## ��J���a�]�w ##\n";
	for (int i = 0; i < playerNum; i++) {
		std::string name;
		while (true) {
			std::cout << "\n���a " << i + 1 << " ���W�١]�̦h6�Ӧr�A�B���i���ơ^�G";
			std::cin >> name;

			if (name.length() > 6) {
				std::cout << "�W�٤Ӫ��A�Э��s��J�C\n";
				continue;
			}
			bool duplicate = false;
			for (const auto& p : players) {
				if (p->GetName() == name) {
					std::cout << "�W�٤w�Q�ϥΡA�Э��s��J�C\n";
					duplicate = true;
					break;
				}
			}

			if (!duplicate) break;
		}

		Player* p = new Player(name, colors[i], 1000);
		players.push_back(p);
	}


	std::cout << "\n�C����l�Ƨ����A�@ " << players.size() << " �쪱�a�C" << std::endl;

	Clear(); // �b���a�]�w������M���e������ܪ�l���A
	Monopoly::WaitForEnter();
}

void Game::StartGame()
{
	while (!gameOver) {
		NextTurn();
		CheckWinCondition();
	}
	EndGame();
}

void Game::NextTurn()
{
	system("cls");
	PrintMapStatus();
	PrintPlayerStatus();

	// �C�^�X�}�l�ɥ��ˬd�C���O�_�w�g�i�H����
	if (CheckWinCondition()) {
		return; // �p�G�w�g���ӧQ����Q�����A���������^�X
	}

	// �T�O currentPlayerIdx �b���Ľd��
	if (currentPlayerIdx < 0 || currentPlayerIdx >= static_cast<int>(players.size())) {
		std::cerr << "ĵ�i�G��e���a���� " << currentPlayerIdx << " �L�ġA���]�� 0\n";
		currentPlayerIdx = 0;
	}

	auto currentPlayer = players[currentPlayerIdx];

	// �p�G��e���a�w�}���A����U�@��
	if (currentPlayer->GetMoney() <= 0) {
		// ���U�@��
		currentPlayerIdx = (currentPlayerIdx + 1) % static_cast<int>(players.size());
		NextTurn(); // ���j�I�s�A����U�@�Ӫ��a
		return;
	}

	std::cout << "\n���� " << currentPlayer->GetName() << " ���^�X�C" << std::endl;
	bool playerTurnCompleted = false;

	// �B�z��|���A
	if (currentPlayer->inHospital) {
		// ��ܦ�|���A
		std::cout << currentPlayer->GetName() << " ���b��|�A�w�g�L�F "
			<< currentPlayer->hosipitalDay << " �ѡC" << std::endl;

		if (currentPlayer->hosipitalDay >= 2) {
			// �w�g��|3�ѤF(0,1,2)�A�i�H�X�|
			std::cout << "\n" << currentPlayer->GetName() << " �w�X�|�I\n";
			currentPlayer->inHospital = false;
			currentPlayer->hosipitalDay = 0;
		}
		else {
			// �W�[��|�Ѽƨø��L�^�X
			currentPlayer->hosipitalDay++;
			playerTurnCompleted = true;
			std::cout << currentPlayer->GetName() << " ���b��|�A�L�k��ʡC\n";
		}
	}

	while (!playerTurnCompleted) {
		std::string question = "���� " + currentPlayer->GetName() + " ���^�X�C\n�ثe���B: $" + std::to_string(currentPlayer->GetMoney());
		std::vector<std::string> options = {
			"�i���Y��",
			"�ϥιD��",
			//"�@��"
		};
		int choice = Monopoly::GetUserChoice(question, options, true, true);

		if (choice == 0) {
			// �i���Y��
			int roll = RollDiceWithAsciiAnimation();
			currentPlayer->Move(roll, gameMap->getSize());
			int pos = currentPlayer->GetPosition();
			std::cout << currentPlayer->GetName() << " ���ʨ�� " << pos << " ��C\n";

			auto currentTile = gameMap->GetTileAt(pos);
			currentTile->OnLand(currentPlayer);
			playerTurnCompleted = true; // ���a�^�X����
		}
		else if (choice == 1) {
			// �ϥιD�㪺�޿�
			std::vector<Item*> items = currentPlayer->GetItem();
			if (items.empty()) {
				std::cout << "�A�S������D��i�H�ϥΡI\n";
				Monopoly::WaitForEnter(); // ��ܰT���ᵥ�ݪ��a��Enter�~��
				continue; // �^�쪱�a��ʿ��
			}
			else {
				std::string itemQuestion = "��ܭn�ϥΪ��D��G";
				std::vector<std::string> itemOptions;
				for (Item* item : items) {
					itemOptions.push_back(item->GetName() + " - " + item->GetDescription());
				}
				itemOptions.push_back("��^");

				int itemChoice = Monopoly::GetUserChoice(itemQuestion, itemOptions, true, false);
				if (itemChoice < static_cast<int>(items.size())) {
					// �ϥο�w���D��
					Item* selectedItem = items[itemChoice];
					// �O�s��l���D��C��j�p
					size_t originalItemCount = items.size();

					// �ϥιD��
					currentPlayer->UseItem(selectedItem);

					// �ˬd�ϥιD���O�_�ݭn�i���L�ʧ@
					// �p�G�O�����l�D��A���a�w�g���ʦ�m�A���P�����^�X
					if (selectedItem->GetName() == "�����l") {
						// ���o���a��e��m
						int pos = currentPlayer->GetPosition();
						std::cout << currentPlayer->GetName() << " ���ʨ�� " << pos << " ��C\n";

						// Ĳ�o��e�Ҧb��l���ĪG
						auto currentTile = gameMap->GetTileAt(pos);
						currentTile->OnLand(currentPlayer);
						playerTurnCompleted = true; // ���a�^�X����
					}
					else {
						// ��L�������D��ϥΫ�A���a�i�H�~���ܦ��
						// ���]�w playerTurnCompleted �� true�A�����a�^���ʿ��
						continue;
					}

					delete selectedItem; // �R���w�ϥΪ��D��
				}
				else {
					// ��ܪ�^
					continue; // �^�쪱�a��ʿ��
				}
			}
		}
		// ��J���O
		else if (choice == -1) {
			playerTurnCompleted = true; // ���a�^�X����
		}
		//else if (choice == 2) {
		//	int temp;
		//	std::cout << "��J�Q�h����l: ";
		//	std::cin >> temp;

		//	if (temp < 0)
		//		temp = 0;

		//	temp %= gameMap->getSize();

		//	if (currentPlayer->GetPosition() != temp) {
		//		if (currentPlayer->GetPosition() > temp)
		//			currentPlayer->Move(gameMap->getSize() - currentPlayer->GetPosition() + temp, gameMap->getSize());
		//		else
		//			currentPlayer->Move(temp - currentPlayer->GetPosition(), gameMap->getSize());
		//	}

		//	gameMap->GetTileAt(temp)->OnLand(currentPlayer);
		//	playerTurnCompleted = true; // ���a�^�X����
		//}
	}

	Monopoly::WaitForEnter();
	Monopoly::UpdateScreen();

	// �ˬd���a�O�_�}��
	if (currentPlayer->GetMoney() <= 0) {
		std::cout << currentPlayer->GetName() << " �w�}���I\n";

		// �ߧY�ˬd�O�_�u�ѤU�@�쪱�a�A�p�G�O�h�����C��
		if (CheckWinCondition()) {
			EndGame();
			return;
		}
	}

	// �۰ʦs��
	SaveGame();

	// ���U�@��
	currentPlayerIdx = (currentPlayerIdx + 1) % static_cast<int>(players.size());
}

bool Game::CheckWinCondition()
{
	// �p�⤴�b�C���������a��
	int activePlayers = 0;
	int lastPlayerIdx = -1;

	for (size_t i = 0; i < players.size(); i++) {
		// �u�����B����Τp��0�~��}���A���B��0����}��
		if (players[i]->GetMoney() > 0) {
			activePlayers++;
			lastPlayerIdx = i;
		}
	}

	// �p�G�u�Ѥ@�쪱�a�S���}���A�h�L���
	if (activePlayers == 1 && lastPlayerIdx != -1) {
		std::cout << "\n" << players[lastPlayerIdx]->GetName() << " �O�ߤ@�S���}�������a�A��o�ӧQ�I" << std::endl;
		gameOver = true;
		return true;
	}

	// �ˬd�O�_�����a�F�����������
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i]->GetMoney() >= 2000) {
			std::cout << "\n" << players[i]->GetName() << " Ĺ�o�ӧQ�I" << std::endl;
			gameOver = true;
			return true;
		}
	}

	return false;
}

void Game::EndGame()
{
	std::cout << "\n�C�������A�̲׸���G" << std::endl;
	for (size_t i = 0; i < players.size(); i++) {
		std::cout << players[i]->GetName() << ": $" << players[i]->GetMoney() << std::endl;
	}

	// �C�������ɧR���s��
	DeleteSaveGame();
}

void Game::Clear()
{
	//std::cout << "\033[2J\033[H"; // �M���e��
	system("cls");
	PrintMapStatus();
	PrintPlayerStatus();
	std::cout << std::endl;
}

void Game::PrintMapStatus()
{
	// �����I�s Map ���O�� PrintMap �禡�A�ϥηs������G��
	gameMap->PrintMap(players);
}

void Game::PrintPlayerStatus()
{
	std::cout << "+------------+--------+-------+------------+--------------------------+\n";
	std::cout << "| ���a�W��   | ���   | ��m  | ���A       | �֦��D��                 |\n";
	std::cout << "+------------+--------+-------+------------+--------------------------+\n";

	const std::string RESET = "\033[0m";

	for (int i = 0; i < players.size(); i++) {
		std::string colorCode = Monopoly::GetColorCode(players[i]->GetColor());
		std::cout << colorCode;

		std::string name = "[" + std::string(1, '1' + i) + "]" + players[i]->GetName();
		std::string status;

		if (players[i]->inHospital) {
			status = "��|�� (" + std::to_string(players[i]->hosipitalDay) + ")";
		}
		else {
			status = "���`";
		}

		// �D��r��
		std::string itemStr;
		if (players[i]->GetItem().empty()) {
			itemStr = "�L";
		}
		else {
			for (size_t j = 0; j < players[i]->GetItem().size(); j++) {
				itemStr += players[i]->GetItem()[j]->GetName();
				if (j != players[i]->GetItem().size() - 1)
					itemStr += ", ";
			}
		}

		// ��X�C��Asetw ����e��
		std::cout << RESET << "| "
			<< colorCode << "[" + std::string(1, '1' + i) + "] "
			<< RESET << std::left << std::setw(6) << players[i]->GetName()
			<< " | " << std::right << std::setw(6) << players[i]->GetMoney()
			<< " | " << std::right << std::setw(5) << players[i]->GetPosition()
			<< " | " << std::left << std::setw(10) << status
			<< " | " << std::left << std::setw(24) << itemStr
			<< " |\n";

		std::cout << "\033[0m";
	}
	std::cout << "+------------+--------+-------+------------+--------------------------+\n";

}

void Game::PrintDice(int d1, int d2) {

	auto& face1 = diceFaces[d1 - 1];
	auto& face2 = diceFaces[d2 - 1];

	for (int i = 0; i < 5; i++) {
		std::cout << face1[i] << "  " << face2[i] << std::endl;
	}
}

int Game::RollDiceWithAsciiAnimation() {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	const int rollTimes = 10;
	int finalRoll = 1;

	std::cout << "�Y�뤤...\n";

	for (int i = 0; i < rollTimes; ++i) {
		int d1 = rand() % 6 + 1;
		int d2 = rand() % 6 + 1;

		Clear();
		PrintDice(d1, d2);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		finalRoll = d1 + d2;
	}

	std::cout << "�Y�뵲�G�O " << finalRoll << " �I�I" << std::endl;
	Monopoly::WaitForEnter();
	return finalRoll;
}

bool Game::SaveGame(const std::string& filename)
{
	// �}���ɮ�
	std::ofstream saveFile(filename);
	if (!saveFile.is_open()) {
		std::cerr << "�L�k�Ыئs�ɡG" << filename << std::endl;
		return false;
	}

	try {
		// �g�J JSON �榡���s��
		saveFile << "{\n";

		// �O�s�ثe���a����
		saveFile << "  \"currentPlayerIdx\": " << currentPlayerIdx << ",\n";

		// �O�s���a�ƶq�]�W�[�����H�K�ˬd�^
		saveFile << "  \"playerCount\": " << players.size() << ",\n";

		// �O�s�a�Ϫ��A�]�o�̶ȫO�s�a�Ϯ�l���֦��̡^
		saveFile << "  \"map\": {\n";
		auto mapData = gameMap->getMap();
		saveFile << "    \"size\": " << gameMap->getSize() << ",\n";

		// �O�s�C�Ӯ�l���֦��̡]�p�G�O�a���^
		saveFile << "    \"tiles\": [\n";
		for (int i = 0; i < gameMap->getSize(); ++i) {
			Tile* tile = gameMap->GetTileAt(i);
			saveFile << "      {\n";
			saveFile << "        \"index\": " << i << ",\n";
			saveFile << "        \"name\": \"" << mapData.second[i] << "\",\n";

			// �p�G�O�a����l�A��������֦���
			PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile);
			if (propTile && propTile->GetOwner()) {
				// ��X�֦��̦b���a�C��������
				int ownerIdx = -1;
				for (size_t j = 0; j < players.size(); ++j) {
					if (players[j] == propTile->GetOwner()) {
						ownerIdx = static_cast<int>(j);
						break;
					}
				}
				saveFile << "        \"ownerIdx\": " << ownerIdx << ",\n";
				saveFile << "        \"level\": " << propTile->GetLevel() << "\n";
			}
			else {
				saveFile << "        \"ownerIdx\": -1,\n";
				saveFile << "        \"level\": 0\n";
			}

			if (i < gameMap->getSize() - 1) {
				saveFile << "      },\n";
			}
			else {
				saveFile << "      }\n";
			}
		}
		saveFile << "    ]\n";
		saveFile << "  },\n";

		// �O�s���a��T
		saveFile << "  \"players\": [\n";
		for (size_t i = 0; i < players.size(); ++i) {
			Player* p = players[i];
			saveFile << "    {\n";
			saveFile << "      \"name\": \"" << p->GetName() << "\",\n";
			saveFile << "      \"color\": \"" << p->GetColor() << "\",\n";
			saveFile << "      \"money\": " << p->GetMoney() << ",\n";
			saveFile << "      \"position\": " << p->GetPosition() << ",\n";
			saveFile << "      \"inHospital\": " << (p->inHospital ? "true" : "false") << ",\n";
			saveFile << "      \"hospitalDay\": " << p->hosipitalDay << ",\n";

			// �O�s�D��M��
			saveFile << "      \"items\": [\n";
			std::vector<Item*> items = p->GetItem();
			for (size_t j = 0; j < items.size(); ++j) {
				saveFile << "        {\n";
				saveFile << "          \"name\": \"" << items[j]->GetName() << "\",\n";
				saveFile << "          \"type\": \"" << "item" << "\"\n";  // �o�̥i�H�X�i�A�x�s���P�������D��
				if (j < items.size() - 1) {
					saveFile << "        },\n";
				}
				else {
					saveFile << "        }\n";
				}
			}
			saveFile << "      ]\n";

			if (i < players.size() - 1) {
				saveFile << "    },\n";
			}
			else {
				saveFile << "    }\n";
			}
		}
		saveFile << "  ]\n";

		saveFile << "}\n";
		saveFile.close();

		std::cout << "�C���i�פw���\�O�s�� " << filename << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "�O�s�C���ɵo�Ϳ��~�G" << e.what() << std::endl;
		saveFile.close();
		return false;
	}
}

bool Game::LoadGame(const std::string& filename)
{
	std::ifstream loadFile(filename);
	if (!loadFile.is_open()) {
		std::cerr << "�L�k�}�Ҧs�ɡG" << filename << std::endl;
		return false;
	}

	try {
		// �M����e�C�����A
		for (Player* p : players) {
			delete p;
		}
		players.clear();

		if (gameMap) {
			delete gameMap;
		}
		gameMap = new Map();  // ��l�Ƥ@�ӷs�a��

		// Ū������ɮפ��e
		std::string content;
		std::string line;
		while (std::getline(loadFile, line)) {
			content += line + "\n";
		}
		loadFile.close();

		std::cout << "�wŪ���s�ɤ��A���b���R...\n";

		// �ѪR JSON - �o�̨ϥ�²�檺�r��B�z�覡
		// ������Τ���ĳ�ϥαM�~�� JSON �ѪR�w

		// �ȮɱN currentPlayerIdx �]�� 0�A������θѪR���~
		currentPlayerIdx = 0;

		// �ѪR��e���a����
		size_t currentPlayerIdxPos = content.find("\"currentPlayerIdx\":");
		if (currentPlayerIdxPos != std::string::npos) {
			size_t valueStart = content.find(":", currentPlayerIdxPos) + 1;
			size_t valueEnd = content.find(",", valueStart);
			std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
			valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
			currentPlayerIdx = std::stoi(valueStr);
		}

		// �ˬd���a�ƶq�A�T�O���T���J�Ҧ����a
		int expectedPlayerCount = 0;
		size_t playerCountPos = content.find("\"playerCount\":");
		if (playerCountPos != std::string::npos) {
			size_t valueStart = content.find(":", playerCountPos) + 1;
			size_t valueEnd = content.find(",", valueStart);
			std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
			valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
			expectedPlayerCount = std::stoi(valueStr);
			std::cout << "�w�����J " << expectedPlayerCount << " �W���a\n";
		}

		// �ϥΧ�²������k�ѪR���a���
		// ��� players �}�C���}�l�M������m
		size_t playersArrayStart = content.find("\"players\":");
		if (playersArrayStart == std::string::npos) {
			std::cerr << "�L�k��쪱�a��ư϶�\n";
			return false;
		}

		// �M�� players �}�C���}�l [
		size_t arrayStart = content.find("[", playersArrayStart);
		if (arrayStart == std::string::npos) {
			std::cerr << "�L�k��쪱�a��ư}�C�}�l�аO '['\n";
			return false;
		}

		// �M����������� ]�A�ݭn�p��O�M�h��
		size_t arrayEnd = arrayStart + 1;
		int bracketLevel = 1; // �w�g���@�� [

		while (bracketLevel > 0 && arrayEnd < content.length()) {
			if (content[arrayEnd] == '[') {
				bracketLevel++;
			}
			else if (content[arrayEnd] == ']') {
				bracketLevel--;
			}
			arrayEnd++;
		}

		if (bracketLevel != 0) {
			std::cerr << "�L�k���ǰt�� players �}�C�����аO ']'\n";
			return false;
		}

		// ������㪺 players �}�C���e
		std::string playersArray = content.substr(arrayStart + 1, arrayEnd - arrayStart - 2);
		std::cout << "���a��ư}�C���e�G\n" << playersArray << std::endl;

		// �ھڳr�����Φh�Ӫ��a�� JSON ��H
		// ���ݭn�`�N�O�M����H�M�}�C�A�����¥γr������
		int playerCount = 0;
		size_t pos = 0;

		std::cout << "�}�l�v�@�ѪR���a���...\n";

		// �v�ӸѪR�C�Ӫ��a�� JSON ��H
		while (pos < playersArray.length()) {
			// ���L�ť�
			while (pos < playersArray.length() &&
				(playersArray[pos] == ' ' || playersArray[pos] == '\n' ||
					playersArray[pos] == '\r' || playersArray[pos] == '\t')) {
				pos++;
			}

			// �ˬd�O�_��F����
			if (pos >= playersArray.length()) {
				break;
			}

			// �M�䪱�a��H�}�l�� {
			if (playersArray[pos] != '{') {
				std::cerr << "�L�k��쪱�a " << (playerCount + 1) << " ���}�l�аO '{'"
					<< " (��m " << pos << ", ��ڦr��: " << playersArray[pos] << ")\n";
				// ���ո��L�o�Ӱ��D�r���A�~��M��
				pos++;
				continue;
			}

			// ����H������ }�A�ݭn�p��O�M�h��
			size_t objStart = pos;
			size_t objEnd = objStart + 1;
			int braceLevel = 1; // �w�g���@�� {

			while (braceLevel > 0 && objEnd < playersArray.length()) {
				if (playersArray[objEnd] == '{') {
					braceLevel++;
				}
				else if (playersArray[objEnd] == '}') {
					braceLevel--;
				}
				objEnd++;
			}

			if (braceLevel != 0) {
				std::cerr << "���a " << (playerCount + 1) << " �� JSON �ѪR���~�G�j�A�����ǰt\n";
				break;
			}

			// ���o���㪺���a JSON ��H
			std::string playerJson = playersArray.substr(objStart, objEnd - objStart);
			std::cout << "���o���a " << (playerCount + 1) << " ��ơG���� " << playerJson.length() << " �r��\n";
			std::cout << "���a JSON: " << playerJson << "\n";

			// �ѪR���a�ݩ�
			std::string name, color;
			int money = 0, position = 0;
			bool inHospital = false;
			int hospitalDay = 0;

			// �W��
			size_t namePos = playerJson.find("\"name\":");
			if (namePos != std::string::npos) {
				size_t valueStart = playerJson.find("\"", namePos + 7) + 1;
				size_t valueEnd = playerJson.find("\"", valueStart);
				name = playerJson.substr(valueStart, valueEnd - valueStart);
			}
			else {
				std::cerr << "�L�k��쪱�a�W��\n";
				return false;
			}

			// �C��
			size_t colorPos = playerJson.find("\"color\":");
			if (colorPos != std::string::npos) {
				size_t valueStart = playerJson.find("\"", colorPos + 8) + 1;
				size_t valueEnd = playerJson.find("\"", valueStart);
				color = playerJson.substr(valueStart, valueEnd - valueStart);
			}
			else {
				std::cerr << "�L�k��쪱�a�C��\n";
				return false;
			}

			// ����
			size_t moneyPos = playerJson.find("\"money\":");
			if (moneyPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", moneyPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				money = std::stoi(valueStr);
			}
			else {
				std::cerr << "�L�k��쪱�a����\n";
				return false;
			}

			// ��m
			size_t positionPos = playerJson.find("\"position\":");
			if (positionPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", positionPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				position = std::stoi(valueStr);
			}
			else {
				std::cerr << "�L�k��쪱�a��m\n";
				return false;
			}

			// �O�_�b��|
			size_t inHospitalPos = playerJson.find("\"inHospital\":");
			if (inHospitalPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", inHospitalPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				inHospital = (valueStr.find("true") != std::string::npos);
			}

			// ��|�Ѽ�
			size_t hospitalDayPos = playerJson.find("\"hospitalDay\":");
			if (hospitalDayPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", hospitalDayPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				if (valueEnd == std::string::npos) {
					// �i��O�̫�@���ݩʡA�M���L�i�઺�����аO
					valueEnd = playerJson.find("}", valueStart);
					if (valueEnd == std::string::npos) {
						valueEnd = playerJson.length() - 1;
					}
				}
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				hospitalDay = std::stoi(valueStr);
			}

			std::cout << "���a���: �W��=" << name << ", �C��=" << color << ", ����=" << money
				<< ", ��m=" << position << ", ��|=" << (inHospital ? "�O" : "�_") << "\n";

			// �Ыت��a
			Player* player = new Player(name, color, money);
			player->SetPosition(position);
			player->inHospital = inHospital;
			player->hosipitalDay = hospitalDay;

			// �D��ѪR�Ȯɲ��L

			players.push_back(player);
			playerCount++;

			// ���ʨ�U�@�Ӫ��a����m
			pos = objEnd;

			// ���L�i�઺�r���M�ť�
			while (pos < playersArray.length() &&
				(playersArray[pos] == ',' || playersArray[pos] == ' ' ||
					playersArray[pos] == '\n' || playersArray[pos] == '\r' ||
					playersArray[pos] == '\t')) {
				pos++;
			}

			std::cout << "�U�@�ӸѪR��m: " << pos
				<< " (�r��: " << (pos < playersArray.length() ?
					std::string(1, playersArray[pos]) : "����") << ")\n";
		}

		// �ˬd�O�_���J�F���T�ƶq�����a
		if (expectedPlayerCount > 0 && playerCount != expectedPlayerCount) {
			std::cerr << "ĵ�i�G�w�����J " << expectedPlayerCount << " �W���a�A����ڥu���J�F "
				<< playerCount << " �W���a�I\n";
		}

		// �T�O currentPlayerIdx �b���Ľd��
		if (players.empty()) {
			std::cerr << "���J�C�����ѡG�S���o�{���a���\n";
			return false;
		}

		if (currentPlayerIdx < 0 || currentPlayerIdx >= static_cast<int>(players.size())) {
			std::cerr << "ĵ�i�G���J�����a���� " << currentPlayerIdx << " �L�ġA���]�� 0\n";
			currentPlayerIdx = 0; // ���]���Ĥ@�Ӫ��a
		}

		// �٭�a�Ϯ�l���֦���
		size_t tilesPos = content.find("\"tiles\":");
		if (tilesPos != std::string::npos) {
			size_t tilesStart = content.find("[", tilesPos);
			size_t tilesEnd = content.find("]", tilesStart);
			std::string tilesStr = content.substr(tilesStart, tilesEnd - tilesStart + 1);

			size_t pos = tilesStr.find("{");
			while (pos != std::string::npos) {
				size_t endPos = tilesStr.find("}", pos);
				std::string tileStr = tilesStr.substr(pos, endPos - pos + 1);

				// �ѪR��l�ݩ�
				int index = -1, ownerIdx = -1, level = 0;

				// ����
				size_t indexPos = tileStr.find("\"index\":");
				if (indexPos != std::string::npos) {
					size_t valueStart = tileStr.find(":", indexPos) + 1;
					size_t valueEnd = tileStr.find(",", valueStart);
					std::string valueStr = tileStr.substr(valueStart, valueEnd - valueStart);
					valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
					index = std::stoi(valueStr);
				}

				// �֦��̯���
				size_t ownerIdxPos = tileStr.find("\"ownerIdx\":");
				if (ownerIdxPos != std::string::npos) {
					size_t valueStart = tileStr.find(":", ownerIdxPos) + 1;
					size_t valueEnd = tileStr.find(",", valueStart);
					if (valueEnd == std::string::npos) {
						valueEnd = tileStr.find("\n", valueStart);
					}
					std::string valueStr = tileStr.substr(valueStart, valueEnd - valueStart);
					valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
					ownerIdx = std::stoi(valueStr);
				}

				// ����
				size_t levelPos = tileStr.find("\"level\":");
				if (levelPos != std::string::npos) {
					size_t valueStart = tileStr.find(":", levelPos) + 1;
					size_t valueEnd = tileStr.find(",", valueStart);
					if (valueEnd == std::string::npos) {
						valueEnd = tileStr.find("\n", valueStart);
					}
					std::string valueStr = tileStr.substr(valueStart, valueEnd - valueStart);
					valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
					level = std::stoi(valueStr);
				}

				// �]�w��l�֦���
				if (index >= 0 && index < gameMap->getSize() && ownerIdx >= 0 && ownerIdx < static_cast<int>(players.size())) {
					Tile* tile = gameMap->GetTileAt(index);
					PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile);
					if (propTile) {
						propTile->SetOwner(players[ownerIdx]);

						// �]�w����
						for (int i = 0; i < level; ++i) {
							propTile->Upgrade();
						}
					}
				}

				pos = tilesStr.find("{", endPos);
			}
		}

		// ���L���J�����a��T
		std::cout << "�w���\���J " << players.size() << " �W���a�G\n";
		for (size_t i = 0; i < players.size(); i++) {
			std::cout << (i + 1) << ". " << players[i]->GetName()
				<< " (����: " << players[i]->GetMoney() << ")\n";
		}

		std::cout << "�C���i�פw���\���J�I" << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "���J�C���ɵo�Ϳ��~�G" << e.what() << std::endl;
		loadFile.close();

		// �X���ɲM�z�í��m�C�����A
		for (Player* p : players) {
			delete p;
		}
		players.clear();

		if (gameMap) {
			delete gameMap;
			gameMap = new Map();
		}

		currentPlayerIdx = 0;
		return false;
	}
}

bool Game::HasSavedGame(const std::string& filename)
{
	std::ifstream file(filename);
	bool exists = file.good();
	file.close();
	return exists;
}

bool Game::DeleteSaveGame(const std::string& filename)
{
	if (std::remove(filename.c_str()) != 0) {
		std::cerr << "�R���s�ɥ��ѡG" << filename << std::endl;
		return false;
	}
	std::cout << "�s�ɤw�R���G" << filename << std::endl;
	return true;
}

// �W�[�B�z���a�}�������
void Game::HandlePlayerBankruptcy(Player* bankruptPlayer, Player* creditor)
{
	// �p�G�}�����a�N�O�Ȧ�A�Ϊ̶ťD�O�Ȧ�A������S��B�z
	if (bankruptPlayer->GetName() == "bank" || (creditor && creditor->GetName() == "bank")) {
		return;
	}

	std::cout << bankruptPlayer->GetName() << " �}���F�A�N�Ҧ��a���ಾ�� " << creditor->GetName() << "�I\n";

	// �M���a�ϴM��}�����a�֦����g�a
	auto mapData = gameMap->getMap();
	for (int i = 0; i < gameMap->getSize(); i++) {
		Tile* tile = gameMap->GetTileAt(i);
		PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile);

		// �p�G�O�}�����a���a���A�ಾ���ťD
		if (propTile && propTile->GetOwner() == bankruptPlayer) {
			std::cout << "�a���u" << mapData.second[i] << "�v�q " << bankruptPlayer->GetName()
				<< " �ಾ�� " << creditor->GetName() << "�C\n";
			propTile->SetOwner(creditor);
		}
	}

	// �ˬd�C���O�_���ӵ����]�u�Ѥ@�W���a�^
	if (CheckWinCondition()) {
		Monopoly::WaitForEnter();
	}
}

Player* Game::getCurrentPlayer() {
	return players[currentPlayerIdx];
}

// return false ���a�o�H�~���ʡAreturn true ���a�^�X����
bool Game::HandleHiddenCommand(const std::string& input) {
	std::istringstream iss(input);
	std::string cmd;
	iss >> cmd;

	Player* p = getCurrentPlayer();

	if (cmd == "/help" || cmd == "/list") {
		std::cout << "�i�Ϋ��O���G/move /get /give /card /gamestate /info /minigame /refresh\n";
	}
	else if (cmd == "/move") {
		int steps;
		if (iss >> steps) {
			std::cout << "�ϥΫ��O���� " << steps << " �B�C\n";
			p->Move(steps, gameMap->getSize());
			auto currentTile = gameMap->GetTileAt(p->GetPosition());
			currentTile->OnLand(p);
			return true;
		}
		else {
			std::cout << "�榡���~�I�Шϥ� /move [�B��]\n";
			Monopoly::WaitForEnter();
			return false;
		}
	}
	else if (cmd == "/get") {
	}
	else if (cmd == "/give") {
		std::vector<std::string> names;

		for (int i = 0; i < players.size(); i++) {
			std::string name = "���a" + std::string(1, 'A' + i) + ": " + players[i]->GetName();
			if (p == players[i])name += "�]���o��ܦۤv�^";
			names.push_back(name);
		}

		int choice = Monopoly::GetUserChoice("", names);
		Player* chosenP = p;

		switch (choice) {
		case 0:
			if (p == players[0]) {
				std::cout << "�����ܦۤv��I\n";
				Monopoly::WaitForEnter();
				return false;
			}
			chosenP = players[0];
			break;
		case 1:
			if (p == players[1]) {
				std::cout << "�����ܦۤv��I\n";
				Monopoly::WaitForEnter();
				return false;
			}
			chosenP = players[1];
			break;
		case 2:
			if (p == players[2]) {
				std::cout << "�����ܦۤv��I\n";
				Monopoly::WaitForEnter();
				return false;
			}
			chosenP = players[2];
			break;
		case 3:
			if (p == players[3]) {
				std::cout << "�����ܦۤv��I\n";
				Monopoly::WaitForEnter();
				return false;
			}
			chosenP = players[3];
			break;
		}

		int money;
		std::cout << "��J�������B�G";
		std::cin >> money;

		p->Pay(chosenP, money);

		std::cout << "������O�G�������a " << chosenP->GetName() << " " << to_string(money) << " ���C";

		Monopoly::WaitForEnter();
		return false;
	}
	else if (cmd == "/card") {
		// �ӫ~�C��
		std::vector<std::string> itemNames = {
			"�����l"
		};

		do {
			std::vector<std::string> goods;

			for (size_t i = 0; i < itemNames.size(); ++i) {
				goods.push_back(itemNames[i]);
			}

			goods.push_back("���}");

			int choice = Monopoly::GetUserChoice("", goods);

			// �ˬd�O�_������}
			if (choice == static_cast<int>(goods.size()) - 1) break;

			// �T�O���ަ���
			if (choice >= 0 && choice < itemNames.size()) {
				p->AddItem(new ControlDiceItem());
				std::cout << "���o���w�d���I\n";
			}

			Monopoly::WaitForEnter();
		} while (true);

		return false;
	}
	else if (cmd == "/gamestate") {
		gameOver = true;
	}
	else if (cmd == "/info") {

	}
	else if (cmd == "/minigame") {
		int game;
		if (iss >> game) {
			if (game == 1) {
				HorseRacing miniGame1;

				Monopoly::UpdateScreen();
				std::cout << "�N�~���i�䰨��\n";
				Monopoly::WaitForEnter();
				miniGame1.init(p);
				miniGame1.gameStart();

				return true;
			}
			else if (game == 2) {
				SheLongMen miniGame2;

				Monopoly::UpdateScreen();
				std::cout << "�N�~���i���\n";
				Monopoly::WaitForEnter();
				miniGame2.init(p);
				miniGame2.gameStart();

				return true;
			}
			else {
				std::cout << "�榡���~�I�Шϥ� /minigame [1 or 2]\n";
				Monopoly::WaitForEnter();
				return false;
			}
		}
		else {
			std::cout << "�榡���~�I�Шϥ� /minigame [1 or 2]\n";
			Monopoly::WaitForEnter();
			return false;
		}
	}
	else if (cmd == "/refresh") {
		Monopoly::UpdateScreen();
		return false;
	}
	else {
		std::cout << "�L�ī��O�G" << cmd << "\n";
		Monopoly::WaitForEnter();
		return false;
	}
}