// Copyright (C) 2026 Jake Rieger


#include "SeetheController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SeetheGameInstance.h"

void ASeetheController::BeginPlay() {
    Super::BeginPlay();
}

void ASeetheController::SetupInputComponent() {
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent)) {
        EIC->BindAction(ToggleDevToolsAction, ETriggerEvent::Started, this, &ASeetheController::ToggleDevTools);
    }
}

void ASeetheController::ToggleDevTools() {
    if (const USeetheGameInstance* GI = Cast<USeetheGameInstance>(GetGameInstance())) {
        GI->ToggleDevToolsVisibility();
    }
}