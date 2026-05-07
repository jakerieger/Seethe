// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Seethe/BaseEquipable.h"
#include "GameFramework/Actor.h"
#include "FlashlightTool.generated.h"

class USpotLightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateBatteryLife, EBatteryChargeState, ChargeState);

UCLASS()
class SEETHE_API AFlashlightTool : public ABaseEquipable {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpotLightComponent> LightComponent;

public:
    AFlashlightTool();

    virtual void UnEquip(ASeetheCharacter* Character) override;
    virtual void Use(ASeetheCharacter* Character) override;
    virtual FString GetName() override { return "Flashlight"; };

    UFUNCTION(BlueprintCallable)
    void SetOn(bool bShouldBeOn) const;

    UFUNCTION(BlueprintCallable)
    void ToggleOn() const;

    UFUNCTION(BlueprintPure)
    bool IsOn() const;

    UFUNCTION(BlueprintPure)
    bool IsDead() const;

    UFUNCTION(BlueprintCallable)
    void Recharge(float Amount);

    UPROPERTY()
    FOnUpdateBatteryLife OnUpdateBatteryLife;

protected:
    virtual void BeginPlay() override;

    void UpdateBatteryLife();
    FTimerHandle TimerHandle_BatteryLifeUpdate;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UAnimMontage> PowerMontage;

private:
    float BatteryLife {33.3f};
    /** Battery life is 5 minutes. This value represents how much to drain the battery EACH SECOND. **/
    float BatteryDrainRate {0.333333f};

    UFUNCTION()
    void OnPowerMontageEnded(UAnimMontage* Montage, bool bInterrupted, bool bFlashlightOn) const;
};