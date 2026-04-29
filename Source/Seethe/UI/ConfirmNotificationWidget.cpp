// Copyright (C) 2026 Jake Rieger


#include "ConfirmNotificationWidget.h"

void UConfirmNotificationWidget::BeginNotification(const FConfirmNotification& Notification) {
    NotifIcon->SetBrushFromTexture(Notification.Icon);
    NotifTitle->SetText(Notification.Title);
    NotifMessage->SetText(Notification.Message);

    if (ShowAnimation) {
        PlayAnimation(ShowAnimation);
    }
}

void UConfirmNotificationWidget::ConfirmNotification(const bool bSkipAnimation) {
    if (HideAnimation && !bSkipAnimation) {
        FWidgetAnimationDynamicEvent EndDelegate;
        EndDelegate.BindDynamic(this, &UConfirmNotificationWidget::EndNotification);
        BindToAnimationFinished(HideAnimation, EndDelegate);
        PlayAnimation(HideAnimation);
    } else {
        EndNotification();
    }
}

void UConfirmNotificationWidget::EndNotification() {
    if (HideAnimation) {
        UnbindAllFromAnimationFinished(HideAnimation);
    }

    if (OnConfirmed.IsBound()) {
        OnConfirmed.Broadcast();
    }

    RemoveFromParent();
}