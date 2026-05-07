// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FlareGunCockedNotify.generated.h"

UCLASS()
class SEETHE_API UFlareGunCockedNotify : public UAnimNotify {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    bool bCocked {true};

    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};