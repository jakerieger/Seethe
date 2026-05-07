// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "WeaponData.h"
#include "Seethe/Interfaces/WeaponInterface.h"
#include "Seethe/BaseEquipable.h"
#include "BaseWeapon.generated.h"

// forward decl
class ASeetheCharacter;

UCLASS()
class SEETHE_API ABaseWeapon : public ABaseEquipable, public IWeaponInterface {
    GENERATED_BODY()

public:
    ABaseWeapon();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<UWeaponData> WeaponData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
    float BaseDamage {20.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
    float HeadshotMultiplier {2.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
    float AttackRate {0.2f};

    /** ABaseEquipable overrides **/
    virtual void Use(ASeetheCharacter* Character) override { Attack(Character); };

    /** IWeaponInterface overrides **/
    virtual bool CanAttack() override;
    virtual void Attack(ASeetheCharacter* Character) override {}
    virtual void Reload(ASeetheCharacter* Character) override {}
    virtual FString GetName() override { return {}; }
    virtual int32 GetCurrentAmmo() override { return 0; }
    virtual int32 GetTotalAmmo() override { return 0; }

    /** Virtual methods **/
    virtual void SetHitDetectionActive(bool bActive) {}

protected:
    virtual void PerformTrace(const FVector& Start, const FVector& End) override {}
    virtual void OnHit(const FHitResult& HitResult) {}

    float LastAttackTime {0.0f};
};