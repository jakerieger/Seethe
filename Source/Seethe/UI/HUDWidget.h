// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConfirmNotificationWidget.h"
#include "ToastNotificationWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "HUDWidget.generated.h"

UENUM(BlueprintType)
enum class EBatteryChargeState : uint8 {
    Dead UMETA(DisplayName = "Dead"),
    LowCharge UMETA(DisplayName = "Low Charge"),
    MidCharge UMETA(DisplayName = "Mid Charge"),
    FullCharge UMETA(DisplayName = "Full Charge"),
};

UCLASS()
class SEETHE_API UHUDWidget : public UUserWidget {
    GENERATED_BODY()
    friend class ASeetheCharacter;

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel> RootCanvas;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> WeaponCrosshairImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> HitmarkerImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentAmmoText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TotalAmmoText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentWeaponText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ChargeIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> BatteryIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> ToastNotificationContainer;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USizeBox> ConfirmNotificationContainer;

    /*********************************************/
    UPROPERTY(EditAnywhere, Category = "HUD")
    TObjectPtr<UTexture2D> BatteryDeadIcon;

    UPROPERTY(EditAnywhere, Category = "HUD")
    TObjectPtr<UTexture2D> BatteryLowChargeIcon;

    UPROPERTY(EditAnywhere, Category = "HUD")
    TObjectPtr<UTexture2D> BatteryMidChargeIcon;

    UPROPERTY(EditAnywhere, Category = "HUD")
    TObjectPtr<UTexture2D> BatteryFullChargeIcon;

    UPROPERTY(EditAnywhere, Category = "HUD")
    TSubclassOf<UToastNotificationWidget> ToastNotificationWidgetClass;

    UPROPERTY(EditAnywhere, Category = "HUD")
    TSubclassOf<UConfirmNotificationWidget> ConfirmNotificationWidgetClass;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& Geometry, float TimeDelta) override;

public:
    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* UpdateHealth(float HealthPercentage);

    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* SetCrosshairTexture(UTexture2D* CrosshairTexture);

    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* ShowCrosshair();

    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* HideCrosshair();

    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* TriggerHitmarker();

    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* SetCrosshairColor(const FColor& Color);

    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* UpdateBatteryChargeState(EBatteryChargeState State);

    UFUNCTION(BlueprintCallable, Category="HUD")
    UHUDWidget* SetChargeIconColor(const FColor& Color);

    UFUNCTION(BlueprintCallable, Category="HUD")
    void PostToastNotification(const FToastNotification& Notification, float Duration = 3.f) const;

    UFUNCTION(BlueprintCallable, Category="HUD")
    void PostConfirmNotification(const FConfirmNotification& Notification);

    UFUNCTION(BlueprintCallable, Category="HUD")
    UConfirmNotificationWidget* GetCurrentConfirmWidget();

private:
    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwayIntensity {10.0f};

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwaySmoothing {14.0f};

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> ChargeBlinkAnim;

    FVector2D LastLookInput;
    FVector2D CurrentSwayOffset;
    float HitmarkerTimer {0.0f};
    float HitmarkerDuration {0.3f};

    UPROPERTY()
    TArray<FConfirmNotification> ConfirmNotificationQueue;

    UPROPERTY()
    TObjectPtr<UConfirmNotificationWidget> CurrentConfirmWidget;

    UHUDWidget* UpdateLastLookInput(const FVector2D& LookInput);
    UHUDWidget* SetChargeIconBlink(bool bBlink);

    UFUNCTION()
    void ProcessNextConfirmNotification();
};