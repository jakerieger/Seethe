// Copyright (C) 2026 Jake Rieger


#include "BatteryPickup.h"
#include "InventoryItem.h"

ABatteryPickup::ABatteryPickup() {
    PrimaryActorTick.bCanEverTick = false;
}

FText ABatteryPickup::GetInteractMessage() {
    const auto MsgStr = FString::Printf(TEXT("Equip %s (x%d)"),
                                        *(InventoryItem->ItemName.ToString()),
                                        InventoryItem->Quantity);
    return FText::FromString(MsgStr);
}