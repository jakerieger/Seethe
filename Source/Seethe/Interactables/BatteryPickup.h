// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "ItemPickupBase.h"
#include "GameFramework/Actor.h"
#include "BatteryPickup.generated.h"

UCLASS()
class SEETHE_API ABatteryPickup : public AItemPickupBase {
    GENERATED_BODY()

public:
    ABatteryPickup();
    virtual FText GetInteractMessage() override;
};