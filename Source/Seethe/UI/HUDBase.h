// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDBase.generated.h"

class UHUDWidget;
class UInventoryWidget;
class AInventoryItem3dPreview;

UCLASS()
class SEETHE_API AHUDBase : public AHUD {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<AInventoryItem3dPreview> PreviewActorClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
    TObjectPtr<UHUDWidget> HUDWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
    TObjectPtr<UInventoryWidget> InventoryWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="HUD")
    TObjectPtr<AInventoryItem3dPreview> PreviewActor;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowInventory() const;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideInventory() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    bool IsInventoryOpen() const;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    AInventoryItem3dPreview* GetInventoryItem3dPreview() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};