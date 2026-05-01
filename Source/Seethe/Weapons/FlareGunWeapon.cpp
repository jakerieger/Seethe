// Copyright (C) 2026 Jake Rieger


#include "FlareGunWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Seethe/FirstPersonAnimInstance.h"
#include "Seethe/SeetheCharacter.h"

AFlareGunWeapon::AFlareGunWeapon() {
    PrimaryActorTick.bCanEverTick = false;
}

void AFlareGunWeapon::Attack(ASeetheCharacter* Character) {
    if (!CanAttack() || !Character || !ProjectileClass || !GetWorld()) { return; }

    const FTransform MuzzleTransform = GetMesh1P()->GetSocketTransform(MuzzleSocketName, RTS_World);
    const FVector SpawnLocation      = MuzzleTransform.GetLocation();
    const FRotator SpawnRotation     = MuzzleTransform.Rotator();

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Owner                          = this;
    SpawnInfo.Instigator                     = GetInstigator();
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (GetWorld()->SpawnActor<AFlareGunProjectile>(
        ProjectileClass,
        SpawnLocation,
        SpawnRotation,
        SpawnInfo)) {
        // Spawn effects
        if (AttackMontage) {
            Character->GetAnimInstance1P()->Montage_Play(AttackMontage);
        }

        if (AttackSound) {
            UGameplayStatics::PlaySoundAtLocation(this, AttackSound, SpawnLocation);
        }

        if (AttackFX) {
            UNiagaraFunctionLibrary::SpawnSystemAttached(AttackFX,
                                                         GetMesh1P(),
                                                         MuzzleSocketName,
                                                         FVector::ZeroVector,
                                                         FRotator::ZeroRotator,
                                                         EAttachLocation::SnapToTarget,
                                                         true,
                                                         true);
        }

        if (RecoilShake) {
            Character->GetPlayerCameraManager()->StartCameraShake(RecoilShake, 1.0f);
        }

        if (RecoilFFB) {
            if (auto* PC = Cast<APlayerController>(Character->GetController())) {
                PC->ClientPlayForceFeedback(RecoilFFB);
            }
        }
    }
}