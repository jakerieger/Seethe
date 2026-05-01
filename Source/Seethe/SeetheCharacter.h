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
class UInventorySlotWidget;
class UInventoryItemData;
class UCharacterInputData;

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

    friend UInventoryWidget;
    friend UInventorySlotWidget;

public:
    ASeetheCharacter();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Input methods **/
    void OnMove(const FInputActionValue& Value);
    void OnLook(const FInputActionValue& Value);

    void OnUse();
    void OnReload();
    void OnToggleInventory();
    void OnInteract();
    void OnUnEquip();
    void OnSprintStarted();
    void OnSprintEnded();

protected:
    /** Properties (Editable) **/
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UCharacterInputData> InputData;

    UPROPERTY(EditAnywhere, Category = "Controller")
    float WalkSpeed {200.0f};

    UPROPERTY(EditAnywhere, Category = "Controller")
    float SprintSpeed {325.0f};

    UPROPERTY(EditAnywhere, Category = "Controller")
    float JumpHeight {325.0f};

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float DrawbackSpeed {15.0f};

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwayAmount {1.0f};

    UPROPERTY(EditAnywhere, Category = "Sway")
    float MaxSway {5.0f};

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwaySmoothing {15.0f};

    UPROPERTY(EditAnywhere, Category = "First Person")
    TSubclassOf<UCameraShakeBase> HitCameraShake;

    UPROPERTY(EditAnywhere, Category = "First Person")
    TObjectPtr<UForceFeedbackEffect> HitFFB;

    /** Properties (Read-Only) **/
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    float DrawbackDisplacement;

    UPROPERTY(BlueprintReadOnly, Category = "Sway")
    FRotator EquipSwayRotation;

    void Die();

    UFUNCTION()
    void Respawn();

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;

    /** Public Getters **/
    UFUNCTION(BlueprintPure, Category="Seethe")
    USkeletalMeshComponent* GetMesh1P() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    UCameraComponent* GetCamera1P() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    UInventoryComponent* GetInventory() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    ABaseEquipable* GetCurrentEquipable() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    ABaseWeapon* GetCurrentWeapon() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    bool HasEquippedItem() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    class AHUDBase* GetHUDInstance() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    UHUDWidget* GetHUDWidget() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    UInventoryWidget* GetInventoryWidget() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    APlayerCameraManager* GetPlayerCameraManager() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    UFirstPersonAnimInstance* GetAnimInstance1P() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    FLeftHandSocketResult GetLeftHandSocketTransform() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    bool IsSprinting() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    bool IsGrounded() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    bool IsFalling() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    float GetWalkSpeed() const;

    UFUNCTION(BlueprintPure, Category="Seethe")
    float GetSprintSpeed() const;

    virtual float TakeDamage(float DamageAmount,
                             const FDamageEvent& DamageEvent,
                             AController* EventInstigator,
                             AActor* DamageCauser) override;

    void Equip(const UInventoryItemEquipable* Item);
    void UnEquip();
    void Drop();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UseItem(int32 Index, const EInventoryCategory& Category);

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
    bool bSprinting {false};
    FTimerHandle RespawnHandle;

    UPROPERTY()
    TMap<UClass*, ABaseEquipable*> CachedEquipables;

    UPROPERTY()
    TObjectPtr<ABaseEquipable> CurrentEquipable;

    UPROPERTY()
    TScriptInterface<IInteractableInterface> CurrentInteractable;

    IEquipableInterface* GetEquipableInterface() const;
    IWeaponInterface* GetWeaponInterface() const;

    void Mesh1PSway(float DeltaTime);
    void Mesh1PAvoidClipping(float DeltaTime);
    void CameraBob(float DeltaTime);
    void TraceForInteractables();
    void TraceForEnemies() const;

    UFUNCTION()
    void OnStopLook();
};