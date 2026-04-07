// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "SeetheCharacter.generated.h"

UCLASS()
class SEETHE_API ASeetheCharacter : public ACharacter {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> FirstPersonArms;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class USpotLightComponent> Flashlight;

public:
    ASeetheCharacter();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ShootAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> HolsterAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ReloadAction;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Shoot(const FInputActionValue& Value);
    void Holster(const FInputActionValue& Value);
    void Reload(const FInputActionValue& Value);

    UPROPERTY(EditAnywhere, Category = "HUD")
    TSubclassOf<class UHUDWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere, Category = "Revolver")
    TSubclassOf<class ARevolver> RevolverClass;

    UPROPERTY(EditAnywhere, Category = "Revolver")
    float DrawbackSpeed = 15.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Revolver")
    float WeaponDrawbackDisplacement;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwayAmount = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float MaxSway = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwaySmoothing = 8.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Sway")
    FRotator WeaponSwayRotation;

    void Die();

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    FORCEINLINE USkeletalMeshComponent* GetFirstPersonMesh() const {
        return FirstPersonArms;
    }

    virtual float TakeDamage(float DamageAmount,
                             const struct FDamageEvent& DamageEvent,
                             class AController* EventInstigator,
                             AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "Revolver")
    void EquipRevolver();

    UFUNCTION(BlueprintCallable, Category = "Revolver")
    bool GetRevolverHolstered() const;

private:
    void UpdateHolstered(bool Holstered);

    static constexpr float KWalkSpeedHolstered   = 400;
    static constexpr float KWalkSpeedUnholstered = 300;

    static constexpr float KBobFreqHolstered   = 4.0f;
    static constexpr float KBobFreqUnholstered = 2.0f;

    float BobTimer     = 0.0f;
    float BobFrequency = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Camera Bob")
    float BobAmplitude = 4.0f;

    FVector DefaultCameraLocation;

    int32 CurrentHealth = 100;

    float MouseX, MouseY;

    UPROPERTY()
    TObjectPtr<UHUDWidget> HUDWidget;

    UPROPERTY()
    TObjectPtr<ARevolver> RevolverInstance;

    void WeaponSway(float DeltaTime);
    void WeaponAvoidClipping(float DeltaTime);
    void CameraBob(float DeltaTime);
};