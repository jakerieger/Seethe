// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "HUDWidget.generated.h"

UENUM(BlueprintType)
enum class ECrosshairType : uint8 {
    Default = 0,
    Revolver = 1,
};

UCLASS()
class SEETHE_API UHUDWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel> RootCanvas;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CrosshairImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY(EditAnywhere, Category = "Crosshair")
    TObjectPtr<UTexture2D> DefaultCrosshairTexture;

    UPROPERTY(EditAnywhere, Category = "Crosshair")
    TObjectPtr<UTexture2D> RevolverCrosshairTexture;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateHealth(float HealthPercentage);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetCrosshairType(ECrosshairType Type);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& Geometry, float TimeDelta) override;

private:
    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwayIntensity = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwaySmoothing = 14.0f;

    FVector2D CurrentSwayOffset;
    float CurrentCrosshairOpacity = 1.0f;
};