// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataAsset.h"
#include "Seethe/BaseEquipable.h"
#include "WeaponData.generated.h"

UCLASS()
class SEETHE_API UWeaponActionData : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UEquipableAnimationData> Animations;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UNiagaraSystem> ParticleSystem;

    UPROPERTY(EditAnywhere, Category = "Weapon Data")
    TObjectPtr<USoundBase> Sound;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TSubclassOf<UCameraShakeBase> CameraShake;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UForceFeedbackEffect> ForceFeedback;
};

UCLASS()
class SEETHE_API UWeaponImpactData : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UMaterialInterface> ImpactDecal;

    UPROPERTY(EditAnywhere, Category = "Weapon Data")
    TObjectPtr<USoundBase> ImpactSound;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UNiagaraSystem> ImpactParticleSystem;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UForceFeedbackEffect> ForceFeedback;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UMaterialInterface> ImpactExitDecal;

    UPROPERTY(EditAnywhere, Category = "Weapon Data")
    TObjectPtr<USoundBase> ImpactExitSound;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UNiagaraSystem> ImpactExitParticleSystem;
};

UCLASS()
class SEETHE_API UWeaponData : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UWeaponActionData> AttackData;

    UPROPERTY(EditAnywhere, Category="Weapon Data")
    TObjectPtr<UWeaponImpactData> ImpactData;
};

UCLASS()
class SEETHE_API UWeaponFirearmData : public UWeaponData {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Weapon Data")
    TObjectPtr<UWeaponActionData> ReloadData;
};