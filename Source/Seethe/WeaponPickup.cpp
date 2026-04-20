// Copyright (C) 2026 Jake Rieger


#include "WeaponPickup.h"

#include "GameUtils.h"
#include "SeetheCharacter.h"

AWeaponPickup::AWeaponPickup() {
    PrimaryActorTick.bCanEverTick = false;

    PickupMesh    = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
    RootComponent = PickupMesh;

    // Custom depth and stencil ID for outline effect
    PickupMesh->SetRenderCustomDepth(false);
    PickupMesh->SetCustomDepthStencilValue(2);
}

void AWeaponPickup::Interact(ASeetheCharacter* Character) {
    if (Character && WeaponClass) {
        Character->EquipWeapon(GetClass(), WeaponClass);
        Destroy();
    }
}

void AWeaponPickup::LookAt() {
    PickupMesh->SetRenderCustomDepth(true);
}

void AWeaponPickup::LookAway() {
    PickupMesh->SetRenderCustomDepth(false);
}

FString AWeaponPickup::GetInteractMessage() {
    return FString::Printf(TEXT("[E] Pickup %s"), *WeaponName);
}