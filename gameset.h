#pragma once
#include <stdbool.h>

#define RANK_COUNT 13   //トランプの数字
#define SUIT_COUNT 4    //トランプの絵札の数

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

typedef struct {        //トランプの構造体
    int cardRank;       //数字
    SUIT cardSuit;      //絵札
    bool isDeploy;      //配置されているか

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


typedef struct MemberInfo {        //メンバーの構造体
    char name[32];      //名前
    int point;          //ポイント
    int chip;           //チップ
    int id;
    bool isCOM;
    TrumpInfo* deck[2]; //手札
    int stake;
    bool isDied;
    ROLE ownRole;
    //int numOfDeck; 
    //type role; //役
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
    do {                                            //ランダムにトランプを選ぶ
        randomRank = rand() % RANK_COUNT;
        randomSuit = rand() % SUIT_COUNT;
    } while (trump[randomRank][randomSuit].isDeploy);   //まだ選ばれていないトランプが出るまでループ
    trump[randomRank][randomSuit].isDeploy = true;

    return &trump[randomRank][randomSuit];              //選ばれたトランプのアドレスを返す
}

void MakeTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = (i + 2) % RANK_COUNT;             //トランプの数字を設定
            trump[i][j].cardSuit = (SUIT)j;         //トランプの絵札を設定
            trump[i][j].isDeploy = false;
        }
    }
}


