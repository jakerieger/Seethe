// Copyright (C) 2026 Jake Rieger


#include "MainMenuWidget.h"

#include "Kismet/GameplayStatics.h"

bool UMainMenuWidget::HasExistingGameSave() const {
    return false;
}

void UMainMenuWidget::NativeOnInitialized() {
    Super::NativeOnInitialized();

    if (ContinueButton) {
        ContinueButton->OnPressed.AddDynamic(this, &UMainMenuWidget::OnContinue);
    }

    if (NewGameButton) {
        NewGameButton->OnPressed.AddDynamic(this, &UMainMenuWidget::OnNewGame);
    }

    if (LoadGameButton) {
        LoadGameButton->OnPressed.AddDynamic(this, &UMainMenuWidget::OnLoadGame);
    }

    if (SettingsButton) {
        SettingsButton->OnPressed.AddDynamic(this, &UMainMenuWidget::OnSettings);
    }

    if (QuitToDesktopButton) {
        QuitToDesktopButton->OnPressed.AddDynamic(this, &UMainMenuWidget::OnQuitToDesktop);
    }

    if (!HasExistingGameSave()) {
        ContinueButton->SetIsEnabled(false);
        ContinueButton->SetRenderOpacity(0.3f);

        LoadGameButton->SetIsEnabled(false);
        LoadGameButton->SetRenderOpacity(0.3f);
    }
}

void UMainMenuWidget::OnContinue() {
    UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_ConcordResearchBase"));
}

void UMainMenuWidget::OnNewGame() {
    // TODO: Create New Game screen
}

void UMainMenuWidget::OnLoadGame() {
    // TODO: Create Load Game screen
}

void UMainMenuWidget::OnSettings() {
    // TODO: Create Settings screen
}

void UMainMenuWidget::OnQuitToDesktop() {
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}