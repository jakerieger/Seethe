// Copyright (C) 2026 Jake Rieger


#include "FlareGunWeapon.h"
#include "FlareShellItem.h"
#include "InventoryComponent.h"
#include "CharacterAnimInstance.h"
#include "SeetheCharacter.h"
#include "EquipableAnimationData.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AFlareGunWeapon::AFlareGunWeapon() {
    PrimaryActorTick.bCanEverTick = false;
    AttackRate                    = 0.0f;
    bLoaded                       = false;
}

AFlareGunWeapon::FSpreadRanges AFlareGunWeapon::GetSpreadRanges(const ELocomotionState State) const {
    FFloatInterval RangeZ {0.0f, 0.0f};
    FFloatInterval RangeY {0.0f, 0.0f};

    const auto FoundRangeZ = SpreadRanges_Z.Find(State);
    if (FoundRangeZ) { RangeZ = *FoundRangeZ; }

    const auto FoundRangeY = SpreadRanges_Y.Find(State);
    if (FoundRangeY) { RangeY = *FoundRangeY; }

    return {RangeZ, RangeY};
}

FRotator AFlareGunWeapon::CalculateSpread(const FRotator& BaseRotation,
                                          const FFloatInterval RangeZ,
                                          const FFloatInterval RangeY) {
    FRotator Spread {BaseRotation};
    Spread.Roll  += FMath::RandRange(RangeZ.Min, RangeZ.Max);
    Spread.Pitch += FMath::RandRange(RangeY.Min, RangeY.Max);
    return Spread;
}

void AFlareGunWeapon::Attack(ASeetheCharacter* Character) {
    if (!Character || !ProjectileClass || !GetWorld()) { return; }

    auto* CharacterAI = Character->GetAnimInstance1P();
    auto* EquipableAI = GetMesh1P()->GetAnimInstance();

    if (!CanAttack()) {
        if (DryFireAnimations) {
            DryFireAnimations->PlayAnimations(CharacterAI, EquipableAI);
        }

        return;
    }

    const FTransform MuzzleTransform = GetMesh1P()->GetSocketTransform(MuzzleSocketName, RTS_World);
    const FVector MuzzleLocation     = MuzzleTransform.GetLocation();
    const FRotator MuzzleRotation    = MuzzleTransform.Rotator();

    // Apply a small, random offset to add spread based on our movement speed.
    // Spread is only applied in two axes, Z and Y.
    const auto SpreadRanges           = GetSpreadRanges(Character->GetLocomotionState());
    const FRotator ProjectileRotation = CalculateSpread(MuzzleRotation, SpreadRanges.RangeZ, SpreadRanges.RangeY);

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Owner                          = this;
    SpawnInfo.Instigator                     = GetInstigator();
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (GetWorld()->SpawnActor<AFlareGunProjectile>(
        ProjectileClass,
        MuzzleLocation,
        ProjectileRotation,
        SpawnInfo)) {
        bLoaded = false;

        if (WeaponData && WeaponData->AttackData) {
            const auto& AttackData = WeaponData->AttackData;

            if (AttackData->Animations) {
                AttackData->Animations->PlayAnimations(CharacterAI, EquipableAI);
            }

            if (const auto& Sound = AttackData->Sound) {
                UGameplayStatics::PlaySoundAtLocation(this, Sound, MuzzleLocation);
            }

            if (const auto& FX = AttackData->ParticleSystem) {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
                                                               FX,
                                                               MuzzleLocation,
                                                               MuzzleRotation);
            }

            if (const auto& CameraShake = AttackData->CameraShake) {
                if (auto* CameraManager = Character->GetPlayerCameraManager()) {
                    CameraManager->StartCameraShake(CameraShake);
                }
            }

            if (const auto& ForceFeedback = AttackData->ForceFeedback) {
                if (auto* PC = Cast<APlayerController>(Character->GetController())) {
                    PC->ClientPlayForceFeedback(ForceFeedback);
                }
            }
        }
    }
}

