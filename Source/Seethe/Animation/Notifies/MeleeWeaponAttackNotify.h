// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MeleeWeaponAttackNotify.generated.h"

UCLASS()
class SEETHE_API UMeleeWeaponAttackNotify : public UAnimNotifyState {
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
                             UAnimSequenceBase* Animation,
                             float TotalDuration,
                             const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,
                           UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference) override;
};