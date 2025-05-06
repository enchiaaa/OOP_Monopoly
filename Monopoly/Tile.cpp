#include "Tile.h"
#include "Player.h"
#include "HorseRacing.h"
#include <conio.h>
#include <string>

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

Player bank("bank", "black", 1 << 31);

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
		std::string question = "��F" + name + "�I�g�a���欰 " + std::to_string(price) + " ���A�O�_�n�ʶR�H";
		std::vector<std::string> options = {
		"�ʶR",
		"���"
		};
		int choice = GetUserChoice(level, question, options);

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

		std::string question = "�O�_�n��O 300 ���ɯŤg�a�H\n";
		std::vector<std::string> options = {
		"�ɯ�",
		"���"
		};
		int choice = GetUserChoice(level, question, options);

		if (choice == 0) {
			if (p->BuyProperty(price)) { // �����D �u��200�N�ɯŤF
				std::cout << "\n�ɯŦ��\�I\n";
				Upgrade();
			}
			else {
				std::cout << "���������A�L�k�ɯšC\n";
			}
		}
	}
	else if (owner != p) { // �����D ���i�H�ܭt��
		std::cout << "���g�a�� " << owner->GetName() << " ���g�a�A��I "
			<< price << " �� " << owner->GetName() << "�C\n";
		p->Pay(owner, price);
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
	int choice = GetUserChoice(3, question, options);
	
	if (choice) return;

	std::string welcome = "�w����{�ө�!";
	std::vector<std::string> goods = {
		"test1",
		"test2",
		"���}"
	};
	choice = GetUserChoice(3, welcome, goods);

	switch (choice) {
	case 0:
		break;
	case 1:
		break;
	default:
		break;
	}
}

HospitalTile::HospitalTile()
{
	color = "gray";
}

void HospitalTile::OnLand(Player* p)
{
	PrintHouse(4);
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
	GetUserChoice(5, "���|��A�ө�����|�a!!!", { "��" });
	char chance = rand() % 2;

	HorseRacing miniGame;
	switch (chance) {
	case 0:
		std::cout << "�N�~���i�䰨��\n";
		WaitForEnter();
		miniGame.init(p);
		miniGame.gameStart();
		break;
	case 1:
		std::cout << "���˼Ȱ����1��\n";
		p->inHospital = true;
		p->hosipitalDay = 2;
		break;
	}
}

FateTile::FateTile()
{
	color = "purple";
}

void FateTile::OnLand(Player* p)
{
	GetUserChoice(5, "�R�B��A�ө���R�B�a!!!", { "��" });
	char fate = rand() % 2;

	switch (fate) {
	case 0:
		std::cout << "�a�W�ߨ���]�A���D��100��\n";
		bank.Pay(p, 100);
		break;
	case 1:
		std::cout << "�վ���������A�l��100��\n";
		p->Pay(&bank, 100);
		break;
	}
}

MiniGameTile::MiniGameTile()
{
	color = "brown";
}

void MiniGameTile::OnLand(Player* p)
{
	int choice = GetUserChoice(5, "�C���p�C��", {
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

void Tile::PrintHouse(int level) {
	if (level < 0 || static_cast<size_t>(level) >= houses.size()) {
		std::cerr << "ĵ�i�GPrintHouse �� level �W�X�d�� (" << level << ")\n";
		return;
	}
	auto house = houses[level];
	for (size_t i = 0; i < house.size(); i++) {
		std::cout << house[i] << std::endl;
	}
	std::cout << std::endl;
}

int Tile::GetUserChoice(int idx, const std::string question, const std::vector<std::string> options) {

	int selected = 0;

	while (true) {
		std::cout << "\033[2J\033[H";
		std::cout << question << "\n\n";
		PrintHouse(idx);
		for (size_t i = 0; i < options.size(); ++i) {
			if (static_cast<int>(i) == selected)
				std::cout << " > " << "�i" << options[i] << "�j" << "\n";
			else
				std::cout << "   " << "�i" << options[i] << "�j" << "\n";
		}

		int key = _getch();
		if (key == 224) {
			key = _getch();
			if (key == 72) selected = (selected - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size());
			if (key == 80) selected = (selected + 1) % static_cast<int>(options.size());
		}
		else if (key == '\r') {
			return selected;
		}
	}
}

