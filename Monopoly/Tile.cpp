#include "Tile.h"
#include "Player.h"
#include "HorseRacing.h"
#include "SheLongMen.h"
#include "Map.h"
#include "Item.h"
#include "Monopoly.h"
#include "Game.h"

std::vector<std::vector<std::string>> houses = {
	{
		"   /\\",
		"  /  \\",
		" /____\\",
		" | [] |",
		" |____|\n"
	},
	{
		"  _____",
		" /    /\\",
		"/____/__\\",
		"| [] [] |",
		"|_______|"
	},
	{
		"   /\\  /\\  ",
		"  /__\\/__\\ ",
		"  | [] [] |",
		"  |  __   |",
		" /| |__| |\\ ",
		"/_|______|_\\\n"
	},
	{
		"  _______",
		" / _____ \\",
		"| |SHOP | |",
		"| |_____| |",
		"|  ___    |",
		"| |___|[] |",
		"|_________|\n"
	},
	{
		"  _______",
		" / _____ \\",
		"| | H + | |",
		"| |_____| |",
		"|   ___   |",
		"|  |___|  |",
		"|_________|\n"
	},
	{

	}
};

string getHouse(int level) {
	if (level < 0 || static_cast<size_t>(level) >= houses.size()) {
		std::cerr << "ĵ�i�GPrintHouse �� level �W�X�d�� (" << level << ")\n";
		return "";
	}
	string result("");

	auto house = houses[level];
	for (size_t i = 0; i < house.size(); i++) {
		result += house[i] + '\n';
	}
	return result;
}


StartTile::StartTile()
{
	color = "cyan";
}

void StartTile::OnLand(Player* p)
{
	Monopoly::bank.Pay(p, 200);
	std::cout << "�^��_�I��: ���y200��\n";
}

PropertyTile::PropertyTile(int _id, int _price, std::string _name)
{
	color = "";
	level = _id;
	owner = NULL;
	name = _name;
	price = _price;
}

int PropertyTile::GetPrice()
{
	return price;
}

int PropertyTile::GetLevel()
{
	return level;
}

void PropertyTile::Upgrade()
{
	level++;
}

Player* PropertyTile::GetOwner()
{
	return owner;
}

void PropertyTile::SetOwner(Player* p)
{
	owner = p;
	color = p->GetColor();
}

void PropertyTile::OnLand(Player* p)
{
	Monopoly::UpdateScreen();

	if (owner == NULL) {
		std::string question = "��F " + name + "�I\n�g�a���欰 " + std::to_string(price) + " ��\n�A�{�b�� " + std::to_string(p->GetMoney()) + " ���A�O�_�n�ʶR�H";
		std::vector<std::string> options = {
		"�ʶR",
		"���"
		};
		int choice = Monopoly::GetUserChoice(getHouse(level) + question, options);

		if (choice == 0) {
			if (p->BuyProperty(price)) {
				std::cout << "\n�ʶR���\�I\n";
				SetOwner(p);
			}
			else {
				std::cout << "���������A�L�k�ʶR�C\n";
			}
		}
	}
	else if (owner == p) {
		if (level >= 2) {
			std::cout << "�g�a�w�ɦ̰ܳ��šI\n";
			return;
		}

		std::string question = "�O�_�n��O 300 ���ɯŤg�a�H\n�A�{�b�� " + std::to_string(p->GetMoney()) + " ��";
		std::vector<std::string> options = {
		"�ɯ�",
		"���"
		};
		int choice = Monopoly::GetUserChoice(getHouse(level) + question, options);

		if (choice == 0) {
			if (p->BuyProperty(300)) {
				std::cout << "\n�ɯŦ��\�I\n";
				Upgrade();
			}
			else {
				std::cout << "���������A�L�k�ɯšC\n";
			}
		}
	}
	else if (owner != p) {
		int rent = price;
		if (level == 1) rent = price * 2;
		else if (level == 2) rent = price * 3;

		p->Pay(owner, rent);

		std::cout << "���g�a�� " << owner->GetName() << " ���g�a�A�ݤ�I "
			<< rent << " ���� " << owner->GetName() << "�C\n";
		std::cout << "��I��Ѿl " << p->GetMoney() << " ��\n";
	}
}


