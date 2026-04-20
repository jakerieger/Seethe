// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "WeaponType.h"
#include "UObject/Interface.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponInterface.generated.h"

UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface {
    GENERATED_BODY()
};

class SEETHE_API IWeaponInterface {
    GENERATED_BODY()

public:
    virtual void Equip(APlayerController* PC, USkeletalMeshComponent* Arms) = 0;
    virtual void Attack(APlayerController* PC, USkeletalMeshComponent* Arms) = 0;
    virtual void Reload(APlayerController* PC, USkeletalMeshComponent* Arms) = 0;
    virtual void ToggleHolster(APlayerController* PC, USkeletalMeshComponent* Arms) = 0;
    virtual void Inspect(APlayerController* PC, USkeletalMeshComponent* Arms) = 0;
    virtual bool IsHolstered() const = 0;
    virtual bool CanAttack() = 0;
    virtual EWeaponType GetWeaponType() = 0;

protected:
    virtual void PerformTrace(const FVector& Start, const FVector& End) = 0;
};