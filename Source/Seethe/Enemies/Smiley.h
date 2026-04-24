// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Smiley.generated.h"

UCLASS()
class SEETHE_API ASmiley : public APawn {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> SmileyMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UPhysicalAnimationComponent> PhysicalAnimationComponent;

public:
    ASmiley();

    virtual float TakeDamage(float DamageAmount,
                             const struct FDamageEvent& DamageEvent,
                             class AController* EventInstigator,
                             AActor* DamageCauser) override;

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    void Die(const FVector& HitDirection);

    void Hit(FName BoneName, const FVector& ImpulseVector, const FVector& HitLocation) const;

    USkeletalMeshComponent* GetMesh() const;

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<USoundBase> DieSound;

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    float CurrentHealth{10000.0f};
    bool bDead{false};
    FName PhysicsProfileName{"HitReactionProfile"};
};