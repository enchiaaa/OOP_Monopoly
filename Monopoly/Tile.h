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

	/**
	 * ���o���檺�C��
	 * @return �^���C��r��
	 */
	std::string getColor();

	/**
	 * ���a��F�����Ĳ�o���ĪG�]�i�Q�мg�^
	 */
	virtual void OnLand(Player* p);

	/**
	 * ��ܿﶵ���A�ѨϥΪ̰����
	 * @param idx ��e�ﶵ���ޡ]�i�Ψ���ܹw�ﶵ�^
	 * @param question ���D�ԭz
	 * @param options �i�ﶵ�ئC��
	 * @return ��ܪ����ޭ�
	 */
	int GetUserChoice(int idx, const std::string question, const std::vector<std::string> options);

	/**
	 * �H ASCII �ϥ���ܩЫε���
	 * @param n �Ыε��š]�μơ^
	 */
	void PrintHouse(int n);
};

/**
 * �_�I��]�C�^�X�g�L�|�[���^
 */
class StartTile : public Tile {
public:
	/**
	 * �غc�_�I��A�w�]�C���T
	 */
	StartTile();

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
	PropertyTile(int level, int price, std::string name);

	/**
	 * ���o�a��
	 */
	int GetPrice();

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
	ShopTile();

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
	HospitalTile();

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
	ChanceTile();

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
	FateTile();

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
	MiniGameTile();

	/**
	 * ���a��F�p�C�����Ĳ�o�ĪG
	 */
	void OnLand(Player* p) override;
};
