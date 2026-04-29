// Copyright (C) 2026 Jake Rieger


#include "FlashlightTool.h"
#include "Seethe/SeetheCharacter.h"
#include "Seethe/UI/HUDWidget.h"
#include "Components/SpotLightComponent.h"

AFlashlightTool::AFlashlightTool() {
    LightComponent = CreateDefaultSubobject<USpotLightComponent>("LightComponent");
    LightComponent->SetupAttachment(GetRootComponent());
    LightComponent->SetIntensityUnits(ELightUnits::Lumens);
    LightComponent->SetVisibility(false);
}

void AFlashlightTool::UnEquip(ASeetheCharacter* Character) {
    OnPowerMontageEnded(nullptr, false, false);
    Super::UnEquip(Character);
}

void AFlashlightTool::Use(ASeetheCharacter* Character) {
    SetOn(!IsOn());
}

void AFlashlightTool::SetOn(const bool bShouldBeOn) const {
    if (PowerMontage) {
        const ASeetheCharacter* PC = Cast<ASeetheCharacter>(GetOwner());
        if (PC) {
            UAnimInstance* Anim  = PC->GetMesh1P()->GetAnimInstance();
            const float Duration = Anim->Montage_Play(PowerMontage);
            if (Anim && Duration > 0.f) {
                FOnMontageEnded MontageEnded;
                MontageEnded.BindUObject(this, &AFlashlightTool::OnPowerMontageEnded, bShouldBeOn);
                Anim->Montage_SetEndDelegate(MontageEnded, PowerMontage);
            }
        }
    }
}

bool AFlashlightTool::IsOn() const {
    return LightComponent && LightComponent->IsVisible();
}

bool AFlashlightTool::IsDead() const {
    return BatteryLife <= 0.0f;
}

void AFlashlightTool::Recharge(const float Amount) {
    const ASeetheCharacter* PC = Cast<ASeetheCharacter>(GetOwner());
    if (!PC) { return; }

    BatteryLife += Amount;
    if (BatteryLife > 100.0f) {
        BatteryLife = 100.0f;
    }

    if (BatteryLife <= 33.0f) {
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::LowCharge);
    } else if (BatteryLife <= 67.0f) {
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::MidCharge);
    } else {
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::FullCharge);
    }

    if (BatteryLife < 0.0f) {
        BatteryLife = 0.0f;
        PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::Dead);
    }
}

void AFlashlightTool::BeginPlay() {
    Super::BeginPlay();

    // Executes once per second
    GetWorldTimerManager().SetTimer(
        TimerHandle_BatteryLifeUpdate,
        this,
        &AFlashlightTool::UpdateBatteryLife,
        1.0f,
        true);
}

void AFlashlightTool::UpdateBatteryLife() {
    const ASeetheCharacter* PC = Cast<ASeetheCharacter>(GetOwner());
    if (!PC) { return; }

    if (IsOn()) {
        if (!IsDead()) {
            BatteryLife -= BatteryDrainRate;

            if (BatteryLife <= 33.0f) {
                PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::LowCharge);
            } else if (BatteryLife <= 67.0f) {
                PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::MidCharge);
            } else {
                PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::FullCharge);
            }
        } else {
            BatteryLife = 0.0f;
            SetOn(false);
            PC->GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::Dead);
        }
    }
}

void AFlashlightTool::OnPowerMontageEnded(UAnimMontage*, bool, const bool bFlashlightOn) const {
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