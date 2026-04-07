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

public:
    ASmiley();

    virtual float TakeDamage(float DamageAmount,
                             const struct FDamageEvent& DamageEvent,
                             class AController* EventInstigator,
                             AActor* DamageCauser) override;
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

    void Die(const FVector& HitDirection);

    UPROPERTY(EditAnywhere, Category="Effects")
    TObjectPtr<USoundBase> DieSound;

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    float CurrentHealth = 100.0f;
    bool bDead          = false;
};