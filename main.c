#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debuglib.h"
#include <math.h>
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

struct MemberInfo{        //メンバーの構造体(typedef宣言すると自身のポインタを定義できない)
    char name[32];      //名前
    int point;          //ポイント
    int chip;           //チップ
    int id;
    bool isCOM;
    TrumpInfo* deck[2]; //手札
    int ownPot;
    bool isDied;
    //int numOfDeck; 
    //type role; //役
    struct MemberInfo* nextMember;
};

typedef struct MemberInfo MemberInfo;

int MakeMember(MemberInfo**); //メンバー作成

void poker(void);   //ポーカー実行

TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]); //手札，コミュニティカードの選択

void RoleJudge(MemberInfo*, TrumpInfo* communityCard[5]); //役判定

MemberInfo* NextMember(MemberInfo* memberPtr, int count);

bool IsBettinground(MemberInfo* members, int count);

void Pay(MemberInfo* member, int chip);

void MoveChip(int* source, int* dest, int amount);


int main(void) {
    srand((unsigned int)time(NULL));//乱数の初期化

    poker();

    return 0;
}

int MakeMember(MemberInfo** memberPtr) {
    int comCount = 0;           //COMの人数
    int playerCount = 1;        //プレイヤーの人数(1人固定)
    int initChip = 0;           //チップの初期枚数
    printf("comの人数を入力：");
    scanf("%d", &comCount);
    *memberPtr = (MemberInfo*)calloc(sizeof(MemberInfo), playerCount + comCount);

    if (*memberPtr != NULL) { //メンバーが正しく作成されたか
        printf("プレイヤーの名前を入力：");
        scanf("%s", (*memberPtr)[0].name);

        printf("チップの初期枚数を入力：");
        scanf("%d", &initChip);
        
        (*memberPtr)[0].id = 1; //プレイヤーIDの設定
        (*memberPtr)[0].isCOM = false;
        (*memberPtr)[0].isDied = false;
        (*memberPtr)[0].chip = initChip;
        (*memberPtr)[0].nextMember = *memberPtr + 1;

        for (int i = playerCount; i < playerCount + comCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);

            (*memberPtr)[i].id = i + 1;   //COMのIDの設定
            (*memberPtr)[i].isCOM = true;
            (*memberPtr)[i].isDied = false;
            (*memberPtr)[i].chip = initChip;
            (*memberPtr)[i].nextMember = (playerCount + comCount - 1 == i) ? 
                                         *memberPtr :
                                         &(*memberPtr)[i] + 1;
        }
    }
    else {
        exit(1);
    }

    return playerCount + comCount;
}

MemberInfo* NextMember(MemberInfo* memberPtr, int count) {
    if (count > 0 || (memberPtr)->isDied) {
        memberPtr = (memberPtr)->nextMember;
        memberPtr = NextMember(memberPtr, count - !memberPtr->isDied);
    }
    return memberPtr;
}

