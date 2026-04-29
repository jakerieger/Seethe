// Copyright (C) 2026 Jake Rieger


#include "InventorySlotWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Seethe/SeetheCharacter.h"

void UInventorySlotWidget::SetupSlot(const FInventorySlot& InItem, const int32 InIndex) {
    bEmptySlot = false;

    Item      = InItem;
    ItemIndex = InIndex;
    ItemIcon->SetBrushFromTexture(InItem.ItemData->ItemIcon);
    ItemQuantity->SetText(FText::FromString(*FString::Printf(TEXT("x%d"), InItem.Quantity)));

    ItemIcon->SetVisibility(ESlateVisibility::Visible);
    ItemQuantity->SetVisibility(ESlateVisibility::Visible);

    SetSelected(false);
}

void UInventorySlotWidget::SetupAsEmptySlot() {
    bEmptySlot = true;

    ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    ItemQuantity->SetVisibility(ESlateVisibility::Hidden);

    SetSelected(false);
}

void UInventorySlotWidget::SetSelected(const bool bSelected) {
    if (bEmptySlot) { return; }

    const auto BorderColor = bSelected
                                 ? FLinearColor(1.f, 0.79f, 0.3f, 1.f)
                                 : FLinearColor::White.CopyWithNewOpacity(0.1f);
    ItemBorder->SetBrushColor(BorderColor);

    ItemBackground->SetColorAndOpacity(FLinearColor::Black.CopyWithNewOpacity(bSelected ? 0.8f : 0.4f));
    ItemQuantity->SetColorAndOpacity(FLinearColor::White.CopyWithNewOpacity(bSelected ? 0.75f : 0.4f));
    bSlotSelected = bSelected;
}

bool UInventorySlotWidget::Selected() const {
    return bSlotSelected;
}

void UInventorySlotWidget::NativeConstruct() {
    Super::NativeConstruct();
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
    if (bEmptySlot) { return FReply::Unhandled(); }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
        OnSelected();
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply UInventorySlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
                                                            const FPointerEvent& InMouseEvent) {
    if (bEmptySlot) { return FReply::Unhandled(); }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
        OnUse();
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
    if (!bSlotSelected) {
        ItemBorder->SetBrushColor(FLinearColor::White.CopyWithNewOpacity(0.25f));
        ItemBackground->SetColorAndOpacity(FLinearColor::Black.CopyWithNewOpacity(0.8f));
    }
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
    if (!bSlotSelected) {
        ItemBorder->SetBrushColor(FLinearColor::White.CopyWithNewOpacity(0.1f));
        ItemBackground->SetColorAndOpacity(FLinearColor::Black.CopyWithNewOpacity(0.4f));
    }
}

FReply UInventorySlotWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) {
    if (bEmptySlot) { return FReply::Unhandled(); }
    OnSelected();
    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UInventorySlotWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent) {
    if (!bSlotSelected) {
        ItemBorder->SetBrushColor(FLinearColor::White.CopyWithNewOpacity(0.1f));
        ItemBackground->SetColorAndOpacity(FLinearColor::Black.CopyWithNewOpacity(0.4f));
    }
    Super::NativeOnFocusLost(InFocusEvent);
}

void UInventorySlotWidget::OnSelected() {
    if (bEmptySlot) { return; }
    OnSlotClicked.Broadcast(this);
}

void UInventorySlotWidget::OnUse() {
    if (!bSlotSelected || bEmptySlot) { return; }

    ASeetheCharacter* SC = Cast<ASeetheCharacter>(GetOwningPlayerPawn());
    if (SC) {
        SC->UseItem(ItemIndex, Item.ItemData->ItemCategory);
    }
}