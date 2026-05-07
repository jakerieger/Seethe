// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "CharacterInputData.generated.h"

UCLASS()
class SEETHE_API UCharacterInputData : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> UseAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ReloadAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> InventoryAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> UnEquipAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> JumpAction;
};