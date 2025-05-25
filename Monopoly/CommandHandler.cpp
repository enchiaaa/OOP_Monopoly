#include "CommandHandler.h"
#include "Game.h"
#include "Monopoly.h"
#include "HorseRacing.h"
#include "SheLongMen.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "../nlohmann/json.hpp"

// Using nlohmann json
using json = nlohmann::json;

CommandHandler::CommandHandler() : game(nullptr) {
}

// Helper function to find a player by name
Player* CommandHandler::FindPlayerByName(const std::string& name) {
	if (!game) return nullptr;

	// Search through all players
	for (auto& player : game->players) {
		if (player->GetName() == name) {
			return player;
		}
	}

	// Not found
	return nullptr;
}

void CommandHandler::Initialize() {
	// Try multiple possible paths for command.json
	std::vector<std::string> possiblePaths = {
		"../json/command.json",
		"json/command.json",
		"../../json/command.json",
		"../../../json/command.json",
		"./json/command.json"
	};

	bool fileOpened = false;
	std::ifstream file;

	for (const auto& path : possiblePaths) {
		file.open(path);
		if (file.is_open()) {
			std::cout << "���\�}�ҫ��O��: " << path << std::endl;
			fileOpened = true;
			break;
		}
	}

	if (!fileOpened) {
		std::cerr << "���~: �䤣�� command.json!" << std::endl;
		return;
	}

	try {
		json commandData;
		file >> commandData;

		// Parse commands from JSON
		commands.clear();
		for (const auto& cmd : commandData["commands"]) {
			Command command;
			command.name = cmd["name"];
			command.description = cmd["description"];
			command.usage = cmd["usage"];
			command.example = cmd["example"];
			commands.push_back(command);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "�ѪR command.json �ɵo�Ϳ��~: " << e.what() << std::endl;
	}

	file.close();
}

bool CommandHandler::ProcessCommand(std::shared_ptr<Player> player, const std::string& input) {
	if (input.empty() || input[0] != '/') {
		return false;
	}

	// Split the command and its arguments
	std::vector<std::string> parts = SplitCommand(input);
	if (parts.empty()) {
		return false;
	}

	// Extract command name (without the /)
	std::string cmdName = parts[0].substr(1);
	if (cmdName.empty()) {
		return false;
	}

	// Remove command from arguments
	parts.erase(parts.begin());

	bool commandResult = false;

	// Find and execute the appropriate command handler
	if (cmdName == "move") {
		commandResult = HandleMoveCommand(player, parts);
	}
	else if (cmdName == "get") {
		commandResult = HandleGetCommand(player, parts);
	}
	else if (cmdName == "give") {
		commandResult = HandleGiveCommand(player, parts);
	}
	else if (cmdName == "card") {
		commandResult = HandleCardCommand(player, parts);
	}
	else if (cmdName == "gamestate") {
		commandResult = HandleGameStateCommand(player, parts);
	}
	else if (cmdName == "info") {
		commandResult = HandleInfoCommand(player, parts);
	}
	else if (cmdName == "refresh") {
		commandResult = HandleRefreshCommand(player, parts);
	}
	else if (cmdName == "list" || cmdName == "help") {
		commandResult = HandleListCommand(player, parts);
	}
	else if (cmdName == "minigame") {
		commandResult = HandleMinigameCommand(player, parts);
	}
	else {
		std::cout << "�������O: " << cmdName << std::endl;
		return false;
	}

	// If command was successful and game reference is valid, save the game state
	if (commandResult && game) {
		game->SaveGame();
		// std::cout << "�w�x�s�C���i��" << std::endl;
	}

	return commandResult;
}

std::vector<std::string> CommandHandler::GetCommandList(bool detailed) {
	std::vector<std::string> result;

	for (const auto& cmd : commands) {
		if (detailed) {
			std::string cmdInfo = "/" + cmd.name + " - " + cmd.description + "\n";
			cmdInfo += "    �Ϊk: " + cmd.usage + "\n";
			cmdInfo += "    �d��: " + cmd.example;
			result.push_back(cmdInfo);
		}
		else {
			result.push_back("/" + cmd.name + " - " + cmd.description);
		}
	}

	return result;
}

bool CommandHandler::IsValidCommand(const std::string& cmdName) {
	for (const auto& cmd : commands) {
		if (cmd.name == cmdName) {
			return true;
		}
	}
	return false;
}

void CommandHandler::SetGameReference(Game* gamePtr) {
	game = gamePtr;
}

std::vector<std::string> CommandHandler::SplitCommand(const std::string& input) {
	std::vector<std::string> result;
	std::istringstream iss(input);
	std::string token;

	while (std::getline(iss, token, ' ')) {
		if (!token.empty()) {
			result.push_back(TrimString(token));
		}
	}

	return result;
}

std::string CommandHandler::TrimString(const std::string& str) {
	auto start = std::find_if_not(str.begin(), str.end(), [](char c) { return std::isspace(c); });
	auto end = std::find_if_not(str.rbegin(), str.rend(), [](char c) { return std::isspace(c); }).base();
	return (start < end) ? std::string(start, end) : std::string();
}

// ================================
// ���O�B�z�϶�
// ================================

bool CommandHandler::HandleMoveCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	if (args.empty()) {
		std::cout << "�Ϊk: /move <��m>" << std::endl;
		return false;
	}

	try {
		int position;
		if (args[0] == "to" && args.size() > 1) {
			// �z�L�a�W�M���l
			std::string tileName;
			for (size_t i = 1; i < args.size(); ++i) {
				tileName += args[i];
				if (i < args.size() - 1) {
					tileName += " ";
				}
			}

			// ���o�a�ϸ��
			std::vector<Tile*> tiles = game->gameMap->getMap().first;
			std::vector<std::string> names = game->gameMap->getMap().second;

			auto it = std::find(names.begin(), names.end(), tileName);
			if (it == names.end()) {
				std::cout << "�䤣���l: " << tileName << std::endl;
				return false;
			}
			position = static_cast<int>(std::distance(names.begin(), it));
		}
		else {
			position = std::stoi(args[0]);
			if (position < 0 || position >= game->gameMap->getSize()) {
				std::cout << "�L�Ħ�m: " << position << std::endl;
				return false;
			}
		}

		player->SetPosition(position);
		std::cout << player->GetName() << " ���ʨ��m " << position << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cout << "�ѪR��m�ɵo�Ϳ��~: " << e.what() << std::endl;
		return false;
	}
}

