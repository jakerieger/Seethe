// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SeetheGameMode.generated.h"

UCLASS()
class SEETHE_API ASeetheGameMode : public AGameModeBase {
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category="Seethe")
    TSubclassOf<class UDeathScreenWidget> DeathScreenClass;

    UFUNCTION(BlueprintCallable, Category="Seethe")
    void ShowDeathScreen(APlayerController* WidgetOwner) const;
};