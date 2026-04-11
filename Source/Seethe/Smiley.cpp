// Copyright (C) 2026 Jake Rieger

#include "Smiley.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

ASmiley::ASmiley() {
    PrimaryActorTick.bCanEverTick = true;

    SmileyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SmileyMesh"));
    SmileyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SmileyMesh->SetCollisionObjectType(ECC_Pawn);
    SmileyMesh->SetNotifyRigidBodyCollision(true);
    SmileyMesh->SetCollisionResponseToAllChannels(ECR_Block);
    SmileyMesh->SetSimulatePhysics(false);

    RootComponent = SmileyMesh;

    PhysicalAnimationComponent = CreateDefaultSubobject<
        UPhysicalAnimationComponent>(TEXT("PhysicalAnimationComponent"));
}

float ASmiley::TakeDamage(float DamageAmount,
                          const FDamageEvent& DamageEvent,
                          AController* EventInstigator,
                          AActor* DamageCauser) {
    const float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= DamageToApply;

    FVector ImpulseDir = FVector::ZeroVector;

    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
        const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        ImpulseDir                                = PointDamageEvent->ShotDirection;
        Hit(PointDamageEvent->HitInfo.BoneName, ImpulseDir, PointDamageEvent->HitInfo.ImpactPoint);
    }

    if (CurrentHealth <= 0.0f) {
        Die(ImpulseDir);
    }

    return DamageToApply;
}

void ASmiley::BeginPlay() {
    Super::BeginPlay();

    PhysicalAnimationComponent->SetSkeletalMeshComponent(GetMesh());
    // Apply the strength profile. 
    // "IsIncludeSelf" should be true to affect the bone you hit and its children.
    PhysicalAnimationComponent->ApplyPhysicalAnimationProfileBelow(FName("head"), PhysicsProfileName, true);
    GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("head"), true, false);
}

void ASmiley::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}

void ASmiley::Die(const FVector& HitDirection) {
    bDead = true;
    UGameplayStatics::PlaySoundAtLocation(this, DieSound, GetOwner()->GetActorLocation());

    DetachFromControllerPendingDestroy();

    GetMesh()->SetAnimationMode(EAnimationMode::AnimationCustomMode);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->WakeAllRigidBodies();
    GetMesh()->SetComponentTickEnabled(false);

    SetLifeSpan(5.0f);
}

void ASmiley::Hit(const FName BoneName, const FVector& ImpulseVector, const FVector& HitLocation) const {
    GetMesh()->AddImpulseAtLocation(ImpulseVector, HitLocation, BoneName);

    FPhysicalAnimationData Data;
    Data.bIsLocalSimulation      = true;
    Data.OrientationStrength     = 500.0f; // Lower = more "floppy" hit
    Data.AngularVelocityStrength = 100.0f;
    Data.PositionStrength        = 0.0f;
    Data.VelocityStrength        = 0.0f;

    PhysicalAnimationComponent->ApplyPhysicalAnimationSettingsBelow(BoneName, Data);
}

USkeletalMeshComponent* ASmiley::GetMesh() const {
    return SmileyMesh;
}

void ASmiley::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}