// Copyright (C) 2026 Jake Rieger


#include "HideCrosshairNotifyState.h"
#include "HUDWidget.h"
#include "SeetheCharacter.h"

void UHideCrosshairNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
                                            UAnimSequenceBase* Animation,
                                            float TotalDuration,
                                            const FAnimNotifyEventReference& EventReference) {
    if (!MeshComp) { return; }

    if (MeshComp->GetOwner() && MeshComp->GetOwner()->IsA<ASeetheCharacter>()) {
        if (const auto* SC = Cast<ASeetheCharacter>(MeshComp->GetOwner())) {
            if (auto* HUD = SC->GetHUDWidget()) {
                HUD->HideCrosshair();
            }
        }
    }
}

void UHideCrosshairNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
                                          UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference) {
    if (!MeshComp) { return; }

    if (MeshComp->GetOwner() && MeshComp->GetOwner()->IsA<ASeetheCharacter>()) {
        if (const auto* SC = Cast<ASeetheCharacter>(MeshComp->GetOwner())) {
            if (auto* HUD = SC->GetHUDWidget()) {
                HUD->ShowCrosshair();
            }
        }
    }
}