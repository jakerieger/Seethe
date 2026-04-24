// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
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

    UPROPERTY(EditAnywhere, Category="Stats")
    float BaseDamage{20.0f};

    UPROPERTY(EditAnywhere, Category="Stats")
    float HeadshotMultiplier{2.0f};

    UPROPERTY(EditAnywhere, Category="Stats")
    float AttackRate{0.2f};

    /** ABaseEquipable overrides **/
    virtual void Use(ASeetheCharacter* Character) override { Attack(Character); };

    /** IWeaponInterface overrides **/
    virtual bool CanAttack() override;
    virtual void Attack(ASeetheCharacter* Character) override {}
    virtual void Reload(ASeetheCharacter* Character) override {}

    /** Virtual methods **/
    virtual void SetHitDetectionActive(bool bActive) {}

protected:
    virtual void PerformTrace(const FVector& Start, const FVector& End) override {}
    virtual void OnHit(const FHitResult& HitResult) {}

    void PlayImpactForceFeedback(const ASeetheCharacter* Character) const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USoundBase> AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USoundBase> ImpactSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USoundBase> ImpactSoundEnemy;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditAnywhere, Category="Weapon")
    TObjectPtr<UNiagaraSystem> ImpactFX;

    UPROPERTY(EditAnywhere, Category="Weapon")
    TObjectPtr<UNiagaraSystem> ImpactEnemyFX;

    UPROPERTY(EditAnywhere, Category="Weapon")
    TObjectPtr<UNiagaraSystem> ExitWoundFX;

    UPROPERTY(EditAnywhere, Category="Weapon")
    TObjectPtr<UMaterialInterface> ImpactDecal;

    UPROPERTY(EditAnywhere, Category="Weapon")
    TObjectPtr<UMaterialInterface> ImpactEnemyDecal;

    UPROPERTY(EditAnywhere, Category="Weapon")
    TObjectPtr<UMaterialInterface> BloodSpatterDecal;

    UPROPERTY(EditAnywhere, Category="Weapon")
    TObjectPtr<UForceFeedbackEffect> ImpactFFB;

    float LastAttackTime{0.0f};
};