// Copyright (C) 2026 Jake Rieger


#include "DeathScreenWidget.h"

void UDeathScreenWidget::NativeConstruct() {
    Super::NativeConstruct();

    GetOwningPlayer()
        ->GetWorld()
        ->GetTimerManager()
        .SetTimer(DestructHandle,
                  this,
                  &UDeathScreenWidget::SelfDestruct,
                  6.f,
                  false);

    PlayAnimation(FadeInOut);
}

void UDeathScreenWidget::SelfDestruct() {
    RemoveFromParent();
}