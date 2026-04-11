// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/CanvasPanelSlot.h"

void UHUDWidget::UpdateHealth(float HealthPercentage) {
    if (HealthBar) {
        HealthBar->SetPercent(HealthPercentage);
    }
}

void UHUDWidget::ToggleCrosshair(bool Visible) {
    TargetCrosshairOpacity = Visible ? 0.5f : 0.0f;
}

void UHUDWidget::TriggerHitmarker() {
    if (HitmarkerImage) {
        HitmarkerTimer = HitmarkerDuration;
        HitmarkerImage->SetOpacity(1.0f);
    }
}

void UHUDWidget::NativeConstruct() {
    Super::NativeConstruct();
    CrosshairImage->SetOpacity(CurrentCrosshairOpacity);
}

void UHUDWidget::NativeTick(const FGeometry& Geometry, float TimeDelta) {
    Super::NativeTick(Geometry, TimeDelta);

    if (const APlayerController* Pc = GetOwningPlayer()) {
        float MouseX, MouseY;
        Pc->GetInputMouseDelta(MouseX, MouseY);

        const FVector2D TargetOffset = FVector2D(MouseX * -SwayIntensity, MouseY * SwayIntensity);
        CurrentSwayOffset = FMath::Vector2DInterpTo(CurrentSwayOffset, TargetOffset, TimeDelta, SwaySmoothing);

        if (CrosshairImage) {
            if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CrosshairImage->Slot)) {
                CanvasSlot->SetPosition(CurrentSwayOffset);
            }
        }

        if (HitmarkerImage) {
            if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HitmarkerImage->Slot)) {
                CanvasSlot->SetPosition(CurrentSwayOffset);
            }
        }

        CurrentCrosshairOpacity = FMath::FInterpTo(CurrentCrosshairOpacity,
                                                   TargetCrosshairOpacity,
                                                   GetWorld()->GetDeltaSeconds(),
                                                   10.0f);
        CrosshairImage->SetOpacity(CurrentCrosshairOpacity);
    }

    if (HitmarkerTimer > 0.0f) {
        HitmarkerTimer -= TimeDelta;

        float HitOpacity = FMath::Clamp(HitmarkerTimer / HitmarkerDuration, 0.0f, 1.0f);
        HitmarkerImage->SetOpacity(HitOpacity);

        if (HitmarkerTimer <= 0.0f) {
            HitmarkerImage->SetOpacity(0.0f);
        }
    }
}