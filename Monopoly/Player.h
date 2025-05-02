#pragma once
#include <vector>
#include <iostream>
#include "Item.h"
#include "Tile.h"

class Player
{
public:
	/**
	 * �غc�l�G��l�ƪ��a�W�١B�C��P�_�l����
	 */
	Player(std::string name, std::string color, int money);

	/** �O�_�b��|�� */
	bool inHospital = false;

	/** ��|�Ѽ� */
	int hosipitalDay = 0;

	/**
	 * ���o���a�W��
	 */
	std::string GetName();

	/**
	 * ���o���a�C��
	 */
	std::string GetColor();

	/**
	 * ���o�ثe����
	 */
	int GetMoney();

	/**
	 * ���o�ثe��m
	 */
	int GetPosition();

	/**
	 * �]�w���a��m
	 */
	void SetPosition(int position);

	/**
	 * ���o���a�ҫ������D��
	 */
	std::vector<Item*> GetItem();

	/**
	 * �ϥιD��]�|����@�^
	 */
	void UseItem(Item* item);

	/**
	 * �W�[�@�ӹD��]�|����@�^
	 */
	void AddItem(Item* item);

	/**
	 * ���Y���I�Ʋ��ʪ��a��m
	 * @param steps �Y���
	 * @param mapSize �a���`���
	 */
	void Move(int steps, int mapSize);

	/**
	 * �ʶR�a��
	 * @param price ����
	 * @return �O�_�ʶR���\
	 */
	bool BuyProperty(int price);

	/**
	 * �I������L���a
	 * @param other �������������a
	 * @param amount ���B
	 */
	void Pay(Player* other, int amount);

	/**
	 * �L�X���a�ثe���A
	 */
	void PrintStatus();

private:
	/** ���a�W�� */
	std::string name;

	/** ���a�C�� */
	std::string color;

	/** ���a���� */
	int money;

	/** ���a�ثe��m�]�a�Ϯ�s���^ */
	int position;

	/** �����D��M�� */
	std::vector<Item*> item;
};
