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

// 全域遊戲實例指標，用於玩家破產處理
Game* gameInstance = nullptr;

std::string colors[4] = { "red", "yellow", "green", "blue" };

std::vector<std::vector<std::string>> diceFaces = {
	{
		"┌───────┐",
		"│       │",
		"│   ●   │",
		"│       │",
		"└───────┘"
	},
	{
		"┌───────┐",
		"│ ●     │",
		"│       │",
		"│     ● │",
		"└───────┘"
	},
	{
		"┌───────┐",
		"│ ●     │",
		"│   ●   │",
		"│     ● │",
		"└───────┘"
	},
	{
		"┌───────┐",
		"│ ●   ● │",
		"│       │",
		"│ ●   ● │",
		"└───────┘"
	},
	{
		"┌───────┐",
		"│ ●   ● │",
		"│   ●   │",
		"│ ●   ● │",
		"└───────┘"
	},
	{
		"┌───────┐",
		"│ ●   ● │",
		"│ ●   ● │",
		"│ ●   ● │",
		"└───────┘"
	}
};

Game::Game()
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
		std::cerr << "錯誤: 找不到 configs.json!" << std::endl;
	}
	nlohmann::json data;
	file >> data;
	file.close();

	currentPlayerIdx = 0;
	gameOver = false;
	gameMap = nullptr; // 初始化為 nullptr
	srand(static_cast<unsigned int>(time(nullptr)));

	// 設定全域遊戲實例指標
	gameInstance = this;

	if (data["init"]["winMoney"].is_number()) {
		winMoney = data["init"]["winMoney"];
	}
	else {
		winMoney = 5000;
	}
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
		std::cerr << "錯誤: 找不到 configs.json!" << std::endl;
	}
	nlohmann::json data;
	file >> data;
	file.close();

	gameMap = new Map();

	int playerNum = 0;

	std::string question = "## 選擇遊玩人數 ##";
	std::vector<std::string> options = {
		"2人",
		"3人",
		"4人"
	};

	int choice = Monopoly::GetUserChoice(question, options, true, false);

	playerNum = choice + 2;

	Clear();
	std::cout << "## 輸入玩家設定 ##\n";
	int money = 1000;
	if (data["init"]["money"].is_number()) {
		money = data["init"]["money"];
	}
	for (int i = 0; i < playerNum; i++) {
		std::string name;
		while (true) {
			std::cout << "\n玩家 " << i + 1 << " 的名稱（最多6個字，且不可重複）：";
			std::cin >> name;

			if (name.length() > 6) {
				std::cout << "名稱太長，請重新輸入。\n";
				continue;
			}
			bool duplicate = false;
			for (const auto& p : players) {
				if (p->GetName() == name) {
					std::cout << "名稱已被使用，請重新輸入。\n";
					duplicate = true;
					break;
				}
			}

			if (!duplicate) break;
		}

		Player* p = new Player(name, colors[i], money);
		players.push_back(p);
	}

	std::string itemName("");
	for (auto& a : data["init"]["items"].items()) {
		itemName = a.key();
		if (itemName != "") {
			if (itemName == u8"控制骰子") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new ControlDiceItem());
				}
			}
			else if (itemName == u8"命運卡") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new FateCard());
				}
			}
			else if (itemName == u8"火箭卡") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new RocketCard());
				}
			}
			else if (itemName == u8"摧毀卡") {
				for (auto& p : players) {
					for (int i = 0; i < a.value(); ++i)
						p->AddItem(new DestroyCard());
				}
			}
		}
	}

	std::cout << "\n遊戲初始化完成，共 " << players.size() << " 位玩家。" << std::endl;
	std::cout << "設定起始資金：" << money << std::endl;
	std::cout << "勝利金額：" << winMoney << std::endl;

	// Clear(); // 在玩家設定完成後清除畫面並顯示初始狀態
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

	// 每回合開始時先檢查遊戲是否已經可以結束
	if (CheckWinCondition()) {
		return; // 如果已經有勝利條件被滿足，直接結束回合
	}

	// 確保 currentPlayerIdx 在有效範圍內
	if (currentPlayerIdx < 0 || currentPlayerIdx >= static_cast<int>(players.size())) {
		std::cerr << "警告：當前玩家索引 " << currentPlayerIdx << " 無效，重設為 0\n";
		currentPlayerIdx = 0;
	}

	auto currentPlayer = players[currentPlayerIdx];

	// 如果當前玩家已破產，跳到下一位
	if (currentPlayer->GetMoney() < 0 || currentPlayer->isDead) {
		// 換下一位
		currentPlayerIdx = (currentPlayerIdx + 1) % static_cast<int>(players.size());
		NextTurn(); // 遞迴呼叫，跳到下一個玩家
		return;
	}

	std::cout << "\n輪到 " << currentPlayer->GetName() << " 的回合。" << std::endl;
	bool playerTurnCompleted = false;

	// 處理醫院狀態
	if (currentPlayer->inHospital) {
		// 顯示住院狀態
		std::cout << currentPlayer->GetName() << " 正在住院，已經過了 "
			<< currentPlayer->hospitalDay << " 天。" << std::endl;

		if (currentPlayer->hospitalDay == 0) {
			std::cout << "\n" << currentPlayer->GetName() << " 已出院！\n";
			currentPlayer->inHospital = false;
			currentPlayer->hospitalDay = 0;
		}
		else {
			// 顯示出院選項
			std::vector<std::string> hospitalOptions = {
				"擲骰子（擲出 4 點或以上即可出院）",
				"支付 $50 出院費",
				"乖乖在醫院休養"
			};

			std::string hospitalPrompt = "輪到 " + currentPlayer->GetName() + " 的回合。" + "\n你正在醫院休養，選擇行動：";
			int hospitalChoice = Monopoly::GetUserChoice(hospitalPrompt, hospitalOptions, true, false);

			if (hospitalChoice == 0) {
				// 擲骰嘗試出院
				int roll = RollDiceWithAsciiAnimation();
				if (roll >= 4) {
					// 成功出院
					std::cout << "擲出了 " << roll << " 點，成功出院！" << std::endl;
					currentPlayer->inHospital = false;
					currentPlayer->hospitalDay = 0;
				}
				else {
					// 失敗，繼續住院
					std::cout << "擲出了 " << roll << " 點，未達到4點，需繼續住院。" << std::endl;
					currentPlayer->hospitalDay--;
					playerTurnCompleted = true;
				}
			}
			else if (hospitalChoice == 1) {
				// 支付出院費
				if (currentPlayer->GetMoney() >= 50) {
					currentPlayer->BuyProperty(50); // 扣除50元出院費
					std::cout << currentPlayer->GetName() << " 支付了$50出院費，提前出院！" << std::endl;
					currentPlayer->inHospital = false;
					currentPlayer->hospitalDay = 0;
				}
				else {
					std::cout << "你沒有足夠的錢支付出院費！" << std::endl;
					// 增加住院天數並跳過回合
					currentPlayer->hospitalDay--;
					playerTurnCompleted = true;
				}
			}
			else {
				// 選擇繼續住院
				std::cout << currentPlayer->GetName() << " 選擇繼續在醫院休養。" << std::endl;
				// 減少住院天數並跳過回合
				currentPlayer->hospitalDay--;
				playerTurnCompleted = true;
			}
		}
	}

	while (!playerTurnCompleted) {
		std::string question = "輪到 " + currentPlayer->GetName() + " 的回合。\n目前金額: $" + std::to_string(currentPlayer->GetMoney());
		std::vector<std::string> options = {
			"進行擲骰",
			"使用道具",
		};
		// 使用true參數表示顯示地圖
		int choice = Monopoly::GetUserChoice(question, options, true, true);

		bool passStart = false;
		Tile* start = NULL;
		if (choice == 0) {
			// 直接進行擲骰，不顯示額外提示
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
			std::cout << currentPlayer->GetName() << " 移動到第 " << pos << " 格。\n";

			if (passStart) {
				start->OnLand(currentPlayer);
			}
			Monopoly::WaitForEnter();
			auto currentTile = gameMap->GetTileAt(pos);

			currentTile->OnLand(currentPlayer);
			playerTurnCompleted = true; // 玩家回合完成
		}
		else if (choice == 1) {
			// 使用道具的邏輯
			std::vector<Item*> items = currentPlayer->GetItem();
			if (items.empty()) {
				std::cout << "你沒有任何道具可以使用！\n";
				Monopoly::WaitForEnter(); // 顯示訊息後等待玩家按Enter繼續
				system("cls"); // 清除螢幕
				PrintMapStatus();
				PrintPlayerStatus();
				continue; // 回到玩家行動選單
			}
			else {
				std::string itemQuestion = "選擇要使用的道具：";
				std::vector<std::string> itemOptions;
				for (Item* item : items) {
					itemOptions.push_back(item->GetName() + " - " + item->GetDescription());
				}
				itemOptions.push_back("返回");

				int itemChoice = Monopoly::GetUserChoice(itemQuestion, itemOptions, true, false);
				if (itemChoice < static_cast<int>(items.size())) {
					// 使用選定的道具
					Item* selectedItem = items[itemChoice];
					// 保存原始的道具列表大小
					size_t originalItemCount = items.size();

					// 使用道具
					if (currentPlayer->UseItem(selectedItem)) {
						// 檢查使用道具後是否需要進行其他動作
						// 如果是控制骰子道具，玩家已經移動位置，視同完成回合
						if (selectedItem->GetName() == "Control Dice") {
							// 取得玩家當前位置
							int pos = currentPlayer->GetPosition();
							std::cout << currentPlayer->GetName() << " 移動到第 " << pos << " 格。\n";

							// 觸發當前所在格子的效果
							auto currentTile = gameMap->GetTileAt(pos);
							currentTile->OnLand(currentPlayer);
							playerTurnCompleted = true; // 玩家回合完成
						}
						else {
							// 其他類型的道具使用後，玩家可以繼續選擇行動
							// 不設定 playerTurnCompleted 為 true，讓玩家回到行動選單
							continue;
						}
						delete selectedItem; // 刪除已使用的道具
					}

				}
				else {
					// 選擇返回
					continue; // 回到玩家行動選單
				}
			}
		}
		// 輸入指令
		else if (choice == -1) {
			playerTurnCompleted = true; // 玩家回合完成
		}
	}

	Monopoly::WaitForEnter();
	Monopoly::UpdateScreen();

	// 檢查玩家是否破產
	if (currentPlayer->GetMoney() < 0) {
		std::cout << currentPlayer->GetName() << " 已破產！\n";
		currentPlayer->isDead = true;

		// 立即檢查是否只剩下一位玩家，如果是則結束遊戲
		if (CheckWinCondition()) {
			EndGame();
			return;
		}
	}

	// 自動存檔
	SaveGame();

	// 換下一位
	currentPlayerIdx = (currentPlayerIdx + 1) % static_cast<int>(players.size());
}

