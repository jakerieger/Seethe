// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "Templates/UnrealTypeTraits.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

inline constexpr uint32 kInventorySize {25};

USTRUCT(BlueprintType)
struct FInventoryCategory {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FInventorySlot> Slots;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 UsedSlots {0};

    void Initialize();
    bool AddItem(UInventoryItem* Item);
    bool RemoveItem(int32 Index);
};

template<typename T>
concept IsInventoryItem = TIsDerivedFrom<T, UInventoryItem>::IsDerived;

template<IsInventoryItem T>
struct FFindItemResult {
    bool bFound {false};
    T* Item {nullptr};
    FInventorySlot* Slot {nullptr};
    int32 Index {-1};

    FFindItemResult(T* InItem, FInventorySlot& InSlot, const int32 InIndex) : bFound(true), Item(InItem), Slot(&InSlot),
                                                                              Index(InIndex) {}

    FFindItemResult() = default;
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

    virtual void BeginPlay() override;
    void ResetInventory();

    template<IsInventoryItem T>
    FFindItemResult<T> FindItem(const EInventoryCategory& Category) {
        const auto* CategoryData = InventoryCategories.Find(Category);
        if (!CategoryData) { return FFindItemResult<T>(); }

        const auto& Slots = GetSlots(Category);
        for (int32 i = 0; i < CategoryData->UsedSlots; i++) {
            auto& Slot = const_cast<FInventorySlot&>(Slots[i]);
            if (Slot.ItemData && Slot.ItemData->IsA<T>()) {
                return FFindItemResult<T>(Cast<T>(Slot.ItemData), Slot, i);
            }
        }

        return FFindItemResult<T>();
    }

private:
    bool CanUseItem(const int32 Index, const EInventoryCategory& Category);
    void UpdateInventory();
};