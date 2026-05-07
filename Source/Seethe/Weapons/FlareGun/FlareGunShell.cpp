// Copyright (C) 2026 Jake Rieger


#include "FlareGunShell.h"

AFlareGunShell::AFlareGunShell() {
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot     = CreateDefaultSubobject<USceneComponent>("SceneRoot");
    RootComponent = SceneRoot;

    CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("SphereCollider");
    CapsuleCollider->SetupAttachment(SceneRoot);

    ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>("ShellMesh");
    ShellMesh->SetupAttachment(CapsuleCollider);
}

UStaticMeshComponent* AFlareGunShell::GetMesh() {
    return ShellMesh;
}

void AFlareGunShell::BeginPlay() {
    Super::BeginPlay();

    GetWorldTimerManager()
        .SetTimer(SelfDestructTimer,
                  this,
                  &AFlareGunShell::SelfDestruct,
                  Lifetime,
                  true);
}

void AFlareGunShell::SelfDestruct() {
    Destroy();
}