// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "Components/NamedSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/HUD.h"
#include "HUDBase.generated.h"

class UHUDWidget;
class UInventoryWidget;
class UInventoryItem;

class UItemPickedUpNotificationWidget;
class UItemDroppedNotificationWidget;
class UItemEquippedNotificationWidget;

class AInventoryItem3dPreview;

UCLASS()
class SEETHE_API AHUDBase : public AHUD {
    GENERATED_BODY()

public:
    /************************************************************/
    /*                    Class Properties                      */
    /************************************************************/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<UHUDWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<UInventoryWidget> InventoryWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD")
    TSubclassOf<AInventoryItem3dPreview> PreviewActorClass;

    /************************************************************/
    /*                 Class Instance Getters                   */
    /************************************************************/
    UFUNCTION(BlueprintPure, Category = "HUD")
    FORCEINLINE UHUDWidget* GetHUDWidget() const { return HUDWidget; };

    UFUNCTION(BlueprintPure, Category = "HUD")
    FORCEINLINE UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; };

    UFUNCTION(BlueprintPure, Category = "HUD")
    FORCEINLINE AInventoryItem3dPreview* GetPreviewActor() const { return PreviewActor; };

    /************************************************************/
    /*                        Helpers                           */
    /************************************************************/
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowInventory() const;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideInventory() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    bool IsInventoryOpen() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /************************************************************/
    /*                     Class Instances                      */
    /************************************************************/
    UPROPERTY()
    TObjectPtr<UHUDWidget> HUDWidget;

    UPROPERTY()
    TObjectPtr<UInventoryWidget> InventoryWidget;

    UPROPERTY()
    TObjectPtr<AInventoryItem3dPreview> PreviewActor;
};