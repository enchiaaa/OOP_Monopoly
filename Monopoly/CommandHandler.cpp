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

#include <windows.h>

std::string utf8ToBig5(const std::string& utf8_str) {
	// UTF-8 �� UTF-16
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
	std::wstring wstr(wlen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wlen);

	// UTF-16 �� Big5
	int blen = WideCharToMultiByte(950 /* Big5 code page */, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string big5_str(blen, 0);
	WideCharToMultiByte(950, 0, wstr.c_str(), -1, &big5_str[0], blen, nullptr, nullptr);

	return big5_str;
}

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
	std::map<std::string, std::function<bool(std::shared_ptr<Player> player, const std::vector<std::string>& args)>> handlers;

	handlers["move"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleMoveCommand(player, args); };
	handlers["get"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleGetCommand(player, args); };
	handlers["give"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleGiveCommand(player, args); };
	handlers["card"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleCardCommand(player, args); };
	handlers["gamestate"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleGameStateCommand(player, args); };
	handlers["info"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleInfoCommand(player, args); };
	handlers["refresh"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleRefreshCommand(player, args); };
	handlers["help"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleListCommand(player, args); };
	handlers["list"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleListCommand(player, args); };
	handlers["minigame"] = [this](std::shared_ptr<Player> player, const std::vector<std::string>& args) {return HandleMinigameCommand(player, args); };


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
			command.description = utf8ToBig5(cmd["description"]);
			command.usage = utf8ToBig5(cmd["usage"]);
			command.example = utf8ToBig5(cmd["example"]);
			if (handlers.find(command.name) == handlers.end()) {
				command.handler = [command](std::shared_ptr<Player> player, const std::vector<std::string>& args) {
					std::cout << command.name << "���O�ʤ�Handler\n";
					return true;
				};
			}
			else {
				command.handler = handlers[command.name];
			}
			commands.emplace(cmd["name"], command);
		}
		items.clear();
		for (const auto& item : commandData["card_types"]) {
			items.emplace(utf8ToBig5(item["name"]), utf8ToBig5(item["description"]));
		}
	}
	catch (const std::exception& e) {
		std::cerr << "�ѪR command.json �ɵo�Ϳ��~: " << e.what() << std::endl;
	}

	file.close();
}


std::pair<bool, int> CommandHandler::ProcessCommand(std::shared_ptr<Player> player, const std::string& input) {
	if (input.empty() || input[0] != '/') {
		return std::make_pair(false, 0);
	}

	// Split the command and its arguments
	std::vector<std::string> parts = SplitCommand(input);
	if (parts.empty()) {
		return std::make_pair(false, 0);
	}

	// Extract command name (without the /)
	std::string cmdName = parts[0].substr(1);
	if (cmdName.empty()) {
		return std::make_pair(false, 0);
	}

	// Remove command from arguments
	parts.erase(parts.begin());

	bool commandResult = false;

	// Find and execute the appropriate command handler
	if (IsValidCommand(cmdName)) {
		commandResult = commands[cmdName].handler(player, parts);
		if (commandResult && game && cmdName == "move") {
			game->SaveGame();
			return std::make_pair(true, 1);
		}
	}
	else {
		std::cout << "�������O: " << cmdName << std::endl;
		return std::make_pair(false, 0);
	}

	// If command was successful and game reference is valid, save the game state
	if (commandResult && game) {
		game->SaveGame();
		// std::cout << "�w�x�s�C���i��" << std::endl;
	}

	return std::make_pair(true, 0);
}

std::vector<std::string> CommandHandler::GetCommandList(bool detailed) {
	std::vector<std::string> result;

	for (const auto& cmd : commands) {
		if (detailed) {
			std::string cmdInfo = "/" + cmd.second.name + " - " + cmd.second.description + "\n";
			cmdInfo += "    �Ϊk: " + cmd.second.usage + "\n";
			cmdInfo += "    �d��: " + cmd.second.example;
			result.push_back(cmdInfo);
		}
		else {
			result.push_back("/" + cmd.second.name + " - " + cmd.second.description);
		}
	}

	return result;
}


bool CommandHandler::IsValidCommand(const std::string& cmdName) {
	return commands.find(cmdName) != commands.end();
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

		auto currentTile = game->gameMap->GetTileAt(position);
		std::string tileName = currentTile->GetName();
		if (tileName.empty()) {
			tileName = "�� " + std::to_string(position) + " ��";
		}

		std::vector<std::string> options = { "�O","�_" };
		std::string question = "�O�_�nĲ�o" + tileName + "���ĪG�H";
		int choice = Monopoly::GetUserChoice(question, options, true);

		if (choice == 0) {
			currentTile->OnLand(game->getCurrentPlayer());
		}

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
		std::cout << "�i�Υd�P: �����l, ���b�d, �R�B�d�A�R���d" << std::endl;
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
	else if (cardName == "destroy card" || cardName == "�R���d" || cardName == "destroy") {
		player->AddItem(new DestroyCard());
		std::cout << player->GetName() << " ��o�d�P: �R���d" << std::endl;
		return true;
	}
	else {
		std::cout << "�����d�P: " << cardName << std::endl;
		std::cout << "�i�Υd�P: �����l, ���b�d, �R�B�d�A�R���d" << std::endl;
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
		std::cout << "�i�Ϊ��A: finish" << std::endl;
		return false;
	}

	std::string stateStr = args[0];

	if (stateStr == "end" || stateStr == "finish" || stateStr == "over") {
		game->gameOver = true;
		std::cout << "�C���w�аO������! �U�^�X�N��̲ܳ׵��G�C" << std::endl;
		return true;
	}
	else {
		std::cout << "�������A: " << stateStr << std::endl;
		std::cout << "�i�Ϊ��A: finish" << std::endl;
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
			status = "��| (" + std::to_string(game->players[i]->hospitalDay) + ")";
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
		for (auto& a : commands) {
			showDetail(a.second);
		}

		std::cout << "\n�i�Υd�P����:" << std::endl;
		int count(1);
		for (auto& a : items) {
			std::cout << count << ". " << a.first << " - " << a.second << std::endl;
			count++;
		}
	}
	else {
		for (auto& a : commands) {
			showDescription(a.second);
		}
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

void CommandHandler::showDetail(const Command& cmd) {
	showDescription(cmd);
	std::cout << "    ";
	showUsage(cmd);
	std::cout << "    ";
	showExample(cmd);
}

void CommandHandler::showDescription(const Command& cmd) {
	std::cout << "/" << cmd.name << " - " << cmd.description << std::endl;
}

void CommandHandler::showUsage(const Command& cmd) {
	std::cout << "�Ϊk: " << cmd.usage << std::endl;
}

void CommandHandler::showExample(const Command& cmd) {
	std::cout << "�d��: " << cmd.example << std::endl;
}