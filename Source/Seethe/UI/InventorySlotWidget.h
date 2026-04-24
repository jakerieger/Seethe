// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "../Inventory/InventoryItem.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "InventorySlotWidget.generated.h"

UCLASS()
class SEETHE_API UInventorySlotWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ItemButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemName;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemQuantity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    FInventorySlot Item;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    int32 ItemIndex;

    void SetupSlot(const FInventorySlot& InItem, int32 InIndex);

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnButtonClicked();
};