// Copyright (C) 2026 Jake Rieger


#include "SeetheSession.h"


// Sets default values
ASeetheSession::ASeetheSession() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASeetheSession::BeginPlay() {
    Super::BeginPlay();
    
}

// Called every frame
void ASeetheSession::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

