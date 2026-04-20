// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flashlight.generated.h"

class USpotLightComponent;

UCLASS()
class SEETHE_API AFlashlight : public AActor {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpotLightComponent> LightComponent;

public:
    AFlashlight();

    UFUNCTION(BlueprintCallable, Category="Flashlight")
    void SetOn(bool bShouldBeOn) const;

    UFUNCTION(BlueprintPure, Category="Flashlight")
    bool IsOn() const;

    UFUNCTION(BlueprintPure, Category="Flashlight")
    bool IsDead() const;

    UFUNCTION(BlueprintCallable, Category="Flashlight")
    void Recharge(float Amount);

protected:
    virtual void BeginPlay() override;

    void UpdateBatteryLife();
    FTimerHandle TimerHandle_BatteryLifeUpdate;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flashlight")
    TObjectPtr<UAnimMontage> PowerMontage;

private:
    float BatteryLife{100.0f};
    float BatteryDrainRate{0.2f};

    UFUNCTION()
    void OnPowerMontageEnded(UAnimMontage* Montage, bool bInterrupted, bool bFlashlightOn) const;
};