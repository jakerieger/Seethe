// Copyright (C) 2026 Jake Rieger


#include "WeaponHitNotifyState.h"
#include "Seethe/SeetheCharacter.h"
#include "BaseWeapon.h"

void UWeaponHitNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
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

void UWeaponHitNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
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