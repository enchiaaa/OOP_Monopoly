#include "Player.h"
#include "Game.h"  // �s�W Game ���O

// �~���ŧi�A�ݦb��@���ϥ� 
extern Game* gameInstance; // �N�b Game ����@����l��

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
	// �ˬd�D��O�_���ū��w
	if (!item) {
		std::cout << "���~�G���ըϥεL�Ī��D��I\n";
		return;
	}

	// �ˬd���a�O�_�֦��ӹD��
	bool hasItem = false;
	size_t itemIndex = 0;
	for (size_t i = 0; i < this->item.size(); ++i) {
		if (this->item[i] == item) {
			hasItem = true;
			itemIndex = i;
			break;
		}
	}

	if (!hasItem) {
		std::cout << "���~�G���a���֦����D��I\n";
		return;
	}

	// �ϥιD��ĪG
	std::cout << "���a " << name << " �ϥΤF " << item->GetName() << "�I\n";
	std::cout << "�ĪG�G" << item->GetDescription() << "\n";
	
	// �I�s�D�㪺��ڮĪG���
	item->Use(this);
	
	// �ϥΫ�q���a���D��M�椤����
	this->item.erase(this->item.begin() + itemIndex);
	
	// ����D��O����Ŷ�
	delete item;
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
	// �S��B�z�Ȧ榬�I
	if (other && other->GetName() == "bank") {
		// ��I���Ȧ�A�u�ݴ�֦ۤv�����B
		if (money < amount) {
			// �`�N�G���B�O"�Q�ʤ�I"�A���B�����ɭP�l�B�p��0�~��}��
			std::cout << name << " ��������A�u���I " << money << " �����Ȧ�C\n";
			int oldMoney = money;
			money = 0; // �]��0�A�Ӥ��O�t��
			std::cout << name << " �w��I�������B " << oldMoney << " ���ï}���I\n";
			
			// �ˬd�C���O�_���ӵ����]�}����u��1�W���a�^
			if (gameInstance) {
				gameInstance->CheckWinCondition();
			}
		} else {
			money -= amount;
		}
		return;
	}
	
	if (name == "bank") {
		// �Ȧ��I�����a�A���Ҽ{�Ȧ�l�B
		if (other) {
			// �T�O���a���ڤ��|����
			if (amount > 0 && other->money > INT_MAX - amount) {
				std::cout << "ĵ�i�G���B�L�j�A�]�w���a������̤j�ȡC\n";
				other->money = INT_MAX;
			} else {
				other->money += amount;
			}
		}
		return;
	}
	
	// �@�몱�a����� - �Q�ʤ�I
	if (money < amount) {
		// �p�G�S�����������A�u��I�Ѿl�����B�A�B�}��
		std::cout << name << " ��������A�u���I " << money << " ���C\n";
		
		// �T�O���ڤ褣�|����
		if (money > 0 && other && other->money > INT_MAX - money) {
			std::cout << "ĵ�i�G�����B�N����A�]�w���̤j�ȡC\n";
			other->money = INT_MAX;
		} else if (other) {
			other->money += money;
		}
		
		money = 0; // �]��0�A��ܤw�Χ��Ҧ�����
		std::cout << name << " �L�k������I�A�w�}���I\n";
		
		// ���a�}���ɡ]�B�O�]���I�ڵ���L���a�^�A�ಾ�Ҧ��a�����ťD
		if (other && gameInstance) {
			gameInstance->HandlePlayerBankruptcy(this, other);
			
			// �}����ߧY�ˬd�C���O�_���ӵ����]�u�Ѥ@�H�^
			if (gameInstance->CheckWinCondition()) {
				gameInstance->EndGame();
			}
		}
	}
	else {
		// �����������A���`��I
		if (amount > 0 && other) {
			// �T�O���ڤ褣�|����
			if (other->money > INT_MAX - amount) {
				std::cout << "ĵ�i�G�����B�N����A�]�w���̤j�ȡC\n";
				other->money = INT_MAX;
			} else {
				other->money += amount;
			}
		}
		
		money -= amount; // ��I�覩��
	}
}

void Player::PrintStatus()
{
	std::cout << "���a: " << name << " | ";
	std::cout << "��m: " << position << " | ";
	std::cout << "����: $" << money << std::endl;
}
