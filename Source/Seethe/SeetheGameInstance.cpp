// Copyright (C) 2026 Jake Rieger


#include "SeetheGameInstance.h"

#include "DevToolsWidget.h"
#include "SeetheCharacter.h"
#include "SeetheSaveGame.h"
#include "Kismet/GameplayStatics.h"

static FString GetSaveSlotName(const int32 Slot) {
    const FString PlatformUsername = UKismetSystemLibrary::GetPlatformUserName();
    return FString::Printf(TEXT("%s_%d"), *PlatformUsername, Slot);
}

void USeetheGameInstance::SaveGame(const int32 Slot) const {
    auto* SaveGameInstance = Cast<USeetheSaveGame>(
        UGameplayStatics::CreateSaveGameObject(USeetheSaveGame::StaticClass()));

    const ASeetheCharacter* SC = Cast<ASeetheCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (SC && SaveGameInstance) {
        SaveGameInstance->SaveSlotName   = GetSaveSlotName(Slot);
        SaveGameInstance->PlayerHealth   = SC->GetHealthPercent() * 100;
        SaveGameInstance->PlayerLocation = SC->GetActorLocation();
    }

    UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, 0);
}

void USeetheGameInstance::LoadGame(const int32 Slot) const {
    const FString SlotName = GetSaveSlotName(Slot);

    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0)) {
        if (const auto* LoadedGame = Cast<USeetheSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0))) {
            if (ASeetheCharacter* SC = Cast<ASeetheCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
                SC->SetHealth(LoadedGame->PlayerHealth);
                SC->SetActorLocation(LoadedGame->PlayerLocation);
            }
        }
    }
}

void USeetheGameInstance::ToggleDevToolsVisibility() const {
    if (DevToolsWidget) {
        const ESlateVisibility Visibility = DevToolsWidget->GetIsVisible()
                                                ? ESlateVisibility::Hidden
                                                : ESlateVisibility::Visible;
        DevToolsWidget->SetVisibility(Visibility);
    }
}

void USeetheGameInstance::OnStart() {
    Super::OnStart();

    if (bEnableDevTools && DevToolsWidgetClass) {
        auto* Widget = CreateWidget<UDevToolsWidget>(this, DevToolsWidgetClass);
        if (Widget) {
            Widget->AddToViewport(99);
            Widget->SetVisibility(ESlateVisibility::Hidden);
            DevToolsWidget = Widget;
        }
    }
}