// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConfirmNotificationWidget.h"
#include "CrosshairWidget.h"
#include "EquipableInterface.h"
#include "ToastNotificationWidget.h"
#include "WeaponInterface.h"
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
    TObjectPtr<UCanvasPanel> CrosshairContainer;

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
    virtual void NativeOnInitialized() override;

public:
    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateHealth(float HealthPercent);

    UFUNCTION(BlueprintCallable, Category="HUD")
    void ShowCrosshair() const;

    UFUNCTION(BlueprintCallable, Category="HUD")
    void HideCrosshair() const;

    UFUNCTION(BlueprintCallable, Category="HUD")
    void UpdateBatteryChargeState(EBatteryChargeState State);

    UFUNCTION(BlueprintCallable, Category="HUD")
    void SetChargeIconColor(const FColor& Color);

    UFUNCTION(BlueprintCallable, Category="HUD")
    void PostToastNotification(const FToastNotification& Notification, float Duration = 3.0f) const;

    UFUNCTION(BlueprintCallable, Category="HUD")
    void PostConfirmNotification(const FConfirmNotification& Notification);

    UFUNCTION(BlueprintCallable, Category="HUD")
    UConfirmNotificationWidget* GetCurrentConfirmWidget();

    UCrosshairWidget* GetCurrentCrosshairWidget();

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

    int32 LastCurrentAmmo {0};
    int32 LastTotalAmmo {0};

    UPROPERTY()
    TArray<FConfirmNotification> ConfirmNotificationQueue;

    UPROPERTY()
    TObjectPtr<UConfirmNotificationWidget> CurrentConfirmWidget;

    UPROPERTY()
    TObjectPtr<UCrosshairWidget> CurrentCrosshairWidget;

    UPROPERTY()
    TScriptInterface<IWeaponInterface> CurrentWeaponInterface;

    UPROPERTY()
    TScriptInterface<IEquipableInterface> CurrentEquipableInterface;

    void UpdateLastLookInput(const FVector2D& LookInput);
    void SetChargeIconBlink(bool bBlink);

    UFUNCTION()
    void ProcessNextConfirmNotification();

    UFUNCTION()
    void UpdateEquippedItem(class ABaseEquipable* Equipable);

    void CrosshairSway(float TimeDelta);
};