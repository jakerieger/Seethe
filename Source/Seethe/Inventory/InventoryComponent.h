// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

inline constexpr uint32 kInventorySize{25};

USTRUCT(BlueprintType)
struct FInventoryCategory {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FInventorySlot> Slots;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 UsedSlots{0};

    void Initialize();
    bool AddItem(UInventoryItem* Item);
    bool RemoveItem(int32 Index);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SEETHE_API UInventoryComponent : public UActorComponent {
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    TMap<EInventoryCategory, FInventoryCategory> InventoryCategories;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void AddItem(UInventoryItem* Item);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void RemoveItem(int32 Index, const EInventoryCategory& Category);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void UseItem(int32 Index, const EInventoryCategory& Category);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    TArray<FInventorySlot>& GetSlots(const EInventoryCategory& Category);

protected:
    virtual void BeginPlay() override;

private:
    bool CanUseItem(const int32 Index, const EInventoryCategory& Category);
    void UpdateInventory();
};