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
    int chip;           //チップ
    int id;
    TrumpInfo* deck[2]; //手札
    //int numOfDeck; 
    //type role; //役
}PlayerInfo;

int MakeMember(PlayerInfo**); //メンバー作成

void poker(void);   //ポーカー実行

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]);

void RoleJudge(PlayerInfo*, TrumpInfo* communityCard[5]); //役判定


int main(void) {
    poker();

    return 0;
}

int MakeMember(PlayerInfo** memberPtr) {
    int cpuCount = 0;
    int playerCount = 1;
    int initChip = 0;
    printf("comの人数を入力：");
    scanf("%d", &cpuCount);

    *memberPtr = (PlayerInfo*)calloc(sizeof(PlayerInfo), playerCount + cpuCount);
    if (*memberPtr != NULL) { //メンバーが正しく作成されたか
        printf("プレイヤーの名前を入力：");
        scanf("%s", (*memberPtr)[0].name);
        printf("チップの初期枚数を入力：");
        scanf("%d", &initChip);
        (*memberPtr)[0].id = 0; //プレイヤーIDの設定
        (*memberPtr)[0].chip = initChip;

        for (int i = playerCount; i < playerCount + cpuCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);
            (*memberPtr)[i].id = 100 + i;   //COMのIDの設定
            (*memberPtr)[i].chip = initChip;
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
    TrumpInfo* communityCard[5] = { NULL };
    int round = 1;
    int game = 1;

    //トランプカードの作成
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = i+1;             //トランプの数字を設定
            //trump[i][j].cardSuit = (SUIT)j;
            strcpy(trump[i][j].cardSuit, suit[j]);  //トランプの絵札を設定
            trump[i][j].isDeploy = false;
        }
    }

    numOfMember = MakeMember(&member);  //メンバーの作成

    //メンバーの確認
    //for (int i = 0; i < numOfMember; i++) {
    //	sck(member[i].name);
    //	ick(member[i].id)
    //}

    bool testflag = true;
    //mainloop
    while (testflag) {
        if (round == 1) {
            //手札の配布
            for (int i = 0; i < numOfMember; i++) {
                for (int j = 0; j < sizeof(member[i].deck) / sizeof(TrumpInfo*); j++) {
                    member[i].deck[j] = ChoiceTrump(trump);
                }

            }
            //プリフロップ

        }
        //コミュニティカードの配布
        int i = 0;
        do {
            communityCard[i] = ChoiceTrump(trump);
            i++;
        } while (communityCard[2] == NULL);
       
        
        //役の判定
        RoleJudge(&member[0], communityCard);


        scanf("%s");
    }
}

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]) {
    int randRank = 0;
    int randSuit = 0;
    do {                                    //ランダムにトランプを選ぶ
        randRank = rand() % RANK_COUNT;
        randSuit = rand() % SUIT_COUNT;
    } while (trump[randRank][randSuit].isDeploy);
    trump[randRank][randSuit].isDeploy = true;

    return &trump[randRank][randSuit];
}

void RoleJudge(PlayerInfo* member, TrumpInfo* communityCard[5]) {
    sck(member->name);              //メンバー名を表示
    ick(member->deck[0]->cardRank); //0枚目の手札の数字を表示
    sck(member->deck[0]->cardSuit); //0枚目の手札の絵柄を表示
    ick(member->deck[1]->cardRank);
    sck(member->deck[1]->cardSuit);
    printf("\n");
    for (int i = 0; i < 5; i++) {
        pck(communityCard[i]);
    }
    printf("\n");

}


