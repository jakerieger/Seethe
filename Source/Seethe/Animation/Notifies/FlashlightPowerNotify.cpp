// Copyright (C) 2026 Jake Rieger


#include "FlashlightPowerNotify.h"
#include "FlashlightTool.h"
#include "SeetheCharacter.h"

void UFlashlightPowerNotify::Notify(USkeletalMeshComponent* MeshComp,
                                    UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference) {
    if (!MeshComp) { return; }

    const auto* Character = Cast<ASeetheCharacter>(MeshComp->GetOwner());
    if (Character && Character->HasEquippedItem()) {
        auto* Equipped = Character->GetCurrentEquipable();
        if (Equipped && Equipped->IsA<AFlashlightTool>()) {
            if (const auto* Flashlight = Cast<AFlashlightTool>(Equipped)) {
                Flashlight->ToggleOn();
            }
        }
    }
}