// Copyright (C) 2026 Jake Rieger


#include "CrosshairFlareGun.h"
#include "Components/CanvasPanelSlot.h"

void UCrosshairFlareGun::NativeConstruct() {
    Super::NativeConstruct();

    auto SetRestPosition = [this](const UImage* Segment, FVector2D& OutRestPosition) {
        if (const auto* CanvasSlot = Cast<UCanvasPanelSlot>(Segment->Slot)) {
            OutRestPosition = CanvasSlot->GetPosition();
        }
    };

    SetRestPosition(SegmentTL, SegmentTLRestPosition);
    SetRestPosition(SegmentTR, SegmentTRRestPosition);
    SetRestPosition(SegmentBL, SegmentBLRestPosition);
    SetRestPosition(SegmentBR, SegmentBRRestPosition);
}

void UCrosshairFlareGun::HandleSpread() {
    const FVector2D TLVector {-1.0f, -0.5f};
    const FVector2D TRVector {1.0f, -0.5f};
    const FVector2D BLVector {-1.0f, 0.5f};
    const FVector2D BRVector {1.0f, 0.5f};

    auto Spread = [this](const UImage* Segment, const FVector2D& SpreadVector, const FVector2D& RestPosition) {
        // Interp position from [0,0] to (SpreadDistance * SpreadVector) * SpreadAmount
        const FVector2D TargetPosition = RestPosition + (SpreadDistance * SpreadVector * GetCurrentSpread());
        auto* CanvasSlot               = Cast<UCanvasPanelSlot>(Segment->Slot);
        if (CanvasSlot) {
            CanvasSlot->SetPosition(TargetPosition);
        }
    };

    Spread(SegmentTL, TLVector, SegmentTLRestPosition);
    Spread(SegmentTR, TRVector, SegmentTRRestPosition);
    Spread(SegmentBL, BLVector, SegmentBLRestPosition);
    Spread(SegmentBR, BRVector, SegmentBRRestPosition);
}