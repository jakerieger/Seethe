// Copyright (C) 2026 Jake Rieger


#include "ToastNotificationWidget.h"

void UToastNotificationWidget::BeginNotification(const FToastNotification& Notification, const float Duration) {
    NotifIcon->SetBrushFromTexture(Notification.Icon);
    NotifMessage->SetText(Notification.Message);

    if (ShowAnimation) {
        PlayAnimation(ShowAnimation);
    }

    GetWorld()->GetTimerManager().SetTimer(TimerHandle,
                                           this,
                                           &UToastNotificationWidget::EndNotification,
                                           Duration,
                                           false
        );
}

void UToastNotificationWidget::EndNotification() {
    if (HideAnimation) {
        FWidgetAnimationDynamicEvent EndDelegate;
        EndDelegate.BindDynamic(this, &UToastNotificationWidget::OnHideAnimationFinished);
        BindToAnimationFinished(HideAnimation, EndDelegate);
        PlayAnimation(HideAnimation);
    } else {
        OnHideAnimationFinished();
    }
}

void UToastNotificationWidget::OnHideAnimationFinished() {
    if (HideAnimation) {
        UnbindAllFromAnimationFinished(HideAnimation);
    }

    RemoveFromParent();
}