bool CommandHandler::HandleGetCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	if (args.empty()) {
		std::cout << "�Ϊk: /get <���B> �� /get <���a> <���B>" << std::endl;
		return false;
	}

	try {
		if (args.size() == 1) {
			int amount = std::stoi(args[0]);
			if (amount <= 0) {
				std::cout << "���B�����j�� 0!" << std::endl;
				return false;
			}
			player->BuyProperty(-amount);
			std::cout << player->GetName() << " �q�Ȧ���o $" << amount << std::endl;
			return true;
		}
		else if (args.size() == 2) {
			std::string targetPlayerName = args[0];
			int amount = std::stoi(args[1]);

			if (amount <= 0) {
				std::cout << "���B�����j�� 0!" << std::endl;
				return false;
			}

			Player* targetPlayer = nullptr;
			if (targetPlayerName == "bank") {
				targetPlayer = &Monopoly::bank;
			}
			else {
				targetPlayer = FindPlayerByName(targetPlayerName);
			}

			if (!targetPlayer) {
				std::cout << "�䤣�쪱�a: " << targetPlayerName << std::endl;
				return false;
			}

			targetPlayer->BuyProperty(-amount);
			std::cout << targetPlayerName << " �q�Ȧ���o $" << amount << std::endl;
			return true;
		}
		else {
			std::cout << "�Ϊk: /get <���B> �� /get <���a> <���B>" << std::endl;
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cout << "�ѪR���B�ɵo�Ϳ��~: " << e.what() << std::endl;
		return false;
	}
}

bool CommandHandler::HandleGiveCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	if (args.size() < 2) {
		std::cout << "�Ϊk: /give <���a> <���B>" << std::endl;
		return false;
	}

	try {
		std::string targetPlayerName = args[0];
		int amount = std::stoi(args[1]);

		if (amount <= 0) {
			std::cout << "���B�����j�� 0!" << std::endl;
			return false;
		}

		if (player->GetMoney() < amount) {
			std::cout << "���B����!" << std::endl;
			return false;
		}

		Player* targetPlayer = nullptr;
		if (targetPlayerName == "bank") {
			targetPlayer = &Monopoly::bank;
		}
		else {
			targetPlayer = FindPlayerByName(targetPlayerName);
		}

		if (!targetPlayer) {
			std::cout << "�䤣�쪱�a: " << targetPlayerName << std::endl;
			return false;
		}

		player->Pay(targetPlayer, amount);
		std::cout << player->GetName() << " ���� " << targetPlayerName << " $" << amount << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cout << "�ѪR���B�ɵo�Ϳ��~: " << e.what() << std::endl;
		return false;
	}
}

