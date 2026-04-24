// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SEETHE_API UInventoryComponent : public UActorComponent {
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    TArray<FInventorySlot> InventoryItems;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void AddItem(UInventoryItem* Item);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void RemoveItem(int32 Index);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void UseItem(int32 Index, bool bShouldConsume);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    TArray<FInventorySlot>& GetSlots();

private:
    bool CanUseItem(const int32 Index) const;
    void UpdateInventory();
};