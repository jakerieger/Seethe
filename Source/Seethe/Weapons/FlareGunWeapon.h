// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "FlareGunWeapon.generated.h"

UCLASS()
class SEETHE_API AFlareGunWeapon : public ABaseWeapon {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AFlareGunWeapon();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};