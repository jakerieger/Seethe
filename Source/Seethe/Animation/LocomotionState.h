// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ELocomotionState : uint8 {
    Idle,
    IdleInactive,
    Walking,
    Sprinting,
    Jump,
    Falling,
    Land
};