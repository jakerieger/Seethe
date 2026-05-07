// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "CrosshairWidget.h"
#include "Components/Image.h"
#include "CrosshairFlareGun.generated.h"

UCLASS()
class SEETHE_API UCrosshairFlareGun : public UCrosshairWidget {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FVector2D SpreadDistance {40.0f};

protected:
    virtual void NativeConstruct() override;
    virtual void HandleSpread() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SegmentTL;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SegmentTR;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SegmentBL;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SegmentBR;

private:
    FVector2D SegmentTLRestPosition;
    FVector2D SegmentTRRestPosition;
    FVector2D SegmentBLRestPosition;
    FVector2D SegmentBRRestPosition;
};