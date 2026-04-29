// Copyright (C) 2026 Jake Rieger


#include "InventoryComponent.h"
#include "Seethe/SeetheCharacter.h"

void FInventoryCategory::Initialize() {
    Slots.Empty(kInventorySize);
    for (int32 i = 0; i < kInventorySize; ++i) {
        Slots.Add(FInventorySlot());
    }
    UsedSlots = 0;

    UE_LOG(LogTemp, Warning, TEXT("FInventoryCategory::Initialize()"));
}

bool FInventoryCategory::AddItem(UInventoryItem* Item) {
    if ((UsedSlots + 1) > kInventorySize) { return false; }
    auto& Slot    = Slots[UsedSlots];
    Slot.ItemData = Item;
    Slot.Quantity = Item->Quantity;
    UsedSlots++;
    return true;
}

bool FInventoryCategory::RemoveItem(const int32 Index) {
    if (Slots.IsValidIndex(Index) && Slots[Index].ItemData != nullptr) {
        // Shift everything down
        for (int32 i = Index; i < Slots.Num() - 1; ++i) {
            Slots[i] = Slots[i + 1];
        }

        // Clear the very last slot
        Slots.Last() = FInventorySlot();
        UsedSlots--;

        return true;
    }

    return false;
}

UInventoryComponent::UInventoryComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(UInventoryItem* Item) {
    auto& ItemCategory = InventoryCategories.FindOrAdd(Item->ItemCategory);
    if (ItemCategory.AddItem(Item)) {
        UpdateInventory();
    }
}

void UInventoryComponent::RemoveItem(const int32 Index, const EInventoryCategory& Category) {
    auto& ItemCategory = InventoryCategories.FindOrAdd(Category);
    if (ItemCategory.RemoveItem(Index)) {
        UpdateInventory();
    }
}

void UInventoryComponent::UseItem(const int32 Index, const EInventoryCategory& Category) {
    if (!CanUseItem(Index, Category)) { return; }

    auto& ItemCategory       = InventoryCategories.FindOrAdd(Category);
    auto& [Slots, UsedSlots] = ItemCategory;
    if (Slots.IsValidIndex(Index)) {
        if (Slots[Index].Quantity > 0) {
            Slots[Index].ItemData->Use(Cast<ASeetheCharacter>(GetOwner()), Index, Category);

            if (Slots[Index].ShouldConsume()) {
                if (--Slots[Index].Quantity <= 0) {
                    ItemCategory.RemoveItem(Index);
                }
            }

            UpdateInventory();
        }
    }
}

TArray<FInventorySlot>& UInventoryComponent::GetSlots(const EInventoryCategory& Category) {
    return InventoryCategories.FindOrAdd(Category).Slots;
}

void UInventoryComponent::BeginPlay() {
    Super::BeginPlay();

    for (auto i = 0; i < static_cast<int32>(EInventoryCategory::NUM_CATEGORIES); i++) {
        EInventoryCategory Category = static_cast<EInventoryCategory>(i);
        auto& NewCategory           = InventoryCategories.Add(Category);
        NewCategory.Initialize();
    }
}

bool UInventoryComponent::CanUseItem(const int32 Index, const EInventoryCategory& Category) {
    auto& [Slots, UsedSlots] = InventoryCategories.FindOrAdd(Category);
    if (Slots.IsValidIndex(Index)) {
        return Slots[Index].ItemData->CanUse(Cast<ASeetheCharacter>(GetOwner()), Index, Category);
    }

    return false;
}

void UInventoryComponent::UpdateInventory() {
    OnInventoryChanged.Broadcast();
}