// Copyright (C) 2026 Jake Rieger


#include "HUDBase.h"
#include "HUDWidget.h"
#include "InventoryWidget.h"
#include "InventoryItem3dPreview.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneCaptureComponent2D.h"

void AHUDBase::ShowInventory() const {
    HUDWidget->SetVisibility(ESlateVisibility::Hidden);
    InventoryWidget->SetVisibility(ESlateVisibility::Visible);
}

void AHUDBase::HideInventory() const {
    HUDWidget->SetVisibility(ESlateVisibility::Visible);
    InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
}

bool AHUDBase::IsInventoryOpen() const {
    return InventoryWidget->IsVisible();
}

AInventoryItem3dPreview* AHUDBase::GetInventoryItem3dPreview() const {
    return PreviewActor;
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

    if (PreviewActorClass) {
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.Owner      = GetOwner();
        SpawnInfo.Instigator = GetInstigator();
        auto* NewActor       = GetWorld()->SpawnActor<AInventoryItem3dPreview>(PreviewActorClass, SpawnInfo);
        if (NewActor) {
            PreviewActor = NewActor;
            PreviewActor->SetActorLocation(FVector(999999.f, 999999.f, 999999.f));
        }
    }
}

void AHUDBase::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    if (HUDWidget) {
        HUDWidget->RemoveFromParent();
    }
    if (InventoryWidget) {
        InventoryWidget->RemoveFromParent();
    }
    if (PreviewActor) {
        PreviewActor->Destroy();
    }
    Super::EndPlay(EndPlayReason);
}