// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InventorySlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "InventoryWidget.generated.h"

class UInventoryItemData;
class UInventoryComponent;

UCLASS()
class SEETHE_API UInventoryWidget : public UUserWidget {
    GENERATED_BODY()
    friend class ASeetheCharacter;

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> ItemContainer;

    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetInventoryIndex() const;

    UFUNCTION(BlueprintPure, Category="Inventory")
    UInventoryComponent* GetInventoryComponent();

    UFUNCTION(BlueprintCallable, Category="Inventory")
    TArray<FInventorySlot> GetInventoryItems();

    UPROPERTY(EditAnywhere, Category="Inventory")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

protected:
    UFUNCTION()
    void OnInventoryUpdate();

private:
    float LookX, LookY;
    void UpdateLookAxes(float X, float Y);
    void InitializeWidget(UInventoryComponent* InInventory);

    UPROPERTY()
    TObjectPtr<UInventoryComponent> LinkedInventory;

    UPROPERTY()
    TArray<FInventorySlot> Items;

    void PositionSlotInCircle(const UInventorySlotWidget* InSlot, int32 Index, int32 Total) const;
};