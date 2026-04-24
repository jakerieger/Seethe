// Copyright (C) 2026 Jake Rieger


#include "InventoryComponent.h"
#include "../SeetheCharacter.h"

UInventoryComponent::UInventoryComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(UInventoryItem* Item) {
    FInventorySlot NewSlot;
    NewSlot.ItemData = Item;
    NewSlot.Quantity = Item->Quantity;
    InventoryItems.Add(NewSlot);
    UpdateInventory();
}

void UInventoryComponent::RemoveItem(const int32 Index) {
    if (InventoryItems.IsValidIndex(Index)) {
        InventoryItems.RemoveAt(Index);
    }
    UpdateInventory();
}

void UInventoryComponent::UseItem(const int32 Index, const bool bShouldConsume) {
    if (!CanUseItem(Index)) { return; }
    if (InventoryItems.IsValidIndex(Index)) {
        if (InventoryItems[Index].Quantity > 0) {
            InventoryItems[Index].ItemData->Use(Cast<ASeetheCharacter>(GetOwner()), Index);

            if (bShouldConsume) {
                if (--InventoryItems[Index].Quantity == 0) {
                    InventoryItems.RemoveAt(Index);
                }
            }

            UpdateInventory();
        }
    }
}

TArray<FInventorySlot>& UInventoryComponent::GetSlots() {
    return InventoryItems;
}

bool UInventoryComponent::CanUseItem(const int32 Index) const {
    if (InventoryItems.IsValidIndex(Index)) {
        return InventoryItems[Index].bCanUse;
    }
    return false;
}

void UInventoryComponent::UpdateInventory() {
    OnInventoryChanged.Broadcast();
}