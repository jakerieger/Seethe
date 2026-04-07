// Copyright (C) 2026 Jake Rieger

#include "Revolver.h"
#include "NiagaraFunctionLibrary.h"
#include "RevolverProjectile.h"
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

void ARevolver::Fire(USkeletalMeshComponent* Arms) {
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

    if (ProjectileClass) {
        FVector MuzzleLocation        = RevolverMesh->GetSocketLocation("MuzzleSocket");
        const FRotator MuzzleRotation = RevolverMesh->GetSocketRotation("MuzzleSocket");

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner      = GetOwner();
        SpawnParams.Instigator = GetInstigator();

        GetWorld()->SpawnActor<ARevolverProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
    }

    int32 BulletIndex = (KTotalChambers - CurrentAmmo);
    if (VisualBullets.IsValidIndex(BulletIndex) && VisualBullets[BulletIndex]) {
        VisualBullets[BulletIndex]->DestroyComponent();
        VisualBullets[BulletIndex] = nullptr;
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