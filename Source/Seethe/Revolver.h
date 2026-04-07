// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Revolver.generated.h"

UCLASS()
class SEETHE_API ARevolver : public AActor {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> RevolverMesh;

public:
    ARevolver();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float BaseDamage = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<class ARevolverProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> ShootSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> EmptySound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> EquipSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<USoundBase> HolsterSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UAnimMontage> ShootMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UAnimMontage> EmptyMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UAnimMontage> EquipMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UAnimMontage> HolsterMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<class UNiagaraSystem> MuzzleFlashFX;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TSubclassOf<UCameraShakeBase> RecoilShake;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UStaticMesh> BulletMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transform")
    FTransform AttachOffset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transform")
    FTransform BulletOffset;

    UPROPERTY(BlueprintReadOnly, Category = "Revolver")
    FRotator CylinderRotation;

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    TArray<TObjectPtr<UStaticMeshComponent>> VisualBullets;

public:
    UFUNCTION(BlueprintCallable, Category = "Revolver")
    void Equip(USkeletalMeshComponent* Arms, bool Holstered = false);

    UFUNCTION(BlueprintCallable, Category = "Revolver")
    void Fire(USkeletalMeshComponent* Arms);

    UFUNCTION(BlueprintCallable, Category = "Revolver")
    void ToggleHolster(USkeletalMeshComponent* Arms);

    UFUNCTION(BlueprintCallable, Category = "Revolver")
    bool GetHolstered() const;

private:
    int32 CurrentAmmo;
    bool bHolstered              = false;
    bool bIsHolstering           = false;
    float CylinderRotationTarget = 0.0f;

    UFUNCTION()
    void OnHolsterMontageEnded(UAnimMontage* Montage, bool bInterrupted, USkeletalMeshComponent* Arms);
};