void poker(void) { //ポーカー
    MemberInfo* members = NULL;       //メンバーを格納する構造体配列
    int numMembers = 0;
    MemberInfo* smallBlind = NULL;
    MemberInfo* bigBlind = NULL;                                   //メンバーの数
    MemberInfo* currBetMember = NULL;
    MemberInfo* winner = NULL;
    const char suit[][15] = { "spade", "\x1b[31mheart\x1b[0m", "\x1b[31mdia\x1b[0m", "clab" }; //絵札の種類
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //トランプカードを格納する構造体配列
    TrumpInfo* communityCard[5] = { NULL };      //コミュニティカードを格納する構造体配列
    int numAlive = 0;
    int round = 0;                                              //ラウンド
    int game = 0;                                               //ゲーム
    int numCommCards = 0;
    int ante = 50;
    int blind = ante*2;
    int callChip = 0;
    int pot = 0;
    BET choiceAct = 0;
    const char* action[] = { "bet", "check", "call", "raise", "fold" };
    int stake = 0;
    
    //トランプカードの作成
    for (int i = 0; i < RANK_COUNT; i++) {
        for (int j = 0; j < SUIT_COUNT; j++) {
            trump[i][j].cardRank = i + 1;             //トランプの数字を設定
            trump[i][j].cardSuit = (SUIT)j;         //トランプの絵札を設定
            trump[i][j].isDeploy = false;
            //strcpy(trump[i][j].cardSuit, suit[j]);  //トランプの絵札を設定
        }
    }
    numMembers = numAlive = MakeMember(&members);  //メンバーの作成

    
    smallBlind = &members[(game) % numMembers];
    bigBlind = &members[(game + 1) % numMembers];
    currBetMember = &members[(game + 2) % numMembers];
    sck(currBetMember->name);

   
    //mainloop
    for(game = 1; true; game++) {
        ick(game);
        //アンティ（実装するかは未確定）
        printf("アンティ：%d\n", ante);        
        for (int i = 0; i < numAlive; i++) {
            MemberInfo* tmp = NextMember(members, i);
            MoveChip(&(tmp->chip), &pot, ante);
        }


        //スモールブラインド
        MoveChip(&(smallBlind->chip), &(smallBlind->ownPot), blind);
        ick(smallBlind->ownPot);

        //ビッグブラインド
        MoveChip(&(bigBlind->chip), &(bigBlind->ownPot), 2 * blind);
        ick(bigBlind->ownPot);

        //手札の配布・確認
        for (int i = 0; i < numAlive; i++) {
            MemberInfo* tmp = NextMember(members, i);
            for (int j = 0; j < sizeof(tmp->deck) / sizeof(TrumpInfo*); j++) {
                tmp->deck[j] = ChoiceTrump(trump);
                ick(tmp->deck[j]->cardRank);
            }
        }
        printf("%s - %d\n", suit[members[0].deck[0]->cardSuit], members[0].deck[0]->cardRank);
        printf("%s - %d\n", suit[members[0].deck[1]->cardSuit], members[0].deck[1]->cardRank);

        stop();
        //ラウンド開始
        for (round = 1; round <= 4; round++) {
            ick(round);
            //ベッティングラウンド
            do{
                //ベッドされていないならベッド，チェック
                // ベッドの最低額はBB
                //ベッドされているならコール，レイズ，フォールド
                // コールは直前のメンバーのポットに合わせる
                //ベッティングラウンドの条件は「全員の賭け金の一致」∧「メンバーが一周している」こと
                //⇒for文の一時変数？


                //プレイヤーの手札の確認
                for (int i = 0; i < numCommCards; i++) {
                    printf("%s - %d\n", suit[communityCard[i]->cardSuit], communityCard[i]->cardRank);
                }

                sck(currBetMember->name);   //ベットするプレイヤーの確認

                //コールする額の決定
                for (int i = 0; i < numMembers; i++) {
                    callChip = (members[i].ownPot > callChip ? members[i].ownPot : callChip);
                }
                ick(callChip);

                //ベッドされていない時の動作
                if (callChip == 0) {
                    printf("1:%s, 2:%s", action[0], action[1]);
                    scanf("%d", &choiceAct);
                    choiceAct -= 1;
                }
                else {  //ベッドされているときの動作
                    printf("1:%s, 2:%s, 3:%s", action[2], action[3], action[4]);
                    scanf("%d", &choiceAct);
                    choiceAct += 1;
                }
                sck(action[choiceAct]);
                stop();
                switch (choiceAct)
                {
                case bet:
                    stake = 2 * blind;
                    break;
                    
                case check:
                    //することなし
                    break;

                case call:
                    stake = callChip - currBetMember->ownPot;
                    break;

                case raise:
                    //要ルール理解
                    break;

                case fold:
                    //作らねば
                    currBetMember->isDied = true;
                    break;
                default:

                    break;
                }
                ick(stake);
                //掛け金を自分のポットへ
                MoveChip(&(currBetMember->chip), &currBetMember->ownPot, stake);

                //メンバーの交代
                currBetMember = NextMember(currBetMember, 1);
            } while (IsBettinground(members, numMembers)); //ラウンドの終端
            
            stop();

            for (int i = 0; i < numMembers; i++) {
                //pot += members[i].ownPot;
                MoveChip(&(members[i].ownPot), &pot, members[i].ownPot);
            }
            callChip = 0;
            stake = 0;

            if (round == 4) {
                //ショーダウン
                for (int i = 0; i < numAlive; i++) {
                    MemberInfo* tmp = NextMember(members, i);
                    RoleJudge(tmp, communityCard);
                }
                //勝者判定
                winner = members;
                MoveChip(&pot, &winner->chip, pot);
                //winner->chip += pot;
                //pot = 0;
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

        //敗者脱落
        for (int i = 0; i < numMembers; i++) {
            if (members[i].chip == 0) {
                members[i].isDied = true;
            }
            else {
                members[i].isDied = false;
            }
        }

        smallBlind = NextMember(smallBlind, 1);
        bigBlind =  NextMember(bigBlind, 1);

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

void Pay(MemberInfo* member, int chip) {
    if (chip > member->chip) {
        chip = member->chip;
    }
    //chip = member->chip >= chip ? chip : member->chip;
    member->ownPot += chip;
    member->chip -= chip;
}

void MoveChip(int* source, int* dest, int amount) {
    if (amount > *source) {
        amount = *source;
    }
    *dest += amount;
    *source -= amount;

}


bool IsBettinground(MemberInfo* members, int count) {
    for (int i = 0; i < count-1; i++) {
        if (members[i].ownPot != members[i + 1].ownPot) {
            return true;
        }
    }
    return false;
}

void RoleJudge(MemberInfo* member, TrumpInfo* communityCard[5]) {
    //sck(member->name);
    //ick(member->deck[0]->cardRank);
    //ick(member->deck[0]->cardSuit);
    //printf("\n");
}
