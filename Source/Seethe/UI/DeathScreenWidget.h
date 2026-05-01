// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreenWidget.generated.h"

UCLASS()
class SEETHE_API UDeathScreenWidget : public UUserWidget {
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> FadeInOut;

private:
    FTimerHandle DestructHandle;

    UFUNCTION()
    void SelfDestruct();
};