bool CommandHandler::HandleCardCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	if (args.empty()) {
		std::cout << "�Ϊk: /card <�d�P�W��>" << std::endl;
		std::cout << "�i�Υd�P: �����l, ���b�d, �R�B�d" << std::endl;
		return false;
	}

	std::string cardName;
	for (size_t i = 0; i < args.size(); ++i) {
		cardName += args[i];
		if (i < args.size() - 1) {
			cardName += " ";
		}
	}
	std::transform(cardName.begin(), cardName.end(), cardName.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (cardName == "control dice" || cardName == "�����l" || cardName == "control") {
		player->AddItem(new ControlDiceItem());
		std::cout << player->GetName() << " ��o�d�P: �����l" << std::endl;
		return true;
	}
	else if (cardName == "rocket card" || cardName == "���b�d" || cardName == "rocket") {
		player->AddItem(new RocketCard());
		std::cout << player->GetName() << " ��o�d�P: ���b�d" << std::endl;
		return true;
	}
	else if (cardName == "fate card" || cardName == "�R�B�d" || cardName == "fate") {
		player->AddItem(new FateCard());
		std::cout << player->GetName() << " ��o�d�P: �R�B�d" << std::endl;
		return true;
	}
	else {
		std::cout << "�����d�P: " << cardName << std::endl;
		std::cout << "�i�Υd�P: �����l, ���b�d, �R�B�d" << std::endl;
		return false;
	}
}

bool CommandHandler::HandleGameStateCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	if (args.empty()) {
		std::cout << "�Ϊk: /gamestate <���A>" << std::endl;
		std::cout << "�i�Ϊ��A: moved, finish" << std::endl;
		return false;
	}

	std::string stateStr = args[0];

	if (stateStr == "end" || stateStr == "finish" || stateStr == "over") {
		game->gameOver = true;
		std::cout << "�C���w�аO������! �U�^�X�N��̲ܳ׵��G�C" << std::endl;
		return true;
	}
	else if (stateStr == "moved") {
		auto currentTile = game->gameMap->GetTileAt(player->GetPosition());
		currentTile->OnLand(game->getCurrentPlayer());
		return true;
	}
	else {
		std::cout << "�������A: " << stateStr << std::endl;
		std::cout << "�i�Ϊ��A: moved, round_end, finish" << std::endl;
		return false;
	}
}

bool CommandHandler::HandleInfoCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	std::cout << "+------------+--------+-------+------------+--------------------------+\n";
	std::cout << "| Player     | Money  | Pos   | Status     | Items                    |\n";
	std::cout << "+------------+--------+-------+------------+--------------------------+\n";

	const std::string RESET = "\033[0m";

	for (int i = 0; i < game->players.size(); i++) {
		std::string colorCode = Monopoly::GetColorCode(game->players[i]->GetColor());
		std::cout << colorCode;

		std::string status;
		if (game->players[i]->inHospital) {
			status = "��| (" + std::to_string(game->players[i]->hosipitalDay) + ")";
		}
		else {
			status = "���`";
		}

		std::string itemStr;
		if (game->players[i]->GetItem().empty()) {
			itemStr = "�L";
		}
		else {
			for (size_t j = 0; j < game->players[i]->GetItem().size(); j++) {
				itemStr += game->players[i]->GetItem()[j]->GetName();
				if (j != game->players[i]->GetItem().size() - 1)
					itemStr += ", ";
			}
		}

		std::cout << RESET << "| "
			<< colorCode << "[" + std::string(1, '1' + i) + "] "
			<< RESET << std::left << std::setw(6) << game->players[i]->GetName()
			<< " | " << std::right << std::setw(6) << game->players[i]->GetMoney()
			<< " | " << std::right << std::setw(5) << game->players[i]->GetPosition()
			<< " | " << std::left << std::setw(10) << status
			<< " | " << std::left << std::setw(24) << itemStr
			<< " |\n";

		std::cout << "\033[0m";
	}
	std::cout << "+------------+--------+-------+------------+--------------------------+\n";

	return true;
}

bool CommandHandler::HandleRefreshCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	game->Clear();
	std::cout << "�a�Ϥw���sø�s�C" << std::endl;

	return true;
}

