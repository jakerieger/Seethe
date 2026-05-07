// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponInterface.generated.h"

class ASeetheCharacter;

UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface {
    GENERATED_BODY()
};

class SEETHE_API IWeaponInterface {
    GENERATED_BODY()

public:
    virtual void Attack(ASeetheCharacter* Character) = 0;
    virtual void Reload(ASeetheCharacter* Character) = 0;
    virtual bool CanAttack() = 0;
    virtual int32 GetCurrentAmmo() = 0;
    virtual int32 GetTotalAmmo() = 0;

protected:
    virtual void PerformTrace(const FVector& Start, const FVector& End) = 0;
};