ShopTile::ShopTile()
{
	color = "teal";
}

void ShopTile::OnLand(Player* p)
{
	std::cout << "���a " << p->GetName() << " �i�J�ө�\n";

	std::string question = "��F�ө��I�O�_�i�J�ө��H";
	std::vector<std::string> options = {
		"�O",
		"�_"
	};

	int choice = Monopoly::GetUserChoice(getHouse(3) + question, options);
	if (choice == 1) return;

	// �ӫ~�C��
	std::vector<std::string> itemNames = {
		"�����l, 300��"
	};
	std::vector<int> itemPrices = {
		300
	};

	// �O���ӫ~�O�_�w�ʶR
	std::vector<bool> purchased(itemNames.size(), false);

	do {
		std::vector<std::string> goods;

		for (size_t i = 0; i < itemNames.size(); ++i) {
			if (purchased[i])
				goods.push_back(itemNames[i] + "�]�w�ʶR�^");
			else
				goods.push_back(itemNames[i]);
		}

		goods.push_back("���}");

		std::string welcome = "�w����{�ө�!";
		choice = Monopoly::GetUserChoice(getHouse(3) + welcome, goods);

		// �ˬd�O�_������}
		if (choice == static_cast<int>(goods.size()) - 1) break;

		// �T�O���ަ���
		if (choice >= 0 && choice < itemNames.size()) {
			if (purchased[choice]) {
				std::cout << "���ӫ~�w�g�ʶR�L�A�L�k�A���ʶR�C\n";
			}
			else if (p->BuyProperty(itemPrices[choice])) {
				p->AddItem(new ControlDiceItem());
				purchased[choice] = true;
				std::cout << "�ʶR���\�I\n";
			}
			else {
				std::cout << "�l�B�����A�ʶR���ѡC\n";
			}
		}
		Monopoly::WaitForEnter();
	} while (true);
}

HospitalTile::HospitalTile()
{
	color = "gray";
}

void HospitalTile::OnLand(Player* p)
{
	Monopoly::UpdateScreen();

	std::cout << getHouse(4);
	if (!p->inHospital) {
		std::cout << "�i��|�𮧤T�ѡI";
		p->inHospital = true;
	}
	else {
		p->hosipitalDay++;
	}
}

ChanceTile::ChanceTile()
{
	color = "orange";
}

void ChanceTile::OnLand(Player* p)
{
	Monopoly::GetUserChoice(getHouse(5) + "���|��A�ө�����|�a!!!", { "��" });
	char chance = rand() % 9;

	std::vector<PropertyTile*> ownedProperties;
	std::vector<std::string> propertyNames;
	HorseRacing miniGame1;
	SheLongMen miniGame2;
	Map* gameMap = Monopoly::game->gameMap;

	switch (chance) {
	case 0:
		std::cout << "�N�~���i�䰨��\n";
		Monopoly::WaitForEnter();
		miniGame1.init(p);
		miniGame1.gameStart();
		break;
	case 1:
		std::cout << "���˼Ȱ����1��\n";
		p->inHospital = true;
		p->hosipitalDay = 1;
		break;
	case 2:
		std::cout << "�ͯf��|2�^�X\n";
		p->inHospital = true;
		p->hosipitalDay = 0;
		break;
	case 3:
		std::cout << "���F�ֳz�p���A��o 300 ��\n";
		Monopoly::bank.Pay(p, 300);
		break;
	case 4:
		std::cout << "��o�K�O�ɯŤ@�B�g�a�����|\n";

		for (int i = 0; i < 28; i++) {
			Tile* tile = gameMap->GetTileAt(i);
			PropertyTile* propTile = dynamic_cast<PropertyTile*>(tile);
			if (propTile && propTile->GetOwner() == p) {
				if (propTile->GetLevel() < 2) {
					ownedProperties.push_back(propTile);
					std::string tileName = "�� " + std::to_string(i) + " �檺�g�a";
					propertyNames.push_back(tileName);
				}
			}
		}

		if (ownedProperties.empty()) {
			std::cout << "�A�S���i�ɯŪ��g�a�I\n";
		}
		else {
			propertyNames.push_back("���ɯ�");
			int choice = Monopoly::GetUserChoice(getHouse(4) + "��ܭn�K�O�ɯŪ��g�a�G", propertyNames);

			if (choice < static_cast<int>(ownedProperties.size())) {
				PropertyTile* selectedProperty = ownedProperties[choice];
				selectedProperty->Upgrade();
				std::cout << "�g�a�ɯŦ��\�I\n";
			}
			else {
				std::cout << "�A��ܩ��K�O�ɯŪ����|�C\n";
			}
		}
		break;
	case 5:
		std::cout << "��o�Ȧ�ɧU�� 150 ��\n";
		Monopoly::bank.Pay(p, 150);
		break;
	case 6:
		std::cout << "�H�ϥ�q�W�h�A�@�� 100 ��\n";
		p->Pay(&Monopoly::bank, 100);
		break;
	case 7:
		std::cout << "�������r�A��|�v���A�Ȱ����1�^�X\n";
		p->inHospital = true;
		p->hosipitalDay = 1;
		break;
	case 8:
		std::cout << "�N�~���i���\n";
		Monopoly::WaitForEnter();
		miniGame2.init(p);
		miniGame2.gameStart();
		break;
	}
}

