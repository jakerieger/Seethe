// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipableInterface.generated.h"

class ASeetheCharacter;

UINTERFACE()
class UEquipableInterface : public UInterface {
    GENERATED_BODY()
};

class SEETHE_API IEquipableInterface {
    GENERATED_BODY()

public:
    virtual void Equip(ASeetheCharacter* Character) = 0;
    virtual void UnEquip(ASeetheCharacter* Character) = 0;
    virtual void Drop(ASeetheCharacter* Character) = 0;
    virtual void Use(ASeetheCharacter* Character) = 0;
};