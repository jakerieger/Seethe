// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface {
    GENERATED_BODY()
};

class SEETHE_API IInteractableInterface {
    GENERATED_BODY()

public:
    virtual void Interact(class ASeetheCharacter* Character) = 0;
    virtual void LookAt() = 0;
    virtual void LookAway() = 0;
    virtual FText GetInteractMessage() = 0;
};