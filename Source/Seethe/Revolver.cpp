// Copyright (C) 2026 Jake Rieger

#include "Revolver.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

static constexpr int32 KTotalChambers      = 5;
static constexpr float KChamberRotateDelta = 360.0f / KTotalChambers;

ARevolver::ARevolver() {
    PrimaryActorTick.bCanEverTick = true; // Needed for cylinder rotation lerp

    RevolverMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RevolverMesh"));
    RevolverMesh->SetupAttachment(RootComponent);

    CurrentAmmo = /*KTotalChambers;*/ 500;
}

void ARevolver::BeginPlay() {
    Super::BeginPlay();

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
            Bullet->SetVisibility(false);
            VisualBullets.Add(Bullet);
        }
    }
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

void ARevolver::Fire(APlayerController* PC, USkeletalMeshComponent* Arms) {
    if (bIsHolstering || bHolstered) {
        return;
    }

    if (CurrentAmmo <= 0) {
        if (EmptySound) {
            UGameplayStatics::PlaySoundAtLocation(this, EmptySound, GetOwner()->GetActorLocation());
        }

        if (EmptyMontage) {
            Arms->GetAnimInstance()->Montage_Play(EmptyMontage);
        }

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

    if (RecoilShake) {
        if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController())) {
            PC->ClientStartCameraShake(RecoilShake);
        }
    }

    int32 BulletIndex = (KTotalChambers - CurrentAmmo);
    if (VisualBullets.IsValidIndex(BulletIndex) && VisualBullets[BulletIndex]) {
        VisualBullets[BulletIndex]->DestroyComponent();
        VisualBullets[BulletIndex] = nullptr;
    }

    if (PC) {
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

        if (GetWorld()->LineTraceSingleByChannel(Hit,
                                                 TraceStart,
                                                 TraceEnd,
                                                 ECC_Visibility,
                                                 Params)) {
            if (ImpactFX) {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
                                                               ImpactFX,
                                                               Hit.ImpactPoint,
                                                               Hit.ImpactNormal.Rotation());
            }

            if (ImpactDecal) {
                auto* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
                                                                     ImpactDecal,
                                                                     FVector(8.f, 8.f, 8.f),
                                                                     Hit.ImpactPoint,
                                                                     Hit.ImpactNormal.Rotation(),
                                                                     10.0f);
                if (Decal) {
                    Decal->SetFadeScreenSize(0.0001f);
                }
            }

            // Calculate damage and apply
            if (Hit.BoneName != NAME_None) {
                float Damage = BaseDamage;
                if (Hit.BoneName == FName("head")) {
                    Damage *= 2.0f;
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
            } else {
                if (ImpactSound) {
                    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Hit.ImpactPoint);
                }
            }
        }
    }

    CylinderRotationTarget += KChamberRotateDelta;
    CurrentAmmo--;
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