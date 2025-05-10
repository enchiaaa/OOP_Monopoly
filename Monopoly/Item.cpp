#include "Item.h"
#include "Player.h"
#include <conio.h>
#include <iostream>
#include <string>

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
