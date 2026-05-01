// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "SeetheSession.generated.h"

UCLASS()
class SEETHE_API ASeetheSession : public AGameSession {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASeetheSession();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};