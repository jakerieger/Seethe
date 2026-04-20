// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractableInterface.h"
#include "WeaponInterface.h"
#include "SeetheCharacter.generated.h"

class UInputAction;
class ABaseWeapon;
class AWeaponPickup;
class UCameraComponent;
class AFlashlight;
class UHUDWidget;
struct FInputActionValue;

UCLASS()
class SEETHE_API ASeetheCharacter : public ACharacter {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> FirstPersonArms;

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
    TObjectPtr<UInputAction> AttackAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> HolsterAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ReloadAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> InspectAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> FlashlightPowerAction;

    UPROPERTY(EditAnywhere, Category="Flashlight")
    TSubclassOf<AFlashlight> FlashlightClass;

    UPROPERTY(EditAnywhere, Category="Flashlight")
    TSubclassOf<UAnimInstance> FlashlightAnimLayer;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float DrawbackSpeed = 15.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    float WeaponDrawbackDisplacement;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwayAmount = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float MaxSway = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwaySmoothing = 10.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Sway")
    FRotator WeaponSwayRotation;

    /** Input methods **/
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);
    void Holster(const FInputActionValue& Value);
    void Reload(const FInputActionValue& Value);
    void Inspect(const FInputActionValue& Value);
    void Interact(const FInputActionValue& Value);
    void FlashlightPower(const FInputActionValue& Value);

    void Die();

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    /** Public Getters **/
    UFUNCTION(BlueprintCallable, Category = "First Person")
    USkeletalMeshComponent* GetMesh1P() const;

    UFUNCTION(BlueprintCallable, Category = "First Person")
    UCameraComponent* GetCamera1P() const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    ABaseWeapon* GetCurrentWeapon() const;

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    AFlashlight* GetFlashlight() const;

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool HasWeapon() const;

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    UHUDWidget* GetHUD() const;

    virtual float TakeDamage(float DamageAmount,
                             const FDamageEvent& DamageEvent,
                             AController* EventInstigator,
                             AActor* DamageCauser) override;

    void EquipWeapon(const TSubclassOf<AWeaponPickup>& PickupClass,
                     const TSubclassOf<ABaseWeapon>& WeaponClass);
    void DropWeapon();

private:
    float BobAmplitude = 2.0f;
    float BobTimer     = 0.0f;
    float BobFrequency = 7.0f;
    FVector DefaultCameraLocation;
    int32 CurrentHealth    = 100;
    float InteractRange    = 300.0f;
    float EnemyDetectRange = 1200.0f;
    float LookAxisX        = 0, LookAxisY = 0;
    FTransform FlashlightOffset;

    UPROPERTY()
    TObjectPtr<AFlashlight> Flashlight;

    UPROPERTY()
    TObjectPtr<ABaseWeapon> CurrentWeapon;

    UPROPERTY()
    TSubclassOf<ABaseWeapon> CurrentWeaponClass;

    UPROPERTY()
    TSubclassOf<AWeaponPickup> LastWeaponPickupClass;

    UPROPERTY()
    TScriptInterface<IInteractableInterface> CurrentInteractable;

    IWeaponInterface* GetWeaponInterface() const;
    void WeaponSway(float DeltaTime);
    void WeaponAvoidClipping(float DeltaTime);
    void CameraBob(float DeltaTime);
    void TraceForInteractables();
    void TraceForEnemies() const;

    UFUNCTION()
    void StopLook();
};