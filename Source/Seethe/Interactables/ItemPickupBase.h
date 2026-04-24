// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/InteractableInterface.h"
#include "Components/WidgetComponent.h"
#include "ItemPickupBase.generated.h"

class UInventoryItem;

UCLASS()
class SEETHE_API AItemPickupBase : public AActor, public IInteractableInterface {
    GENERATED_BODY()

    /** Blueprint Components **/
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> ItemMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UWidgetComponent> InteractWidget;

public:
    /** CTOR **/
    AItemPickupBase();

    /** Properties **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<USoundBase> EquipSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UInventoryItem> InventoryItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UTexture2D> InteractIcon;

    /** IInteractableInterface overrides **/
    virtual void Interact(ASeetheCharacter* Character) override;
    virtual void LookAt() override;
    virtual void LookAway() override;
    virtual FText GetInteractMessage() override { return FText::FromString(""); }

    /** Helpers **/
    UFUNCTION(BlueprintPure, Category="Inventory")
    UStaticMeshComponent* GetMesh() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ShowInteractWidget() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void HideInteractWidget() const;

protected:
    virtual void BeginPlay() override;
};