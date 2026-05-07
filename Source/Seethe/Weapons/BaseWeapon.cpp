// Copyright (C) 2026 Jake Rieger


#include "BaseWeapon.h"
#include "Seethe/SeetheCharacter.h"

ABaseWeapon::ABaseWeapon() {
    PrimaryActorTick.bCanEverTick = false;
}

bool ABaseWeapon::CanAttack() {
    bool bCanAttack = false;

    if (const float CurrentTime = GetWorld()->GetTimeSeconds(); CurrentTime - LastAttackTime >= AttackRate) {
        bCanAttack     = true;
        LastAttackTime = CurrentTime;
    }

    return bCanAttack;
}