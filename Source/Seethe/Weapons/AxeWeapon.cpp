// Copyright (C) 2026 Jake Rieger


#include "AxeWeapon.h"
#include "Seethe/SeetheCharacter.h"
#include "Seethe/UI/HUDWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Seethe/FirstPersonAnimInstance.h"
#include "Seethe/SeetheUtilities.h"

AAxeWeapon::AAxeWeapon() {
    PrimaryActorTick.bCanEverTick = true;
}

void AAxeWeapon::Attack(ASeetheCharacter* Character) {
    if (bIsAttacking || SwingMontages.IsEmpty() || !Character) {
        return;
    }

    LastSocketLocation = FVector::ZeroVector;

    const int32 MontageIndex      = FMath::RandRange(0, SwingMontages.Num() - 1);
    UAnimMontage* SelectedMontage = *SwingMontages.begin();
    if (SwingMontages.IsValidIndex(MontageIndex)) {
        SelectedMontage = SwingMontages[MontageIndex];
    }

    Character->GetAnimInstance1P()->Montage_Play(SelectedMontage);

    if (AttackSound) {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }
}

void AAxeWeapon::Tick(const float DeltaSeconds) {
    Super::Tick(DeltaSeconds);

    if (bIsAttacking) {
        const FVector CurrentSocketLocation = GetMesh1P()->GetSocketLocation(HitSocketName);

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
    const FVector BladeHalfSize = FVector(5.0f, 20.0f, 5.0f);
    FHitResult Hit;
    const FQuat TraceRotation = GetMesh1P()->GetSocketQuaternion(HitSocketName);

    const bool bHit = UKismetSystemLibrary::BoxTraceSingle(this,
                                                           Start,
                                                           End,
                                                           BladeHalfSize,
                                                           TraceRotation.Rotator(),
                                                           UEngineTypes::ConvertToTraceType(ECC_Visibility),
                                                           true,
                                                           TArray<AActor*> {this, GetOwner()},
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
                USeetheUtilities::SpawnHitDecalWithRandomRollRotation(this,
                                                                      ImpactEnemyDecal,
                                                                      FVector(24.0f, 24.0f, 24.0f),
                                                                      Hit);
            }

            if (ImpactEnemyFX) {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
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
        if (auto* HUD = Character->GetHUDWidget()) {
            HUD->TriggerHitmarker();
        }
    }

    bIsAttacking = false; // Prevent multiple strikes   
}

void AAxeWeapon::BeginPlay() {
    Super::BeginPlay();
}