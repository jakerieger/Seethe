// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupAnimData.generated.h"

UCLASS()
class SEETHE_API UPickupAnimData : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
    TObjectPtr<UAnimMontage> EquippedMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
    TObjectPtr<UAnimMontage> UnEquippedMontage;

    UAnimMontage* GetPickupMontage(bool bEquipped);
};