// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class SEETHE_API UMainMenuWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ContinueButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> NewGameButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> LoadGameButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SettingsButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitToDesktopButton;

    UFUNCTION()
    bool HasExistingGameSave() const;

protected:
    virtual void NativeOnInitialized() override;

private:
    UFUNCTION()
    void OnContinue();

    UFUNCTION()
    void OnNewGame();

    UFUNCTION()
    void OnLoadGame();

    UFUNCTION()
    void OnSettings();

    UFUNCTION()
    void OnQuitToDesktop();
};