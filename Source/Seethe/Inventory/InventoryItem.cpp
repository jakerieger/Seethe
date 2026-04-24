// Copyright (C) 2026 Jake Rieger


#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "Seethe/SeetheCharacter.h"

void UInventoryItem::Use_Implementation(ASeetheCharacter* Character, int32 Index) {
    if (UseAnim) {
        Character->GetMesh1P()->GetAnimInstance()->Montage_Play(UseAnim);
    }
}

void UInventoryItem::UseMultiple_Implementation(ASeetheCharacter* Character, const int32 Index, const int32 Count) {
    for (int32 i = 0; i < Count; i++) {
        Use(Character, Index);
    }
}

void UInventoryItem::Drop_Implementation(ASeetheCharacter* Character, const int32 Index) {
    Character->GetInventory()->RemoveItem(Index);

    if (DropAnim) {
        Character->GetMesh1P()->GetAnimInstance()->Montage_Play(DropAnim);
    }
}

void UInventoryItem::DropMultiple_Implementation(ASeetheCharacter* Character, const int32 Index, const int32 Count) {
    for (int32 i = 0; i < Count; i++) {
        Drop(Character, Index);
    }
}

void UInventoryItemEquipable::Use_Implementation(ASeetheCharacter* Character, const int32 Index) {
    if (Character) {
        Character->Equip(this);
        Super::Use_Implementation(Character, Index);
    }
}