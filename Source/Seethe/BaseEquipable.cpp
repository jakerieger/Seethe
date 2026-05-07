// Copyright (C) 2026 Jake Rieger


#include "BaseEquipable.h"
#include "CharacterAnimInstance.h"
#include "EquipableAnimationData.h"
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

        auto* CharacterAI = Character->GetAnimInstance1P();
        auto* EquipableAI = GetMesh1P()->GetAnimInstance();

        if (Locomotion) {
            if (CharacterAI) {
                CharacterAI->UpdateLocomotionSequences(Locomotion);
            }
        }

        if (EquipAnimations) {
            EquipAnimations->PlayAnimations(CharacterAI, EquipableAI);
        }
    }
}

void ABaseEquipable::UnEquip(ASeetheCharacter* Character) {
    if (!Character) { return; }

    auto* CharacterAI = Character->GetAnimInstance1P();
    auto* EquipableAI = GetMesh1P()->GetAnimInstance();

    if (UnEquipAnimations) {
        const auto Result = UnEquipAnimations->PlayAnimations(CharacterAI, EquipableAI);
        if (Result.bPlayedCharacterMontage && Result.CharacterMontageDuration) {
            FOnMontageEnded End;
            End.BindUObject(this, &ABaseEquipable::OnMontageEnded_UnEquip);
            CharacterAI->Montage_SetEndDelegate(End, UnEquipAnimations->CharacterMontage);
            return;
        }
    }

    OnMontageEnded_UnEquip(nullptr, false);
}

void ABaseEquipable::Drop(ASeetheCharacter* Character) {
    if (!Character) { return; }

    auto* CharacterAI = Character->GetAnimInstance1P();
    auto* EquipableAI = GetMesh1P()->GetAnimInstance();

    if (DropAnimations) {
        const auto Result = DropAnimations->PlayAnimations(CharacterAI, EquipableAI);
        if (Result.bPlayedCharacterMontage && Result.CharacterMontageDuration) {
            FOnMontageEnded End;
            End.BindUObject(this, &ABaseEquipable::OnMontageEnded_UnEquip);
            CharacterAI->Montage_SetEndDelegate(End, DropAnimations->CharacterMontage);
            return;
        }
    }

    OnMontageEnded_Drop(nullptr, false);
}

void ABaseEquipable::OnEquipNotify(const EEquipMontageNotifyAction EquipAction) {
    switch (EquipAction) {
        case EEquipMontageNotifyAction::Equip:
            OnEquipNotify_Equip();
            break;
        case EEquipMontageNotifyAction::UnEquip:
            OnEquipNotify_UnEquip();
            break;
        case EEquipMontageNotifyAction::Drop:
            OnEquipNotify_Drop();
            break;
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

void ABaseEquipable::OnEquipNotify_Equip() {}

void ABaseEquipable::OnEquipNotify_UnEquip() {
    SetActorHiddenInGame(true);

    const auto* Character = Cast<ASeetheCharacter>(GetOwner());
    if (Character) {
        if (auto* CharacterAI = Character->GetAnimInstance1P()) {
            CharacterAI->StopAllMontages(0.2f);
            CharacterAI->SetDefaultLocomotion();
        }
    }
}

void ABaseEquipable::OnEquipNotify_Drop() {
    SetActorHiddenInGame(true);

    const auto* Character = Cast<ASeetheCharacter>(GetOwner());
    if (Character) {
        if (auto* CharacterAI = Character->GetAnimInstance1P()) {
            CharacterAI->StopAllMontages(0.2f);
            CharacterAI->SetDefaultLocomotion();
        }
    }

    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    Destroy();
}

void ABaseEquipable::OnMontageEnded_Equip(UAnimMontage*,
                                          bool) {}

void ABaseEquipable::OnMontageEnded_UnEquip(UAnimMontage*,
                                            bool) {
    auto* Character = Cast<ASeetheCharacter>(GetOwner());
    if (Character) {
        Character->SetCurrentEquipable(nullptr);
    }
}

void ABaseEquipable::OnMontageEnded_Drop(UAnimMontage*,
                                         bool) {
    auto* Character = Cast<ASeetheCharacter>(GetOwner());
    if (Character) {
        Character->SetCurrentEquipable(nullptr);
    }
}