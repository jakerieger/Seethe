// Copyright (C) 2026 Jake Rieger


#include "Flashlight.h"
#include "SeetheCharacter.h"
#include "UI/HUDWidget.h"
#include "Components/SpotLightComponent.h"

AFlashlight::AFlashlight() {
    LightComponent = CreateDefaultSubobject<USpotLightComponent>("LightComponent");
    LightComponent->SetupAttachment(GetRootComponent());
    LightComponent->SetIntensityUnits(ELightUnits::Lumens);
    LightComponent->SetVisibility(false);
}

void AFlashlight::Use(ASeetheCharacter* Character) {
    SetOn(!IsOn());
}

void AFlashlight::SetOn(const bool bShouldBeOn) const {
    if (PowerMontage) {
        const ASeetheCharacter* PC = Cast<ASeetheCharacter>(GetOwner());
        if (PC) {
            UAnimInstance* Anim  = PC->GetMesh1P()->GetAnimInstance();
            const float Duration = Anim->Montage_Play(PowerMontage);
            if (Anim && Duration > 0.f) {
                FOnMontageEnded MontageEnded;
                MontageEnded.BindUObject(this, &AFlashlight::OnPowerMontageEnded, bShouldBeOn);
                Anim->Montage_SetEndDelegate(MontageEnded, PowerMontage);
            }
        }
    }
}

bool AFlashlight::IsOn() const {
    return LightComponent && LightComponent->IsVisible();
}

bool AFlashlight::IsDead() const {
    return BatteryLife <= 0.0f;
}

void AFlashlight::Recharge(const float Amount) {
    const ASeetheCharacter* PC = Cast<ASeetheCharacter>(GetOwner());
    if (!PC) { return; }

    BatteryLife += Amount;
    if (BatteryLife > 100.0f) {
        BatteryLife = 100.0f;
    }

    if (BatteryLife <= 33.0f) {
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Low);
    } else if (BatteryLife <= 67.0f) {
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Mid);
    } else {
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Full);
    }

    if (BatteryLife < 0.0f) {
        BatteryLife = 0.0f;
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Dead);
    }
}

void AFlashlight::BeginPlay() {
    Super::BeginPlay();

    // Executes once per second
    GetWorldTimerManager().SetTimer(
        TimerHandle_BatteryLifeUpdate,
        this,
        &AFlashlight::UpdateBatteryLife,
        1.0f,
        true);
}

void AFlashlight::UpdateBatteryLife() {
    const ASeetheCharacter* PC = Cast<ASeetheCharacter>(GetOwner());
    if (!PC) { return; }

    if (IsOn()) {
        if (!IsDead()) {
            BatteryLife -= BatteryDrainRate;

            if (BatteryLife <= 33.0f) {
                PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Low);
            } else if (BatteryLife <= 67.0f) {
                PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Mid);
            } else {
                PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Full);
            }
        } else {
            BatteryLife = 0.0f;
            SetOn(false);
            PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Dead);
        }
    }
}

void AFlashlight::OnPowerMontageEnded(UAnimMontage*, bool, const bool bFlashlightOn) const {
    const ASeetheCharacter* PC = Cast<ASeetheCharacter>(GetOwner());
    if (!PC) { return; }

    if (bFlashlightOn && !IsDead()) {
        LightComponent->SetVisibility(true);
        PC->GetHUDWidget()->SetChargeIconColor(FColor::Green);
    } else {
        LightComponent->SetVisibility(false);
        PC->GetHUDWidget()->SetChargeIconColor({255, 255, 255, 100});
    }
}