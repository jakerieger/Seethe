// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Seethe/BaseEquipable.h"
#include "GameFramework/Actor.h"
#include "FlashlightTool.generated.h"

class USpotLightComponent;

UCLASS()
class SEETHE_API AFlashlightTool : public ABaseEquipable {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpotLightComponent> LightComponent;

public:
    AFlashlightTool();

    virtual void Use(ASeetheCharacter* Character) override;

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
    float BatteryLife{33.3f};
    /** Battery life is 5 minutes. This value represents how much to drain the battery EACH SECOND. **/
    float BatteryDrainRate{0.333333f};

    UFUNCTION()
    void OnPowerMontageEnded(UAnimMontage* Montage, bool bInterrupted, bool bFlashlightOn) const;
};