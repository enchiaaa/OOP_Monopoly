#pragma once
#include <vector>
#include <iostream>
#include "Item.h"
#include "Tile.h"

class Player
{
public:
	/**
	 * �غc�l�G�H�K��ӦW�r�B�C�����i�h
	 */
	Player(std::string name, std::string color, int money);

	/** �O�_�b��|�� */
	bool inHospital = false;

	/** ��|�Ѽ� */
	int hosipitalDay = 0;

	/**
	 * �����a�W�r
	 */
	std::string GetName();

	/**
	 * �����a�C��
	 */
	std::string GetColor();

	/**
	 * �ݬݳѦh�ֿ�
	 */
	int GetMoney();

	/**
	 * �ݬݦb���@��
	 */
	int GetPosition();

	/**
	 * �⪱�a���Y�@��
	 */
	void SetPosition(int position);

	/**
	 * ���쪱�a���D��M��
	 */
	std::vector<Item*> GetItem();

	/**
	 * �ϥιD��]�٨S���n�^
	 */
	void UseItem(Item* item);

	/**
	 *  �N item �[�J vector
	 */
	void AddItem(Item* item);

	/**
	 * ���ʦ�m�G�N�O position + steps �A mod �@�U�a�Ϥj�p
	 */
	void Move(int steps, int mapSize);

	/**
	 * �R�D��G�����N���@���^�� true�A�S���N�ĨĦ^�� false
	 */
	bool BuyItem(int price);

	/**
	 *  �N property(tile) �[�J vector
	 */
	void AddProperty(Tile* p);

	/**
	 * �R�a�G�����N���@���^�� true�A�S���N�ĨĦ^�� false
	 */
	bool BuyProperty(int price);

	/**
	 * ��a
	 */
	void SellProperty(Tile* p);

	/**
	 * �^�� property vector
	 */
	std::vector<Tile*> GetProperty();

	/**
	 * �I���G����൹�O�H
	 */
	void Pay(Player* other, int amount);

	/**
	 * �L�X���a��T�Gԣ�W�r�B��m��h�ֿ�
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

	/** �֦����g�a */
	std::vector<Tile*>property;
};
