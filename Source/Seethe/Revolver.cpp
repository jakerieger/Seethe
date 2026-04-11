// Copyright (C) 2026 Jake Rieger

#include "Revolver.h"
#include "HUDWidget.h"
#include "GameUtils.h"
#include "NiagaraFunctionLibrary.h"
#include "SeetheCharacter.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

static constexpr int32 KTotalChambers      = 5;
static constexpr float KChamberRotateDelta = 360.0f / KTotalChambers;

ARevolver::ARevolver() {
    PrimaryActorTick.bCanEverTick = true; // Needed for cylinder rotation lerp

    RevolverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RevolverMesh"));
    RevolverMesh->SetupAttachment(RootComponent);

    CurrentAmmo = KTotalChambers;
}

void ARevolver::LoadBullets(bool Visible = false) {
    if (!VisualBullets.IsEmpty()) {
        for (const auto& Bullet : VisualBullets) {
            if (Bullet.Get() && Bullet->IsValidLowLevel()) {
                Bullet->DestroyComponent();
            }
        }

        VisualBullets.Empty();
    }

    for (int32 i = 0; i < KTotalChambers; i++) {
        FName SocketName = FName(*FString::Printf(TEXT("Chamber_%d"), i + 1));

        UStaticMeshComponent* Bullet = NewObject<UStaticMeshComponent>(this);
        if (Bullet) {
            Bullet->SetStaticMesh(BulletMesh);
            Bullet->RegisterComponent();
            Bullet->AttachToComponent(RevolverMesh,
                                      FAttachmentTransformRules::SnapToTargetIncludingScale,
                                      SocketName);
            Bullet->SetRelativeTransform(BulletOffset);
            Bullet->SetVisibility(Visible);
            VisualBullets.Add(Bullet);
        }
    }
}

void ARevolver::BeginPlay() {
    Super::BeginPlay();
    LoadBullets();
}

void ARevolver::Tick(const float DeltaSeconds) {
    Super::Tick(DeltaSeconds);

    const FRotator TargetRotation(CylinderRotationTarget, 0.0f, 0.0f);
    CylinderRotation = FMath::RInterpTo(CylinderRotation, TargetRotation, DeltaSeconds, 10.0f);
}

void ARevolver::Equip(USkeletalMeshComponent* Arms, const bool Holstered) {
    const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    AttachToComponent(Arms, AttachmentRules, FName("Wrist_R"));
    RevolverMesh->SetRelativeTransform(AttachOffset);

    RevolverMesh->SetVisibility(!Holstered);
    Arms->SetVisibility(!Holstered);

    for (const auto& Bullet : VisualBullets) {
        if (Bullet->IsValidLowLevel() && Bullet.Get() != nullptr) {
            Bullet->SetVisibility(!Holstered);
        }
    }

    bHolstered    = Holstered;
    bIsHolstering = false;
}

void ARevolver::FireEmpty(USkeletalMeshComponent* Arms) {
    if (EmptySound) {
        UGameplayStatics::PlaySoundAtLocation(this, EmptySound, GetOwner()->GetActorLocation());
    }

    if (EmptyMontage) {
        Arms->GetAnimInstance()->Montage_Play(EmptyMontage);
    }
}

