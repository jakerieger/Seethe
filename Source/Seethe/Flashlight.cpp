// Copyright (C) 2026 Jake Rieger


#include "Flashlight.h"
#include "SeetheCharacter.h"
#include "HUDWidget.h"
#include "Components/SpotLightComponent.h"

AFlashlight::AFlashlight() {
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
    RootComponent = MeshComponent;

    LightComponent = CreateDefaultSubobject<USpotLightComponent>("LightComponent");
    LightComponent->SetupAttachment(MeshComponent);
    LightComponent->SetIntensityUnits(ELightUnits::Lumens);
    LightComponent->SetVisibility(true);
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
    BatteryLife += Amount;
    if (BatteryLife > 100.0f) {
        BatteryLife = 100.0f;
    }
    if (BatteryLife < 0.0f) {
        BatteryLife = 0.0f;
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
                PC->GetHUD()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Low);
            } else if (BatteryLife <= 67.0f) {
                PC->GetHUD()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Mid);
            } else {
                PC->GetHUD()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Full);
            }
        } else {
            BatteryLife = 0.0f;
            SetOn(false);
            PC->GetHUD()->UpdateBatteryChargeState(EBatteryChargeState::BCS_Dead);
        }
    }
}

void AFlashlight::OnPowerMontageEnded(UAnimMontage*, bool, const bool bFlashlightOn) const {
    if (bFlashlightOn && !IsDead()) {
        LightComponent->SetVisibility(true);
    } else {
        LightComponent->SetVisibility(false);
    }
}