bool Game::CheckWinCondition()
{
	// 計算仍在遊戲中的玩家數
	int activePlayers = 0;
	int lastPlayerIdx = -1;

	for (size_t i = 0; i < players.size(); i++) {
		// 只有金額小於0才算破產，金額為0不算破產
		if (players[i]->GetMoney() >= 0) {
			activePlayers++;
			lastPlayerIdx = i;
		}
	}

	// 如果只剩一位玩家沒有破產，則他獲勝
	if (activePlayers == 1 && lastPlayerIdx != -1) {
		std::cout << "\n" << players[lastPlayerIdx]->GetName() << " 是唯一沒有破產的玩家，獲得勝利！" << std::endl;
		gameOver = true;
		return true;
	}

	// 檢查是否有玩家達到資金條件獲勝
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i]->GetMoney() >= winMoney) {
			std::cout << "\n" << players[i]->GetName() << " 贏得勝利！" << std::endl;
			gameOver = true;
			return true;
		}
	}

	return false;
}

void Game::EndGame()
{
	std::cout << "\n遊戲結束！" << std::endl;
	std::cout << "+====================遊戲結果====================+" << std::endl;
	std::cout << "+-------+------------+-----------+---------------+" << std::endl;
	std::cout << "| 排名  | 玩家名稱   | 最終資金  | 狀態          |" << std::endl;
	std::cout << "+-------+------------+-----------+---------------+" << std::endl;

	// 建立玩家索引列表並根據金錢排序
	std::vector<std::pair<int, Player*>> playerRanking;
	for (size_t i = 0; i < players.size(); i++) {
		playerRanking.push_back(std::make_pair(i, players[i]));
	}

	// 根據金錢由高到低排序
	std::sort(playerRanking.begin(), playerRanking.end(),
		[](const std::pair<int, Player*>& a, const std::pair<int, Player*>& b) {
			return a.second->GetMoney() > b.second->GetMoney();
		});

	// 輸出玩家排名與資訊
	for (size_t i = 0; i < playerRanking.size(); i++) {
		Player* p = playerRanking[i].second;
		std::string colorCode = Monopoly::GetColorCode(p->GetColor());
		std::string status = (p->GetMoney() < 0) ? "破產" : (i == 0) ? "勝利" : "遊戲結束";

		std::cout << "| " << std::left << std::setw(5) << (i + 1)
			<< " | " << colorCode << std::left << std::setw(10) << p->GetName() << "\033[0m"
			<< " | " << std::right << std::setw(9) << p->GetMoney()
			<< " | " << std::left << std::setw(13) << status << " |" << std::endl;
	}

	std::cout << "+-------+------------+-----------+---------------+" << std::endl;
	std::cout << "\n恭喜 " << playerRanking[0].second->GetName() << " 獲得遊戲勝利！" << std::endl;

	// 遊戲結束時刪除存檔
	DeleteSaveGame();
}

