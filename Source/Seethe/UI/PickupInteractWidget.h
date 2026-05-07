// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "ActionInputIconData.h"
#include "InventoryItem.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PickupInteractWidget.generated.h"

UCLASS()
class SEETHE_API UPickupInteractWidget : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TObjectPtr<UActionInputIconData> ActionInputIconData;

    void SetItem(const UInventoryItem* Item) const;
    void SetVisible(bool bVisible);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> InputIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemQuantityText;

    virtual void NativeConstruct() override;
};