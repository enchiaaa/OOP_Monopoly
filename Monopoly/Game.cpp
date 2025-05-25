#include "Game.h"
#include "Map.h"
#include "Monopoly.h"
#include "HorseRacing.h"
#include "SheLongMen.h"
#include "CommandHandler.h"
#include "Item.h"

#include "../nlohmann/json.hpp"

#include <cstdlib>
#include <ctime>
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
	std::vector<std::string> possiblePaths = {
		"../json/configs.json",
		"json/configs.json",
		"../../json/configs.json",
		"../../../json/configs.json",
		"./json/configs.json"
	};

	std::ifstream file;

	for (const auto& path : possiblePaths) {
		file.open(path);
		if (file) break;
	}

	if (!file) {
		std::cerr << "���~: �䤣�� configs.json!" << std::endl;
		return;
	}
	nlohmann::json data;
	file >> data;
	file.close();

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
	int money, win_money;
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

		Player* p;
		if (data["init"]["money"].is_number()) {
			p = new Player(name, colors[i], data["init"]["money"]);
			money = data["init"]["money"];
		}
		else {
			p = new Player(name, colors[i], 1000);
		}
		players.push_back(p);
	}

	std::string itemName("");
	for (auto& a : data["init"]["items"].items()) {
		itemName = a.key();
		if (itemName != "") {
			if (itemName == u8"�����l") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new ControlDiceItem());
				}
			}
			else if (itemName == u8"�R�B�d") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new FateCard());
				}
			}
			else if (itemName == u8"���b�d") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new RocketCard());
				}
			}
			else if (itemName == u8"�R���d") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new DestroyCard());
				}
			}
		}
	}

	if (data["init"]["winMoney"].is_number())win_money = data["init"]["winMoney"];
	winMoney = win_money;

	std::cout << "\n�C����l�Ƨ����A�@ " << players.size() << " �쪱�a�C" << std::endl;
	std::cout << "�]�w�_�l����G" << std::to_string(money) << std::endl;
	std::cout << "�ӧQ���B�G" << std::to_string(winMoney) << std::endl;

	// Clear(); // �b���a�]�w������M���e������ܪ�l���A
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
	if (currentPlayer->GetMoney() < 0) {
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
			<< currentPlayer->hospitalDay << " �ѡC" << std::endl;

		if (currentPlayer->hospitalDay == 0) {
			std::cout << "\n" << currentPlayer->GetName() << " �w�X�|�I\n";
			currentPlayer->inHospital = false;
			currentPlayer->hospitalDay = 0;
		}
		else {
			// ��ܥX�|�ﶵ
			std::vector<std::string> hospitalOptions = {
				"�Y��l�]�Y�X 4 �I�ΥH�W�Y�i�X�|�^",
				"��I $50 �X�|�O",
				"�ĨĦb��|��i"
			};

			std::string hospitalPrompt = "���� " + currentPlayer->GetName() + " ���^�X�C" + "\n�A���b��|��i�A��ܦ�ʡG";
			int hospitalChoice = Monopoly::GetUserChoice(hospitalPrompt, hospitalOptions, true, false);

			if (hospitalChoice == 0) {
				// �Y����եX�|
				int roll = RollDiceWithAsciiAnimation();
				if (roll >= 4) {
					// ���\�X�|
					std::cout << "�Y�X�F " << roll << " �I�A���\�X�|�I" << std::endl;
					currentPlayer->inHospital = false;
					currentPlayer->hospitalDay = 0;
				}
				else {
					// ���ѡA�~���|
					std::cout << "�Y�X�F " << roll << " �I�A���F��4�I�A���~���|�C" << std::endl;
					currentPlayer->hospitalDay--;
					playerTurnCompleted = true;
				}
			}
			else if (hospitalChoice == 1) {
				// ��I�X�|�O
				if (currentPlayer->GetMoney() >= 50) {
					currentPlayer->BuyProperty(50); // ����50���X�|�O
					std::cout << currentPlayer->GetName() << " ��I�F$50�X�|�O�A���e�X�|�I" << std::endl;
					currentPlayer->inHospital = false;
					currentPlayer->hospitalDay = 0;
				}
				else {
					std::cout << "�A�S������������I�X�|�O�I" << std::endl;
					// �W�[��|�Ѽƨø��L�^�X
					currentPlayer->hospitalDay--;
					playerTurnCompleted = true;
				}
			}
			else {
				// ����~���|
				std::cout << currentPlayer->GetName() << " ����~��b��|��i�C" << std::endl;
				// ��֦�|�Ѽƨø��L�^�X
				currentPlayer->hospitalDay--;
				playerTurnCompleted = true;
			}
		}
	}

	while (!playerTurnCompleted) {
		std::string question = "���� " + currentPlayer->GetName() + " ���^�X�C\n�ثe���B: $" + std::to_string(currentPlayer->GetMoney());
		std::vector<std::string> options = {
			"�i���Y��",
			"�ϥιD��",
		};
		// �ϥ�true�Ѽƪ����ܦa��
		int choice = Monopoly::GetUserChoice(question, options, true, true);

		bool passStart = false;
		Tile* start = NULL;
		if (choice == 0) {
			// �����i���Y��A������B�~����
			int roll = RollDiceWithAsciiAnimation();
			for (int i = 0; i < roll; ++i) {
				currentPlayer->Move(1, gameMap->getSize());
				if (i != roll - 1 && gameMap->GetTileAt(currentPlayer->GetPosition())->GetName() == "start") {
					passStart = true;
					start = gameMap->GetTileAt(currentPlayer->GetPosition());
				}
				Clear();
				Monopoly::sleepMS(100);
			}
			int pos = currentPlayer->GetPosition();
			std::cout << currentPlayer->GetName() << " ���ʨ�� " << pos << " ��C\n";

			if (passStart) {
				start->OnLand(currentPlayer);
			}
			Monopoly::WaitForEnter();
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
				system("cls"); // �M���ù�
				PrintMapStatus();
				PrintPlayerStatus();
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
					if (currentPlayer->UseItem(selectedItem)) {
						// �ˬd�ϥιD���O�_�ݭn�i���L�ʧ@
						// �p�G�O�����l�D��A���a�w�g���ʦ�m�A���P�����^�X
						if (selectedItem->GetName() == "Control Dice") {
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
	}

	Monopoly::WaitForEnter();
	Monopoly::UpdateScreen();

	// �ˬd���a�O�_�}��
	if (currentPlayer->GetMoney() < 0) {
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
		// �u�����B�p��0�~��}���A���B��0����}��
		if (players[i]->GetMoney() >= 0) {
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
		if (players[i]->GetMoney() >= winMoney) {
			std::cout << "\n" << players[i]->GetName() << " Ĺ�o�ӧQ�I" << std::endl;
			gameOver = true;
			return true;
		}
	}

	return false;
}

void Game::EndGame()
{
	std::cout << "\n�C�������I" << std::endl;
	std::cout << "+====================�C�����G====================+" << std::endl;
	std::cout << "+-------+------------+-----------+---------------+" << std::endl;
	std::cout << "| �ƦW  | ���a�W��   | �̲׸��  | ���A          |" << std::endl;
	std::cout << "+-------+------------+-----------+---------------+" << std::endl;

	// �إߪ��a���ަC��îھڪ����Ƨ�
	std::vector<std::pair<int, Player*>> playerRanking;
	for (size_t i = 0; i < players.size(); i++) {
		playerRanking.push_back(std::make_pair(i, players[i]));
	}

	// �ھڪ����Ѱ���C�Ƨ�
	std::sort(playerRanking.begin(), playerRanking.end(),
		[](const std::pair<int, Player*>& a, const std::pair<int, Player*>& b) {
			return a.second->GetMoney() > b.second->GetMoney();
		});

	// ��X���a�ƦW�P��T
	for (size_t i = 0; i < playerRanking.size(); i++) {
		Player* p = playerRanking[i].second;
		std::string colorCode = Monopoly::GetColorCode(p->GetColor());
		std::string status = (p->GetMoney() < 0) ? "�}��" : (i == 0) ? "�ӧQ" : "�C������";

		std::cout << "| " << std::left << std::setw(5) << (i + 1)
			<< " | " << colorCode << std::left << std::setw(10) << p->GetName() << "\033[0m"
			<< " | " << std::right << std::setw(9) << p->GetMoney()
			<< " | " << std::left << std::setw(13) << status << " |" << std::endl;
	}

	std::cout << "+-------+------------+-----------+---------------+" << std::endl;
	std::cout << "\n���� " << playerRanking[0].second->GetName() << " ��o�C���ӧQ�I" << std::endl;

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
	std::cout << "+------------+--------+-------+------------+--------------------------+--------------------------+\n";
	std::cout << "| ���a�W��   | ���   | ��m  | ���A       | �֦��D��                 | �֦��a��                 |\n";
	std::cout << "+------------+--------+-------+------------+--------------------------+--------------------------+\n";

	const std::string RESET = "\033[0m";

	for (int i = 0; i < players.size(); i++) {
		std::string colorCode = Monopoly::GetColorCode(players[i]->GetColor());
		std::cout << colorCode;

		std::string name = "[" + std::string(1, '1' + i) + "]" + players[i]->GetName();
		std::string status;

		if (players[i]->inHospital) {
			status = "��|�� (" + std::to_string(players[i]->hospitalDay) + ")";
		}
		else {
			status = "���`";
		}

		// �D��r��
		std::string itemStr = players[i]->GetItem().empty() ? "�L" : "";
		for (size_t j = 0; j < players[i]->GetItem().size(); j++) {
			if (players[i]->GetItem()[j]->GetName() == "Control Dice") {
				itemStr += "[��]";
			}
			else if (players[i]->GetItem()[j]->GetName() == "Rocket Card") {
				itemStr += "[��]";
			}
			else if (players[i]->GetItem()[j]->GetName() == "Fate Card") {
				itemStr += "[�R]";
			}
			else if (players[i]->GetItem()[j]->GetName() == "Destroy Card") {
				itemStr += "[��]";
			}


			if (j != players[i]->GetItem().size() - 1)
				itemStr += ", ";
		}

		// �a��
		std::string propertyStr = players[i]->GetProperty().empty() ? "�L" : "";
		vector<int>tmp;
		for (size_t j = 0; j < players[i]->GetProperty().size(); j++) {
			tmp.push_back(players[i]->GetProperty()[j]->number);
		}
		sort(tmp.begin(), tmp.end());
		for (size_t j = 0; j < tmp.size(); j++) {
			propertyStr += to_string(tmp[j]);
			if (j != tmp.size() - 1)
				propertyStr += ", ";
		}

		// ��X�C��
		std::cout << RESET << "| "
			<< colorCode << "[" + std::string(1, '1' + i) + "] "
			<< RESET << std::left << std::setw(6) << players[i]->GetName()
			<< " | " << std::right << std::setw(6) << players[i]->GetMoney()
			<< " | " << std::right << std::setw(5) << players[i]->GetPosition()
			<< " | " << std::left << std::setw(10) << status
			<< " | " << std::left << std::setw(24) << itemStr
			<< " | " << std::left << std::setw(24) << propertyStr
			<< " |\n";

		std::cout << "\033[0m";
	}

	std::cout << "+------------+--------+-------+------------+--------------------------+--------------------------+\n";
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
			saveFile << "      \"hospitalDay\": " << p->hospitalDay << ",\n";

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
			std::vector<Item*>items;

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

			// �D��
			size_t itemsPos = playerJson.find("\"items\":");
			if (itemsPos != std::string::npos) {
				size_t listStart = playerJson.find("[", itemsPos);
				size_t listEnd = playerJson.find("]", listStart);
				std::string itemList = playerJson.substr(listStart, listEnd - listStart + 1);

				size_t itemNamePos = 0;
				while ((itemNamePos = itemList.find("\"name\":", itemNamePos)) != std::string::npos) {
					size_t valueStart = itemList.find("\"", itemNamePos + 7) + 1;
					size_t valueEnd = itemList.find("\"", valueStart);
					std::string itemName = itemList.substr(valueStart, valueEnd - valueStart);

					Item* newItem = NULL;
					if (itemName == "Control Dice") newItem = new ControlDiceItem();
					if (itemName == "Rocket Card") newItem = new RocketCard();
					if (itemName == "Fate Card") newItem = new FateCard();
					if (itemName == "Destroy Card") newItem = new DestroyCard();

					if (newItem) {
						items.push_back(newItem);
					}
					else {
						std::cerr << "�L�k���ѹD��W��: " << itemName << "\n";
					}

					itemNamePos = valueEnd;
				}
			}


			std::cout << "���a���: �W��=" << name << ", �C��=" << color << ", ����=" << money
				<< ", ��m=" << position << ", ��|=" << (inHospital ? "�O" : "�_") << "\n";

			// �Ыت��a
			Player* player = new Player(name, color, money);
			player->SetPosition(position);
			player->inHospital = inHospital;
			player->hospitalDay = hospitalDay;
			for (int i = 0; i < items.size(); i++) {
				player->AddItem(items[i]);
			}

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

						players[ownerIdx]->AddProperty(propTile);
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
	if (currentPlayerIdx >= 0 && currentPlayerIdx < static_cast<int>(players.size())) {
		return players[currentPlayerIdx];
	}
	return nullptr;
}

bool Game::processCommand(std::shared_ptr<Player> player, const std::string& input) {
	if (input.empty() || input[0] != '/') {
		return false;
	}

	// The first time we process a command, initialize the command handler
	static bool initialized = false;
	if (!initialized) {
		commandHandler.Initialize();
		commandHandler.SetGameReference(this);
		initialized = true;
	}

	return commandHandler.ProcessCommand(player, input);
}