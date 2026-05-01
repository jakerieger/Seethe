// Copyright (C) 2026 Jake Rieger


#include "SeetheGameMode.h"
#include "DeathScreenWidget.h"

void ASeetheGameMode::ShowDeathScreen(APlayerController* WidgetOwner) const {
    if (DeathScreenClass) {
        UDeathScreenWidget* DeathScreen = CreateWidget<UDeathScreenWidget>(WidgetOwner, DeathScreenClass);
        if (DeathScreen) {
            DeathScreen->AddToViewport(1);
        }
    }
}