void AFlareGunWeapon::Reload(ASeetheCharacter* Character) {
    if (!Character || GetTotalAmmo() <= 0 || bLoaded) { return; }

    bool bHadReloadMontage = false;
    auto Finished          = [this, Character](UAnimMontage* = nullptr, bool = false) {
        bLoaded = true;
        if (UInventoryComponent* IC = Character->GetInventory()) {
            const auto FindResult = IC->FindItem<
                UFlareShellItem>(EInventoryCategory::EIC_Supplies);
            if (FindResult.bFound) {
                if (FindResult.Slot) {
                    if (--FindResult.Slot->Quantity <= 0) {
                        IC->RemoveItem(FindResult.Index, EInventoryCategory::EIC_Supplies);
                    }
                }
            }
        }
    };

    auto* CharacterAI = Character->GetAnimInstance1P();
    auto* EquipableAI = GetMesh1P()->GetAnimInstance();

    if (WeaponData && WeaponData.IsA<UWeaponFirearmData>()) {
        const UWeaponFirearmData* FirearmData = Cast<UWeaponFirearmData>(WeaponData);
        const auto& ReloadData                = FirearmData->ReloadData;

        if (ReloadData->Animations) {
            const auto Result = ReloadData->Animations->PlayAnimations(CharacterAI, EquipableAI);
            if (Result.bPlayedCharacterMontage && Result.CharacterMontageDuration > 0.0f) {
                FOnMontageEnded End;
                End.BindLambda(Finished);
                CharacterAI->Montage_SetEndDelegate(End, ReloadData->Animations->CharacterMontage);
                bHadReloadMontage = true;
            }
        }

        const FVector SpawnLocation  = GetActorLocation();
        const FRotator SpawnRotation = GetActorRotation();

        if (const auto& Sound = ReloadData->Sound) {
            UGameplayStatics::PlaySoundAtLocation(this, Sound, SpawnLocation);
        }

        if (const auto& FX = ReloadData->ParticleSystem) {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FX, SpawnLocation, SpawnRotation);
        }

        if (const auto& CameraShake = ReloadData->CameraShake) {
            if (auto* CameraManager = Character->GetPlayerCameraManager()) {
                CameraManager->StartCameraShake(CameraShake);
            }
        }

        if (const auto& ForceFeedback = ReloadData->ForceFeedback) {
            if (auto* PC = Cast<APlayerController>(Character->GetController())) {
                PC->ClientPlayForceFeedback(ForceFeedback);
            }
        }
    }

    if (!bHadReloadMontage) {
        Finished();
    }
}

bool AFlareGunWeapon::CanAttack() {
    return bLoaded;
}

int32 AFlareGunWeapon::GetTotalAmmo() {
    int32 AvailableAmmo {0};

    if (const ASeetheCharacter* SC = Cast<ASeetheCharacter>(GetOwner())) {
        if (UInventoryComponent* IC = SC->GetInventory()) {
            const auto FindResult = IC->FindItem<
                UFlareShellItem>(EInventoryCategory::EIC_Supplies);
            if (FindResult.bFound) {
                AvailableAmmo += FindResult.Slot->Quantity;
            }
        }
    }

    return AvailableAmmo;
}

void AFlareGunWeapon::Equip(ASeetheCharacter* Character) {
    if (!Character) { return; }

    SetActorHiddenInGame(false);

    if (auto* Arms = Character->GetMesh1P()) {
        const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
        AttachToComponent(Arms, AttachmentRules, FName("S_Attach"));
        GetMesh1P()->SetRelativeTransform(AttachOffset);

        auto* CharacterAI = Character->GetAnimInstance1P();
        auto* EquipableAI = GetMesh1P()->GetAnimInstance();

        if (Locomotion) {
            CharacterAI->UpdateLocomotionSequences(Locomotion);
        }

        // - If the gun is loaded, play the cocking equip anim
        if (bLoaded) {
            if (EquipAnimations) {
                EquipAnimations->PlayAnimations(CharacterAI, EquipableAI);
            }

            return;
        }

        // - If the gun was NOT loaded but you have ammunition, play the reload anim
        if (GetTotalAmmo() > 0) {
            Reload(Character);
            return;
        }

        // - If the gun was NOT loaded and you have no ammo, play simple equip anim (no cocking)
        if (SimpleEquipAnimations) {
            SimpleEquipAnimations->PlayAnimations(CharacterAI, EquipableAI);
        }
    }
}

void AFlareGunWeapon::SpawnEjectedShellActor() {
    FTransform SpawnTransform = GetMesh1P()->GetSocketTransform(FName("b_shell"));
    SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

    FActorSpawnParameters SpawnInfo {};
    SpawnInfo.Owner                          = this;
    SpawnInfo.Instigator                     = GetInstigator();
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AFlareGunShell* Shell = GetWorld()->SpawnActor<AFlareGunShell>(ShellClass,
                                                                   SpawnTransform,
                                                                   SpawnInfo);

    if (Shell) {
        const FVector PlayerLeft   = -(GetOwner()->GetActorRightVector()) * FVector {0.5f, 0.5f, 0.5f};
        const FVector PlayerBehind = -(GetOwner()->GetActorForwardVector());
        constexpr auto Speed       = 2500.0f;
        const FVector ImpulseDir   = (PlayerLeft + PlayerBehind).GetSafeNormal() * Speed;
        UStaticMeshComponent* SM   = Shell->GetMesh();

        // Add impulse to shell so that it doesn't just fall on top of the character. Impulse direction is behind the
        // player and slightly to the left (to match the trajectory of the animated shell).
        SM->AddImpulse(ImpulseDir);
    }
}

void AFlareGunWeapon::SetActionCocked(const bool bCocked) {
    bActionCocked = bCocked;
}