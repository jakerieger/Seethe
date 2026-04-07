// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RevolverProjectile.generated.h"

UCLASS()
class SEETHE_API ARevolverProjectile : public AActor {
    GENERATED_BODY()

public:
    ARevolverProjectile();

    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    TObjectPtr<class USphereComponent> Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp,
               AActor* OtherActor,
               UPrimitiveComponent* OtherComp,
               FVector NormalImpulse,
               const FHitResult& Hit);

    UPROPERTY(EditAnywhere, Category="Effects")
    class UNiagaraSystem* ImpactFX;

    UPROPERTY(EditAnywhere, Category="Effects")
    UMaterialInterface* ImpactDecal;

protected:
    virtual void BeginPlay() override;
};