bool CommandHandler::HandleListCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	bool detailed = false;

	if (!args.empty() && args[0] == "-a") {
		detailed = true;
	}

	std::cout << "�i�Ϋ��O:" << std::endl;
	if (detailed) {
		std::cout << "/move - ���ʪ��a�ܫ��w��m�]�䴩��l�W�٩μƦr�^" << std::endl;
		std::cout << "    �Ϊk: /move <��m>" << std::endl;
		std::cout << "    �d��: /move 5�B/move to USA" << std::endl;

		std::cout << "/get - �q�t�Ψ��o�����]�ۤv�Ϋ��w���a�^" << std::endl;
		std::cout << "    �Ϊk: /get <���B> �� /get <���a> <���B>" << std::endl;
		std::cout << "    �d��: /get 1000�B/get Bob 500" << std::endl;

		std::cout << "/give - �����w���a�����]�q�ۤv�����^" << std::endl;
		std::cout << "    �Ϊk: /give <���a> <���B>" << std::endl;
		std::cout << "    �d��: /give Alice 300" << std::endl;

		std::cout << "/card - ���o���w�W�٪��d�P" << std::endl;
		std::cout << "    �Ϊk: /card <�d�P�W��>" << std::endl;
		std::cout << "    �d��: /card Rocket Card" << std::endl;

		std::cout << "/gamestate - �j����ܷ�e�C�����A" << std::endl;
		std::cout << "    �Ϊk: /gamestate <���A>" << std::endl;
		std::cout << "    �d��: /gamestate finish" << std::endl;

		std::cout << "/info - ��ܩҦ����a��T" << std::endl;
		std::cout << "    �Ϊk: /info" << std::endl;
		std::cout << "    �d��: /info" << std::endl;

		std::cout << "/refresh - �j��sø�s�a��" << std::endl;
		std::cout << "    �Ϊk: /refresh" << std::endl;
		std::cout << "    �d��: /refresh" << std::endl;

		std::cout << "/list - ��ܩҦ��i�Ϋ��O�]�i�[ -a ��ܥΪk�^" << std::endl;
		std::cout << "    �Ϊk: /list [-a]" << std::endl;
		std::cout << "    �d��: /list -a" << std::endl;

		std::cout << "/help - ��ܩҦ��i�Ϋ��O�]�i�[ -a ��ܥΪk�^" << std::endl;
		std::cout << "    �Ϊk: /help [-a]" << std::endl;
		std::cout << "    �d��: /help -a" << std::endl;

		std::cout << "/minigame - �����i�J�p�C�����" << std::endl;
		std::cout << "    �Ϊk: /minigame" << std::endl;
		std::cout << "    �d��: /minigame" << std::endl;

		std::cout << "\n�i�Υd�P����:" << std::endl;
		std::cout << "1. �����l - �i�H���w�ۤv����l�I��" << std::endl;
		std::cout << "2. ���b�d - �����w���a��|��^�X" << std::endl;
		std::cout << "3. �R�B�d - �j��Ĳ�o�R�B�ƥ�" << std::endl;

		std::cout << "\n�i�ϥιC�����A:" << std::endl;
		std::cout << "1. moved - Ĳ�o���d�a�������ƥ�" << std::endl;
		std::cout << "2. finish - �C���j���" << std::endl;

	}
	else {
		std::cout << "/move - ���ʪ��a�ܫ��w��m�]�䴩��l�W�٩μƦr�^" << std::endl;
		std::cout << "/get - �q�t�Ψ��o�����]�ۤv�Ϋ��w���a�^" << std::endl;
		std::cout << "/give - �����w���a�����]�q�ۤv�����^" << std::endl;
		std::cout << "/card - ���o���w�W�٪��d�P" << std::endl;
		std::cout << "/gamestate - �j����ܷ�e�C�����A" << std::endl;
		std::cout << "/info - ��ܩҦ����a��T" << std::endl;
		std::cout << "/refresh - �j��sø�s�a��" << std::endl;
		std::cout << "/list - ��ܩҦ��i�Ϋ��O�]�i�[ -a ��ܥΪk�^" << std::endl;
		std::cout << "/help - ��ܩҦ��i�Ϋ��O�]�i�[ -a ��ܥΪk�^" << std::endl;
		std::cout << "/minigame - �����i�J�p�C�����" << std::endl;
	}

	return true;
}

bool CommandHandler::HandleMinigameCommand(std::shared_ptr<Player> player, const std::vector<std::string>& args) {
	if (!game) {
		std::cout << "�C���|����l��!" << std::endl;
		return false;
	}

	std::vector<std::string> gameOptions = {
		"�ɰ�",
		"�g�s��",
		"��^"
	};

	std::cout << "��ܭn�C�����g�A�C��:" << std::endl;
	for (size_t i = 0; i < gameOptions.size(); ++i) {
		std::cout << (i + 1) << ". " << gameOptions[i] << std::endl;
	}

	int choice = -1;
	while (choice < 0 || choice >= static_cast<int>(gameOptions.size())) {
		std::cout << "��J�ﶵ�s�� (1-" << gameOptions.size() << "): ";
		std::string input;
		std::getline(std::cin, input);

		try {
			choice = std::stoi(input) - 1;
		}
		catch (...) {
			choice = -1;
		}
	}

	if (choice == 0) {
		HorseRacing miniGame;
		miniGame.init(player.get());
		miniGame.gameStart();
		return true;
	}
	else if (choice == 1) {
		SheLongMen miniGame;
		miniGame.init(player.get());
		miniGame.gameStart();
		return true;
	}
	else if (choice == 2) {
		std::cout << "��^�D���" << std::endl;
		return false;
	}

	return false;
}
