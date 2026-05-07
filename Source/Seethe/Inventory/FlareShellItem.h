// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "FlareShellItem.generated.h"

UCLASS()
class SEETHE_API UFlareShellItem : public UInventoryItem {
    GENERATED_BODY()

public:
    virtual void
    Use_Implementation(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category) override;
    virtual bool
    CanUse_Implementation(ASeetheCharacter* Character, int32 Index, const EInventoryCategory& Category) override;
};