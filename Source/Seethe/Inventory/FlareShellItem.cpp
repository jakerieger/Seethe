// Copyright (C) 2026 Jake Rieger


#include "FlareShellItem.h"
#include "FlareGunWeapon.h"
#include "Seethe/SeetheCharacter.h"

void UFlareShellItem::Use_Implementation(ASeetheCharacter* Character,
                                         int32 Index,
                                         const EInventoryCategory& Category) {
    if (!Character) { return; }

    if (Character->HasEquippedItem()) {
        auto* Current = Character->GetCurrentEquipable();
        if (Current && Current->IsA<AFlareGunWeapon>()) {
            AFlareGunWeapon* FlareGun = Cast<AFlareGunWeapon>(Current);
            if (FlareGun && !FlareGun->bLoaded) {
                FlareGun->Reload(Character);
                Super::Use_Implementation(Character, Index, Category);
                Character->OnToggleInventory();
            }
        }
    }
}

bool UFlareShellItem::CanUse_Implementation(ASeetheCharacter* Character,
                                            int32 Index,
                                            const EInventoryCategory& Category) {
    if (!Character) { return false; }

    if (Character->HasEquippedItem()) {
        const auto* Current = Character->GetCurrentEquipable();
        return Current && Current->IsA<AFlareGunWeapon>();
    }

    return false;
}