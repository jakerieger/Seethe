// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "SeetheController.generated.h"

UCLASS()
class SEETHE_API ASeetheController : public APlayerController {
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UInputAction> ToggleDevToolsAction;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void ToggleDevTools();
};