bool ARevolver::PrimaryTrace(APlayerController* PC, FHitResult& OutResult) {
    FVector CamLoc;
    FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    const FVector MuzzleForward = RevolverMesh->GetSocketRotation("MuzzleSocket").Vector();
    const FVector CamForward    = CamRot.Vector();
    // Blend camera direction toward muzzle direction based on RecoilInfluence
    // When recoil animation raises the muzzle, MuzzleForward diverges from CamForward
    // and shots become inaccurate proportionally
    const FVector BlendedForward = FMath::Lerp(CamForward, MuzzleForward, RecoilInfluence).GetSafeNormal();

    float DynamicSpread = BaseSpreadRadius;

    if (APawn* OwnerPawn = Cast<APawn>(GetOwner())) {
        const float Speed = OwnerPawn->GetVelocity().Size2D();
        DynamicSpread     += FMath::GetMappedRangeValueClamped(
            FVector2D(0.f, 400.f),
            FVector2D(0.f, 3.f),
            Speed);
    }

    const FVector SpreadForward = FMath::VRandCone(BlendedForward, FMath::DegreesToRadians(DynamicSpread));
    const FVector TraceStart    = CamLoc;
    const FVector TraceEnd      = TraceStart + SpreadForward * 50000.f; // ~500m range

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());
    Params.bTraceComplex           = true; // needed for bone-level hits
    Params.bReturnPhysicalMaterial = true;

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,
                                                           TraceStart,
                                                           TraceEnd,
                                                           ECC_Visibility,
                                                           Params);

    UGameUtils::DrawDebugLineTrace(GetWorld(), Hit, FColor::Cyan, 0.5f, 5.f);

    if (bHit) {
        if (Hit.BoneName != NAME_None) {
            FString Msg = FString::Printf(TEXT("[%s] on '%s'"),
                                          *(Hit.BoneName.ToString()),
                                          *(Hit.GetActor()->GetName()));
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, Msg);

            // Show hitmarker
            ASeetheCharacter* Character = Cast<ASeetheCharacter>(GetOwner());
            if (Character) {
                Character->GetHUD()->TriggerHitmarker();
            }

            float Damage = BaseDamage;
            if (Hit.BoneName == FName("head")) {
                Damage *= 2.0f;
            }

            if (auto* SK = Cast<USkeletalMeshComponent>(Hit.GetComponent())) {
                SK->AddImpulseToAllBodiesBelow((-Hit.ImpactNormal) * ImpactVelocity, Hit.BoneName);
            }

            UGameplayStatics::ApplyPointDamage(Hit.GetActor(),
                                               Damage,
                                               CamRot.Vector(),
                                               Hit,
                                               PC,
                                               this,
                                               nullptr);

            if (ImpactSoundEnemy) {
                UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundEnemy, Hit.ImpactPoint);
            }

            if (ImpactEnemyFX) {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
                                                               ImpactEnemyFX,
                                                               Hit.ImpactPoint,
                                                               Hit.ImpactNormal.Rotation());
            }

            if (ImpactEnemyDecal) {
                UGameUtils::SpawnDecalWithRandomRotation(GetWorld(), ImpactEnemyDecal, FVector(24.f, 24.f, 24.f), Hit);
            }

            OutResult = Hit;
            return true;
        }

        if (ImpactSound) {
            UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
        }

        if (ImpactFX) {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
                                                           ImpactFX,
                                                           Hit.ImpactPoint,
                                                           Hit.ImpactNormal.Rotation());
        }

        if (ImpactDecal) {
            UGameUtils::SpawnDecalWithRandomRotation(GetWorld(), ImpactDecal, FVector(8.f, 8.f, 8.f), Hit);
        }

        return false;
    }

    return false;
}

void ARevolver::SecondaryTrace(const APlayerController* PC, const FHitResult& OutResult) const {
    if (!PC || !OutResult.GetActor()) {
        return;
    }

    float PassThruRange   = 400.0f;
    FVector Start         = OutResult.ImpactPoint;
    FVector ShotDirection = (OutResult.ImpactPoint - OutResult.TraceStart).GetSafeNormal();

    // Projectile tumble deviation
    // This simulates the projectile deflecting whilst passing through a medium such as flesh
    float ConeHalfAngleRad  = FMath::DegreesToRadians(10.f); // 20° of total spread
    FVector TumbleDirection = FMath::VRandCone(ShotDirection, ConeHalfAngleRad);

    FVector TraceStart = Start + (TumbleDirection * 5.f);
    FVector TraceEnd   = TraceStart + (TumbleDirection * PassThruRange);

    if (ExitWoundFX) {
        FRotator SprayRotation = TumbleDirection.Rotation();
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
                                                       ExitWoundFX,
                                                       TraceStart,
                                                       SprayRotation);
    }

    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OutResult.GetActor());
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        QueryParams
        );

    UGameUtils::DrawDebugLineTrace(GetWorld(), Hit, FColor::Green, 0.5f, 5.f);

    if (bHit) {
        float HitDistance     = FVector::Dist(TraceStart, Hit.ImpactPoint);
        float ScaleMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(PassThruRange, 0.f),
                                                                  FVector2D(0.1f, 1.0f),
                                                                  HitDistance);
        FVector BaseDecalSize(100.f, 100.f, 100.f);
        FVector FinalSize = (BaseDecalSize * (1 - ScaleMultiplier)) + FVector(10.f);

        UGameUtils::SpawnDecalWithRandomRotation(GetWorld(), BloodSpatterDecal, FinalSize, Hit);
    }
}

