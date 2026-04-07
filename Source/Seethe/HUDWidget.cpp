// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/CanvasPanelSlot.h"

void UHUDWidget::UpdateHealth(float HealthPercentage) {
    if (HealthBar) {
        HealthBar->SetPercent(HealthPercentage);
    }
}

void UHUDWidget::SetCrosshairType(ECrosshairType Type) {
    switch (Type) {
        case ECrosshairType::Default: {
            if (DefaultCrosshairTexture) {
                CrosshairImage->SetBrushFromTexture(DefaultCrosshairTexture);
            }
            break;
        }
        case ECrosshairType::Revolver: {
            if (RevolverCrosshairTexture) {
                CrosshairImage->SetBrushFromTexture(RevolverCrosshairTexture);
            }
            break;
        }
    }
}

void UHUDWidget::NativeConstruct() {
    Super::NativeConstruct();
    SetCrosshairType(ECrosshairType::Default);
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

        // If player is moving, fade crosshair
        float TargetOpacity = 1.0f;
        if (GetOwningPlayerPawn()->GetVelocity().Length() > 10.0f) {
            TargetOpacity = 0.1f;
        }

        CurrentCrosshairOpacity = FMath::FInterpTo(CurrentCrosshairOpacity, TargetOpacity, TimeDelta, 10.0f);
        CrosshairImage->SetOpacity(CurrentCrosshairOpacity);
    }
}