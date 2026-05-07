// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

UCLASS(Abstract)
class SEETHE_API UCrosshairWidget : public UUserWidget {
    GENERATED_BODY()

public:
    // Float value from 0 to 1
    void SetSpread(float Spread);

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual void HandleSpread() {}

    float GetCurrentSpread() const;

private:
    float TargetSpread {0.0f};
    float CurrentSpread {0.0f};
};