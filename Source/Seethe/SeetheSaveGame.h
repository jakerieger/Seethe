// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SeetheSaveGame.generated.h"

UCLASS()
class SEETHE_API USeetheSaveGame : public USaveGame {
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere)
    float PlayerHealth;

    UPROPERTY(VisibleAnywhere)
    FVector PlayerLocation;

    UPROPERTY(VisibleAnywhere)
    FString SaveSlotName;

    USeetheSaveGame();
};