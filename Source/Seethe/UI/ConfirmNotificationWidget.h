// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ConfirmNotificationWidget.generated.h"

USTRUCT(BlueprintType)
struct FConfirmNotification {
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY()
    FText Title;

    UPROPERTY()
    FText Message;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmed);

UCLASS()
class SEETHE_API UConfirmNotificationWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> NotifIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NotifTitle;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NotifMessage;

    UPROPERTY(BlueprintAssignable, Category = "Notification")
    FOnConfirmed OnConfirmed;

    void BeginNotification(const FConfirmNotification& Notification);
    void ConfirmNotification(bool bSkipAnimation);

protected:
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> ShowAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> HideAnimation;

private:
    UFUNCTION()
    void EndNotification();
};