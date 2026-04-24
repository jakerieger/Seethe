// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "AxeWeapon.generated.h"

UCLASS()
class SEETHE_API AAxeWeapon : public ABaseWeapon {
    GENERATED_BODY()

public:
    AAxeWeapon();

    /** AActor overrides **/
    virtual void Tick(float DeltaSeconds) override;

    /** ABaseWeapon overrides **/
    virtual void Attack(ASeetheCharacter* Character) override;

    /* Attack frames are notifies in montage, this is what the notify uses to update whether attack frames are active */
    virtual void SetHitDetectionActive(bool bActive) override;

    /** PROPERTIES **/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float ImpactForce = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<TObjectPtr<UAnimMontage>> SwingMontages;

protected:
    virtual void BeginPlay() override;
    virtual void PerformTrace(const FVector& Start, const FVector& End) override;
    virtual void OnHit(const FHitResult& HitResult) override;

private:
    bool bIsAttacking{false};
    FVector LastSocketLocation;
    const FName HitSocketName{"S_Hit_Detect"};
};