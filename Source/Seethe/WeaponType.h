#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
    None UMETA(DisplayName = "None"),
    Axe UMETA(DisplayName = "Axe"),
    Revolver UMETA(DisplayName = "Revolver"),
    Flare_Gun UMETA(DisplayName = "Flare Gun"),
    Shotgun UMETA(DisplayName = "Shotgun"),
    Pocket_Knife UMETA(DisplayName = "Pocket Knife"),
    Bolt_Rifle UMETA(DisplayName = "Bolt Rifle"),
};