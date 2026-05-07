// Copyright (C) 2026 Jake Rieger


#include "PickupInteractWidget.h"

void UPickupInteractWidget::SetItem(const UInventoryItem* Item) const {
    ItemNameText->SetText(Item->ItemName);

    const FText QuantityFmt = NSLOCTEXT("UI", "Interact", "[x{0}]");
    ItemQuantityText->SetText(FText::Format(QuantityFmt, Item->Quantity));
}

void UPickupInteractWidget::SetVisible(const bool bVisible) {
    if (bVisible) {
        SetVisibility(ESlateVisibility::Visible);
    } else {
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UPickupInteractWidget::NativeConstruct() {
    Super::NativeConstruct();

    if (InputIcon && ActionInputIconData) {
        InputIcon->SetBrushFromTexture(ActionInputIconData->IconMouseKeyboard);
    }
}