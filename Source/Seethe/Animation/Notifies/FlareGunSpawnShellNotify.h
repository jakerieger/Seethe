// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FlareGunSpawnShellNotify.generated.h"

UCLASS()
class SEETHE_API UFlareGunSpawnShellNotify : public UAnimNotify {
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};