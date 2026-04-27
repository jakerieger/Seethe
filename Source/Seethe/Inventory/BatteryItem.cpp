// Copyright (C) 2026 Jake Rieger


#include "BatteryItem.h"
#include "InventoryComponent.h"
#include "Seethe/Tools/FlashlightTool.h"
#include "Seethe/Seethe.h"
#include "Seethe/SeetheCharacter.h"

void UBatteryItem::Use_Implementation(ASeetheCharacter* Character,
                                      const int32 Index,
                                      const EInventoryCategory& Category) {
    if (Character) {
        auto* Current = Character->GetCurrentEquipable();
        if (AFlashlightTool* Flashlight = Cast<AFlashlightTool>(Current)) {
            Flashlight->Recharge(Seethe::Constants::kBatteryChargeAmount);
        }
    }

    Super::Use_Implementation(Character, Index, Category);
}