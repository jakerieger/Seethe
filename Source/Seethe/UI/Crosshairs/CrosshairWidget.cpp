// Copyright (C) 2026 Jake Rieger


#include "CrosshairWidget.h"

void UCrosshairWidget::SetSpread(const float Spread) {
    TargetSpread = Spread;
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime) {
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (CurrentSpread != TargetSpread) {
        CurrentSpread = FMath::FInterpTo(CurrentSpread, TargetSpread, InDeltaTime, 10.0f);
    }

    HandleSpread();
}

float UCrosshairWidget::GetCurrentSpread() const {
    return CurrentSpread;
}