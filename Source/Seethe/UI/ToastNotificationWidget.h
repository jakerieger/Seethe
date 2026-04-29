// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ToastNotificationWidget.generated.h"

USTRUCT(BlueprintType)
struct FToastNotification {
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY()
    FText Message;
};

UCLASS()
class SEETHE_API UToastNotificationWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> NotifIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NotifMessage;

    void BeginNotification(const FToastNotification& Notification, const float Duration);

protected:
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> ShowAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> HideAnimation;

private:
    FTimerHandle TimerHandle;

    UFUNCTION()
    void EndNotification();

    UFUNCTION()
    void OnHideAnimationFinished();
};