#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#define RANK_COUNT 13   //トランプの数字
#define SUIT_COUNT 4    //トランプの絵札の数

typedef enum {
    spade,
    clab,
    heart,
    dia
}SUIT;

typedef struct {        //トランプの構造体
    int cardRank;       //数字
    //SUIT cardSuit;
    char cardSuit[6];   //絵札
    bool isDeploy;      //配置されているか

}TrumpInfo;

typedef struct {        //メンバーの構造体
    char name[20];      //名前
    int point;          //ポイント
    int id;
    TrumpInfo* deck[5]; //手札
    //int numOfDeck; 
    //type role; //役
}PlayerInfo;

int MakeMember(PlayerInfo**, bool); //メンバー作成

void poker(void);   //ポーカー実行

void RoleJudge(PlayerInfo*); //役判定

int main(void) {
    poker();

    return 0;
}

int MakeMember(PlayerInfo** memberPtr, bool isMultPlayer) {
    int cpuCount = 0;
    int playerCount = 0;
    if (isMultPlayer) {
        printf("プレイヤーの数を入力：");
        scanf("%d", &playerCount);
    }
    else {
        playerCount = 1;
    }
    printf("comの人数を入力：");
    scanf("%d", &cpuCount);

    *memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + cpuCount);
    if (*memberPtr != NULL) { //メンバーが正しく作成されたか
        for (int i = 0; i < playerCount; i++) {
            printf("プレイヤー%dの名前を入力：", i+1);

            scanf("%s", (*memberPtr)[i].name);
            //sprintf((*memberPtr)[i].name, "\x1b[31mPLAYER%d\x1b[0m", i + 1);
            (*memberPtr)[i].id = i; //プレイヤーIDの設定
        }
        for (int i = playerCount; i < playerCount + cpuCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);
            (*memberPtr)[i].id = 100 + i;   //COMのIDの設定

        }
    }
    return playerCount + cpuCount;
}

void poker(void) { //ポーカー
    srand((unsigned int)time(NULL));                            //乱数の初期化
    PlayerInfo* member = NULL;                                  //メンバーを格納する構造体配列
    int numOfMember = 0;                                        //メンバーの数
    const char suit[][6] = { "spade", "clab", "heart", "dia" }; //絵札の種類
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0,(SUIT)0 };    //トランプカードを格納する構造体配列
    TrumpInfo* deck[RANK_COUNT * SUIT_COUNT] = { NULL };        //山札を格納する配列
    TrumpInfo** currMasterDeck = deck;                          //山札の一番上のカードを格納する構造体変数

    //トランプカードの作成
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = i+1;             //トランプの数字を設定
            //trump[i][j].cardSuit = (SUIT)j;
            strcpy(trump[i][j].cardSuit, suit[j]);  //トランプの絵札を設定
            trump[i][j].isDeploy = false;
        }
    }
    
    //山札の作成
    for (int i = 0; i < RANK_COUNT * SUIT_COUNT; i++) {
        int randRank = 0;
        int randSuit = 0;
        do {                                    //ランダムにトランプを選ぶ
            randRank = rand() % RANK_COUNT;
            randSuit = rand() % SUIT_COUNT;
        } while (trump[randRank][randSuit].isDeploy);

        deck[i] = &trump[randRank][randSuit];   //手札にトランプを加える
        deck[i]->isDeploy = true;
    }

    //山札の確認
    for (int i = 0; i < RANK_COUNT * SUIT_COUNT; i++) {
        ick(i);
        ick(deck[i]->cardRank);
        sck(deck[i]->cardSuit);
        printf("\n");
    }

    numOfMember = MakeMember(&member, false);   //メンバーの作成
    for (int i = 0; i < numOfMember; i++) {     //初期ポイントの設定
        member[i].point = 1000;
    }

    //メンバーの確認
    //for (int i = 0; i < numOfMember; i++) {
    //	sck(member[i].name);
    //	ick(member[i].id)
    //}

    bool testflag = true;
    //mainloop
    while (testflag) {
        for (int i = 0; i < numOfMember; i++) { //手札の配布
            for (int j = 0; j < 5; j++) {
                member[i].deck[j] = *currMasterDeck;
                currMasterDeck++;
            }
            RoleJudge(&member[i]);  //役の判定
        }
        scanf("%s");

    }
}

void RoleJudge(PlayerInfo* member) {
    sck(member->name);              //メンバー名を表示
    ick(member->deck[0]->cardRank); //0枚目の手札の数字を表示
    sck(member->deck[0]->cardSuit); //0枚目の手札の絵柄を表示
}


