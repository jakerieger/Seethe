// Copyright (C) 2026 Jake Rieger


#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Seethe/Interactables/ItemPickupBase.h"
#include "Seethe/SeetheCharacter.h"

void UInventoryItem::Use_Implementation(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category) {
    if (UseAnim) {
        Character->GetMesh1P()->GetAnimInstance()->Montage_Play(UseAnim);
    }
}

void UInventoryItem::UseMultiple_Implementation(ASeetheCharacter* Character,
                                                const int32 Index,
                                                const EInventoryCategory& Category,
                                                const int32 Count) {
    for (int32 i = 0; i < Count; i++) {
        Use(Character, Index, Category);
    }
}

void UInventoryItem::Drop_Implementation(ASeetheCharacter* Character,
                                         const int32 Index,
                                         const EInventoryCategory& Category) {
    UWorld* World = Character->GetWorld();
    if (World && ItemPickupClass) {
        const auto* Camera       = Character->GetCamera1P();
        const auto CameraPos     = Camera->GetComponentLocation();
        const auto CameraForward = Camera->GetForwardVector();

        const AItemPickupBase* NewPickup = World->SpawnActor<AItemPickupBase>(ItemPickupClass,
                                                                              CameraPos + (
                                                                                  CameraForward *
                                                                                  100.f),
                                                                              FRotator::ZeroRotator);
        if (NewPickup) {
            if (auto* Mesh = NewPickup->GetMesh()) {
                Mesh->AddImpulse(CameraForward * 100.f);
            }
        }
    }

    Character->GetInventory()->RemoveItem(Index, Category);

    if (DropAnim) {
        Character->GetMesh1P()->GetAnimInstance()->Montage_Play(DropAnim);
    }
}

void UInventoryItem::DropMultiple_Implementation(ASeetheCharacter* Character,
                                                 const int32 Index,
                                                 const EInventoryCategory& Category,
                                                 const int32 Count) {
    for (int32 i = 0; i < Count; i++) {
        Drop(Character, Index, Category);
    }
}

void UInventoryItemEquipable::Use_Implementation(ASeetheCharacter* Character,
                                                 const int32 Index,
                                                 const EInventoryCategory& Category) {
    if (Character) {
        Character->Equip(this);
        Super::Use_Implementation(Character, Index, Category);
    }
}

void UInventoryItemEquipable::Drop_Implementation(ASeetheCharacter* Character,
                                                  const int32 Index,
                                                  const EInventoryCategory& Category) {
    if (Character && EquipableClass) {
        const ABaseEquipable* Current = Character->GetCurrentEquipable();
        if (Current && Current->IsA(EquipableClass)) {
            Character->Drop();
        }
    }

    Super::Drop_Implementation(Character, Index, Category);
}

bool FInventorySlot::ShouldConsume() const {
    return ItemData->ItemCategory == EInventoryCategory::EIC_Supplies;
}