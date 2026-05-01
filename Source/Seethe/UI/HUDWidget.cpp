// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBoxSlot.h"

void UHUDWidget::NativeConstruct() {
    Super::NativeConstruct();
    WeaponCrosshairImage->SetVisibility(ESlateVisibility::Hidden);
}

UHUDWidget* UHUDWidget::UpdateHealth(const float HealthPercentage) {
    if (HealthBar) {
        HealthBar->SetPercent(HealthPercentage);

        if (HealthPercentage <= 0.25f) {
            HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
        }
    }
    return this;
}

UHUDWidget* UHUDWidget::SetCrosshairTexture(UTexture2D* CrosshairTexture) {
    if (CrosshairTexture) {
        WeaponCrosshairImage->SetBrushFromTexture(CrosshairTexture);
    } else {
        HideCrosshair();
    }
    return this;
}

UHUDWidget* UHUDWidget::ShowCrosshair() {
    WeaponCrosshairImage->SetVisibility(ESlateVisibility::Visible);
    return this;
}

UHUDWidget* UHUDWidget::HideCrosshair() {
    WeaponCrosshairImage->SetVisibility(ESlateVisibility::Hidden);
    return this;
}

UHUDWidget* UHUDWidget::TriggerHitmarker() {
    if (HitmarkerImage) {
        HitmarkerTimer = HitmarkerDuration;
        HitmarkerImage->SetOpacity(1.0f);
    }
    return this;
}

UHUDWidget* UHUDWidget::SetCrosshairColor(const FColor& Color) {
    WeaponCrosshairImage->SetBrushTintColor(Color);
    return this;
}

UHUDWidget* UHUDWidget::UpdateBatteryChargeState(const EBatteryChargeState State) {
    SetChargeIconBlink(false);

    switch (State) {
        case EBatteryChargeState::Dead: {
            BatteryIcon->SetBrushFromTexture(BatteryDeadIcon);
            SetChargeIconBlink(true);
            break;
        }
        case EBatteryChargeState::LowCharge: {
            BatteryIcon->SetBrushFromTexture(BatteryLowChargeIcon);
            break;
        }
        case EBatteryChargeState::MidCharge: {
            BatteryIcon->SetBrushFromTexture(BatteryMidChargeIcon);
            break;
        }
        case EBatteryChargeState::FullCharge: {
            BatteryIcon->SetBrushFromTexture(BatteryFullChargeIcon);
            break;
        }
    }

    return this;
}

UHUDWidget* UHUDWidget::SetChargeIconColor(const FColor& Color) {
    ChargeIcon->SetColorAndOpacity(Color);
    return this;
}

void UHUDWidget::PostToastNotification(const FToastNotification& Notification, const float Duration) const {
    if (ToastNotificationContainer && ToastNotificationWidgetClass) {
        UToastNotificationWidget* NewWidget = CreateWidget<UToastNotificationWidget>(
            GetOwningPlayer(),
            ToastNotificationWidgetClass);
        if (NewWidget) {
            UVerticalBoxSlot* NewSlot = ToastNotificationContainer->AddChildToVerticalBox(NewWidget);
            if (NewSlot) {
                NewSlot->SetPadding(FMargin(0, 0, 0, 8.0f));
                NewWidget->BeginNotification(Notification, Duration);
                NewWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
        }
    }
}

void UHUDWidget::PostConfirmNotification(const FConfirmNotification& Notification) {
    ConfirmNotificationQueue.Add(Notification);

    if (CurrentConfirmWidget == nullptr) {
        ProcessNextConfirmNotification();
    }
}

UConfirmNotificationWidget* UHUDWidget::GetCurrentConfirmWidget() {
    return CurrentConfirmWidget;
}

void UHUDWidget::ProcessNextConfirmNotification() {
    if (ConfirmNotificationQueue.IsEmpty()) {
        if (CurrentConfirmWidget) {
            CurrentConfirmWidget->RemoveFromParent();
        }
        CurrentConfirmWidget = nullptr;
        return;
    }

    const FConfirmNotification ConfirmNotification = ConfirmNotificationQueue[0];
    ConfirmNotificationQueue.RemoveAt(0);

    CurrentConfirmWidget = CreateWidget<UConfirmNotificationWidget>(GetOwningPlayer(), ConfirmNotificationWidgetClass);
    if (CurrentConfirmWidget) {
        CurrentConfirmWidget->BeginNotification(ConfirmNotification);
        ConfirmNotificationContainer->SetContent(CurrentConfirmWidget);
        CurrentConfirmWidget->OnConfirmed.AddDynamic(this, &UHUDWidget::ProcessNextConfirmNotification);
    }
}

void UHUDWidget::NativeTick(const FGeometry& Geometry, const float TimeDelta) {
    Super::NativeTick(Geometry, TimeDelta);

    const auto TargetOffset = FVector2D(LastLookInput.X * -SwayIntensity, LastLookInput.Y * SwayIntensity);
    CurrentSwayOffset       = FMath::Vector2DInterpTo(CurrentSwayOffset, TargetOffset, TimeDelta, SwaySmoothing);

    auto UpdateSlotPos = [this](const UWidget* Image) {
        if (Image) {
            if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Image->Slot)) {
                CanvasSlot->SetPosition(CurrentSwayOffset);
            }
        }
    };

    UpdateSlotPos(WeaponCrosshairImage);
    UpdateSlotPos(HitmarkerImage);

    if (HitmarkerTimer > 0.0f) {
        HitmarkerTimer -= TimeDelta;

        const float HitOpacity = FMath::Clamp(HitmarkerTimer / HitmarkerDuration, 0.0f, 1.0f);
        HitmarkerImage->SetOpacity(HitOpacity);

        if (HitmarkerTimer <= 0.0f) {
            HitmarkerImage->SetOpacity(0.0f);
        }
    }
}

UHUDWidget* UHUDWidget::UpdateLastLookInput(const FVector2D& LookInput) {
    LastLookInput = LookInput;
    return this;
}

UHUDWidget* UHUDWidget::SetChargeIconBlink(const bool bBlink) {
    if (bBlink) {
        PlayAnimation(ChargeBlinkAnim, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f);
    } else {
        StopAnimation(ChargeBlinkAnim);
    }

    return this;
}