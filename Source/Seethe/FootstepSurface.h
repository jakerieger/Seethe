// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "FootstepSurface.generated.h"

UENUM(BlueprintType)
enum class EFootstepSurface : uint8 {
    Default = 0,
    Concrete,
    Dirt,
    Glass,
    Gravel,
    Leaves,
    Metal,
    Sand,
    Slush,
    Snow,
    Wood,
};