void ARevolver::Fire(APlayerController* PC, USkeletalMeshComponent* Arms) {
    bool bCanShoot    = false;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastFireTime >= FireRate) {
        bCanShoot    = true;
        LastFireTime = CurrentTime;
    }

    if (!bCanShoot || bIsHolstering || bHolstered) {
        return;
    }

    if (CurrentAmmo <= 0) {
        FireEmpty(Arms);
        return;
    }

    if (ShootSound) {
        UGameplayStatics::PlaySoundAtLocation(this, ShootSound, GetOwner()->GetActorLocation());
    }

    if (ShootMontage) {
        Arms->GetAnimInstance()->Montage_Play(ShootMontage, 1.5);
    }

    if (MuzzleFlashFX) {
        UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashFX,
                                                     RevolverMesh,
                                                     FName("MuzzleSocket"),
                                                     FVector(-5, 0, 0),
                                                     FRotator::ZeroRotator,
                                                     EAttachLocation::SnapToTarget,
                                                     true);
    }

    int32 BulletIndex = (KTotalChambers - CurrentAmmo);
    if (VisualBullets.IsValidIndex(BulletIndex) && VisualBullets[BulletIndex]) {
        VisualBullets[BulletIndex]->DestroyComponent();
        VisualBullets[BulletIndex] = nullptr;
    }

    if (PC) {
        if (RecoilShake) {
            PC->ClientStartCameraShake(RecoilShake);
        }

        if (FHitResult PrimaryHitResult; PrimaryTrace(PC, PrimaryHitResult)) {
            SecondaryTrace(PC, PrimaryHitResult);
        }
    }

    CylinderRotationTarget += KChamberRotateDelta;

    // TODO: AUTO-RELOADING FOR TESTING ONLY - TO BE REMOVED
    if (--CurrentAmmo == 0) {
        Reload();
    }
}

void ARevolver::ToggleHolster(USkeletalMeshComponent* Arms) {
    if (bIsHolstering) {
        return;
    }

    if (bHolstered) {
        RevolverMesh->SetVisibility(true);
        Arms->SetVisibility(true);
        for (const auto& Bullet : VisualBullets) {
            if (Bullet->IsValidLowLevel() && Bullet.Get() != nullptr) {
                Bullet->SetVisibility(true);
            }
        }

        if (EquipSound) {
            UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetOwner()->GetActorLocation());
        }

        if (EquipMontage) {
            Arms->GetAnimInstance()->Montage_Play(EquipMontage);
        }

        bHolstered = false;
    } else {
        bIsHolstering = true;

        if (HolsterSound) {
            UGameplayStatics::PlaySoundAtLocation(this, HolsterSound, GetOwner()->GetActorLocation());
        }

        if (HolsterMontage) {
            UAnimInstance* AnimInstance = Arms->GetAnimInstance();
            const float Duration        = AnimInstance->Montage_Play(HolsterMontage);

            if (Duration > 0.f) {
                FOnMontageEnded EndDelegate;
                EndDelegate.BindUObject(this, &ARevolver::OnHolsterMontageEnded, Arms);
                AnimInstance->Montage_SetEndDelegate(EndDelegate, HolsterMontage);
            }
        }
    }
}

bool ARevolver::GetHolstered() const {
    return bHolstered || bIsHolstering;
}

void ARevolver::Reload() {
    if (GetHolstered() || (CurrentAmmo >= KTotalChambers)) {
        return;
    }

    CurrentAmmo = KTotalChambers;
    LoadBullets(true);
}

void ARevolver::Inspect(USkeletalMeshComponent* Arms) {
    if (GetHolstered()) {
        return;
    }

    if (InspectMontage && InspectRevolverMontage) {
        Arms->GetAnimInstance()->Montage_Play(InspectMontage);
        RevolverMesh->GetAnimInstance()->Montage_Play(InspectRevolverMontage);
    }
}

void ARevolver::OnHolsterMontageEnded(UAnimMontage* Montage, bool bInterrupted, USkeletalMeshComponent* Arms) {
    RevolverMesh->SetVisibility(false);
    Arms->SetVisibility(false);

    for (const auto& Bullet : VisualBullets) {
        if (Bullet->IsValidLowLevel() && Bullet.Get() != nullptr) {
            Bullet->SetVisibility(false);
        }
    }

    bIsHolstering = false;
    bHolstered    = true;
}