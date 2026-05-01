// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SeetheUtilities.generated.h"

USTRUCT(BlueprintType)
struct FTimeElapsedResult {
    GENERATED_BODY()

    /** Time elapsed in seconds. **/
    UPROPERTY(BlueprintReadOnly)
    float Elapsed;

    /** Time elapsed (normalized, 0-1) in seconds. **/
    UPROPERTY(BlueprintReadOnly)
    float ElapsedNormalized;

    FTimeElapsedResult() : Elapsed(0.0f), ElapsedNormalized(0.0f) {}

    FTimeElapsedResult(const float InElapsed, const float InElapsedNormalized) : Elapsed(InElapsed),
        ElapsedNormalized(InElapsedNormalized) {}
};

UCLASS()
class SEETHE_API USeetheUtilities : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Seethe Utilities")
    static FTimeElapsedResult GetTimeElapsed(const UObject* WorldContext, float StartTime, float EndTime = 0.0f);

    UFUNCTION(BlueprintCallable, Category="Seethe Utilities")
    static UDecalComponent* SpawnHitDecalWithRandomRollRotation(const UObject* WorldContext,
                                                                UMaterialInterface* DecalMaterial,
                                                                FVector DecalSize,
                                                                const FHitResult& HitResult,
                                                                float Lifespan = 10.0f);

    UFUNCTION(BlueprintCallable, Category="Seethe Utilities")
    static void DrawHitDebugLineTrace(const UObject* WorldContext,
                                      const FHitResult& HitResult,
                                      float Lifetime          = 1.0f,
                                      const FColor& LineColor = FColor::Cyan,
                                      float LineThickness     = 1.0f);
};