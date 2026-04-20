// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "BaseWeapon.h"
#include "WeaponPickup.generated.h"

UCLASS()
class SEETHE_API AWeaponPickup : public AActor, public IInteractableInterface {
    GENERATED_BODY()

public:
    AWeaponPickup();

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UStaticMeshComponent> PickupMesh;

    UPROPERTY(EditAnywhere, Category="Pickup")
    TSubclassOf<ABaseWeapon> WeaponClass;

    UPROPERTY(EditAnywhere, Category="Pickup")
    FString WeaponName;

    virtual void Interact(ASeetheCharacter* Character) override;
    virtual void LookAt() override;
    virtual void LookAway() override;
    virtual FString GetInteractMessage() override;
};