// Copyright (C) 2026 Jake Rieger


#include "InteractPopupWidget.h"

void UInteractPopupWidget::SetupPrompt(UTexture2D* Icon, const FText& Msg) const {
    InputIcon->SetBrushFromTexture(Icon);
    InteractMsgText->SetText(Msg);
}

void UInteractPopupWidget::SetVisible(bool bVisible) {
    if (bVisible) {
        SetVisibility(ESlateVisibility::Visible);
    } else {
        SetVisibility(ESlateVisibility::Hidden);
    }
}