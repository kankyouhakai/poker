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
 

int MakeMember(MemberInfo**); //�����o�[�쐬

MemberInfo* poker(MemberInfo* , int);   //�|�[�J�[���s

//TrumpInfo* ChoiceTrump(TrumpInfo trump[RANK_COUNT][SUIT_COUNT]); //��D�C�R�~���j�e�B�J�[�h�̑I��
//MemberInfo* NextMember(MemberInfo* memberPtr, int count);
//void MoveChip(int* source, int* dest, int amount);

void RoleJudge(MemberInfo*, TrumpInfo* communityCard[5]); //�𔻒�

int cpuMove(const char*, ...);

bool IsBettinground(MemberInfo* members, int count, int betCount);//�x�b�e�B���O���E���h���p�����邩�ǂ���

int ToQsort(const void*, const void*, int);

int main(void) {
    srand((unsigned int)time(NULL));//�����̏�����
    MemberInfo* members = NULL; //�����o�[���i�[����z��
    MemberInfo* winner = NULL;  //���҂��i�[���邽�߂̕ϐ�
    int numMembers = 0;
    

    numMembers = MakeMember(&members);  //�����o�[�̍쐬

    time_t pokerStartUnixTime = time(NULL);  //�Q�[���J�n���̎������L��
    winner = poker(members, numMembers); //poker���J�n
    time_t pokerEndUnixTime = time(NULL);   //�Q�[���I�����̎������L��
    time_t pokerPlayUnixTime = pokerEndUnixTime - pokerStartUnixTime; //�v���C����

    //�f�[�^�̃G�N�X�|�[�g
    FILE* fp = fopen("result.txt", "a");
    struct tm* pokerStartTime = localtime(&pokerStartUnixTime);
    for (int j = 0; j < 2; j++) {
        fprintf(fp, "%d/%d/%d\n", pokerStartTime->tm_year + 1900, pokerStartTime->tm_mon + 1, pokerStartTime->tm_mday);
        fprintf(fp, "�Q�[���J�n���� | %02d:%02d:%02d\n", pokerStartTime->tm_hour, pokerStartTime->tm_min, pokerStartTime->tm_sec);
        fprintf(fp, "�v���C���ԁ@�@ | %02d:%02d\n", pokerPlayUnixTime / 60, pokerPlayUnixTime % 60);
        fprintf(fp, "�Q����\n");
        for (int i = 0; i < numMembers; i++) {
            fprintf(fp, "%d %s\n", members[i].id, members[i].name);
        }
        fprintf(fp, "���� | %s", winner->name);
        if (j == 0) {
            fclose(fp);
            fp = stdout;
        }
    }
    stop();
    free(members);
    //�I�v�V����
    return 0;
}

