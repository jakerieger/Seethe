// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Seethe/Inventory/InventoryItem.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventorySlotWidget.generated.h"

class UInventorySlotWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, UInventorySlotWidget*, SlotWidget);

UCLASS()
class SEETHE_API UInventorySlotWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category="Inventory")
    FOnSlotClicked OnSlotClicked;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> ItemBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemBackground;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemQuantity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    FInventorySlot Item;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
    int32 ItemIndex;

    void SetupSlot(const FInventorySlot& InItem, int32 InIndex);
    void SetupAsEmptySlot();
    void SetSelected(bool bSelected);
    bool Selected() const;

protected:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply
    NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    UFUNCTION()
    void OnSelected();

    UFUNCTION()
    void OnUse();

private:
    bool bSlotSelected{false};
    bool bEmptySlot{true};
};