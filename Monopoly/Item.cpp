#include "Item.h"
#include "Player.h"
#include "Monopoly.h"
#include "Game.h"
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>

void ControlDiceItem::Use(Player* p)
{
    // �����l�G�����a����Y��l�I�ƪ��ĪG
    std::cout << "�п�ܭn�e�i���B�� (1-6)�G\n";
    
    int selected = 1;
    bool confirmed = false;
    
    // �ϥΤ�V��W�U��ܭn�����B��
    while (!confirmed) {
        std::cout << "\033[2J\033[H"; // �M�ſù�
        std::cout << "�����l�G�п�ܭn�e�i���B��\n\n";
        
        // ��ܿﶵ
        for (int i = 1; i <= 6; ++i) {
            if (i == selected) {
                std::cout << " > " << "�i" << i << "�j" << "\n";
            } else {
                std::cout << "   " << "�i" << i << "�j" << "\n";
            }
        }
        
        // �����ϥΪ̿�J
        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72) selected = (selected + 4) % 6 + 1; // �W
            if (key == 80) selected = selected % 6 + 1; // �U
        }
        else if (key == '\r') { // Enter ��T�{
            confirmed = true;
        }
    }
    
    std::cout << "�A��ܤF�e�i " << selected << " �B�I\n";
    
    // ���o�a�Ϥj�p�]���B���]��28��A��ڤW������ Game ���O�ά����޿�^
    // �b�u��������A�o�ӭ����ӱq�C���޿褤���
    const int mapSize = 28;
    
    // ���ʪ��a
    p->Move(selected, mapSize);
}

void RocketCard::Use(Player* p) {
    // ����Ҧ����a�C�� (�ݭn�X��Game��H)
    auto game = Monopoly::game;
    if (!game) {
        std::cout << "�C������l�ơI" << std::endl;
        return;
    }
    
    // �إߪ��a�ﶵ
    std::vector<std::string> playerOptions;
    std::vector<Player*> availablePlayers;
    
    // ���]�ڭ̥i�H�q�Y�Ӧa������Ҧ����a�C��
    // �o�̨ϥ�getCurrentPlayer��k�M�@�ǰ��]�Ӻt��
    Player* currentPlayer = game->getCurrentPlayer();
    
    // �M���Ҧ����a�A�ư���e���a�ۤv
    for (int i = 0; i < 4; i++) { // ���]�̦h4�Ӫ��a
        Player* otherPlayer = nullptr;
        
        // �ϥ�²�檺��k�ӴM���L���a
        // ��ڱ��p���Ӧ���n����k������Ҧ����a
        if (i == 0 && currentPlayer != game->getCurrentPlayer()) {
            otherPlayer = game->getCurrentPlayer();
        }
        
        // ���L��e���a�M���s�b�����a
        if (!otherPlayer || otherPlayer == p) {
            continue;
        }
        
        // �K�[��ﶵ��
        playerOptions.push_back(otherPlayer->GetName());
        availablePlayers.push_back(otherPlayer);
    }
    
    // �p�G�S����L���a�i��
    if (playerOptions.empty()) {
        std::cout << "�S���i�H���w�����a�I" << std::endl;
        return;
    }
    
    playerOptions.push_back("����");
    
    // ���Τ��ܤ@�Ӫ��a
    std::string question = "��ܭn�e�J��|�����a�G";
    int choice = Monopoly::GetUserChoice(question, playerOptions, true, false);
    
    // �p�G�Τ��ܨ���
    if (choice >= static_cast<int>(availablePlayers.size())) {
        std::cout << "�����ϥΤ��b�d�C" << std::endl;
        return;
    }
    
    // �N��ܪ����a�e�J��|
    Player* targetPlayer = availablePlayers[choice];
    targetPlayer->inHospital = true;
    targetPlayer->hosipitalDay = 0;  // �q0�}�l�p�ơA�ݭn�L��^�X
    
    std::cout << targetPlayer->GetName() << " �Q�e�J��|�A�N��|��^�X�I" << std::endl;
}

void FateCard::Use(Player* p) {
    // Ĳ�o�R�B�ƥ�
    std::cout << "�ϥΩR�B�d�AĲ�o�R�B�ƥ�I" << std::endl;
    
    // �H���M�w�R�B�ƥ�
    int eventType = rand() % 5;
    
    switch (eventType) {
        case 0:
            // ��o����
            {
                int amount = 100 + rand() % 400;  // 100-500���H����
                p->BuyProperty(-amount);  // �t�ȥN����o��
                std::cout << "�n�B���{�I�A��o�F $" << amount << " �����y�I" << std::endl;
            }
            break;
        case 1:
            // ���h����
            {
                int amount = 50 + rand() % 200;  // 50-250���H����
                p->BuyProperty(amount);  // ���ȥN���h��
                std::cout << "�������{�I�A���h�F $" << amount << "�I" << std::endl;
            }
            break;
        case 2:
            // �e�i�X�B
            {
                int steps = 1 + rand() % 6;  // 1-6���H����
                const int mapSize = 28;  // ���]�a�Ϥj�p
                p->Move(steps, mapSize);
                std::cout << "�A�Q�R�B�����j�V�e�� " << steps << " �B�I" << std::endl;
            }
            break;
        case 3:
            // ��h�X�B
            {
                int steps = -1 * (1 + rand() % 3);  // -1��-3���H����
                const int mapSize = 28;  // ���]�a�Ϥj�p
                p->Move(steps + mapSize, mapSize);  // �[�W�a�Ϥj�p�H�B�z�t��
                std::cout << "�A�Q�R�B�����j�V��� " << -steps << " �B�I" << std::endl;
            }
            break;
        case 4:
            // ��o�D��
            {
                Item* newItem = new ControlDiceItem();
                p->AddItem(newItem);
                std::cout << "�R�B�����A�@�i�����l�D��d�I" << std::endl;
            }
            break;
    }
}
