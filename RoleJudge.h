#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "gameset.h"



bool IsOnePair(TrumpInfo* cards[7]);
bool isTwoPair(TrumpInfo* cards[7]);
bool IsThreeCard(TrumpInfo* cards[7]);
bool IsStraight(TrumpInfo* cards[7]);
bool IsFlash(TrumpInfo* cards[7]);
bool isFullHouse(TrumpInfo* cards[7]);
bool IsFourCard(TrumpInfo* cards[7]);
bool IsStraightFlash(TrumpInfo* cards[7]);

bool IsStraight(TrumpInfo* cards[7]) {
    //カードはソート済み
    int continuity = 0;
    for (int i = 0; cards[i + 1] != NULL && i < 6; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank + 1) {
            continuity++;
        }
        else {
            continuity = 0;
        }
        if (continuity >= 4) {
            return true;
        }
    }
    return false;
}

bool IsFlash(TrumpInfo* cards[7]) {
    int countSuit[4] = { 0 };
    for (int i = 0; cards[i] != NULL && i < 7;i++) {
        countSuit[cards[i]->cardSuit]++;
    }
    for (int i = 0; i < 4; i++) {
        if (countSuit[i] >= 5) {
            return true;
        }
    }
    return false;
}

bool IsStraightFlash(TrumpInfo* cards[7]) {
    return IsStraight(cards) && IsFlash(cards);
}

bool IsFourCard(TrumpInfo* cards[7]) {
    int continuity = 0;
    for (int i = 0; cards[i + 1] != NULL && i < 6; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            continuity++;
        }
        else {
            continuity = 0;
        }
        if (continuity == 3) {
            return true;
        }
    }
    return false;
}

bool IsOnePair(TrumpInfo* cards[7]) {
    int continuity = 0;
    for (int i = 0; cards[i + 1] != NULL && i < 6; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            continuity++;
        }
    }
    if (continuity == 1) {
        return true;
    }
    return false;
}

bool IsThreeCard(TrumpInfo* cards[7]) {
    int continuity = 0;
    for (int i = 0; cards[i + 1] != NULL && i < 6; i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            continuity++;
        }
    }
    if (continuity == 2) {
        return true;
    }
    return false;
}

bool isTwoPair(TrumpInfo* cards[7]) {
    int continuity[2] = { 0 };
    int index = 0;
    for (int i = 0; cards[i + 1] != NULL && i < 6 && index <= 2;i++) {
        if (cards[i]->cardRank == cards[i + 1]->cardRank) {
            continuity[index]++;
        }
        else {
            if (continuity[index] != 1) {
                continuity[index] = 0;
            }
            else {
                index++;
            }
        }
    }
    if (continuity[0] == 1 && continuity[1] == 1) {
        return true;
    }
    return false;
}

bool isFullHouse(TrumpInfo* cards[7]) {
    return IsThreeCard(cards) && IsOnePair(cards);
}
