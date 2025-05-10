#include "Tile.h"
#include "Player.h"
#include "HorseRacing.h"
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
		" |____|"
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
		"/_|______|_\\"
	},
	{
		"  _______",
		" / _____ \\",
		"| |SHOP| |",
		"| |_____| |",
		"|  ___    |",
		"| |___|[] |",
		"|_________|"
	},
	{
		"  _______",
		" / _____ \\",
		"| | H + | |",
		"| |_____| |",
		"|   ___   |",
		"|  |___|  |",
		"|_________|"
	},
	{}
};

Player bank("bank", "black", 1 << 30);

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
	bank.Pay(p, 200);
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
	if (owner == NULL) {
		std::string question = "��F" + name + "�I\n�g�a���欰 " + std::to_string(price) + " ��\n�A�{�b�� " + std::to_string(p->GetMoney()) + " ��\n�O�_�n�ʶR�H";
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

		std::cout << "���g�a�� " << owner->GetName() << " ���g�a�A�ݤ�I "
			<< rent << " ���� " << owner->GetName() << "�C\n";
		std::cout << "�A�{�b�� " << p->GetMoney() << " ��\n";
		
		p->Pay(owner, rent);
	}
}


ShopTile::ShopTile()
{
	color = "teal";
}

void ShopTile::OnLand(Player* p)
{
	std::string question = "��F�ө��I�O�_�i�J�ө��H";
	std::vector<std::string> options = {
	"�O",
	"�_"
	};
	int choice = Monopoly::GetUserChoice(getHouse(3) + question, options);
	
	if (choice) return;

	do {
		std::string welcome = "�w����{�ө�!";
		std::vector<std::string> goods = {
			"�����l, 300��",
			"���}"
		};
		choice = Monopoly::GetUserChoice(getHouse(3) + welcome, goods);

		switch (choice) {
		case 0:
			if (p->BuyProperty(300)) {
				p->AddItem(new ControlDiceItem());
				cout << "�ʶR�����l���\!\n";
			}
			Monopoly::WaitForEnter();
			break;
		case 1:
			break;
		default:
			break;
		}

	} while (choice != 1);
}

HospitalTile::HospitalTile()
{
	color = "gray";
}

void HospitalTile::OnLand(Player* p)
{
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
	char chance = rand() % 8;

	std::vector<PropertyTile*> ownedProperties;
	std::vector<std::string> propertyNames;
	HorseRacing miniGame;
	Map* gameMap = Monopoly::game->gameMap;
	
	switch (chance) {
	case 0:
		std::cout << "�N�~���i�䰨��\n";
		Monopoly::WaitForEnter();
		miniGame.init(p);
		miniGame.gameStart();
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
		bank.Pay(p, 300);
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
		} else {
			propertyNames.push_back("���ɯ�");
			int choice = Monopoly::GetUserChoice(getHouse(4) + "��ܭn�K�O�ɯŪ��g�a�G", propertyNames);
			
			if (choice < static_cast<int>(ownedProperties.size())) {
				PropertyTile* selectedProperty = ownedProperties[choice];
				selectedProperty->Upgrade();
				std::cout << "�g�a�ɯŦ��\�I\n";
			} else {
				std::cout << "�A��ܩ��K�O�ɯŪ����|�C\n";
			}
		}
		break;
	case 5:
		std::cout << "��o�Ȧ�ɧU�� 150 ��\n";
		bank.Pay(p, 150);
		break;
	case 6:
		std::cout << "�H�ϥ�q�W�h�A�@�� 100 ��\n";
		p->Pay(&bank, 100);
		break;
	case 7:
		std::cout << "�������r�A��|�v���A�Ȱ����1�^�X\n";
		p->inHospital = true;
		p->hosipitalDay = 1;
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
		bank.Pay(p, 100);
		break;
	case 1:
		std::cout << "�վ���������A�l��100��\n";
		p->Pay(&bank, 100);
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
		bank.Pay(p, 200);
		break;
	case 5:
		std::cout << "�ұo�|�ӳ��A��ú�� 150 ��\n";
		p->Pay(&bank, 150);
		break;
	case 6:
		std::cout << "�B���ٿ��A��o 120 ��\n";
		bank.Pay(p, 120);
		break;
	case 7:
		std::cout << "����G�ٺ��סA��O 80 ��\n";
		p->Pay(&bank, 80);
		break;
	case 8:
		std::cout << "�ͤ馬��§���A��o 50 ��\n";
		bank.Pay(p, 50);
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
	HorseRacing miniGame;
	switch (choice) {
	case 0:
		miniGame.init(p);
		miniGame.gameStart();
		break;
	case 1:
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



