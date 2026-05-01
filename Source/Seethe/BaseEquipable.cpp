// Copyright (C) 2026 Jake Rieger


#include "BaseEquipable.h"
#include "FirstPersonAnimInstance.h"
#include "Seethe/SeetheCharacter.h"

ABaseEquipable::ABaseEquipable() {
    PrimaryActorTick.bCanEverTick = false;

    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
    SetRootComponent(Mesh1P);
}

void ABaseEquipable::Equip(ASeetheCharacter* Character) {
    if (!Character) { return; }

    SetActorHiddenInGame(false);

    if (auto* Arms = Character->GetMesh1P()) {
        const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
        AttachToComponent(Arms, AttachmentRules, FName("S_Attach"));
        Mesh1P->SetRelativeTransform(AttachOffset);

        if (UFirstPersonAnimInstance* Anim1P = Character->GetAnimInstance1P()) {
            Anim1P->UpdateLocomotionSequences(AnimData);

            if (EquipMontage) {
                const auto Duration = Anim1P->Montage_Play(EquipMontage);
                if (Duration > 0.0f) {
                    FOnMontageEnded End;
                    End.BindUObject(this, &ABaseEquipable::OnEquipMontageEnded);
                    Anim1P->Montage_SetEndDelegate(End, EquipMontage);
                    return;
                }
            }

            OnEquipMontageEnded(nullptr, false);
        }
    }
}

void ABaseEquipable::UnEquip(ASeetheCharacter* Character) {
    if (!Character) { return; }

    if (const auto* Arms = Character->GetMesh1P()) {
        if (UFirstPersonAnimInstance* Anim1P = Character->GetAnimInstance1P()) {
            if (UnEquipMontage) {
                const auto Duration = Anim1P->Montage_Play(UnEquipMontage);
                if (Duration > 0.0f) {
                    FOnMontageEnded End;
                    End.BindUObject(this, &ABaseEquipable::OnUnEquipMontageEnded, Anim1P);
                    Anim1P->Montage_SetEndDelegate(End, UnEquipMontage);
                    return;
                }
            }

            OnUnEquipMontageEnded(nullptr, false, Anim1P);
        }
    }
}

void ABaseEquipable::Drop(ASeetheCharacter* Character) {
    if (!Character) { return; }

    if (const auto* Arms = Character->GetMesh1P()) {
        if (UFirstPersonAnimInstance* Anim1P = Character->GetAnimInstance1P()) {
            if (DropMontage) {
                const auto Duration = Anim1P->Montage_Play(DropMontage);
                if (Duration > 0.0f) {
                    FOnMontageEnded End;
                    End.BindUObject(this, &ABaseEquipable::OnDropMontageEnded, Anim1P);
                    Anim1P->Montage_SetEndDelegate(End, DropMontage);
                    return;
                }
            }

            OnDropMontageEnded(nullptr, false, Anim1P);
        }
    }
}

USkeletalMeshComponent* ABaseEquipable::GetMesh1P() const {
    return Mesh1P;
}

bool ABaseEquipable::HasLeftHandSocket() const {
    return Mesh1P->DoesSocketExist(LeftHandSocket);
}

FTransform ABaseEquipable::GetLeftHandSocketTransform() const {
    if (HasLeftHandSocket()) {
        return Mesh1P->GetSocketTransform(LeftHandSocket, RTS_World);
    }

    return FTransform::Identity;
}

void ABaseEquipable::OnEquipMontageEnded(UAnimMontage*,
                                         bool) {}

void ABaseEquipable::OnUnEquipMontageEnded(UAnimMontage*,
                                           bool,
                                           UFirstPersonAnimInstance* InAnimInstance) {
    if (InAnimInstance) {
        InAnimInstance->StopAllMontages(0.2f);
        InAnimInstance->SetDefaultLocomotion();
    }

    SetActorHiddenInGame(true);
}

void ABaseEquipable::OnDropMontageEnded(UAnimMontage*,
                                        bool,
                                        UFirstPersonAnimInstance* InAnimInstance) {
    if (InAnimInstance) {
        InAnimInstance->StopAllMontages(0.2f);
        InAnimInstance->SetDefaultLocomotion();
    }

    SetActorHiddenInGame(true);
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Destroy();
}