// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemManufacturer.generated.h"

UCLASS()
class SEETHE_API UItemManufacturer : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FText ManufacturerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FText ManufacturerDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UTexture2D> ManufacturerLogo;
};