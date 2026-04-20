// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponInterface.h"
#include "NiagaraSystem.h"
#include "BaseWeapon.generated.h"

// forward decl
class ASeetheCharacter;

UCLASS()
class SEETHE_API ABaseWeapon : public AActor, public IWeaponInterface {
    GENERATED_BODY()

public:
    ABaseWeapon();

    /** PROPERTIES **/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USkeletalMeshComponent> WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FName LeftHandSocketName = FName("S_Grip_Left");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
    FTransform AttachOffset;

    UPROPERTY(EditAnywhere, Category="Stats")
    float BaseDamage{20.0f};

    UPROPERTY(EditAnywhere, Category="Stats")
    float HeadshotMultiplier{2.0f};

    UPROPERTY(EditAnywhere, Category="Stats")
    float AttackRate{0.2f};

    /** IWeaponInterface methods **/
    virtual void Equip(APlayerController* PC, USkeletalMeshComponent* Arms) override;
    virtual void Inspect(APlayerController* PC, USkeletalMeshComponent* Arms) override;
    virtual void ToggleHolster(APlayerController* PC, USkeletalMeshComponent* Arms) override;
    virtual bool CanAttack() override;
    virtual bool IsHolstered() const override;
    virtual void Attack(APlayerController* PC, USkeletalMeshComponent* Arms) override {}
    virtual void Reload(APlayerController* PC, USkeletalMeshComponent* Arms) override {}
    virtual EWeaponType GetWeaponType() override { return EWeaponType::None; }

    /** Virtual methods **/
    virtual void SetHitDetectionActive(bool bActive) {}
    virtual void Drop(USkeletalMeshComponent* Arms);

    /** Getters **/
    UFUNCTION(BlueprintCallable, Category = "HUD")
    UTexture2D* GetCrosshairTexture() const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    USkeletalMeshComponent* GetWeaponMesh() const;

    UFUNCTION(BlueprintCallable, Category = "Animation (Procedural)")
    FTransform GetLeftHandSocketTransform(USkeletalMeshComponent* Mesh1P) const;

protected:
    virtual void PerformTrace(const FVector& Start, const FVector& End) override {}
    virtual void OnHit(const FHitResult& HitResult) {}
    virtual void OnHolstered() {}
    virtual void OnUnholstered() {}

    void PlayImpactForceFeedback(const ASeetheCharacter* Character) const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> EquipSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> HolsterSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> ImpactSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> ImpactSoundEnemy;

    UPROPERTY(EditAnywhere, Category="Animation")
    TSubclassOf<UAnimInstance> WeaponAnimLayer;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Arms)")
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Arms)")
    TObjectPtr<UAnimMontage> EquipMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Arms)")
    TObjectPtr<UAnimMontage> HolsterMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Arms)")
    TObjectPtr<UAnimMontage> InspectMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Weapon)")
    TObjectPtr<UAnimMontage> AttackWeaponMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Weapon)")
    TObjectPtr<UAnimMontage> EquipWeaponMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Weapon)")
    TObjectPtr<UAnimMontage> HolsterWeaponMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation (Weapon)")
    TObjectPtr<UAnimMontage> InspectWeaponMontage;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UNiagaraSystem> ImpactFX;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UNiagaraSystem> ImpactEnemyFX;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UNiagaraSystem> ExitWoundFX;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UMaterialInterface> ImpactDecal;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UMaterialInterface> ImpactEnemyDecal;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UMaterialInterface> BloodSpatterDecal;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UTexture2D> CrosshairTexture;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<UForceFeedbackEffect> ImpactFFB;

    bool bHolstered{false};
    bool bIsHolstering{false};
    float LastAttackTime{0.0f};

private:
    void OnHolsterMontageEnded(UAnimMontage* Montage, bool bInterrupted, USkeletalMeshComponent* Arms);
};