// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"

/** A collection of general-purpose utilities. **/
struct SEETHE_API FGameUtils {
    static UDecalComponent* SpawnDecalWithRandomRotation(const UObject* WorldContextObject,
                                                         UMaterialInterface* DecalMaterial,
                                                         FVector DecalSize,
                                                         const FHitResult& HitResult,
                                                         float LifeSpan = 10.0f);

    static void DrawDebugLineTrace(const UWorld* World,
                                   const FHitResult& HitResult,
                                   const FColor& LineColor = FColor::Cyan,
                                   float LineThickness     = 1.0f,
                                   float Lifetime          = 1.0f);

    static void DebugPrintToScreen(const FString& Msg);
};