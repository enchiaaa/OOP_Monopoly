#pragma once
#include <iostream>
#include <vector>

class Player;

/**
 * �a�Ϯ������w�q
 */
enum TileType {
	START,
	PROPERTY,
	SHOP,
	HOSPITAL,
	CHANCE,
	FATE,
	GAME,
};

/**
 * ��H��¦���O�G�a�Ϯ�]Tile�^
 */
class Tile
{
public:
	std::string color = "";	// �x�s�C���T
	int number = 0;

	/**
	 * return name
	 */
	virtual std::string GetName();

	/**
	 * ���o�檺�C��
	 */
	std::string getColor();

	/**
	 * ���a���o��|Ĳ�o���ƥ�]�l����@���餺�e�^
	 */
	virtual void OnLand(Player* p);
};

/**
 * �_�I��]�C�^�X�g�L�|�[���^
 */
class StartTile : public Tile {
private:
	std::string name = "start";
public:
	/**
	 * �غc�_�I��A�w�]�C���T
	 */
	StartTile(int);

	/**
	 * ���o�W��
	 */
	std::string GetName() override;

	/**
	 * ���a��F�_�I���Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};

/**
 * �a����]�i�ʶR�P�����^
 */
class PropertyTile : public Tile {
private:
	int level;
	int price;
	std::string name;
	Player* owner;

public:
	/**
	 * �غc�a����
	 * @param id �a������
	 * @param price �a��
	 * @param name �a�W
	 */
	PropertyTile(int level, int price, std::string name, int n);

	/**
	 * ���o�W��
	 */
	std::string GetName() override;

	/**
	 * ���o�a��
	 */
	int GetPrice();

	/**
	 * ���o�a������
	 * @return �a������
	 */
	int GetLevel();

	/**
	 * �ɯŦa��
	 */
	void Upgrade();

	/**
	 * ���o�a���֦���
	 */
	Player* GetOwner();

	/**
	 * �]�w�a���֦���
	 */
	void SetOwner(Player* p);

	/**
	 * ���a��F���a�����Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};

/**
 * �ө���]�i�ʶR�D��^
 */
class ShopTile : public Tile {
public:
	/**
	 * �غc�_�I��A�w�]�C���T
	 */
	ShopTile(int n);

	/**
	 * ���a��F�ө����Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};

/**
 * ��|��]��|�Ȱ��^�X�^
 */
class HospitalTile : public Tile {
public:
	/**
	 * �غc�_�I��A�w�]�C���T
	 */
	HospitalTile(int n);

	/**
	 * ���a��F��|���Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};

/**
 * ���|��]Ĳ�o�H���ƥ�^
 */
class ChanceTile : public Tile {
public:
	/**
	 * �غc�_�I��A�w�]�C���T
	 */
	ChanceTile(int n);

	/**
	 * ���a��F���|���Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};

/**
 * �R�B��]Ĳ�o�����ĪG�^
 */
class FateTile : public Tile {
public:
	/**
	 * �غc�_�I��A�w�]�C���T
	 */
	FateTile(int n);

	/**
	 * ���a��F�R�B���Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};

/**
 * �p�C����]Ĳ�o�p�C���^
 */
class MiniGameTile : public Tile {
public:
	/**
	 * �غc�_�I��A�w�]�C���T
	 */
	MiniGameTile(int n);

	/**
	 * ���a��F�p�C�����Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};
