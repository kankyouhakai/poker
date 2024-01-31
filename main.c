#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
//#include "debuglib.h"
#include "gameset.h"
#include "RoleJudge.h"
 

int MakeMember(MemberInfo**); //メンバー作成

MemberInfo* poker(MemberInfo* , int);   //ポーカー実行

//TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]); //手札，コミュニティカードの選択
//MemberInfo* NextMember(MemberInfo* memberPtr, int count);
//void MoveChip(int* source, int* dest, int amount);

void RoleJudge(MemberInfo*, TrumpInfo* communityCard[5]); //役判定

int cpuMove(const char*, ...);

bool IsBettinground(MemberInfo* members, int count, int betCount);//ベッティングラウンドが継続するかどうか

int ToQsort(const void*, const void*);
int main(void) {
    system("chcp 65001");
    srand((unsigned int)time(NULL));//乱数の初期化
    MemberInfo* members = NULL; //メンバーを格納する配列
    MemberInfo* winner = NULL;  //勝者を格納するための変数
    int numMembers = 0;
    

    numMembers = MakeMember(&members);  //メンバーの作成

    time_t pokerStartUnixTime = time(NULL);  //ゲーム開始時の時刻を記憶
    winner = poker(members, numMembers); //pokerを開始
    time_t pokerEndUnixTime = time(NULL);   //ゲーム終了時の時刻を記憶
    time_t pokerPlayUnixTime = pokerEndUnixTime - pokerStartUnixTime; //プレイ時間

    //データのエクスポート
    FILE* fp = fopen("result.txt", "a");
    struct tm* pokerStartTime = localtime(&pokerStartUnixTime);
    for (int j = 0; j < 2; j++) {
        fprintf(fp, "%d/%d/%d\n", pokerStartTime->tm_year + 1900, pokerStartTime->tm_mon + 1, pokerStartTime->tm_mday);
        fprintf(fp, "ゲーム開始時刻 | %02d:%02d:%02d\n", pokerStartTime->tm_hour, pokerStartTime->tm_min, pokerStartTime->tm_sec);
        fprintf(fp, "プレイ時間　　 | %02d:%02d\n", pokerPlayUnixTime / 60, pokerPlayUnixTime % 60);
        fprintf(fp, "参加者\n");
        for (int i = 0; i < numMembers; i++) {
            fprintf(fp, "%d %s\n", members[i].id, members[i].name);
        }
        fprintf(fp, "勝者 | %s", winner->name);
        if (j == 0) {
            fclose(fp);
            fp = stdout;
        }
    }
    free(members);
    //オプション
    return 0;
}

int MakeMember(MemberInfo** memberPtr) {
    int comCount = 0;           //COMの人数
    int playerCount = 0;        //プレイヤーの人数
    int memberCount = 0;        //メンバーの人数
    int initChip = 0;           //チップの初期枚数
    printf("プレイヤーとCOMの設定をします\n");
    printf("プレイヤーの人数を入力：");
    scanf("%d", &playerCount);

    printf("comの人数を入力：");
    scanf("%d", &comCount);
    
    memberCount = playerCount + comCount;
    *memberPtr = (MemberInfo*)calloc(sizeof(MemberInfo), memberCount);


    //printf("チップの初期枚数を入力：\n");
    //scanf("%d", &initChip);
    initChip = 10000;
    if (*memberPtr != NULL) { //メンバーが正しく作成されたか
        for (int i = 0; i < playerCount; i++) {
            printf("プレイヤー%dの名前を入力：", i + 1);
            scanf("%s", (*memberPtr)[i].name);

            (*memberPtr)[i].id = i+1; //プレイヤーIDの設定
            (*memberPtr)[i].isCOM = false;
            (*memberPtr)[i].isDied = false;
            (*memberPtr)[i].chip = initChip;
            (*memberPtr)[i].nextMember = (memberCount - 1 == i) ?
                *memberPtr : &(*memberPtr)[i] + 1;
        }
        for (int i = playerCount; i < memberCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);

            (*memberPtr)[i].id = i + 1;   //COMのIDの設定
            (*memberPtr)[i].isCOM = true;
            (*memberPtr)[i].isDied = false;
            (*memberPtr)[i].chip = initChip;
            (*memberPtr)[i].nextMember = (memberCount - 1 == i) ? *memberPtr 
                                                                : &(*memberPtr)[i] + 1;
        }
    }
    else {
        exit(1);
    }

    return playerCount + comCount;
}


