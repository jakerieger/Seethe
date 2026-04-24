// Copyright (C) 2026 Jake Rieger


#include "BatteryItem.h"
#include "InventoryComponent.h"
#include "../Flashlight.h"
#include "../Seethe.h"
#include "../SeetheCharacter.h"

void UBatteryItem::Use_Implementation(ASeetheCharacter* Character, const int32 Index) {
    if (Character) {
        auto* Current = Character->GetCurrentEquipable();
        if (AFlashlight* Flashlight = Cast<AFlashlight>(Current)) {
            Flashlight->Recharge(Seethe::Constants::kBatteryChargeAmount);
        }
    }

    Super::Use_Implementation(Character, Index);
}