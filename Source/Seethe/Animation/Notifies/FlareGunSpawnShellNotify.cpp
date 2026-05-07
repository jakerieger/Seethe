// Copyright (C) 2026 Jake Rieger


#include "FlareGunSpawnShellNotify.h"
#include "FlareGunWeapon.h"

void UFlareGunSpawnShellNotify::Notify(USkeletalMeshComponent* MeshComp,
                                   UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference) {
    if (!MeshComp) { return; }
    if (auto* FlareGun = Cast<AFlareGunWeapon>(MeshComp->GetOwner())) {
        FlareGun->SpawnEjectedShellActor();
    }
}