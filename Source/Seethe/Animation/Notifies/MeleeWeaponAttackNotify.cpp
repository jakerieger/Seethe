// Copyright (C) 2026 Jake Rieger


#include "MeleeWeaponAttackNotify.h"
#include "Seethe/SeetheCharacter.h"
#include "BaseWeapon.h"

void UMeleeWeaponAttackNotify::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                           UAnimSequenceBase* Animation,
                                           float TotalDuration,
                                           const FAnimNotifyEventReference& EventReference) {
    if (!MeshComp) { return; }
    if (const ASeetheCharacter* Character = Cast<ASeetheCharacter>(MeshComp->GetOwner())) {
        if (!Character->HasEquippedItem()) { return; }

        if (ABaseWeapon* Weapon = Character->GetCurrentWeapon()) {
            Weapon->SetHitDetectionActive(true);
        }
    }
}

void UMeleeWeaponAttackNotify::NotifyEnd(USkeletalMeshComponent* MeshComp,
                                         UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference) {
    if (!MeshComp) { return; }
    if (const ASeetheCharacter* Character = Cast<ASeetheCharacter>(MeshComp->GetOwner())) {
        if (!Character->HasEquippedItem()) { return; }

        if (ABaseWeapon* Weapon = Character->GetCurrentWeapon()) {
            Weapon->SetHitDetectionActive(false);
        }
    }
}