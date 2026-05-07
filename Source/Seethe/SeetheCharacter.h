// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "FootstepAudioData.h"
#include "FootstepSurface.h"
#include "InventoryItem.h"
#include "WeaponInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/EquipableInterface.h"
#include "Interfaces/InteractableInterface.h"
#include "LocomotionState.h"
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
class UFootstepAudioData;
class UCharacterAnimInstance;

struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, ABaseEquipable*, Equipable);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, HealthPercent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerIdleInactive);

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
    virtual void Jump() override;

    UFUNCTION()
    virtual void Landed(const FHitResult& Hit) override;

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

    void SetHealth(int32 Health);
    void Die();
    void HandleFootstep(EFootstepType Type);

    /** 
     * Returns a value where in range [-1, 1] where:
     *   - 0 = Not strafing at all
     *   - -1 = Strafing Left
     *   - 1 = Strafing Right
     **/
    UFUNCTION(BlueprintPure)
    float GetStrafeFactor() const;

    UFUNCTION(BlueprintPure)
    float GetStrafeBlendAlpha() const;

    UFUNCTION(BlueprintPure)
    FRotator GetStrafeRotation() const;

    UFUNCTION(BlueprintPure)
    FVector GetStrafeTranslation() const;

    UFUNCTION()
    void OnStopLook();

    UFUNCTION()
    void Respawn();

    UPROPERTY(BlueprintAssignable)
    FOnEquippedItemChanged OnEquippedItemChanged;

    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnPlayerIdleInactive OnPlayerIdleInactive;

    UPROPERTY(BlueprintReadOnly, Category = "Idle")
    float IdleTime {0.0f};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idle")
    float IdleInactiveThreshold {15.0f}; // In seconds

    UPROPERTY(BlueprintReadOnly, Category = "Idle")
    bool bIsIdleInactive = false;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UCurveFloat> BobCurveZ;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UCurveFloat> BobCurveY;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UCurveFloat> FootstepCurve;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UCameraShakeBase> BobShakeJump;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UCameraShakeBase> BobShakeJumpLand;

protected:
    /** Properties (Editable) **/
    UPROPERTY(EditAnywhere)
    TObjectPtr<UCharacterInputData> InputData;

    UPROPERTY(EditAnywhere)
    float WalkSpeed {200.0f};

    UPROPERTY(EditAnywhere)
    float SprintSpeed {325.0f};

    UPROPERTY(EditAnywhere)
    float JumpHeight {325.0f};

    UPROPERTY(EditAnywhere)
    float DrawbackSpeed {15.0f};

    UPROPERTY(EditAnywhere)
    float SwayAmount {1.0f};

    UPROPERTY(EditAnywhere)
    float MaxSway {5.0f};

    UPROPERTY(EditAnywhere)
    float SwaySmoothing {15.0f};

    UPROPERTY(EditAnywhere)
    float SwayStopSmoothing {10.f};

    /** How much to rotate the arms **/
    UPROPERTY(EditAnywhere)
    float StrafeRotationAmount {10.0f};

    /** How much to pull them in by **/
    UPROPERTY(EditAnywhere)
    float StrafeTranslationAmount {5.0f};

    UPROPERTY(EditAnywhere)
    TSubclassOf<UCameraShakeBase> HitCameraShake;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UForceFeedbackEffect> HitFFB;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<EFootstepSurface, TSoftObjectPtr<UFootstepAudioData>> FootstepDataMap;

    /** Properties (Read-Only) **/
    UPROPERTY(BlueprintReadOnly)
    float DrawbackDisplacement;

    UPROPERTY(BlueprintReadOnly)
    FRotator EquipSwayRotation;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;

    /** Public Getters **/
    UFUNCTION(BlueprintPure)
    USkeletalMeshComponent* GetMesh1P() const;

    UFUNCTION(BlueprintPure)
    UCameraComponent* GetCamera1P() const;

    UFUNCTION(BlueprintPure)
    UInventoryComponent* GetInventory() const;

    UFUNCTION(BlueprintPure)
    ABaseEquipable* GetCurrentEquipable() const;

    UFUNCTION(BlueprintPure)
    ABaseWeapon* GetCurrentWeapon() const;

    UFUNCTION(BlueprintPure)
    bool HasEquippedItem() const;

    UFUNCTION(BlueprintPure)
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure)
    class AHUDBase* GetHUDInstance() const;

    UFUNCTION(BlueprintPure)
    UHUDWidget* GetHUDWidget() const;

    UFUNCTION(BlueprintPure)
    UInventoryWidget* GetInventoryWidget() const;

    UFUNCTION(BlueprintPure)
    APlayerCameraManager* GetPlayerCameraManager() const;

    UFUNCTION(BlueprintPure)
    UCharacterAnimInstance* GetAnimInstance1P() const;

    UFUNCTION(BlueprintPure)
    FLeftHandSocketResult GetLeftHandSocketTransform() const;

    UFUNCTION(BlueprintPure)
    bool IsSprinting() const;

    UFUNCTION(BlueprintPure)
    bool IsMoving() const;

    UFUNCTION(BlueprintPure)
    bool IsGrounded() const;

    UFUNCTION(BlueprintPure)
    bool IsFalling() const;

    UFUNCTION(BlueprintPure)
    float GetWalkSpeed() const;

    UFUNCTION(BlueprintPure)
    float GetSprintSpeed() const;

    UFUNCTION(BlueprintPure)
    float GetMovementSpeed() const;

    UFUNCTION(BlueprintPure)
    ELocomotionState GetLocomotionState() const;

    virtual float TakeDamage(float DamageAmount,
                             const FDamageEvent& DamageEvent,
                             AController* EventInstigator,
                             AActor* DamageCauser) override;

    void Equip(const UInventoryItemEquipable* Item);
    void UnEquip();
    void Drop();
    void SetCurrentEquipable(ABaseEquipable* NewCurrentEquipable);
    void SetLocomotionState(const ELocomotionState& NewLocomotionState);

    UFUNCTION(BlueprintCallable)
    void UseItem(int32 Index, const EInventoryCategory& Category) const;

private:
    int32 CurrentHealth {100};
    bool bSprinting {false};
    float InteractRange {200.0f};
    float EnemyDetectRange {1200.0f};
    float LookAxisX {0.0f};
    float LookAxisY {0.0f};
    float BobAmplitudeZ {-5.0f};
    float BobAmplitudeSprintingZ {-10.0f};
    float BobAmplitudeY {-2.0f};
    float BobAmplitudeSprintingY {-3.0f};
    float CurveTime {0.0f}; // Time accumulator for head bob curves
    float CurrentBobSpeed {1.0f};
    float CurrentAmplitudeZ {0.0f};
    float CurrentAmplitudeY {0.0f};
    float BobTransitionSpeed {3.0f};
    float FootstepsPrevious {0.0f};
    float FootstepsThreshold {0.5f};

    FVector DefaultCameraLocation;
    FTransform EquipableOffset;
    FTimerHandle RespawnHandle;
    ELocomotionState CurrentLocomotionState {ELocomotionState::Idle};

    UPROPERTY()
    float SmoothedStrafeFactor {0.0f};

    UPROPERTY(EditAnywhere)
    float StrafeInterpSpeed {4.0f};

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
    void TraceForInteractables();
    void TraceForEnemies() const;
    void UpdateEquipable();
    void UpdateHealth();
    void UpdateIdleStatus(float DeltaTime);
    void UpdateStrafeFactor(float DeltaTime);
    void UpdateCurveDrivenEffects(float DeltaTime);
};