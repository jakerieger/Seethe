// Copyright (C) 2026 Jake Rieger


#include "FlareGunCockedNotify.h"
#include "FlareGunWeapon.h"

void UFlareGunCockedNotify::Notify(USkeletalMeshComponent* MeshComp,
                                   UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference) {
    // Reload needs a cocked notify
    // Shoot needs a cocked notify
    // All equip montages need a cocked notify
    if (!MeshComp) { return; }

    AActor* Owner = MeshComp->GetOwner();
    if (Owner && Owner->IsA<AFlareGunWeapon>()) {
        auto* FlareGun = Cast<AFlareGunWeapon>(Owner);
        if (FlareGun) {
            FlareGun->SetActionCocked(bCocked);
        }
    }
}