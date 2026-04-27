// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Seethe/BaseEquipable.h"
#include "Seethe/Interactables/ItemPickupBase.h"
#include "ItemManufacturer.h"
#include "InventoryItem.generated.h"

class ASeetheCharacter;

UENUM(BlueprintType)
enum class EInventoryCategory : uint8 {
    EIC_Supplies UMETA(DisplayName = "Supplies"),
    EIC_Tool UMETA(DisplayName = "Tool"),
    EIC_Weapon UMETA(DisplayName = "Weapon"),
    EIC_Note UMETA(DisplayName = "Note"),
    NUM_CATEGORIES UMETA(Hidden),
};

UCLASS()
class SEETHE_API UInventoryItem : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    EInventoryCategory ItemCategory{EInventoryCategory::EIC_Supplies};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UItemManufacturer> ItemManufacturer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UStaticMesh> ItemPreviewMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    FTransform ItemPreviewTransform{FTransform::Identity};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    int32 MaxStackSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UAnimMontage> UseAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UAnimMontage> DropAnim;

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void Use(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category);
    virtual void Use_Implementation(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category);

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void UseMultiple(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category, int32 Count);
    virtual void UseMultiple_Implementation(ASeetheCharacter* Character,
                                            int32 Index,
                                            const EInventoryCategory& Category,
                                            int32 Count);

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void Drop(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category);
    virtual void Drop_Implementation(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category);

    UFUNCTION(BlueprintNativeEvent, Category="Inventory")
    void DropMultiple(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category, int32 Count);
    virtual void DropMultiple_Implementation(ASeetheCharacter* Character,
                                             int32 Index,
                                             const EInventoryCategory& Category,
                                             int32 Count);
};

UCLASS()
class SEETHE_API UInventoryItemEquipable : public UInventoryItem {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TSubclassOf<AItemPickupBase> PickupClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TSubclassOf<ABaseEquipable> EquipableClass;

    virtual void
    Use_Implementation(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category) override;
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

    bool ShouldConsume() const;

    FInventorySlot() : ItemData(nullptr), Quantity(0) {}

    FInventorySlot(UInventoryItem* InData, int32 InQuantity)
        : ItemData(InData), Quantity(InQuantity) {}
};