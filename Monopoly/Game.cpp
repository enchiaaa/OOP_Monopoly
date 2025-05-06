#include "Game.h"
#include "Map.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include <chrono>
#include <conio.h>

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
	srand(static_cast<unsigned int>(time(nullptr)));
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

	int choice = GetUserChoice(question, options);

	playerNum = choice + 2;

	Clear();
	std::cout << "## ��J���a�]�w ##\n";
	for (int i = 0; i < playerNum; i++) {
		std::string name;
		std::cout << "\n���a " << i + 1 << " ���W�١G";
		std::cin >> name;
		Player* p = new Player(name, colors[i], 1000);
		players.push_back(p);
	}


	std::cout << "\n�C����l�Ƨ����A�@ " << players.size() << " �쪱�a�C" << std::endl;

	Clear(); // �b���a�]�w������M���e������ܪ�l���A
	WaitForEnter();
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
	auto currentPlayer = players[currentPlayerIdx];
	
	// �p�G��e���a�w�}���A����U�@��
	if (currentPlayer->GetMoney() <= 0) {
		// ���U�@��
		currentPlayerIdx = (currentPlayerIdx + 1) % static_cast<int>(players.size());
		return;
	}
	
	std::cout << "\n���� " << currentPlayer->GetName() << " ���^�X�C" << std::endl;
	if (currentPlayer->inHospital) {
		if (currentPlayer->hosipitalDay >= 3) {
			std::cout << "\n�X�|�F�I";
		}
		else {
			std::cout << "\n��J r �i���Y��A�Y�I�Ƭ� 6 �h���X�|�I";
		}
	}

	std::string question = "���� " + currentPlayer->GetName() + " ���^�X�C\n";
	std::vector<std::string> options = {
		"�i���Y��",
		"�ϥιD��"
	};
	int choice = GetUserChoice(question, options);

	if (choice == 0) {
		int roll = RollDiceWithAsciiAnimation();
		currentPlayer->Move(roll, gameMap->getSize());
		int pos = currentPlayer->GetPosition();
		std::cout << currentPlayer->GetName() << " ���ʨ�� " << pos << " ��C\n";

		auto currentTile = gameMap->GetTileAt(pos);

		currentTile->OnLand(currentPlayer);
	}
	else if (choice == 1) {
		// �ϥιD�㪺�޿�
	}

	WaitForEnter();

	// ���U�@��
	currentPlayerIdx = (currentPlayerIdx + 1) % static_cast<int>(players.size());
}

bool Game::CheckWinCondition()
{
	// �p�⤴�b�C���������a��
	int activePlayers = 0;
	int lastPlayerIdx = -1;
	
	for (size_t i = 0; i < players.size(); i++) {
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
}

void Game::HandlePlayerAction(Player* p)
{
}

void Game::Clear()
{
	std::cout << "\033[2J\033[H"; // �M���e��
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
	for (size_t i = 0; i < players.size(); i++) {
		std::string colorCode = GetColorCode(players[i]->GetColor());
		std::cout << colorCode;
		std::cout << "���a�W�١G" << players[i]->GetName() << '\t' << "����G" << players[i]->GetMoney() << '\n';
		std::cout << "\033[0m";
	}
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
		std::this_thread::sleep_for(std::chrono::milliseconds(75));
		finalRoll = d1 + d2;
	}

	std::cout << "�Y�뵲�G�O " << finalRoll << " �I�I" << std::endl;
	return finalRoll;
}


int Game::GetUserChoice(const std::string question, const std::vector<std::string> options) {

	int selected = 0;

	while (true) {
		Clear();
		std::cout << question << std::endl;
		for (size_t i = 0; i < options.size(); ++i) {
			if (static_cast<int>(i) == selected)
				std::cout << " > " << "�i" << options[i] << "�j" << "\n";
			else
				std::cout << "   " << "�i" << options[i] << "�j" << "\n";
		}

		int key = _getch(); // ���o����
		if (key == 224) {   // �S�����
			key = _getch();
			if (key == 72) selected = (selected - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size()); // �W
			if (key == 80) selected = (selected + 1) % static_cast<int>(options.size()); // �U
		}
		else if (key == '\r') { // Enter
			return selected;
		}
	}
}

void Game::WaitForEnter()
{
	std::cout << "\n���U Enter ���~��...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Game::GetColorCode(std::string colorName) {
	if (colorName == "red")		return "\033[31m";
	if (colorName == "green")	return "\033[32m";
	if (colorName == "yellow")	return "\033[33m";
	if (colorName == "blue")	return "\033[34m";
	return "\033[0m"; // �w�]
}

std::string Game::GetBackgroundColorCode(std::string color) {
	if (color == "red")        return "\033[41;37m";
	if (color == "green")      return "\033[42;37m";
	if (color == "yellow")     return "\033[43;30m";
	if (color == "blue")       return "\033[44;37m";
	if (color == "cyan")       return "\033[46;30m";
	if (color == "purple")     return "\033[48;2;128;0;128m\033[37m";
	if (color == "orange")     return "\033[48;2;255;140;0m\033[37m";
	if (color == "teal")       return "\033[48;2;0;128;128m\033[37m";
	if (color == "brown")      return "\033[48;2;139;69;19m\033[37m";
	if (color == "gray")   return "\033[100;37m";
	return "\033[0m"; // �w�]
}

