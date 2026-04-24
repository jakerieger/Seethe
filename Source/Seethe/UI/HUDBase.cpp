// Copyright (C) 2026 Jake Rieger


#include "HUDBase.h"
#include "HUDWidget.h"
#include "InventoryWidget.h"
#include "Blueprint/UserWidget.h"

void AHUDBase::ShowInventory() const {
    HUDWidget->SetVisibility(ESlateVisibility::Hidden);
    InventoryWidget->SetVisibility(ESlateVisibility::Visible);
}

void AHUDBase::HideInventory() const {
    HUDWidget->SetVisibility(ESlateVisibility::Visible);
    InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
}

void AHUDBase::BeginPlay() {
    Super::BeginPlay();

    if (HUDWidgetClass) {
        HUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), HUDWidgetClass);
        HUDWidget->AddToViewport();
    }

    if (InventoryWidgetClass) {
        InventoryWidget = CreateWidget<UInventoryWidget>(GetOwningPlayerController(), InventoryWidgetClass);
        InventoryWidget->AddToViewport();
        InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AHUDBase::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    if (HUDWidget) {
        HUDWidget->RemoveFromParent();
    }
    if (InventoryWidget) {
        InventoryWidget->RemoveFromParent();
    }
    Super::EndPlay(EndPlayReason);
}