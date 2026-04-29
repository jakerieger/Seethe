// Copyright (C) 2026 Jake Rieger


#include "PickupAnimData.h"

UAnimMontage* UPickupAnimData::GetPickupMontage(const bool bEquipped) {
    return bEquipped ? EquippedMontage : UnEquippedMontage;
}