// Copyright Epic Games, Inc. All Rights Reserved.

// ReSharper disable CppEnforceNestedNamespacesStyle
#pragma once

#include "CoreMinimal.h"

/** ECC_GameTraceChannel1 **/
#define ECC_PROJECTILE ECC_GameTraceChannel1
/** ECC_GameTraceChannel2 **/
#define ECC_ENEMY ECC_GameTraceChannel2

namespace Seethe {
    namespace Constants {
        /** Flashlight takes a maximum of six batteries.
        100% / 6 = 16.666...*/
        inline constexpr float kBatteryChargeAmount = 16.6666666667f;
    }
}