// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "FlareGunProjectile.h"
#include "FlareGunWeapon.generated.h"

UCLASS()
class SEETHE_API AFlareGunWeapon : public ABaseWeapon {
    GENERATED_BODY()

public:
    AFlareGunWeapon();
    virtual void Attack(ASeetheCharacter* Character) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MuzzleSocketName {"S_Muzzle"};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AFlareGunProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCameraShakeBase> RecoilShake;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UForceFeedbackEffect> RecoilFFB;
};