void Game::Clear()
{
	//std::cout << "\033[2J\033[H"; // 清除畫面
	system("cls");
	PrintMapStatus();
	PrintPlayerStatus();
	std::cout << std::endl;
}

void Game::PrintMapStatus()
{
	// 直接呼叫 Map 類別的 PrintMap 函式，使用新的網格佈局
	gameMap->PrintMap(players);
}

void Game::PrintPlayerStatus()
{
	std::cout << "+------------+--------+-------+------------+--------------------------+--------------------------+\n";
	std::cout << "| 玩家名稱   | 資金   | 位置  | 狀態       | 擁有道具                 | 擁有地產                 |\n";
	std::cout << "+------------+--------+-------+------------+--------------------------+--------------------------+\n";

	const std::string RESET = "\033[0m";

	for (int i = 0; i < players.size(); i++) {
		std::string colorCode = Monopoly::GetColorCode(players[i]->GetColor());
		std::cout << colorCode;

		std::string name = "[" + std::string(1, '1' + i) + "]" + players[i]->GetName();
		std::string status;

		if (players[i]->inHospital) {
			status = "住院中 (" + std::to_string(players[i]->hospitalDay) + ")";
		}
		else {
			status = "正常";
		}

		if (players[i]->isDead)status = "破產";

		// 道具字串
		std::string itemStr = players[i]->GetItem().empty() ? "無" : "";
		for (size_t j = 0; j < players[i]->GetItem().size(); j++) {
			if (players[i]->GetItem()[j]->GetName() == "Control Dice") {
				itemStr += "[骰]";
			}
			else if (players[i]->GetItem()[j]->GetName() == "Rocket Card") {
				itemStr += "[火]";
			}
			else if (players[i]->GetItem()[j]->GetName() == "Fate Card") {
				itemStr += "[命]";
			}
			else if (players[i]->GetItem()[j]->GetName() == "Destroy Card") {
				itemStr += "[毀]";
			}


			if (j != players[i]->GetItem().size() - 1)
				itemStr += ", ";
		}

		// 地產
		std::string propertyStr = players[i]->GetProperty().empty() ? "無" : "";
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

		// 輸出每欄
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

	std::cout << "擲骰中...\n";

	for (int i = 0; i < rollTimes; ++i) {
		int d1 = rand() % 6 + 1;
		int d2 = rand() % 6 + 1;

		Clear();
		PrintDice(d1, d2);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		finalRoll = d1 + d2;
	}

	std::cout << "擲骰結果是 " << finalRoll << " 點！" << std::endl;
	Monopoly::WaitForEnter();
	return finalRoll;
}

bool Game::SaveGame(const std::string& filename)
{
	// 開啟檔案
	std::ofstream saveFile(filename);
	if (!saveFile.is_open()) {
		std::cerr << "無法創建存檔：" << filename << std::endl;
		return false;
	}

	try {
		// 寫入 JSON 格式的存檔
		saveFile << "{\n";

		// 保存目前玩家索引
		saveFile << "  \"currentPlayerIdx\": " << currentPlayerIdx << ",\n";

		// 保存玩家數量（增加此欄位以便檢查）
		saveFile << "  \"playerCount\": " << players.size() << ",\n";

		// 保存地圖狀態（這裡僅保存地圖格子的擁有者）
		saveFile << "  \"map\": {\n";
		auto mapData = gameMap->getMap();
		saveFile << "    \"size\": " << gameMap->getSize() << ",\n";

		// 保存每個格子的擁有者（如果是地產）
		saveFile << "    \"tiles\": [\n";
		for (int i = 0; i < gameMap->getSize(); ++i) {
			Tile* tile = gameMap->GetTileAt(i);
			saveFile << "      {\n";
			saveFile << "        \"index\": " << i << ",\n";
			saveFile << "        \"name\": \"" << mapData.second[i] << "\",\n";

			// 如果是地產格子，嘗試獲取擁有者
			PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile);
			if (propTile && propTile->GetOwner()) {
				// 找出擁有者在玩家列表中的索引
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

		// 保存玩家資訊
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

			// 保存道具清單
			saveFile << "      \"items\": [\n";
			std::vector<Item*> items = p->GetItem();
			for (size_t j = 0; j < items.size(); ++j) {
				saveFile << "        {\n";
				saveFile << "          \"name\": \"" << items[j]->GetName() << "\",\n";
				saveFile << "          \"type\": \"" << "item" << "\"\n";  // 這裡可以擴展，儲存不同類型的道具
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

		std::cout << "遊戲進度已成功保存到 " << filename << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "保存遊戲時發生錯誤：" << e.what() << std::endl;
		saveFile.close();
		return false;
	}
}

bool Game::LoadGame(const std::string& filename)
{
	std::ifstream loadFile(filename);
	if (!loadFile.is_open()) {
		std::cerr << "無法開啟存檔：" << filename << std::endl;
		return false;
	}

	try {
		// 清除當前遊戲狀態
		for (Player* p : players) {
			delete p;
		}
		players.clear();

		if (gameMap) {
			delete gameMap;
		}
		gameMap = new Map();  // 初始化一個新地圖

		// 讀取整個檔案內容
		std::string content;
		std::string line;
		while (std::getline(loadFile, line)) {
			content += line + "\n";
		}
		loadFile.close();

		std::cout << "已讀取存檔文件，正在分析...\n";

		// 解析 JSON - 這裡使用簡單的字串處理方式
		// 實際應用中建議使用專業的 JSON 解析庫

		// 暫時將 currentPlayerIdx 設為 0，防止未找到或解析錯誤
		currentPlayerIdx = 0;

		// 解析當前玩家索引
		size_t currentPlayerIdxPos = content.find("\"currentPlayerIdx\":");
		if (currentPlayerIdxPos != std::string::npos) {
			size_t valueStart = content.find(":", currentPlayerIdxPos) + 1;
			size_t valueEnd = content.find(",", valueStart);
			std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
			valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
			currentPlayerIdx = std::stoi(valueStr);
		}

		// 檢查玩家數量，確保正確載入所有玩家
		int expectedPlayerCount = 0;
		size_t playerCountPos = content.find("\"playerCount\":");
		if (playerCountPos != std::string::npos) {
			size_t valueStart = content.find(":", playerCountPos) + 1;
			size_t valueEnd = content.find(",", valueStart);
			std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
			valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
			expectedPlayerCount = std::stoi(valueStr);
			std::cout << "預期載入 " << expectedPlayerCount << " 名玩家\n";
		}

		// 使用更簡易的方法解析玩家資料
		// 找到 players 陣列的開始和結束位置
		size_t playersArrayStart = content.find("\"players\":");
		if (playersArrayStart == std::string::npos) {
			std::cerr << "無法找到玩家資料區塊\n";
			return false;
		}

		// 尋找 players 陣列的開始 [
		size_t arrayStart = content.find("[", playersArrayStart);
		if (arrayStart == std::string::npos) {
			std::cerr << "無法找到玩家資料陣列開始標記 '['\n";
			return false;
		}

		// 尋找對應的結束 ]，需要計算嵌套層級
		size_t arrayEnd = arrayStart + 1;
		int bracketLevel = 1; // 已經找到一個 [

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
			std::cerr << "無法找到匹配的 players 陣列結束標記 ']'\n";
			return false;
		}

		// 獲取完整的 players 陣列內容
		std::string playersArray = content.substr(arrayStart + 1, arrayEnd - arrayStart - 2);
		std::cout << "玩家資料陣列內容：\n" << playersArray << std::endl;

		// 根據逗號分割多個玩家的 JSON 對象
		// 但需要注意嵌套的對象和陣列，不能單純用逗號分割
		int playerCount = 0;
		size_t pos = 0;

		std::cout << "開始逐一解析玩家資料...\n";

		// 逐個解析每個玩家的 JSON 對象
		while (pos < playersArray.length()) {
			// 跳過空白
			while (pos < playersArray.length() &&
				(playersArray[pos] == ' ' || playersArray[pos] == '\n' ||
					playersArray[pos] == '\r' || playersArray[pos] == '\t')) {
				pos++;
			}

			// 檢查是否到達結尾
			if (pos >= playersArray.length()) {
				break;
			}

			// 尋找玩家對象開始的 {
			if (playersArray[pos] != '{') {
				std::cerr << "無法找到玩家 " << (playerCount + 1) << " 的開始標記 '{'"
					<< " (位置 " << pos << ", 實際字元: " << playersArray[pos] << ")\n";
				// 嘗試跳過這個問題字元，繼續尋找
				pos++;
				continue;
			}

			// 找到對象的結束 }，需要計算嵌套層級
			size_t objStart = pos;
			size_t objEnd = objStart + 1;
			int braceLevel = 1; // 已經找到一個 {

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
				std::cerr << "玩家 " << (playerCount + 1) << " 的 JSON 解析錯誤：大括號不匹配\n";
				break;
			}

			// 取得完整的玩家 JSON 對象
			std::string playerJson = playersArray.substr(objStart, objEnd - objStart);
			std::cout << "取得玩家 " << (playerCount + 1) << " 資料：長度 " << playerJson.length() << " 字元\n";
			std::cout << "玩家 JSON: " << playerJson << "\n";

			// 解析玩家屬性
			std::string name, color;
			int money = 0, position = 0;
			bool inHospital = false;
			int hospitalDay = 0;
			std::vector<Item*>items;

			// 名稱
			size_t namePos = playerJson.find("\"name\":");
			if (namePos != std::string::npos) {
				size_t valueStart = playerJson.find("\"", namePos + 7) + 1;
				size_t valueEnd = playerJson.find("\"", valueStart);
				name = playerJson.substr(valueStart, valueEnd - valueStart);
			}
			else {
				std::cerr << "無法找到玩家名稱\n";
				return false;
			}

			// 顏色
			size_t colorPos = playerJson.find("\"color\":");
			if (colorPos != std::string::npos) {
				size_t valueStart = playerJson.find("\"", colorPos + 8) + 1;
				size_t valueEnd = playerJson.find("\"", valueStart);
				color = playerJson.substr(valueStart, valueEnd - valueStart);
			}
			else {
				std::cerr << "無法找到玩家顏色\n";
				return false;
			}

			// 金錢
			size_t moneyPos = playerJson.find("\"money\":");
			if (moneyPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", moneyPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				money = std::stoi(valueStr);
			}
			else {
				std::cerr << "無法找到玩家金錢\n";
				return false;
			}

			// 位置
			size_t positionPos = playerJson.find("\"position\":");
			if (positionPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", positionPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				position = std::stoi(valueStr);
			}
			else {
				std::cerr << "無法找到玩家位置\n";
				return false;
			}

			// 是否在醫院
			size_t inHospitalPos = playerJson.find("\"inHospital\":");
			if (inHospitalPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", inHospitalPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				inHospital = (valueStr.find("true") != std::string::npos);
			}

			// 住院天數
			size_t hospitalDayPos = playerJson.find("\"hospitalDay\":");
			if (hospitalDayPos != std::string::npos) {
				size_t valueStart = playerJson.find(":", hospitalDayPos) + 1;
				size_t valueEnd = playerJson.find(",", valueStart);
				if (valueEnd == std::string::npos) {
					// 可能是最後一個屬性，尋找其他可能的結束標記
					valueEnd = playerJson.find("}", valueStart);
					if (valueEnd == std::string::npos) {
						valueEnd = playerJson.length() - 1;
					}
				}
				std::string valueStr = playerJson.substr(valueStart, valueEnd - valueStart);
				valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
				hospitalDay = std::stoi(valueStr);
			}

			// 道具
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
						std::cerr << "無法辨識道具名稱: " << itemName << "\n";
					}

					itemNamePos = valueEnd;
				}
			}


			std::cout << "玩家資料: 名稱=" << name << ", 顏色=" << color << ", 金錢=" << money
				<< ", 位置=" << position << ", 住院=" << (inHospital ? "是" : "否") << "\n";

			// 創建玩家
			Player* player = new Player(name, color, money);
			player->SetPosition(position);
			player->inHospital = inHospital;
			player->hospitalDay = hospitalDay;
			for (int i = 0; i < items.size(); i++) {
				player->AddItem(items[i]);
			}

			players.push_back(player);
			playerCount++;

			// 移動到下一個玩家的位置
			pos = objEnd;

			// 跳過可能的逗號和空白
			while (pos < playersArray.length() &&
				(playersArray[pos] == ',' || playersArray[pos] == ' ' ||
					playersArray[pos] == '\n' || playersArray[pos] == '\r' ||
					playersArray[pos] == '\t')) {
				pos++;
			}

			std::cout << "下一個解析位置: " << pos
				<< " (字元: " << (pos < playersArray.length() ?
					std::string(1, playersArray[pos]) : "結尾") << ")\n";
		}

		// 檢查是否載入了正確數量的玩家
		if (expectedPlayerCount > 0 && playerCount != expectedPlayerCount) {
			std::cerr << "警告：預期載入 " << expectedPlayerCount << " 名玩家，但實際只載入了 "
				<< playerCount << " 名玩家！\n";
		}

		// 確保 currentPlayerIdx 在有效範圍內
		if (players.empty()) {
			std::cerr << "載入遊戲失敗：沒有發現玩家資料\n";
			return false;
		}

		if (currentPlayerIdx < 0 || currentPlayerIdx >= static_cast<int>(players.size())) {
			std::cerr << "警告：載入的玩家索引 " << currentPlayerIdx << " 無效，重設為 0\n";
			currentPlayerIdx = 0; // 重設為第一個玩家
		}

		// 還原地圖格子的擁有者
		size_t tilesPos = content.find("\"tiles\":");
		if (tilesPos != std::string::npos) {
			size_t tilesStart = content.find("[", tilesPos);
			size_t tilesEnd = content.find("]", tilesStart);
			std::string tilesStr = content.substr(tilesStart, tilesEnd - tilesStart + 1);

			size_t pos = tilesStr.find("{");
			while (pos != std::string::npos) {
				size_t endPos = tilesStr.find("}", pos);
				std::string tileStr = tilesStr.substr(pos, endPos - pos + 1);

				// 解析格子屬性
				int index = -1, ownerIdx = -1, level = 0;

				// 索引
				size_t indexPos = tileStr.find("\"index\":");
				if (indexPos != std::string::npos) {
					size_t valueStart = tileStr.find(":", indexPos) + 1;
					size_t valueEnd = tileStr.find(",", valueStart);
					std::string valueStr = tileStr.substr(valueStart, valueEnd - valueStart);
					valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), ::isspace), valueStr.end());
					index = std::stoi(valueStr);
				}

				// 擁有者索引
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

				// 等級
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

				// 設定格子擁有者
				if (index >= 0 && index < gameMap->getSize() && ownerIdx >= 0 && ownerIdx < static_cast<int>(players.size())) {
					Tile* tile = gameMap->GetTileAt(index);
					PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile);
					if (propTile) {
						propTile->SetOwner(players[ownerIdx]);

						// 設定等級
						for (int i = 0; i < level; ++i) {
							propTile->Upgrade();
						}

						players[ownerIdx]->AddProperty(propTile);
					}
				}

				pos = tilesStr.find("{", endPos);
			}
		}

		// 打印載入的玩家資訊
		std::cout << "已成功載入 " << players.size() << " 名玩家：\n";
		for (size_t i = 0; i < players.size(); i++) {
			std::cout << (i + 1) << ". " << players[i]->GetName()
				<< " (金錢: " << players[i]->GetMoney() << ")\n";
		}

		std::cout << "遊戲進度已成功載入！" << std::endl;
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "載入遊戲時發生錯誤：" << e.what() << std::endl;
		loadFile.close();

		// 出錯時清理並重置遊戲狀態
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
		std::cerr << "刪除存檔失敗：" << filename << std::endl;
		return false;
	}
	std::cout << "存檔已刪除：" << filename << std::endl;
	return true;
}

