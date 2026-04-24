// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "ItemPickupBase.h"
#include "FlashlightPickup.generated.h"

UCLASS()
class SEETHE_API AFlashlightPickup : public AItemPickupBase {
    GENERATED_BODY()

public:
    AFlashlightPickup();
    virtual FText GetInteractMessage() override;
};