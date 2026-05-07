// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "FlashlightTool.h"
#include "Seethe.h"
#include "SeetheCharacter.h"
#include "BaseWeapon.h"
#include "GameFramework/PawnMovementComponent.h"

void UHUDWidget::NativeConstruct() {
    Super::NativeConstruct();

    CrosshairContainer->SetVisibility(ESlateVisibility::Hidden);
    CurrentWeaponText->SetText(FTEXT("None"));
    CurrentAmmoText->SetText(FTEXT("-"));
    TotalAmmoText->SetText(FTEXT("-"));
}

void UHUDWidget::NativeTick(const FGeometry& Geometry, const float TimeDelta) {
    Super::NativeTick(Geometry, TimeDelta);

    CrosshairSway(TimeDelta);

    if (CurrentWeaponInterface) {
        const int32 CurrentAmmo = CurrentWeaponInterface->GetCurrentAmmo();
        const int32 TotalAmmo   = CurrentWeaponInterface->GetTotalAmmo();

        if (CurrentAmmo != LastCurrentAmmo) {
            LastCurrentAmmo = CurrentAmmo;
            CurrentAmmoText->SetText(FText::FromString(FString::FromInt(LastCurrentAmmo)));
        }

        if (TotalAmmo != LastTotalAmmo) {
            LastTotalAmmo = TotalAmmo;
            TotalAmmoText->SetText(FText::FromString(FString::FromInt(LastTotalAmmo)));
        }
    }

    if (CurrentCrosshairWidget) {
        if (const ASeetheCharacter* SC = Cast<ASeetheCharacter>(GetOwningPlayerPawn())) {
            const auto CurrentSpeed = SC->GetMovementSpeed();
            if (CurrentSpeed >= SC->GetSprintSpeed()) {
                CurrentCrosshairWidget->SetSpread(1.0f);
            } else if (CurrentSpeed >= SC->GetWalkSpeed()) {
                CurrentCrosshairWidget->SetSpread(0.5f);
            } else {
                CurrentCrosshairWidget->SetSpread(0.0f);
            }
        }
    }
}

void UHUDWidget::NativeOnInitialized() {
    Super::NativeOnInitialized();

    if (ASeetheCharacter* SC = Cast<ASeetheCharacter>(GetOwningPlayerPawn())) {
        SC->OnEquippedItemChanged.AddDynamic(this, &UHUDWidget::UpdateEquippedItem);
        SC->OnHealthChanged.AddDynamic(this, &UHUDWidget::UpdateHealth);
    }
}

void UHUDWidget::UpdateHealth(const float HealthPercent) {
    if (HealthBar) {
        HealthBar->SetPercent(HealthPercent);

        if (HealthPercent <= 0.25f) {
            HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
        }
    }
}

void UHUDWidget::ShowCrosshair() const {
    CrosshairContainer->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UHUDWidget::HideCrosshair() const {
    CrosshairContainer->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDWidget::UpdateBatteryChargeState(const EBatteryChargeState State) {
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
}

void UHUDWidget::SetChargeIconColor(const FColor& Color) {
    ChargeIcon->SetColorAndOpacity(Color);
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

UCrosshairWidget* UHUDWidget::GetCurrentCrosshairWidget() {
    return CurrentCrosshairWidget;
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

void UHUDWidget::UpdateEquippedItem(ABaseEquipable* Equipable) {
    if (Equipable) {
        HideCrosshair();

        CurrentEquipableInterface = Equipable;
        CurrentWeaponText->SetText(FTEXT(CurrentEquipableInterface->GetName()));

        if (Equipable->CrosshairWidgetClass) {
            if (CurrentCrosshairWidget) {
                CurrentCrosshairWidget->RemoveFromParent();
                CurrentCrosshairWidget = nullptr;
            }

            if (auto* NewCrosshair = CreateWidget<UCrosshairWidget>(this, Equipable->CrosshairWidgetClass)) {
                CurrentCrosshairWidget = NewCrosshair;

                if (auto* CanvasSlot = CrosshairContainer->AddChildToCanvas(CurrentCrosshairWidget)) {
                    CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
                    CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
                    CanvasSlot->SetAutoSize(true);
                    CanvasSlot->SetPosition(FVector2D(0.f, 0.f));

                    CurrentCrosshairWidget->SetSpread(0.0f);
                    ShowCrosshair();
                }
            }
        }

        if (Equipable->Implements<UWeaponInterface>()) {
            CurrentWeaponInterface = Equipable;
            CurrentAmmoText->SetText(FText::FromString(FString::FromInt(CurrentWeaponInterface->GetCurrentAmmo())));
            TotalAmmoText->SetText(FText::FromString(FString::FromInt(CurrentWeaponInterface->GetTotalAmmo())));
        } else {
            CurrentAmmoText->SetText(FTEXT("-"));
            TotalAmmoText->SetText(FTEXT("-"));
        }

        if (Equipable->IsA<AFlashlightTool>()) {
            auto* FlashlightTool = Cast<AFlashlightTool>(Equipable);
            if (FlashlightTool) {
                FlashlightTool->OnUpdateBatteryLife.AddDynamic(this, &UHUDWidget::UpdateBatteryChargeState);
            }
        }
    } else {
        HideCrosshair();

        CurrentWeaponText->SetText(FTEXT("None"));
        CurrentAmmoText->SetText(FTEXT("-"));
        TotalAmmoText->SetText(FTEXT("-"));
    }
}

void UHUDWidget::CrosshairSway(const float TimeDelta) {
    const auto TargetOffset = FVector2D(LastLookInput.X * -SwayIntensity, LastLookInput.Y * SwayIntensity);
    CurrentSwayOffset       = FMath::Vector2DInterpTo(CurrentSwayOffset, TargetOffset, TimeDelta, SwaySmoothing);

    auto UpdateSlotPos = [this](const UWidget* InWidget) {
        if (InWidget) {
            if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(InWidget->Slot)) {
                CanvasSlot->SetPosition(CurrentSwayOffset);
            }
        }
    };

    UpdateSlotPos(CrosshairContainer);
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

void UHUDWidget::UpdateLastLookInput(const FVector2D& LookInput) {
    LastLookInput = LookInput;
}

void UHUDWidget::SetChargeIconBlink(const bool bBlink) {
    if (bBlink) {
        PlayAnimation(ChargeBlinkAnim, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f);
    } else {
        StopAnimation(ChargeBlinkAnim);
    }
}