MemberInfo* poker(MemberInfo* members, int numMembers) { //ポーカー
    //MemberInfo* members = NULL;       //メンバーを格納する構造体配列
    MemberInfo* smallBlind = NULL;  //スモールブラインド
    MemberInfo* bigBlind = NULL;    //ビッグブラインド
    MemberInfo* currBetMember = NULL;//現在現在べっとしているメンバー
    MemberInfo* winner = NULL;  //勝った人
    const char* suit[] = {  "clab",  "\x1b[31mdia\x1b[0m","\x1b[31mheart\x1b[0m", "spade"}; //絵札の種類
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //トランプカードを格納する構造体配列
    TrumpInfo* communityCard[5] = { NULL };      //コミュニティカードを格納する構造体配列
    int numAlive = 0;   //生存者の人数
    int round = 0;                                              //ラウンド
    int game = 0;                                               //ゲーム
    int numCommunityCards = 0;  //コミュニティーカードの枚数
    int ante = 50;  //アンティ
    int blind = ante*2; //ブラインド
    int necessaryCallChip = 0;  //コールするチップの量
    int pot = 0;    //ポット
    BET choiceAct = 0;  //選択したアクション
    const char* action[] = { "bet", "check", "call", "raise", "fold" }; //アクションの種類
    const char* role[] = { "highcard", "onepair", "twopair", "threecard", "flash", "straight", "fullhouse","fourcard", "straightflash" };
    int (*DecideAction)(const char*, ...) = NULL;   //COMとプレイヤーで異なる処理をする
    bool isFoolProof = true;    //入力ミスをやり直し可能にする．
    bool isAllFold = false;
    bool isOverflowLine = false;
    //トランプカードの作成
    MakeTrump(trump);
    
    numAlive = numMembers;  //生存者数の設定
    printf("メンバー表\n");
    for (int i = 0; i < numMembers; i++) {
        printf("%d : %s\n", members[i].id, members[i].name);
    }
    smallBlind = &members[(game) % numMembers];
    bigBlind = &members[(game + 1) % numMembers];
    currBetMember = &members[(game + 2) % numMembers];
    DecideAction = (currBetMember->isCOM) ? cpuMove : scanf;    //COMならcpuMove関数, 人ならscanf関数を使うように設定する

   
    //1ゲームごとのループ 
    for(game = 1; numAlive >= 2; game++) {
        //アンティ（実装するかは未確定）
        printf("アンティ：%d\n", ante);        
        for (int i = 0; i < numAlive; i++) {
            printf("157\n");
            //MemberInfo* tmp = NextMember(members, i);pr
            MoveChip(&(NextMember(members, i)->chip), &pot, ante);
        }
        printf("%d\n",__LINE__);
        printf("161\n");
        //スモールブラインド
        MoveChip(&(smallBlind->chip), &(smallBlind->stake), blind);

        //ビッグブラインド
        MoveChip(&(bigBlind->chip), &(bigBlind->stake), 2 * blind);

        //手札の配布・確認
        for (int i = 0; i < numAlive; i++) {
            MemberInfo* tmp = NextMember(members, i);
            for (int j = 0; j < sizeof(tmp->deck) / sizeof(TrumpInfo*); j++) {
                tmp->deck[j] = ChoiceTrump(trump);
            }
        }

        //ラウンド開始
        for (round = 1; round <= 4; round++) {
            //ベッティングラウンド
            int choiceCount = 0;
            do{
                //コールする額の決定
                for (int i = 0; i < numMembers; i++) {
                    necessaryCallChip = (members[i].stake > necessaryCallChip ? members[i].stake : necessaryCallChip);
                }

                RoleJudge(currBetMember, communityCard);
                //情報の表示
                printf("round : %d\n", round);
                printf("コールする額は%d\n", necessaryCallChip);
                printf("ベットするメンバー：%s\n", currBetMember->name);

                if (!currBetMember->isCOM) {
                    printf("%sの手札は\n", currBetMember->name);
                    printf("%s - %d\n", suit[currBetMember->deck[0]->cardSuit], currBetMember->deck[0]->cardRank);
                    printf("%s - %d\n", suit[currBetMember->deck[1]->cardSuit], currBetMember->deck[1]->cardRank);
                }
                //コミュニティカードの確認
                printf("コミュニティカードは\n");
                for (int i = 0; i < numCommunityCards; i++) {
                    printf("%s - %d\n", suit[communityCard[i]->cardSuit], communityCard[i]->cardRank);
                }
                //役の確認
                if (!currBetMember->isCOM) {

                    printf("%sの役は：%s\n", currBetMember->name, role[currBetMember->ownRole]);
                }
                //各プレイヤーのチップの量と賭け金
                for (int i = 0; i < numAlive; i++) {
                    MemberInfo* tmp = NextMember(members, i);
                    printf("name is %s | stake : %d | chip : %d\n", tmp->name, tmp->stake, tmp->chip);

                }
                printf("212\n");
                //アクションの選択
                isFoolProof = true;
                while (isFoolProof) {
                    if (necessaryCallChip == 0) {   //コールする額が0 => 誰もべっとしていない
                        printf("1:%s, 2:%s：", action[0], action[1]);
                    }else {
                        printf("1:%s, 2:%s, 3:%s：", action[2], action[3], action[4]);
                    }
                    if(currBetMember->chip == 0){
                       choiceAct = necessaryCallChip ? 1 : 2;

                    }
                    else {
                        DecideAction("%u", &choiceAct);

                    }
                    isFoolProof = choiceAct > (3 - !necessaryCallChip) ; 

                    printf("%s", isFoolProof ? "\x1b[A\x1b[K" : "");    //入力ミスを削除
                    choiceAct += necessaryCallChip ? 1 : -1;    ////入力した数字を補
                }
                choiceCount++;
                
                switch (choiceAct)
                {
                case bet:
                    do {
                        printf("ベットする額を入力：\n");
                        DecideAction("%d", &currBetMember->stake, currBetMember->chip);
                        isFoolProof = currBetMember->stake < 2 * blind;

                        isOverflowLine = currBetMember->stake > currBetMember->chip;
                        printf("%s", (isFoolProof||isOverflowLine) ? "\x1b[A\x1b[Kもう一度入力してください\n" : "");    //入力ミスを削除

                    } while (isFoolProof);
                    printf("beted\n");
                    break;
                    
                case check:
                    currBetMember->stake = 0;
                    printf("checked\n");
                    break;

                case call:
                    ;int callChip = necessaryCallChip - currBetMember->stake;
                    MoveChip(&currBetMember->chip, &currBetMember->stake, callChip);
                    break;

                case raise:                    
                    ;int raiseChip = 0;
                    do {
                        printf("レイズする額を入力：");
                        DecideAction("%d", &raiseChip, currBetMember->chip);
                        isFoolProof = currBetMember->stake + raiseChip < necessaryCallChip;

                        isOverflowLine = raiseChip > currBetMember->chip;
                        printf("%s", isFoolProof ? "\x1b[A\x1b[Kもう一度入力してください\n" : "");    //入力ミスを削除
                    } while (isFoolProof);
                    MoveChip(&currBetMember->chip, &currBetMember->stake, raiseChip);

                    printf("raised\n");
                    break;

                case fold:
                    currBetMember->isDied = true;
                    numAlive--;
                    printf("folded\n");
                    break;
                default:

                    break;
                }
                if (numAlive == 1) {
                    isAllFold = true;

                    break;
                }
                //メンバーの交代
                char changemember = 0;
                do {
                    printf("メンバーを%sに交代します[y/n]:", currBetMember->nextMember->name);
                    scanf(" %c", &changemember);
                } while (changemember != 'y');
                system("clear");
                currBetMember = NextMember(currBetMember, 1);
                DecideAction = (currBetMember->isCOM) ? cpuMove : scanf;
            } while (IsBettinground(members, numAlive, choiceCount)); //ラウンドの終端
            
            //賭けたチップをポットに移す
            for (int i = 0; i < numMembers; i++) {
                MoveChip(&(members[i].stake), &pot, members[i].stake);
            }
            necessaryCallChip = 0;
            if (isAllFold) {
                winner = NextMember(members, 0);
                printf("勝者は%s\n", winner->name);
                MoveChip(&pot, &winner->chip, pot);
                break;

            }
            
            if (round == 4) {   //ラウンド４
                //ショーダウン
                winner = members;
                for (int i = 0; i < numAlive; i++) {
                  MemberInfo* tmp = NextMember(members, i);
                   RoleJudge(tmp, communityCard);
                   winner = (winner->point < tmp->point) ? tmp : winner;

                }
                printf("勝者は%s\n", winner->name);
                MoveChip(&pot, &winner->chip, pot);
                //勝者判定
                //winner->chip += pot;
                //pot = 0;
            }
            else {      //ラウンド１～３
                //コミュニティカードの配布
                do {
                    communityCard[numCommunityCards] = ChoiceTrump(trump);
                    numCommunityCards++;
                } while (communityCard[2] == NULL);
                for (int i = 0; i < numAlive; i++) {
                    RoleJudge(NextMember(members, i), communityCard);
                }
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
        for (;numCommunityCards > 0; numCommunityCards--) {
            communityCard[numCommunityCards - 1]->isDeploy = false;
            communityCard[numCommunityCards - 1] = NULL;
        }

        //敗者脱落
        numAlive = 0;
        for (int i = 0; i < numMembers; i++) {
            if (members[i].chip == 0) {
                members[i].isDied = true;
            }
            else {
                members[i].isDied = false;
                numAlive++;
            }
            members[i].ownRole = 0;
        }

        smallBlind = NextMember(smallBlind, 1);
        bigBlind =  NextMember(bigBlind, 1);

    }
    return winner;
}

int cpuMove(const char* command, ...) {
    va_list ap;
    va_start(ap, command);
    if (command[1] == 'u') {
        BET* action = va_arg(ap, BET*);
        *action = (rand() % 4) + 1;

        printf("%d\n", *action);
        //*va_arg(ap, BET*) = (rand() % 4) + 1;
    }
    else if (command[1] == 'd') {
        int* stake = va_arg(ap, int*);
        int max = va_arg(ap, int);
        *stake = (rand() % max);
        printf("%d\n", *stake);
        //*va_arg(ap, int*) = (rand() % va_arg(ap, int));
    }
    va_end(ap);
    return 0;
}


bool IsBettinground(MemberInfo* members, int numAlive, int betCount) {
    if (betCount <=  numAlive) { return true; }

    for (int i = 0;i < numAlive; i++) {
        MemberInfo* tmp = NextMember(members, i);

        if (tmp->stake != tmp->nextMember->stake) { return true; }
    }

    return false;
}
            
void RoleJudge(MemberInfo* member, TrumpInfo* communityCard[5]) {
    TrumpInfo* combineCards[7] = { 0 }; //トランプを統合して扱う配列
    for (int i = 0; i < 2; i++) {       //メンバーの手札を統合
        combineCards[i] = member->deck[i];
    }
    for (int i = 2; i < 7; i++) {       //コミュニティーカードの手札を統合
        combineCards[i] = communityCard[i - 2];
    }
    combineCards[0]->cardRank;

    qsort(combineCards, 7, sizeof(TrumpInfo*), ToQsort); //カードを強い順にソートする．

    bool (*isRole[])(TrumpInfo**, MemberInfo*) = { IsOnePair, isTwoPair, IsThreeCard, IsFlash, IsStraight, isFullHouse, IsFourCard, IsStraightFlash };
    member->ownRole = 0;
    for (int i = 0; i < 8; i++) {
        member->ownRole = isRole[i](combineCards, member) ? (ROLE)(i + 1) : (ROLE)member->ownRole;
    }
    member->point = member->ownRole << 4;
    

}

int ToQsort(const void* n1, const void* n2) {   //アドレスが大きいほうが強い
    TrumpInfo* card1 = *(TrumpInfo**)n1;
    TrumpInfo* card2 = *(TrumpInfo**)n2;
    if (card1 == NULL) { return 1; }
    if (card2 == NULL) { return -1; }

    if (card1 > card2) { return -1; }
    if (card1 < card2) { return 1; }
}

void DecidePoint(MemberInfo* member, TrumpInfo* cards[7]) {
    switch (member->ownRole)
    {
    case HIGH_CARD:
        break;
    case ONE_PAIR:
        break;
    case TWO_PAIR:
        break;
    case THREE_CARD:
        break;
    case FLASH:
        break;
    case STRAIGHT:
        break;
    case FULL_HOUSE:
        break;
    case FOUR_CARD:
        break;
    case STRAIGHT_FLASH:
        break;
    default:
        break;
    }
}