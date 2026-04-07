// Copyright (C) 2026 Jake Rieger

#include "Smiley.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

ASmiley::ASmiley() {
    PrimaryActorTick.bCanEverTick = true;

    SmileyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SmileyMesh"));
    SmileyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SmileyMesh->SetCollisionObjectType(ECC_Pawn);
    SmileyMesh->SetNotifyRigidBodyCollision(true);
    SmileyMesh->SetCollisionResponseToAllChannels(ECR_Block);
    RootComponent = SmileyMesh;
}

float ASmiley::TakeDamage(float DamageAmount,
                          const struct FDamageEvent& DamageEvent,
                          class AController* EventInstigator,
                          AActor* DamageCauser) {
    const float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= DamageToApply;

    if (CurrentHealth <= 0.0f) {
        FVector ImpulseDir = FVector::ZeroVector;

        if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
            const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
            ImpulseDir                                = PointDamageEvent->ShotDirection;
        }

        Die(ImpulseDir);
    }

    return DamageToApply;
}

void ASmiley::BeginPlay() {
    Super::BeginPlay();
}

void ASmiley::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void ASmiley::Die(const FVector& HitDirection) {
    bDead = true;
    UGameplayStatics::PlaySoundAtLocation(this, DieSound, GetOwner()->GetActorLocation());

    DetachFromControllerPendingDestroy();

    SmileyMesh->SetAnimationMode(EAnimationMode::AnimationCustomMode);
    SmileyMesh->SetCollisionProfileName(TEXT("Ragdoll"));
    SmileyMesh->SetSimulatePhysics(true);
    SmileyMesh->SetComponentTickEnabled(false);
    SmileyMesh->AddImpulse(HitDirection * 20000.f, FName("head"), true);

    SetLifeSpan(5.0f);
}

void ASmiley::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}