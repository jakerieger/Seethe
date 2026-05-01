// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SeetheGameInstance.generated.h"

UCLASS()
class SEETHE_API USeetheGameInstance : public UGameInstance {
    GENERATED_BODY()

protected:
    virtual void OnStart() override;
};