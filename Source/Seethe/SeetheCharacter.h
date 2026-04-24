// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "WeaponInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/EquipableInterface.h"
#include "Interfaces/InteractableInterface.h"
#include "SeetheCharacter.generated.h"

class ABaseEquipable;
class AItemPickupBase;
class ABaseWeapon;

class UInputAction;
class UCameraComponent;
class UHUDWidget;
class UInventoryComponent;
class UInventoryWidget;
class UInventoryItemData;

struct FInputActionValue;

UCLASS()
class SEETHE_API ASeetheCharacter : public ACharacter {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> FirstPersonArms;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInventoryComponent> InventoryComponent;

public:
    ASeetheCharacter();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    /** Properties **/
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> UseAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ReloadAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> InventoryAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float DrawbackSpeed = 15.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    float DrawbackDisplacement;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwayAmount = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float MaxSway = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwaySmoothing = 10.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Sway")
    FRotator EquipSwayRotation;

    /** Input methods **/
    void OnMove(const FInputActionValue& Value);
    void OnLook(const FInputActionValue& Value);
    void OnUse(const FInputActionValue& Value);
    void OnReload(const FInputActionValue& Value);
    void OnShowInventory(const FInputActionValue& Value);
    void OnInteract(const FInputActionValue& Value);

    void Die();

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /** Public Getters **/
    UFUNCTION(BlueprintCallable, Category = "First Person")
    USkeletalMeshComponent* GetMesh1P() const;

    UFUNCTION(BlueprintCallable, Category = "First Person")
    UCameraComponent* GetCamera1P() const;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    UInventoryComponent* GetInventory();

    ABaseEquipable* GetCurrentEquipable();
    ABaseWeapon* GetCurrentWeapon();

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    UHUDWidget* GetHUDWidget() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    UInventoryWidget* GetInventoryWidget() const;

    virtual float TakeDamage(float DamageAmount,
                             const FDamageEvent& DamageEvent,
                             AController* EventInstigator,
                             AActor* DamageCauser) override;

    void Equip(const UInventoryItemEquipable* Item);
    void Drop();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseItem(int32 Index, bool bShouldConsume);

private:
    float BobAmplitude = 2.0f;
    float BobTimer     = 0.0f;
    float BobFrequency = 7.0f;
    FVector DefaultCameraLocation;
    int32 CurrentHealth    = 100;
    float InteractRange    = 300.0f;
    float EnemyDetectRange = 1200.0f;
    float LookAxisX        = 0, LookAxisY = 0;
    FTransform EquipableOffset;
    bool bIsInventoryOpen = false;

    UPROPERTY()
    TObjectPtr<ABaseEquipable> CurrentEquipable;

    UPROPERTY()
    TSubclassOf<AItemPickupBase> LastPickupClass;

    UPROPERTY()
    TScriptInterface<IInteractableInterface> CurrentInteractable;

    IEquipableInterface* GetEquipableInterface() const;
    IWeaponInterface* GetWeaponInterface() const;

    void EquipableSway(float DeltaTime);
    void Mesh1PAvoidClipping(float DeltaTime);
    void CameraBob(float DeltaTime);
    void TraceForInteractables();
    void TraceForEnemies() const;

    UFUNCTION()
    void OnStopLook();

    UFUNCTION()
    void OnHideInventory();
};