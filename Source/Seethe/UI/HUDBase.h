// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDBase.generated.h"

class UHUDWidget;
class UInventoryWidget;

UCLASS()
class SEETHE_API AHUDBase : public AHUD {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
    TObjectPtr<UHUDWidget> HUDWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
    TObjectPtr<UInventoryWidget> InventoryWidget;

    void ShowInventory() const;
    void HideInventory() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};