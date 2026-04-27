// Copyright (C) 2026 Jake Rieger


#include "InputActionWidget.h"

void UInputActionWidget::NativePreConstruct() {
    Super::NativePreConstruct();

    if (ActionIcon && InIcon) {
        const auto Width  = InIcon->GetSizeX();
        const auto Height = InIcon->GetSizeY();
        ActionIcon->SetBrushFromTexture(InIcon);

        const auto Size = FVector2D((double)Width, (double)Height);
        ActionIcon->SetBrushSize(Size);
    }

    if (ActionPrompt) {
        ActionPrompt->SetText(InPrompt);
    }
}