int MakeMember(MemberInfo** memberPtr) {
    int comCount = 0;           //COM�̐l��
    int playerCount = 0;        //�v���C���[�̐l��
    int memberCount = 0;        //�����o�[�̐l��
    int initChip = 0;           //�`�b�v�̏�������
    printf("�v���C���[��COM�̐ݒ�����܂�\n");
    printf("�v���C���[�̐l������́F");
    scanf("%d", &playerCount);

    printf("com�̐l������́F\n");
    //scanf("%d", &comCount);
    comCount = 0;

    memberCount = playerCount + comCount;
    *memberPtr = (MemberInfo*)calloc(sizeof(MemberInfo), memberCount);


    printf("�`�b�v�̏�����������́F\n");
    //scanf("%d", &initChip);
    initChip = 100000;
    if (*memberPtr != NULL) { //�����o�[���������쐬���ꂽ��
        for (int i = 0; i < playerCount; i++) {
            printf("�v���C���[%d�̖��O����́F", i + 1);
            scanf("%s", (*memberPtr)[i].name);

            (*memberPtr)[i].id = i+1; //�v���C���[ID�̐ݒ�
            (*memberPtr)[i].isCOM = false;
            (*memberPtr)[i].isDied = false;
            (*memberPtr)[i].chip = initChip;
            (*memberPtr)[i].nextMember = (memberCount - 1 == i) ?
                *memberPtr : &(*memberPtr)[i] + 1;
        }
        for (int i = playerCount; i < memberCount; i++) {
            sprintf((*memberPtr)[i].name, "COM%d", i - playerCount + 1);

            (*memberPtr)[i].id = i + 1;   //COM��ID�̐ݒ�
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


MemberInfo* poker(MemberInfo* members, int numMembers) { //�|�[�J�[
    //MemberInfo* members = NULL;       //�����o�[���i�[����\���̔z��
    MemberInfo* smallBlind = NULL;  //�X���[���u���C���h
    MemberInfo* bigBlind = NULL;    //�r�b�O�u���C���h
    MemberInfo* currBetMember = NULL;//���݌��ׂ݂��Ƃ��Ă��郁���o�[
    MemberInfo* winner = NULL;  //�������l
    const char* suit[] = { "spade", "\x1b[31mheart\x1b[0m", "\x1b[31mdia\x1b[0m", "clab" }; //�G�D�̎��
    TrumpInfo trump[RANK_COUNT][SUIT_COUNT] = { 0 };            //�g�����v�J�[�h���i�[����\���̔z��
    TrumpInfo* communityCard[5] = { NULL };      //�R�~���j�e�B�J�[�h���i�[����\���̔z��
    int numAlive = 0;   //�����҂̐l��
    int round = 0;                                              //���E���h
    int game = 0;                                               //�Q�[��
    int numCommunityCards = 0;  //�R�~���j�e�B�[�J�[�h�̖���
    int ante = 50;  //�A���e�B
    int blind = ante*2; //�u���C���h
    int necessaryCallChip = 0;  //�R�[������`�b�v�̗�
    int pot = 0;    //�|�b�g
    BET choiceAct = 0;  //�I�������A�N�V����
    const char* action[] = { "bet", "check", "call", "raise", "fold" }; //�A�N�V�����̎��
    int (*DecideAction)(const char*, ...) = NULL;   //COM�ƃv���C���[�ňقȂ鏈��������
    bool isFoolProof = true;    //���̓~�X����蒼���\�ɂ���D

    //�g�����v�J�[�h�̍쐬
    MakeTrump(trump);

    numAlive = numMembers;  //�����Ґ��̐ݒ�
    printf("�����o�[�\\n");
    for (int i = 0; i < numMembers; i++) {
        printf("%d : %s\n", members[i].id, members[i].name);
    }
    stop();
    smallBlind = &members[(game) % numMembers];
    bigBlind = &members[(game + 1) % numMembers];
    currBetMember = &members[(game + 2) % numMembers];
    DecideAction = (currBetMember->isCOM) ? cpuMove : scanf;    //COM�Ȃ�cpuMove�֐�, �l�Ȃ�scanf�֐����g���悤�ɐݒ肷��

   
    //1�Q�[�����Ƃ̃��[�v
    for(game = 1; numAlive >= 2; game++) {
        //�A���e�B�i�������邩�͖��m��j
        printf("�A���e�B�F%d\n", ante);        
        for (int i = 0; i < numAlive; i++) {
            //MemberInfo* tmp = NextMember(members, i);
            MoveChip(&(NextMember(members, i)->chip), &pot, ante);
        }
        

        //�X���[���u���C���h
        MoveChip(&(smallBlind->chip), &(smallBlind->stake), blind);

        //�r�b�O�u���C���h
        MoveChip(&(bigBlind->chip), &(bigBlind->stake), 2 * blind);

        //��D�̔z�z�E�m�F
        for (int i = 0; i < numAlive; i++) {
            MemberInfo* tmp = NextMember(members, i);
            for (int j = 0; j < sizeof(tmp->deck) / sizeof(TrumpInfo*); j++) {
                tmp->deck[j] = ChoiceTrump(trump);
            }
        }

        //���E���h�J�n
        for (round = 1; round <= 4; round++) {
            //�x�b�e�B���O���E���h
            int choiceCount = 0;
            do{
                //�R�[������z�̌���
                for (int i = 0; i < numMembers; i++) {
                    necessaryCallChip = (members[i].stake > necessaryCallChip ? members[i].stake : necessaryCallChip);
                }


                //���̕\��
                printf("round : %d\n", round);
                printf("�R�[������z��%d\n", necessaryCallChip);
                printf("�x�b�g���郁���o�[�F%s\n", currBetMember->name);
                printf("%s�̎�D��\n", currBetMember->name);
                printf("%s - %d\n", suit[members[0].deck[0]->cardSuit], members[0].deck[0]->cardRank);
                printf("%s - %d\n", suit[members[0].deck[1]->cardSuit], members[0].deck[1]->cardRank);
                //�R�~���j�e�B�J�[�h�̊m�F
                printf("�R�~���j�e�B�J�[�h��\n");
                for (int i = 0; i < numCommunityCards; i++) {
                    printf("%s - %d\n", suit[communityCard[i]->cardSuit], communityCard[i]->cardRank);
                }


                //�e�v���C���[�̃`�b�v�̗ʂƓq����
                for (int i = 0; i < numAlive; i++) {
                    MemberInfo* tmp = NextMember(members, i);
                    printf("name is %s | stake : %d | chip : %d\n", tmp->name, tmp->stake, tmp->chip);

                }

                //�A�N�V�����̑I��
                isFoolProof = true;
                while (isFoolProof) {
                    if (necessaryCallChip == 0) {   //�R�[������z��0 => �N���ׂ��Ƃ��Ă��Ȃ�
                        printf("1:%s, 2:%s�F", action[0], action[1]);
                    }else {
                        printf("1:%s, 2:%s, 3:%s�F", action[2], action[3], action[4]);
                    }

                    DecideAction("%u", &choiceAct);
                    isFoolProof = choiceAct > (3 - !necessaryCallChip); 

                    printf("%s", isFoolProof ? "\x1b[A\x1b[K" : "");    //���̓~�X���폜
                    choiceAct += necessaryCallChip ? 1 : -1;    ////���͂����������
                }
                choiceCount++;
                
                switch (choiceAct)
                {
                case bet:
                    do {
                        printf("�x�b�g����z����́F\n");
                        DecideAction("%d", &currBetMember->stake, currBetMember->chip);
                        isFoolProof = currBetMember->stake < 2 * blind;
                        printf("%s", isFoolProof ? "\x1b[A\x1b[K������x���͂��Ă�������\n" : "");    //���̓~�X���폜

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
                        printf("���C�Y����z����́F");
                        DecideAction("%d", &raiseChip, currBetMember->chip);
                        isFoolProof = currBetMember->stake + raiseChip < necessaryCallChip;
                        printf("%s", isFoolProof ? "\x1b[A\x1b[K������x���͂��Ă�������\n" : "");    //���̓~�X���폜
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
                stop();

                //�����o�[�̌��
                system("cls");
                currBetMember = NextMember(currBetMember, 1);
                DecideAction = (currBetMember->isCOM) ? cpuMove : scanf;
            } while (IsBettinground(members, numAlive, choiceCount)); //���E���h�̏I�[
            
            stop();
            //�q�����`�b�v���|�b�g�Ɉڂ�
            for (int i = 0; i < numMembers; i++) {
                MoveChip(&(members[i].stake), &pot, members[i].stake);
            }
            necessaryCallChip = 0;

            
            if (round == 4) {   //���E���h�S
                //�V���[�_�E��
                for (int i = 0; i < numAlive; i++) {
                   // MemberInfo* tmp = NextMember(members, i);
                   RoleJudge(NextMember(members, i), communityCard);
                }
                //���Ҕ���
                winner = members;
                MoveChip(&pot, &winner->chip, pot);
                //winner->chip += pot;
                //pot = 0;
            }
            else {      //���E���h�P�`�R
                //�R�~���j�e�B�J�[�h�̔z�z
                do {
                    communityCard[numCommunityCards] = ChoiceTrump(trump);
                    numCommunityCards++;
                } while (communityCard[2] == NULL);
                RoleJudge(&members[0], communityCard);
            }
        }
        //���E���h�I��

        //��D�̍폜
        for (int i = 0; i < numMembers; i++) {
            members[i].deck[0]->isDeploy = false;
            members[i].deck[1]->isDeploy = false;
            members[i].deck[0] = NULL;
            members[i].deck[1] = NULL;
        }
        //�R�~���j�e�B�J�[�h�̍폜
        for (;numCommunityCards > 0; numCommunityCards--) {
            communityCard[numCommunityCards - 1]->isDeploy = false;
            communityCard[numCommunityCards - 1] = NULL;
        }

        //�s�ҒE��
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
    va_list ap;
    va_start(ap, command);
    if (command[1] == 'u') {
        BET* action = va_arg(ap, BET*);
        *action = (rand() % 4) + 1;
        if (*action == raise) { *action = (rand() % 4) + 1; }
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
    ick(betCount);
    ick(numAlive);
    stop();
    if (betCount <=  numAlive) { return true; }

    for (int i = 0;i < numAlive; i++) {
        MemberInfo* tmp = NextMember(members, i);

        if (tmp->stake != tmp->nextMember->stake) { return true; }
    }

    return false;
}
            
void RoleJudge(MemberInfo* member, TrumpInfo* communityCard[5]) {
    TrumpInfo* combineCards[7] = { 0 }; //�g�����v�𓝍����Ĉ����z��
    for (int i = 0; i < 2; i++) {       //�����o�[�̎�D�𓝍�
        combineCards[i] = member->deck[i];
    }
    for (int i = 2; i < 7; i++) {       //�R�~���j�e�B�[�J�[�h�̎�D�𓝍�
        combineCards[i] = communityCard[i - 2];
    }
    combineCards[0]->cardRank;
    int cardsdata[RANK_COUNT + SUIT_COUNT] = { 0 };
    int roleData = 0;
    //1�̈ʁF2�ڂ̃J�[�h�̑傫��
    //10�̈ʁF1�ڂ̃J�[�h�̑傫��
    //100�̈ʁF��

    qsort(combineCards, 7, sizeof(TrumpInfo*), ToQsort); //�J�[�h���������Ƀ\�[�g����D

    //�f�[�^���W�v
    //�֐��̖���
    //  �������Ƃ��ă����o�[�C�R�~���j�e�B�J�[�h���Ƃ�C���̃����o�[�̖��𔻒肷��D
    // 
    //�����̐���
    //member : ���𔻒肷��Ώۂ̃����o�[�D
    //  member->name  : �����o�[�̖��O
    //  menber->point : ���Ԃ�g���
    //  member->chip  : ���ݏ������Ă���`�b�v
    //  member->id    : �����o�[�ŗL��ID
    //  member->isCOM : �l�Ԃ�CPU��
    //  member->deck[]: �����o�[�̎�D�D�ڍׂ͌�q����D
    //  member->ownPot: �����o�[�����݂����Ă���`�b�v.
    //  member->isDied: �����o�[�������Ă邩����ł邩�D
    //  member->numOfDeck:�����o�[�̎�D�̖���.
    //  member->role   : ���肳�ꂽ���D�ϐ��̌^�͎��R�Ɍ��߂Ă�
    //  member->nextMember: ���̃����o�[�ւ̃|�C���^�D
    // 
    // member->deck[]�̐���
    //  �E�z�񐔂͂Q�i��D��2��������j
    //  member->deck[i]->cardRank : i���ڂ̎�D�̐���
    //  member->deck[i]->cardSuit : i���ڂ̎�D�̊G�D.(spade = 0, heart = 1, dia = 2,clab = 3)
    // 
    // communitycard[] : �R�~���j�e�B�[�J�[�h���i�[����z��D�ő�5���Ȃ̂Ŕz�񒷂͂T
    //  communitycard[i]->cardRank
    //  communitycard[i]->cardSuit : �����o�[�̎�D�Ɠ��l
    // 
    // 
    // ���̑�
    //  �E�����炭�g���ϐ���
    //      member->deck[]
    //      member->role
    //      communitycard[]
    // �@ ��������Ȃ��D
    // �E���̑��K�v���Ǝv�����ϐ���֐�������͓K�X�ǉ����Ă������
    // �E�ق��ɒm�肽�����Ƃ���������k���N�ɕ����Ή��������邩�������D
    //
}

int ToQsort(const void** n1, const void** n2) {
    TrumpInfo* card1 = (TrumpInfo*)*n1;
    TrumpInfo* card2 = (TrumpInfo*)*n2;
    //(*n1)->cardRank;
    if (card1 == NULL) { return 1; }
    if (card2 == NULL) { return -1; }
    if (card1->cardRank > card2->cardRank) { return -1; }
    if (card1->cardRank < card2->cardRank) { return 1; }

    // �����N�������ꍇ�̓X�[�g���r
    if (card1->cardSuit > card2->cardSuit) { return 1; }
    if (card1->cardSuit < card2->cardSuit) { return -1; }

    return 0;
}