FateTile::FateTile()
{
	color = "purple";
}

void FateTile::OnLand(Player* p)
{
	Monopoly::GetUserChoice(getHouse(5) + "�R�B��A�ө���R�B�a!!!", { "��" });
	char fate = rand() % 10;

	switch (fate) {
	case 0:
		std::cout << "�a�W�ߨ���]�A���D��100��\n";
		Monopoly::bank.Pay(p, 100);
		break;
	case 1:
		std::cout << "�վ���������A�l��100��\n";
		p->Pay(&Monopoly::bank, 100);
		break;
	case 2:
		std::cout << "�P�_�F�A�ݭn�𮧤@�^�X\n";
		p->inHospital = true;
		p->hosipitalDay = 1;
		break;
	case 3:
		std::cout << "���v�l�A�h��|���}�˭��w�A�Ȱ����1�^�X\n";
		p->inHospital = true;
		p->hosipitalDay = 1;
		break;
	case 4:
		std::cout << "���Ѳ���Q�A��o 200 ��\n";
		Monopoly::bank.Pay(p, 200);
		break;
	case 5:
		std::cout << "�ұo�|�ӳ��A��ú�� 150 ��\n";
		p->Pay(&Monopoly::bank, 150);
		break;
	case 6:
		std::cout << "�B���ٿ��A��o 120 ��\n";
		Monopoly::bank.Pay(p, 120);
		break;
	case 7:
		std::cout << "����G�ٺ��סA��O 80 ��\n";
		p->Pay(&Monopoly::bank, 80);
		break;
	case 8:
		std::cout << "�ͤ馬��§���A��o 50 ��\n";
		Monopoly::bank.Pay(p, 50);
		break;
	case 9:
		std::cout << "��o���N�A�ݭn��|�A�Ȱ����1�^�X\n";
		p->inHospital = true;
		p->hosipitalDay = 1;
		break;
	}
}

MiniGameTile::MiniGameTile()
{
	color = "brown";
}

void MiniGameTile::OnLand(Player* p)
{
	int choice = Monopoly::GetUserChoice(getHouse(5) + "�C���p�C��", {
		"�䰨",
		"�g�s��"
		});
	HorseRacing miniGame1;
	SheLongMen miniGame2;
	switch (choice) {
	case 0:
		miniGame1.init(p);
		miniGame2.gameStart();
		break;
	case 1:
		miniGame2.init(p);
		miniGame2.gameStart();
		break;
	}
}

std::string Tile::getColor()
{
	return color;
}

void Tile::OnLand(Player* p)
{
}

//void PrintHouse(int level) {
//	if (level < 0 || static_cast<size_t>(level) >= houses.size()) {
//		std::cerr << "ĵ�i�GPrintHouse �� level �W�X�d�� (" << level << ")\n";
//		return;
//	}
//	auto house = houses[level];
//	for (size_t i = 0; i < house.size(); i++) {
//		std::cout << house[i] << std::endl;
//	}
//	std::cout << std::endl;
//}



