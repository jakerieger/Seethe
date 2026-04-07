// Copyright (C) 2026 Jake Rieger

#include "RevolverProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARevolverProjectile::ARevolverProjectile() {
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(5.0f);
    Collision->BodyInstance.SetCollisionProfileName("Projectile");

    Collision->OnComponentHit.AddDynamic(this, &ARevolverProjectile::OnHit);

    RootComponent = Collision;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = Collision;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
}

void ARevolverProjectile::OnHit(UPrimitiveComponent* HitComp,
                                AActor* OtherActor,
                                UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse,
                                const FHitResult& Hit) {
    if (OtherActor && OtherActor != this && OtherComp) {
        if (ImpactFX) {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
                                                           ImpactFX,
                                                           Hit.ImpactPoint,
                                                           Hit.ImpactNormal.Rotation());
        }

        if (ImpactDecal) {
            auto* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
                                                                 ImpactDecal,
                                                                 FVector(8.f, 8.f, 8.f),
                                                                 Hit.ImpactPoint,
                                                                 Hit.ImpactNormal.Rotation(),
                                                                 10.0f);
            if (Decal) {
                Decal->SetFadeScreenSize(0.0001f);
            }
        }

        // Enemy hit detection
        if (const FName HitBone = Hit.BoneName; HitBone != NAME_None) {
            float DamageAmount = 20.0f;

            if (HitBone == FName("head")) {
                DamageAmount *= 2;
            }

            // Apply damage
            UGameplayStatics::ApplyPointDamage(OtherActor,
                                               DamageAmount,
                                               GetActorForwardVector(),
                                               Hit,
                                               GetInstigatorController(),
                                               this,
                                               nullptr);
        }

        Destroy();
    }
}

void ARevolverProjectile::BeginPlay() {
    Super::BeginPlay();
}