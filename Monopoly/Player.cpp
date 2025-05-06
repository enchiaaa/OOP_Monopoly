#include "Player.h"

Player::Player(std::string _name, std::string _color, int _money)
{
	name = _name;
	color = _color;
	money = _money;
	position = 0;
}


std::string Player::GetName()
{
	return name;
}


std::string Player::GetColor()
{
	return color;
}


int Player::GetMoney()
{
	return money;
}


int Player::GetPosition()
{
	return position;
}


void Player::SetPosition(int pos)
{
	this->position = pos;
}


std::vector<Item*> Player::GetItem()
{
	return item;
}

void Player::UseItem(Item* item)
{
}

void Player::AddItem(Item* _item)
{
	item.push_back(_item);
}

void Player::Move(int steps, int mapSize)
{
	position = (position + steps) % mapSize;
}

bool Player::BuyProperty(int price)
{
	if (money < price) {
		std::cout << "�]�������A���o�ʶR\n";
		return false;
	}
	else {
		money -= price;
		return true;
	}
}

void Player::Pay(Player* other, int amount)
{
	// �ˬd�O�_����������
	if (money < amount) {
		// �p�G�O�Ȧ�ίS���a�A���\���B���t
		if (name == "bank") {
			other->money += amount;
			this->money -= amount;
			return;
		}
		
		// �p�G�S�����������A�u��I�Ѿl�����B
		std::cout << name << " ��������A�u���I " << money << " ���C\n";
		other->money += money;
		this->money = 0;
		
		// �ˬd�O�_�}��
		std::cout << name << " �w�}���I\n";
	}
	else {
		// �����������A���`��I
		other->money += amount;
		this->money -= amount;
	}
}

void Player::PrintStatus()
{
	std::cout << "���a: " << name << " | ";
	std::cout << "��m: " << position << " | ";
	std::cout << "����: $" << money << std::endl;
}
