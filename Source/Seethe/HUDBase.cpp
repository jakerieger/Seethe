// Copyright (C) 2026 Jake Rieger


#include "HUDBase.h"
#include "HUDWidget.h"
#include "Blueprint/UserWidget.h"

void AHUDBase::BeginPlay() {
    Super::BeginPlay();

    if (HUDWidgetClass) {
        HUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), HUDWidgetClass);
        HUDWidget->AddToViewport();
    }
}

void AHUDBase::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    if (HUDWidgetClass) {
        HUDWidget->RemoveFromViewport();
    }
    Super::EndPlay(EndPlayReason);
}