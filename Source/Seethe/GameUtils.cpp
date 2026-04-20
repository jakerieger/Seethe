// Copyright (C) 2026 Jake Rieger


#include "GameUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

UDecalComponent* FGameUtils::SpawnDecalWithRandomRotation(const UObject* WorldContextObject,
                                                          UMaterialInterface* DecalMaterial,
                                                          FVector DecalSize,
                                                          const FHitResult& HitResult,
                                                          float LifeSpan) {
    const FRotator BaseRotation = HitResult.ImpactNormal.Rotation();
    const float RandomRoll      = FMath::FRandRange(0.f, 360.f);
    auto* Decal                 = UGameplayStatics::SpawnDecalAtLocation(WorldContextObject,
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

void FGameUtils::DrawDebugLineTrace(const UWorld* World,
                                    const FHitResult& HitResult,
                                    const FColor& LineColor,
                                    const float LineThickness,
                                    const float Lifetime) {
    DrawDebugLine(World, HitResult.TraceStart, HitResult.TraceEnd, LineColor, false, Lifetime, 0, LineThickness);
}

void FGameUtils::DebugPrintToScreen(const FString& Msg) {
    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, Msg);
    }
}