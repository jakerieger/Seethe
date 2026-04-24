// Copyright (C) 2026 Jake Rieger


#include "FlashlightPickup.h"
#include "InventoryItem.h"

AFlashlightPickup::AFlashlightPickup() {
    PrimaryActorTick.bCanEverTick = false;
}

FText AFlashlightPickup::GetInteractMessage() {
    const auto MsgStr = FString::Printf(TEXT("Equip %s"), *(InventoryItem->ItemName.ToString()));
    return FText::FromString(MsgStr);
}