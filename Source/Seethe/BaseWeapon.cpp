// Copyright (C) 2026 Jake Rieger


#include "BaseWeapon.h"
#include "SeetheCharacter.h"
#include "Kismet/GameplayStatics.h"

ABaseWeapon::ABaseWeapon() {
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh                           = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
    RootComponent                        = WeaponMesh;
}

void ABaseWeapon::Equip(APlayerController* PC, USkeletalMeshComponent* Arms) {
    if (!Arms) { return; }

    const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    AttachToComponent(Arms, AttachmentRules, FName("S_Attach"));
    WeaponMesh->SetRelativeTransform(AttachOffset);
    bHolstered    = false;
    bIsHolstering = false;

    UAnimInstance* AnimInstance = Arms->GetAnimInstance();
    if (AnimInstance) {
        AnimInstance->LinkAnimClassLayers(WeaponAnimLayer);

        if (EquipMontage) {
            AnimInstance->Montage_Play(EquipMontage);
        }
    }

    if (EquipSound) {
        UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
    }

    if (EquipWeaponMontage) {
        WeaponMesh->GetAnimInstance()->Montage_Play(EquipWeaponMontage);
    }
}

void ABaseWeapon::Inspect(APlayerController* PC, USkeletalMeshComponent* Arms) {
    if (IsHolstered()) {
        return;
    }

    if (InspectMontage) {
        Arms->GetAnimInstance()->Montage_Play(InspectMontage);
    }

    if (InspectWeaponMontage) {
        WeaponMesh->GetAnimInstance()->Montage_Play(InspectWeaponMontage);
    }
}

void ABaseWeapon::ToggleHolster(APlayerController* PC, USkeletalMeshComponent* Arms) {
    if (bIsHolstering || !Arms) {
        return;
    }

    if (bHolstered) {
        OnUnholstered();

        WeaponMesh->SetVisibility(true);
        Arms->SetVisibility(true);

        if (EquipSound) {
            UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetOwner()->GetActorLocation());
        }

        if (EquipMontage) {
            Arms->GetAnimInstance()->Montage_Play(EquipMontage);
        }

        if (EquipWeaponMontage) {
            WeaponMesh->GetAnimInstance()->Montage_Play(EquipWeaponMontage);
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
                EndDelegate.BindUObject(this, &ABaseWeapon::OnHolsterMontageEnded, Arms);
                AnimInstance->Montage_SetEndDelegate(EndDelegate, HolsterMontage);
            }
        }

        if (HolsterWeaponMontage) {
            WeaponMesh->GetAnimInstance()->Montage_Play(HolsterWeaponMontage);
        }
    }
}

bool ABaseWeapon::CanAttack() {
    bool bCanAttack = false;

    if (const float CurrentTime = GetWorld()->GetTimeSeconds(); CurrentTime - LastAttackTime >= AttackRate) {
        bCanAttack     = true;
        LastAttackTime = CurrentTime;
    }

    return bCanAttack;
}

bool ABaseWeapon::IsHolstered() const { return bHolstered || bIsHolstering; }

void ABaseWeapon::Drop(USkeletalMeshComponent* Arms) {
    if (!Arms || !Arms->GetAnimInstance()) {
        return;
    }
    UAnimInstance* ArmsAnim = Arms->GetAnimInstance();
    ArmsAnim->StopAllMontages(0.2f);

    if (!WeaponMesh || !WeaponMesh->GetAnimInstance()) {
        return;
    }
    UAnimInstance* WeaponAnim = WeaponMesh->GetAnimInstance();
    WeaponAnim->StopAllMontages(0.2f);

    SetActorHiddenInGame(true);
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

UTexture2D* ABaseWeapon::GetCrosshairTexture() const { return CrosshairTexture; }

USkeletalMeshComponent* ABaseWeapon::GetWeaponMesh() const { return WeaponMesh; }

FTransform ABaseWeapon::GetLeftHandSocketTransform(USkeletalMeshComponent* Mesh1P) const {
    if (!WeaponMesh) {
        return FTransform::Identity;
    }

    return WeaponMesh->GetSocketTransform(LeftHandSocketName, RTS_World);
}

void ABaseWeapon::PlayImpactForceFeedback(const ASeetheCharacter* Character) const {
    if (!Character) { return; }
    if (APlayerController* PC = Cast<APlayerController>(Character->GetController())) {
        if (ImpactFFB) {
            FForceFeedbackParameters Parameters;
            Parameters.bLooping = false;
            PC->ClientPlayForceFeedback(ImpactFFB, Parameters);
        }
    }
}

void ABaseWeapon::OnHolsterMontageEnded(UAnimMontage*, bool, USkeletalMeshComponent* Arms) {
    WeaponMesh->SetVisibility(false);
    Arms->SetVisibility(false);
    bIsHolstering = false;
    bHolstered    = true;
    OnHolstered();
}