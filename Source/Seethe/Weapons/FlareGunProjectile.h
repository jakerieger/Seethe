// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FlareGunProjectile.generated.h"

UCLASS()
class SEETHE_API AFlareGunProjectile : public AActor {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USphereComponent> ImpactCollider;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCapsuleComponent> TrailCollider;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> FlareMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UPointLightComponent> FlareLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTimelineComponent> BrightnessTimeline;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTimelineComponent> AttenuationTimeline;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTimelineComponent> SpeedTimeline;

public:
    AFlareGunProjectile();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UCurveFloat> BrightnessCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UCurveFloat> AttenuationCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UCurveFloat> SpeedCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxSpeed {10000.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float InitialSpeed {2000.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HitDamage {25.0f};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TrailDamage {5.0f};

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    FTimerHandle AutoDestroyTimer;
    float TickRate {0.25f};

    FOnTimelineFloat BrightnessUpdateDelegate;
    FOnTimelineEvent BrightnessFinishedDelegate;
    FOnTimelineFloat AttenuationUpdateDelegate;
    FOnTimelineFloat SpeedUpdateDelegate;

    UPROPERTY()
    TMap<APawn*, FTimerHandle> TickDamageMap;

    UFUNCTION()
    void BrightnessTimelineUpdate(float Brightness) const;

    UFUNCTION()
    void BrightnessTimelineFinished();

    UFUNCTION()
    void AttenuationTimelineUpdate(float Radius) const;

    UFUNCTION()
    void SpeedTimelineUpdate(float SpeedDelta) const;

    UFUNCTION(BlueprintCallable)
    void SelfDestruct();

    UFUNCTION(BlueprintCallable)
    void OnProjectileHit(UPrimitiveComponent* HitComp,
                         AActor* OtherActor,
                         UPrimitiveComponent* OtherComp,
                         FVector NormalImpulse,
                         const FHitResult& Hit);

    UFUNCTION(BlueprintCallable)
    void OnTrailBeginOverlap(UPrimitiveComponent* OverlappedComp,
                             AActor* OtherActor,
                             UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex,
                             bool bFromSweep,
                             const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable)
    void OnTrailEndOverlap(UPrimitiveComponent* OverlappedComponent,
                           AActor* OtherActor,
                           UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex);

    UFUNCTION()
    void ApplyTickDamage(APawn* DamageReceiver);
};