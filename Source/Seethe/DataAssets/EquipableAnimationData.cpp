// Copyright (C) 2026 Jake Rieger


#include "EquipableAnimationData.h"

FEquipableAnimationPlayResult UEquipableAnimationData::PlayAnimations(UAnimInstance* CharacterAI,
                                                                      UAnimInstance* EquipableAI) const {
    FEquipableAnimationPlayResult Result;

    if (CharacterAI) {
        Result.CharacterMontageDuration = CharacterAI->Montage_Play(CharacterMontage);
        Result.bPlayedCharacterMontage  = true;
    }

    if (EquipableAI) {
        Result.EquipableMontageDuration = EquipableAI->Montage_Play(EquipableMontage);
        Result.bPlayedEquipableMontage  = true;
    }

    return Result;
}