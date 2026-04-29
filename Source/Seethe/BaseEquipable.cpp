// Copyright (C) 2026 Jake Rieger


#include "BaseEquipable.h"
#include "Seethe/SeetheCharacter.h"
#include "Kismet/GameplayStatics.h"

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

        if (UAnimInstance* AnimInstance = Arms->GetAnimInstance()) {
            if (EquipableAnimLayer) {
                AnimInstance->LinkAnimClassLayers(EquipableAnimLayer);
            }

            if (EquipMontage) {
                const auto Duration = AnimInstance->Montage_Play(EquipMontage);
                if (Duration > 0.f) {
                    FOnMontageEnded End;
                    End.BindUObject(this, &ABaseEquipable::OnEquipMontageEnded);
                    AnimInstance->Montage_SetEndDelegate(End, EquipMontage);
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
        if (UAnimInstance* AnimInstance = Arms->GetAnimInstance()) {
            if (EquipableAnimLayer) {
                AnimInstance->UnlinkAnimClassLayers(EquipableAnimLayer);
            }

            if (UnEquipMontage) {
                const auto Duration = AnimInstance->Montage_Play(UnEquipMontage);
                if (Duration > 0.f) {
                    FOnMontageEnded End;
                    End.BindUObject(this, &ABaseEquipable::OnUnEquipMontageEnded, AnimInstance);
                    AnimInstance->Montage_SetEndDelegate(End, UnEquipMontage);
                    return;
                }
            }

            OnUnEquipMontageEnded(nullptr, false, AnimInstance);
        }
    }
}

void ABaseEquipable::Drop(ASeetheCharacter* Character) {
    if (!Character) { return; }

    if (const auto* Arms = Character->GetMesh1P()) {
        if (UAnimInstance* AnimInstance = Arms->GetAnimInstance()) {
            if (DropMontage) {
                const auto Duration = AnimInstance->Montage_Play(DropMontage);
                if (Duration > 0.f) {
                    FOnMontageEnded End;
                    End.BindUObject(this, &ABaseEquipable::OnDropMontageEnded, AnimInstance);
                    AnimInstance->Montage_SetEndDelegate(End, DropMontage);
                    return;
                }
            }

            OnDropMontageEnded(nullptr, false, AnimInstance);
        }
    }
}

USkeletalMeshComponent* ABaseEquipable::GetMesh1P() const {
    return Mesh1P;
}

void ABaseEquipable::OnEquipMontageEnded(UAnimMontage*,
                                         bool) {
    if (OnEquipped.IsBound()) {
        OnEquipped.Broadcast(this);
    }
}

void ABaseEquipable::OnUnEquipMontageEnded(UAnimMontage*,
                                           bool,
                                           UAnimInstance* InAnimInstance) {
    if (InAnimInstance) {
        InAnimInstance->StopAllMontages(0.2f);
    }

    SetActorHiddenInGame(true);

    if (OnUnEquipped.IsBound()) {
        OnUnEquipped.Broadcast(this);
    }

    OnEquipped.Clear();
    OnUnEquipped.Clear();
    OnDropped.Clear();
}

void ABaseEquipable::OnDropMontageEnded(UAnimMontage*,
                                        bool,
                                        UAnimInstance* InAnimInstance) {
    if (InAnimInstance) {
        InAnimInstance->StopAllMontages(0.2f);
        InAnimInstance->UnlinkAnimClassLayers(EquipableAnimLayer);
    }

    if (OnDropped.IsBound()) {
        OnDropped.Broadcast(this);
    }

    OnEquipped.Clear();
    OnUnEquipped.Clear();
    OnDropped.Clear();

    SetActorHiddenInGame(true);
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Destroy();
}