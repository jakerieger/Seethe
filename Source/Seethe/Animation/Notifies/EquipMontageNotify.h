// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EquipMontageNotify.generated.h"

UENUM(BlueprintType)
enum class EEquipMontageNotifyAction : uint8 {
    Equip,
    UnEquip,
    Drop
};

UCLASS()
class SEETHE_API UEquipMontageNotify : public UAnimNotify {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    EEquipMontageNotifyAction EquipAction;

    virtual void Notify(USkeletalMeshComponent* MeshComp,
                        UAnimSequenceBase* Animation,
                        const FAnimNotifyEventReference& EventReference) override;
};