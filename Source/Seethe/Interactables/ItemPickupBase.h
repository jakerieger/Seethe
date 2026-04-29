// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Seethe/Interfaces/InteractableInterface.h"
#include "Components/WidgetComponent.h"
#include "ItemPickupBase.generated.h"

class UInventoryItem;
class UPickupAnimData;

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
    TObjectPtr<UInventoryItem> InventoryItem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UTexture2D> InteractIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    TObjectPtr<UPickupAnimData> AnimData;

    /** IInteractableInterface overrides **/
    virtual bool Interact(ASeetheCharacter* Character) override;
    virtual void LookAt() override;
    virtual void LookAway() override;
    virtual FText GetInteractMessage() override;

    UFUNCTION(BlueprintNativeEvent, Category="Pickup")
    void OnItemPickedUp(ASeetheCharacter* Character);
    virtual void OnItemPickedUp_Implementation(ASeetheCharacter* Character) {};

    /** Helpers **/
    UFUNCTION(BlueprintPure, Category="Inventory")
    UStaticMeshComponent* GetMesh() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ShowInteractWidget() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void HideInteractWidget() const;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnPickupMontageEnded(UAnimMontage* Montage, bool bInterrupted, ASeetheCharacter* Character);
};