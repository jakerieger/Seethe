// Copyright (C) 2026 Jake Rieger


#include "SeetheUtilities.h"

#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

FTimeElapsedResult USeetheUtilities::GetTimeElapsed(const UObject* WorldContext,
                                                    const float StartTime,
                                                    const float EndTime) {
    FTimeElapsedResult Result;

    if (!WorldContext) { return Result; }
    if (const auto* World = WorldContext->GetWorld()) {
        const auto CurrentTime = static_cast<float>(World->GetTimeSeconds());
        Result.Elapsed         = CurrentTime - StartTime;

        if (EndTime > 0.0f) {
            const auto InputRange    = FVector2D(0.0f, EndTime);
            const auto OutputRange   = FVector2D(0.0f, 1.0f);
            Result.ElapsedNormalized = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, Result.Elapsed);
        }
    }

    return Result;
}

UDecalComponent* USeetheUtilities::SpawnHitDecalWithRandomRollRotation(const UObject* WorldContext,
                                                                       UMaterialInterface* DecalMaterial,
                                                                       const FVector DecalSize,
                                                                       const FHitResult& HitResult,
                                                                       const float Lifespan) {
    if (!WorldContext || !DecalMaterial) { return nullptr; }

    const FRotator BaseRotation = HitResult.ImpactNormal.Rotation();
    const float RandomRoll      = FMath::FRandRange(0.0f, 360.0f);
    auto* Decal                 = UGameplayStatics::SpawnDecalAtLocation(WorldContext,
                                                         DecalMaterial,
                                                         DecalSize,
                                                         HitResult.ImpactPoint,
                                                         FRotator(BaseRotation.Pitch,
                                                                  BaseRotation.Yaw,
                                                                  RandomRoll),
                                                         Lifespan);
    if (Decal) {
        Decal->SetFadeScreenSize(0.0001f);
    }

    return Decal;
}

void USeetheUtilities::DrawHitDebugLineTrace(const UObject* WorldContext,
                                             const FHitResult& HitResult,
                                             const float Lifetime,
                                             const FColor& LineColor,
                                             const float LineThickness) {
    if (!WorldContext) { return; }

    if (const auto* World = WorldContext->GetWorld()) {
        DrawDebugLine(World, HitResult.TraceStart, HitResult.TraceEnd, LineColor, false, Lifetime, 0, LineThickness);
    }
}

void USeetheUtilities::PrintStringToScreen(const FString& String) {
    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, String);
    }
}