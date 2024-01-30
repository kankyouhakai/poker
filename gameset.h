#pragma once
#include <stdbool.h>

#define RANK_COUNT 13   //�g�����v�̐���
#define SUIT_COUNT 4    //�g�����v�̊G�D�̐�

typedef enum {
    clab,      //0
    dia,      //1
    heart,        //2
    spade        //3
}SUIT;

typedef enum {
    bet,
    check,
    call,
    raise,
    fold
}BET;

typedef struct {        //�g�����v�̍\����
    int cardRank;       //����
    SUIT cardSuit;      //�G�D
    bool isDeploy;      //�z�u����Ă��邩

}TrumpInfo;

typedef enum {
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_CARD,
    FLASH,
    STRAIGHT,
    FULL_HOUSE,
    FOUR_CARD,
    STRAIGHT_FLASH
}ROLE;


typedef struct MemberInfo {        //�����o�[�̍\����
    char name[32];      //���O
    int point;          //�|�C���g
    int chip;           //�`�b�v
    int id;
    bool isCOM;
    TrumpInfo* deck[2]; //��D
    int stake;
    bool isDied;
    ROLE ownRole;
    //int numOfDeck; 
    //type role; //��
    struct MemberInfo* nextMember;
}MemberInfo;

typedef struct {
    int round;
    int game;
    int ante;
    int blind;
    int callchip;
    int pot;
    TrumpInfo* communityCard[5];
}TableInfo;

MemberInfo* NextMember(MemberInfo* memberPtr, int count) {
    if (count > 0 || (memberPtr)->isDied) {
        memberPtr = (memberPtr)->nextMember;
        memberPtr = NextMember(memberPtr, count - !memberPtr->isDied);
    }
    return memberPtr;
}

void MoveChip(int* source, int* dest, int amount) {
    if (amount > *source) {
        amount = *source;
    }
    *dest += amount;
    *source -= amount;
}

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
    int randomRank = 0;
    SUIT randomSuit = 0;
    do {                                            //�����_���Ƀg�����v��I��
        randomRank = rand() % RANK_COUNT;
        randomSuit = rand() % SUIT_COUNT;
    } while (trump[randomRank][randomSuit].isDeploy);   //�܂��I�΂�Ă��Ȃ��g�����v���o��܂Ń��[�v
    trump[randomRank][randomSuit].isDeploy = true;

    return &trump[randomRank][randomSuit];              //�I�΂ꂽ�g�����v�̃A�h���X��Ԃ�
}

void MakeTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = (i + 2) % RANK_COUNT;             //�g�����v�̐�����ݒ�
            trump[i][j].cardSuit = (SUIT)j;         //�g�����v�̊G�D��ݒ�
            trump[i][j].isDeploy = false;
        }
    }
}


