// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../BaseEquipable.h"
#include "../Interactables/ItemPickupBase.h"
#include "InventoryItem.generated.h"

class ASeetheCharacter;

UCLASS()
class SEETHE_API UInventoryItem : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UAnimMontage> UseAnim;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
    TObjectPtr<UAnimMontage> DropAnim;

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void Use(ASeetheCharacter* Character, int32 Index);
    virtual void Use_Implementation(ASeetheCharacter* Character, int32 Index);

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void UseMultiple(ASeetheCharacter* Character, int32 Index, int32 Count);
    virtual void UseMultiple_Implementation(ASeetheCharacter* Character, int32 Index, int32 Count);

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void Drop(ASeetheCharacter* Character, int32 Index);
    virtual void Drop_Implementation(ASeetheCharacter* Character, int32 Index);

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void DropMultiple(ASeetheCharacter* Character, int32 Index, int32 Count);
    virtual void DropMultiple_Implementation(ASeetheCharacter* Character, int32 Index, int32 Count);
};

UCLASS()
class SEETHE_API UInventoryItemEquipable : public UInventoryItem {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TSubclassOf<AItemPickupBase> PickupClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TSubclassOf<ABaseEquipable> EquipableClass;

    virtual void Use_Implementation(ASeetheCharacter* Character, int32 Index) override;
};

USTRUCT(BlueprintType)
struct SEETHE_API FInventorySlot {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UInventoryItem> ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Quantity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    bool bCanUse{true};

    FInventorySlot() : ItemData(nullptr), Quantity(0) {}

    FInventorySlot(UInventoryItem* InData, int32 InQuantity)
        : ItemData(InData), Quantity(InQuantity) {}
};