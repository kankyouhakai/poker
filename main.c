#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "debuglib.h"
#include "gameset.h"
#include "cpumove.h"
 

int MakeMember(MemberInfo**); //メンバー作成

MemberInfo* poker(MemberInfo* , int);   //ポーカー実行

//TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]); //手札，コミュニティカードの選択
//MemberInfo* NextMember(MemberInfo* memberPtr, int count);
//void MoveChip(int* source, int* dest, int amount);

void RoleJudge(MemberInfo*, TrumpInfo* communityCard[5]); //役判定

int cpuMove(const char*, ...);

bool IsBettinground(MemberInfo* members, int count, int betCount);//ベッティングラウンドが継続するかどうか


int main(void) {
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
    stop();
    free(members);
    //オプション
    return 0;
}

int MakeMember(MemberInfo** memberPtr) {
    int comCount = 0;           //COMの人数
    int playerCount = 0;        //プレイヤーの人数(1人固定)
    int memberCount = 0;
    int initChip = 0;           //チップの初期枚数
    printf("プレイヤーとCOMの設定をします\n");
    printf("プレイヤーの人数を入力：");
    scanf("%d", &playerCount);

    printf("comの人数を入力：");
    scanf("%d", &comCount);

    memberCount = playerCount + comCount;
    *memberPtr = (MemberInfo*)calloc(sizeof(MemberInfo), memberCount);


    printf("チップの初期枚数を入力：");
    scanf("%d", &initChip);

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
    for (int i = 0; i < numMembers; i++) {
        sck(members[i].name);
        pck(members[i].nextMember);
        pck(&members[i].name);
    }
    TableInfo table = { 0 };
    //MemberInfo* members = NULL;       //メンバーを格納する構造体配列
    //int numMembers = 0;
    MemberInfo* smallBlind = NULL;  //スモールブラインド
    MemberInfo* bigBlind = NULL;    //ビッグブラインド
    MemberInfo* currBetMember = NULL;//現在現在べっとしているメンバー
    MemberInfo* winner = NULL;  //勝った人
    const char* suit[] = { "spade", "\x1b[31mheart\x1b[0m", "\x1b[31mdia\x1b[0m", "clab" }; //絵札の種類
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //トランプカードを格納する構造体配列
    TrumpInfo* communityCard[5] = { NULL };      //コミュニティカードを格納する構造体配列
    int numAlive = 0;   //生存者の人数
    int round = 0;                                              //ラウンド
    int game = 0;                                               //ゲーム
    int numCommunityCards = 0;
    int ante = 50;
    int blind = ante*2;
    int callChip = 0;
    int pot = 0;
    BET choiceAct = 0;
    const char* action[] = { "bet", "check", "call", "raise", "fold" };
    int (*DecideAction)(const char*, ...) = NULL;
    bool isFoolProof = false;

    //トランプカードの作成
    MakeTrump(trump);

    numAlive = numMembers;
    printf("メンバー表\n");
    for (int i = 0; i < numMembers; i++) {
        printf("%d : %s\n", members[i].id, members[i].name);
    }
    stop();
    smallBlind = &members[(game) % numMembers];
    bigBlind = &members[(game + 1) % numMembers];
    currBetMember = &members[(game + 2) % numMembers];
    DecideAction = (currBetMember->isCOM) ? cpuMove : scanf;

    printf("ベットするメンバー：%s", currBetMember->name);

   
    //mainloop
    for(game = 1; numAlive >= 2; game++) {
        ick(game);
        //アンティ（実装するかは未確定）
        printf("アンティ：%d\n", ante);        
        for (int i = 0; i < numAlive; i++) {
            //MemberInfo* tmp = NextMember(members, i);
            MoveChip(&(NextMember(members, i)->chip), &pot, ante);
        }
        

        //スモールブラインド
        MoveChip(&(smallBlind->chip), &(smallBlind->stake), blind);
        ick(smallBlind->stake);

        //ビッグブラインド
        MoveChip(&(bigBlind->chip), &(bigBlind->stake), 2 * blind);
        ick(bigBlind->stake);

        //手札の配布・確認
        for (int i = 0; i < numAlive; i++) {
            MemberInfo* tmp = NextMember(members, i);
            for (int j = 0; j < sizeof(tmp->deck) / sizeof(TrumpInfo*); j++) {
                tmp->deck[j] = ChoiceTrump(trump);
            }
        }
        printf("\x1b[s");
        printf("%sの手札は\n", currBetMember->name);
        printf("%s - %d\n", suit[members[0].deck[0]->cardSuit], members[0].deck[0]->cardRank);
        printf("%s - %d\n", suit[members[0].deck[1]->cardSuit], members[0].deck[1]->cardRank);

        stop();
        //ラウンド開始
        for (round = 1; round <= 4; round++) {
            ick(round);
            //ベッティングラウンド
            int choiceCount = 0;
            do{
                //コミュニティカードの確認
                for (int i = 0; i < numCommunityCards; i++) {
                    printf("%s - %d\n", suit[communityCard[i]->cardSuit], communityCard[i]->cardRank);
                }

                sck(currBetMember->name);   //ベットするプレイヤーの確認

                //コールする額の決定
                for (int i = 0; i < numMembers; i++) {
                    callChip = (members[i].stake > callChip ? members[i].stake : callChip);
                }

                ick(callChip);
                ick(pot);
                for (int i = 0; i < numAlive; i++) {
                    MemberInfo* tmp = NextMember(members, i);
                    printf("name is %s | ownPot : %d | chip : %d\n", tmp->name, tmp->stake, tmp->chip);

                }

                //アクションの選択
                ick(currBetMember->isCOM);
                do {
                    if (callChip == 0) {
                        printf("1:%s, 2:%s：", action[0], action[1]);
                        DecideAction("%u", &choiceAct);
                        isFoolProof = choiceAct > 2;
                        if (isFoolProof) {
                            printf("\x1b[A\x1b[K");
                        }
                        else {
                            choiceAct -= 1;
                        }
                    }
                    else {
                        printf("1:%s, 2:%s, 3:%s：", action[2], action[3], action[4]);
                        DecideAction("%u", &choiceAct);
                        isFoolProof = choiceAct > 3;
                        if (isFoolProof) {
                            printf("\x1b[A\x1b[K");
                        }
                        else {
                            choiceAct += 1;
                        }
                    }
                }while (isFoolProof);

                //if (callChip == 0) {                    //ベッドされていない時の動作
                //    do {
                //        printf("1:%s, 2:%s", action[0], action[1]);
                //        DecideAction("%u", &choiceAct);
                //        isFoolProof = choiceAct > 2;
                //        if (isFoolProof) {

                //        }
                //        else {
                //            choiceAct += 1;
                //        }
                //    } while (isFoolProof);
                //}
                //else {  //ベッドされているときの動作
                //    printf("1:%s, 2:%s, 3:%s", action[2], action[3], action[4]);
                //    DecideAction("%u", &choiceAct);
                //    choiceAct += 1;
                //}
                choiceCount++;
                sck(action[choiceAct]);
                
                
                switch (choiceAct)
                {
                case bet:
                    do {
                        printf("bet\n");
                        DecideAction("%d", &currBetMember->stake, currBetMember->chip);
                    } while (currBetMember->stake < 2 * blind);
                    printf("beted\n");
                    //currBetMember->stake = 2 * blind;
                    break;
                    
                case check:
                    currBetMember->stake = 0;
                    printf("checked\n");
                    //することなし
                    break;

                case call:
                    currBetMember->stake = callChip - currBetMember->stake;
                    break;

                case raise:
                    //要ルール理解
                    
                    ;int raiseChip = 0;
                    do {
                        printf("raise\n");
                        DecideAction("%d", &raiseChip, currBetMember->chip);
                    } while (currBetMember->stake + raiseChip < callChip);
                    printf("raised\n");
                    break;

                case fold:
                    currBetMember->isDied = true;
                    numAlive--;
                    printf("died\n");
                    break;
                default:

                    break;
                }
                ick(currBetMember->stake);
                //賭ケグルイ
                //MoveChip(&(currBetMember->chip), &currBetMember->stake, stake);

                //メンバーの交代
                //system("cls");
                currBetMember = NextMember(currBetMember, 1);
                DecideAction = (currBetMember->isCOM) ? cpuMove : scanf;
            } while (IsBettinground(members, numAlive, choiceCount)); //ラウンドの終端
            
            stop();
            //賭けたチップをポットに移す
            for (int i = 0; i < numMembers; i++) {
                MoveChip(&(members[i].stake), &pot, members[i].stake);
            }
            callChip = 0;

            if (round == 4) {
                //ショーダウン
                for (int i = 0; i < numAlive; i++) {
                   // MemberInfo* tmp = NextMember(members, i);
                   RoleJudge(NextMember(members, i), communityCard);
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
                    communityCard[numCommunityCards] = ChoiceTrump(trump);
                    numCommunityCards++;
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
        }

        smallBlind = NextMember(smallBlind, 1);
        bigBlind =  NextMember(bigBlind, 1);

        stop();
    }
    return winner;
}

int cpuMove(const char* command, ...) {
    stop();
    va_list ap;
    va_start(ap, command);
    if (command[1] == 'u') {
        *va_arg(ap, BET*) = (rand() % 4) + 1;
    }
    else if (command[1] == 'd') {
        int* stake = va_arg(ap, int*);
        pck(stake);
        int max = va_arg(ap, int);
        ick(max);
        *stake = (rand() % max);
    }
    va_end(ap);
    return 0;
}


bool IsBettinground(MemberInfo* members, int numAlive, int betCount) {
    if (betCount < numAlive) { return true; }

    for (int i = 0;i < numAlive; i++) {
        MemberInfo* tmp = NextMember(members, i);
        pck(tmp);

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
    //関数の役割
    //  実引数としてメンバー，コミュニティカードをとり，そのメンバーの役を判定する．
    // 
    //引数の説明
    //member : 役を判定する対象のメンバー．
    //  member->name  : メンバーの名前
    //  menber->point : たぶん使わん
    //  member->chip  : 現在所持しているチップ
    //  member->id    : メンバー固有のID
    //  member->isCOM : 人間かCPUか
    //  member->deck[]: メンバーの手札．詳細は後述する．
    //  member->ownPot: メンバーが現在かけているチップ.
    //  member->isDied: メンバーが生きてるか死んでるか．
    //  member->numOfDeck:メンバーの手札の枚数.
    //  member->role   : 判定された役．変数の型は自由に決めてね
    //  member->nextMember: 次のメンバーへのポインタ．
    // 
    // member->deck[]の説明
    //  ・配列数は２（手札は2枚だから）
    //  member->deck[i]->cardRank : i枚目の手札の数字
    //  member->deck[i]->cardSuit : i枚目の手札の絵札.(spade = 0, heart = 1, dia = 2,clab = 3)
    // 
    // communitycard[] : コミュニティーカードを格納する配列．最大5枚なので配列長は５
    //  communitycard[i]->cardRank
    //  communitycard[i]->cardSuit : メンバーの手札と同様
    // 
    // 
    // その他
    //  ・おそらく使う変数は
    //      member->deck[]
    //      member->role
    //      communitycard[]
    // 　 かもしれない．
    // ・その他必要だと思った変数や関数や引数は適宜追加してもらって
    // ・ほかに知りたいことがあったら北尾君に聞けば何か得られるかもしれん．
    //
}
