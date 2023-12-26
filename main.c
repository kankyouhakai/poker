#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#define RANK_COUNT 13   //トランプの数字
#define SUIT_COUNT 4    //トランプの絵札の数

typedef enum {
    spade,      //0
    heart,      //1
    dia,        //2
    clab        //3
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
    //char cardSuit[6];
    bool isDeploy;      //配置されているか

}TrumpInfo;

typedef struct {        //メンバーの構造体
    char name[20];      //名前
    int point;          //ポイント
    int chip;           //チップ
    int id;
    bool isCOM;
    TrumpInfo* deck[2]; //手札
    int ownPot;
    //int numOfDeck; 
    //type role; //役
}MemberInfo;


int MakeMember(MemberInfo**); //メンバー作成

void poker(void);   //ポーカー実行

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]); //手札，コミュニティカードの選択

void RoleJudge(MemberInfo*, TrumpInfo* communityCard[5]); //役判定

void NextMember(MemberInfo** memberPtr, int count, MemberInfo* begin);

bool IsBettinground(MemberInfo* members, int count);

int main(void) {
    
    poker();

    return 0;
}

int MakeMember(MemberInfo** memberPtr) {
    int comCount = 0;           //COMの人数
    int playerCount = 1;        //プレイヤーの人数(1人固定)
    int initChip = 0;           //チップの初期枚数
    printf("comの人数を入力：");
    scanf("%d", &comCount);
    *memberPtr = (MemberInfo*)calloc(sizeof(MemberInfo), playerCount + comCount + 1);
    if (*memberPtr != NULL) { //メンバーが正しく作成されたか
        printf("プレイヤーの名前を入力：");
        scanf("%s", (*memberPtr)[0].name);

        printf("チップの初期枚数を入力：");
        scanf("%d", &initChip);
         
        (*memberPtr)[0].id = 1; //プレイヤーIDの設定
        (*memberPtr)[0].isCOM = false;
        (*memberPtr)[0].chip = initChip;

        for (int i = playerCount; i < playerCount + comCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);

            (*memberPtr)[i].id = i + 1;   //COMのIDの設定
            (*memberPtr)[i].isCOM = true;
            (*memberPtr)[i].chip = initChip;
        }
    }
    else {
        exit(1);
    }

    return playerCount + comCount;
}

void NextMember(MemberInfo** memberPtr, int count, MemberInfo* begin) {
    for (int i = 0; i < count; i++) {
        if ((*memberPtr + 1)->id == 0) {
            *memberPtr = begin;
        }
        else {
            (*memberPtr)++;
        }
    }
}

void poker(void) { //ポーカー
    srand((unsigned int)time(NULL));                            //乱数の初期化
    MemberInfo* members = NULL;                                  //メンバーを格納する構造体配列
    int numMembers = 0;
    MemberInfo* smallBlind;
    MemberInfo* bigBlind;                                   //メンバーの数
    MemberInfo* currBetMember;
    const char suit[][15] = { "spade", "\x1b[31mheart\x1b[0m", "\x1b[31mdia\x1b[0m", "clab" }; //絵札の種類
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //トランプカードを格納する構造体配列
    TrumpInfo* communityCard[5] = { NULL };                     //コミュニティカードを格納する構造体配列
    int round = 0;                                              //ラウンド
    int game = 0;                                               //ゲーム
    int numCommCards = 0;
    int ante = 50;
    int blind = 0;
    BET choiceAct = 0;
    const char* betAct[] = { "bet", "check", "call", "raise", "fold" };

    //トランプカードの作成
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = i + 1;             //トランプの数字を設定
            trump[i][j].cardSuit = (SUIT)j;         //トランプの絵札を設定
            trump[i][j].isDeploy = false;
            //strcpy(trump[i][j].cardSuit, suit[j]);  //トランプの絵札を設定
        }
    }
    numMembers = MakeMember(&members);  //メンバーの作成

    //members[numMembers]; //メンバーの終端
    //mainloop
    while (1) {
        game++;

        //ブラインドする人の設定
        smallBlind = &members[(game-1) % numMembers];
        bigBlind = &members[(game) % numMembers];
        currBetMember = &members[(game + 1) % numMembers];

        //ビッグブラインド
        bigBlind->ownPot = (bigBlind->chip >= ante ? ante : bigBlind->chip);
        bigBlind->chip = (bigBlind->chip >= ante ? bigBlind->chip - ante : 0);
        ick(bigBlind->ownPot);

        //スモールブラインド
        smallBlind->ownPot = (smallBlind->chip >= (ante / 2) ? ante / 2 : smallBlind->chip);
        smallBlind->chip = (smallBlind->chip >= (ante / 2) ? smallBlind->chip - (ante / 2) : 0);
        ick(smallBlind->ownPot);

        //手札の配布
        for (int i = 0; i < numMembers; i++) {                                     //配布されるメンバー
            for (int j = 0; j < sizeof(members[i].deck) / sizeof(TrumpInfo*); j++) { //2枚配布する
                members[i].deck[j] = ChoiceTrump(trump);
            }
        }
        //ラウンド開始
        for (round = 1; round <= 4; round++) {
            ick(round);
            //ベッティングラウンド
            do{
                
                currBetMember->ownPot = 20;
                ick(IsBettinground(members, numMembers));
                NextMember(&currBetMember, 1, members);
            } while (!IsBettinground(members, numMembers));
            
            
            ick(round);
            stop();
            ick(round);
            if (round == 4) {
                //ショーダウン
                for (int i = 0; i < numMembers; i++) {
                    RoleJudge(&members[i], communityCard);
                }
            }
            else {
                //コミュニティカードの配布
                do {
                    communityCard[numCommCards] = ChoiceTrump(trump);
                    numCommCards++;
                } while (communityCard[2] == NULL);
                RoleJudge(&members[0], communityCard);
            }
        }
        //ラウンド終了

        //手札の削除
        for (int i = 0; i < numMembers; i++) {
            members[i].deck[0]->isDeploy = false;
            members[i].deck[1]->isDeploy = false;
            members[i].deck[0] = NULL;
            members[i].deck[1] = NULL;
        }
        //コミュニティカードの削除
        for (;numCommCards > 0; numCommCards--) {
            communityCard[numCommCards - 1]->isDeploy = false;
            communityCard[numCommCards - 1] = NULL;
        }

        //ブラインドを変更
        NextMember(&smallBlind, 1, members);
        NextMember(&bigBlind, 1, members);

        stop();
    }
    
    int choice = 0;
    scanf("%d", &choice);
    ick(choice);
    switch (choice) {
    case(1):
        ick(choice);
        break;

    case(2):
        ick(choice);
        break;

    default:
        break;
    }
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

bool IsBettinground(MemberInfo* members, int count) {
    for (int i = 0; i < count-1; i++) {
        if (members[i].ownPot != members[i + 1].ownPot) {
            return false;
        }
    }
    return true;
}

void RoleJudge(MemberInfo* member, TrumpInfo* communityCard[5]) {
    //sck(member->name);
    //ick(member->deck[0]->cardRank);
    //ick(member->deck[0]->cardSuit);
    //printf("\n");
}


void CpuMove(void) {


}