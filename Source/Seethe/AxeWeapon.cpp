// Copyright (C) 2026 Jake Rieger


#include "AxeWeapon.h"
#include "SeetheCharacter.h"
#include "GameUtils.h"
#include "HUDWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AAxeWeapon::AAxeWeapon() {
    PrimaryActorTick.bCanEverTick = true;
}

void AAxeWeapon::Attack(APlayerController* PC, USkeletalMeshComponent* Arms) {
    if (bIsAttacking || SwingMontages.IsEmpty()) {
        return;
    }

    LastSocketLocation = FVector::ZeroVector;

    const int32 MontageIndex      = FMath::RandRange(0, SwingMontages.Num() - 1);
    UAnimMontage* SelectedMontage = *SwingMontages.begin();
    if (SwingMontages.IsValidIndex(MontageIndex)) {
        SelectedMontage = SwingMontages[MontageIndex];
    }

    UAnimInstance* AnimInstance = Arms->GetAnimInstance();
    AnimInstance->Montage_Play(SelectedMontage);

    if (AttackWeaponMontage) {
        WeaponMesh->GetAnimInstance()->Montage_Play(AttackWeaponMontage);
    }

    if (AttackSound) {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }
}

EWeaponType AAxeWeapon::GetWeaponType() {
    return EWeaponType::Axe;
}

void AAxeWeapon::Tick(const float DeltaSeconds) {
    Super::Tick(DeltaSeconds);

    if (bIsAttacking) {
        const FVector CurrentSocketLocation = GetWeaponMesh()->GetSocketLocation(HitSocketName);

        if (!LastSocketLocation.IsZero()) {
            PerformTrace(LastSocketLocation, CurrentSocketLocation);
        }

        LastSocketLocation = CurrentSocketLocation;
    }
}

void AAxeWeapon::SetHitDetectionActive(const bool bActive) {
    bIsAttacking = bActive;
}

void AAxeWeapon::PerformTrace(const FVector& Start, const FVector& End) {
    const FVector BladeHalfSize = FVector(5.f, 20.f, 5.f);
    FHitResult Hit;
    const FQuat TraceRotation = WeaponMesh->GetSocketQuaternion(HitSocketName);

    const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(),
                                                           Start,
                                                           End,
                                                           BladeHalfSize,
                                                           TraceRotation.Rotator(),
                                                           UEngineTypes::ConvertToTraceType(ECC_Visibility),
                                                           true,
                                                           TArray<AActor*>{this, GetOwner()},
                                                           EDrawDebugTrace::None,
                                                           Hit,
                                                           true);

    if (bHit) {
        if (Hit.BoneName != NAME_None) {
            OnHit(Hit);

            if (ImpactSoundEnemy) {
                UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundEnemy, Hit.ImpactPoint, FRotator::ZeroRotator);
            }

            if (ImpactEnemyDecal) {
                FGameUtils::SpawnDecalWithRandomRotation(GetWorld(), ImpactEnemyDecal, FVector(24.f, 24.f, 24.f), Hit);
            }

            if (ImpactEnemyFX) {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
                                                               ImpactEnemyFX,
                                                               Hit.ImpactPoint,
                                                               Hit.ImpactNormal.Rotation());
            }

            if (const ASeetheCharacter* Character = Cast<ASeetheCharacter>(GetOwner())) {
                PlayImpactForceFeedback(Character);
            }
        } else {
            // Handle FX
        }
    }
}

void AAxeWeapon::OnHit(const FHitResult& HitResult) {
    float DamageToApply = BaseDamage;

    if (HitResult.BoneName == FName("head")) {
        DamageToApply *= HeadshotMultiplier;
    }

    if (UPrimitiveComponent* HitComp = HitResult.GetComponent()) {
        if (HitComp->IsSimulatingPhysics(HitResult.BoneName)) {
            const FVector ImpactDirection = (HitResult.ImpactPoint - GetActorLocation()).GetSafeNormal();
            if (auto* Sk = Cast<USkeletalMeshComponent>(HitComp)) {
                Sk->AddImpulseToAllBodiesBelow(ImpactDirection * ImpactForce, HitResult.BoneName, false, true);
            }
        }
    }

    UGameplayStatics::ApplyDamage(HitResult.GetActor(),
                                  DamageToApply,
                                  GetOwner()->GetInstigatorController(),
                                  this,
                                  nullptr);

    if (const ASeetheCharacter* Character = Cast<ASeetheCharacter>(GetOwner())) {
        if (auto* HUD = Character->GetHUD()) {
            HUD->TriggerHitmarker();
        }
    }

    bIsAttacking = false; // Prevent multiple strikes   
}

void AAxeWeapon::BeginPlay() {
    Super::BeginPlay();
}