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
    if (Character) {
        auto* Arms = Character->GetMesh1P();
        if (Arms) {
            const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
            AttachToComponent(Arms, AttachmentRules, FName("S_Attach"));
            Mesh1P->SetRelativeTransform(AttachOffset);

            UAnimInstance* ArmsAnim = Arms->GetAnimInstance();
            if (ArmsAnim) {
                ArmsAnim->LinkAnimClassLayers(EquipableAnimLayer);
                if (EquipMontage) {
                    ArmsAnim->Montage_Play(EquipMontage);
                }
            }

            if (EquipSound) {
                UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
            }
        }
    }
}

void ABaseEquipable::Drop(ASeetheCharacter* Character) {
    // TODO: Implement drop animations
    if (Character) {
        const auto* Arms = Character->GetMesh1P();
        if (Arms) {
            UAnimInstance* ArmsAnim = Arms->GetAnimInstance();
            if (ArmsAnim) {
                ArmsAnim->StopAllMontages(0.2f);
                ArmsAnim->UnlinkAnimClassLayers(EquipableAnimLayer);
            }
        }

        auto* Anim = Mesh1P->GetAnimInstance();
        if (Anim) {
            Anim->StopAllMontages(0.2f);
        }

        if (DropSound) {
            UGameplayStatics::PlaySoundAtLocation(this, DropSound, GetActorLocation());
        }

        SetActorHiddenInGame(true);
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    }
}

USkeletalMeshComponent* ABaseEquipable::GetMesh1P() const {
    return Mesh1P;
}