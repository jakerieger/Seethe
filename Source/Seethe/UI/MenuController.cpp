// Copyright (C) 2026 Jake Rieger


#include "MenuController.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

AMenuController::AMenuController() {
    bShowMouseCursor       = true;
    bEnableClickEvents     = true;
    bEnableMouseOverEvents = true;
}

void AMenuController::BeginPlay() {
    Super::BeginPlay();

    if (MainMenuClass) {
        if (auto* Widget = CreateWidget<UMainMenuWidget>(this, MainMenuClass, FName("MainMenu"))) {
            MainMenuWidget = Widget;
            MainMenuWidget->AddToViewport();
        }
    }

    SetInputMode(FInputModeUIOnly());

    // Attach to level camera actor
    if (AActor* CameraActor = UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass())) {
        SetViewTarget(CameraActor);
    }
}