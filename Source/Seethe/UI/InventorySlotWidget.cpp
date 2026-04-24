// Copyright (C) 2026 Jake Rieger


#include "InventorySlotWidget.h"
#include "../SeetheCharacter.h"

void UInventorySlotWidget::SetupSlot(const FInventorySlot& InItem, const int32 InIndex) {
    Item      = InItem;
    ItemIndex = InIndex;
    ItemName->SetText(InItem.ItemData->ItemName);
    ItemQuantity->SetText(FText::FromString(*FString::Printf(TEXT("x%d"), InItem.Quantity)));
}

void UInventorySlotWidget::NativeConstruct() {
    Super::NativeConstruct();

    if (ItemButton) {
        ItemButton->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnButtonClicked);
    }
}

void UInventorySlotWidget::OnButtonClicked() {
    const APlayerController* PC = GetOwningPlayer();
    if (PC) {
        ASeetheCharacter* Character = Cast<ASeetheCharacter>(PC->GetPawn());
        if (Character) {
            bool bShouldConsume = true;
            if (Item.ItemData.IsA(UInventoryItemEquipable::StaticClass())) { bShouldConsume = false; }
            Character->UseItem(ItemIndex, bShouldConsume);
        }
    }
}