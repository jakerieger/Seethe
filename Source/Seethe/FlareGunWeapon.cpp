// Copyright (C) 2026 Jake Rieger


#include "FlareGunWeapon.h"


// Sets default values
AFlareGunWeapon::AFlareGunWeapon() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFlareGunWeapon::BeginPlay() {
    Super::BeginPlay();
    
}

// Called every frame
void AFlareGunWeapon::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

