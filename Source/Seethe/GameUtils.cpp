// Copyright (C) 2026 Jake Rieger


#include "GameUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

UDecalComponent* UGameUtils::SpawnDecalWithRandomRotation(const UObject* WorldContextObject,
                                                          UMaterialInterface* DecalMaterial,
                                                          FVector DecalSize,
                                                          const FHitResult& HitResult,
                                                          float LifeSpan) {
    FRotator BaseRotation = HitResult.ImpactNormal.Rotation();
    float RandomRoll      = FMath::FRandRange(0.f, 360.f);
    auto* Decal           = UGameplayStatics::SpawnDecalAtLocation(WorldContextObject,
                                                         DecalMaterial,
                                                         DecalSize,
                                                         HitResult.ImpactPoint,
                                                         FRotator(BaseRotation.Pitch,
                                                                  BaseRotation.Yaw,
                                                                  RandomRoll),
                                                         LifeSpan);
    if (Decal) {
        Decal->SetFadeScreenSize(0.0001f);
    }

    return Decal;
}

void UGameUtils::DrawDebugLineTrace(const UWorld* World,
                                    const FHitResult& HitResult,
                                    const FColor& LineColor,
                                    const float LineThickness,
                                    const float Lifetime) {
    #if defined(SEETHE_ENABLE_DEBUG_TRACES)
    DrawDebugLine(World, HitResult.TraceStart, HitResult.TraceEnd, LineColor, false, Lifetime, 0, LineThickness);
    #endif
}