// 增加處理玩家破產的函數
void Game::HandlePlayerBankruptcy(Player* bankruptPlayer, Player* creditor)
{
	// 如果破產玩家就是銀行，或者債主是銀行，不執行特殊處理
	if (bankruptPlayer->GetName() == "bank" || (creditor && creditor->GetName() == "bank")) {
		return;
	}

	std::cout << bankruptPlayer->GetName() << " 破產了，將所有地產轉移給 " << creditor->GetName() << "！\n";

	// 遍歷地圖尋找破產玩家擁有的土地
	auto mapData = gameMap->getMap();
	for (int i = 0; i < gameMap->getSize(); i++) {
		Tile* tile = gameMap->GetTileAt(i);
		PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile);

		// 如果是破產玩家的地產，轉移給債主
		if (propTile && propTile->GetOwner() == bankruptPlayer) {
			std::cout << "地產「" << mapData.second[i] << "」從 " << bankruptPlayer->GetName()
				<< " 轉移給 " << creditor->GetName() << "。\n";
			propTile->SetOwner(creditor);
		}
	}

	// 檢查遊戲是否應該結束（只剩一名玩家）
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

std::pair<bool, int>  Game::processCommand(std::shared_ptr<Player> player, const std::string& input) {
	if (input.empty() || input[0] != '/') {
		return std::make_pair(false, 0);
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