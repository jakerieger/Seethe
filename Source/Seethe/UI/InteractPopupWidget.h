// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InteractPopupWidget.generated.h"

UCLASS()
class SEETHE_API UInteractPopupWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> InputIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> InteractMsgText;

    UFUNCTION(BlueprintCallable, Category="HUD")
    void SetupPrompt(UTexture2D* Icon, const FText& Msg) const;
    
    UFUNCTION(BlueprintCallable, Category="HUD")
    void SetVisible(bool bVisible);
};