// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InputActionWidget.generated.h"

UCLASS()
class SEETHE_API UInputActionWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ActionIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ActionPrompt;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ExposeOnSpawn = "true"))
    UTexture2D* InIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ExposeOnSpawn = "true"))
    FText InPrompt;

    virtual void NativePreConstruct() override;
};