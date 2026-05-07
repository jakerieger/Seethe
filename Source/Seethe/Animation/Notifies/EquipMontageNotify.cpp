// Copyright (C) 2026 Jake Rieger


#include "EquipMontageNotify.h"
#include "SeetheCharacter.h"
#include "BaseEquipable.h"

void UEquipMontageNotify::Notify(USkeletalMeshComponent* MeshComp,
                                 UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference) {
    if (!MeshComp) { return; }

    const auto* Character = Cast<ASeetheCharacter>(MeshComp->GetOwner());
    if (Character && Character->HasEquippedItem()) {
        if (ABaseEquipable* Equipable = Character->GetCurrentEquipable()) {
            Equipable->OnEquipNotify(EquipAction);
        }
    }
}