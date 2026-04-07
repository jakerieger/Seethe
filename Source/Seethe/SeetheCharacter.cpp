// Copyright (C) 2026 Jake Rieger

#include "SeetheCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/GameModeBase.h"
#include "HUDWidget.h"
#include "Revolver.h"

ASeetheCharacter::ASeetheCharacter() {
    MouseX = 0.0f;
    MouseY = 0.0f;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    FirstPersonCamera->FirstPersonFieldOfView  = 70.f;
    FirstPersonCamera->FirstPersonScale        = 0.6f;

    FirstPersonArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArms"));
    FirstPersonArms->SetupAttachment(FirstPersonCamera);
    FirstPersonArms->SetOnlyOwnerSee(true);
    FirstPersonArms->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

    Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
    Flashlight->SetupAttachment(FirstPersonCamera);
    Flashlight->SetIntensityUnits(ELightUnits::Lumens);
    Flashlight->SetIntensity(15000.f);
    Flashlight->SetOuterConeAngle(30.f);
    Flashlight->SetAttenuationRadius(5000.f);
    Flashlight->SetVisibility(true);
}

void ASeetheCharacter::BeginPlay() {
    Super::BeginPlay();

    DefaultCameraLocation = FirstPersonCamera->GetRelativeLocation();

    if (HUDWidgetClass) {
        HUDWidget = CreateWidget<UHUDWidget>(GetWorld(), HUDWidgetClass);
        if (HUDWidget) {
            HUDWidget->AddToViewport();
            HUDWidget->UpdateHealth(1.f);
        }
    }

    EquipRevolver();
}

void ASeetheCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    if (RevolverInstance) {
        RevolverInstance->Destroy();
    }

    Super::EndPlay(EndPlayReason);
}

void ASeetheCharacter::Move(const FInputActionValue& Value) {
    const FVector2D MovementVector = Value.Get<FVector2D>();
    if (GetController()) {
        AddMovementInput(GetActorRightVector(), MovementVector.X);
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
    }
}

void ASeetheCharacter::Look(const FInputActionValue& Value) {
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (GetController()) {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
        MouseX = LookAxisVector.X;
        MouseY = LookAxisVector.Y;
    }
}

void ASeetheCharacter::Shoot(const FInputActionValue& Value) {
    if (RevolverInstance) {
        APlayerController* PC = Cast<APlayerController>(GetController());
        RevolverInstance->Fire(PC, FirstPersonArms);
    }
}

void ASeetheCharacter::Holster(const FInputActionValue& Value) {
    if (RevolverInstance) {
        RevolverInstance->ToggleHolster(FirstPersonArms);
        UpdateHolstered(RevolverInstance->GetHolstered());
    }
}

void ASeetheCharacter::Die() {
    DetachFromControllerPendingDestroy();
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);

    if (AGameModeBase* Gm = GetWorld()->GetAuthGameMode()) {
        Gm->RestartPlayer(GetController());
    }

    Destroy();
}

void ASeetheCharacter::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // Arm sway calculation
    const float TargetPitch = FMath::Clamp(MouseY * SwayAmount, -MaxSway, MaxSway);
    const float TargetYaw   = FMath::Clamp(MouseX * SwayAmount, -MaxSway, MaxSway);

    const FRotator TargetRot = FRotator(TargetPitch, TargetYaw, TargetYaw * 0.5f);
    WeaponSwayRotation       = FMath::RInterpTo(WeaponSwayRotation, TargetRot, DeltaTime, SwaySmoothing);

    MouseX = FMath::FInterpTo(MouseX, 0.f, DeltaTime, 10.0f);
    MouseY = FMath::FInterpTo(MouseY, 0.f, DeltaTime, 10.0f);

    // Camera bob
    FVector Velocity = GetVelocity();
    Velocity.Z       = 0; // Ignore jumping/falling

    if (const float Speed = Velocity.Size(); Speed > 0 && !GetCharacterMovement()->IsFalling()) {
        BobTimer += DeltaTime * (Speed / 100.0f) * BobFrequency;

        FVector NewLocation = DefaultCameraLocation;
        NewLocation.Z       += FMath::Sin(BobTimer) * BobAmplitude;
        NewLocation.Y       += FMath::Cos(BobTimer * 0.5f) * (BobAmplitude * 0.5f);
        FirstPersonCamera->SetRelativeLocation(NewLocation);
    } else {
        BobTimer                 = 0.0f;
        const FVector CurrentLoc = FirstPersonCamera->GetRelativeLocation();
        const FVector ResetLoc   = FMath::VInterpTo(CurrentLoc, DefaultCameraLocation, DeltaTime, 10.0f);
        FirstPersonCamera->SetRelativeLocation(ResetLoc);
    }
}

void ASeetheCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Look);
        EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Shoot);
        EnhancedInputComponent->BindAction(HolsterAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Holster);
    }
}

float ASeetheCharacter::TakeDamage(float DamageAmount,
                                   const struct FDamageEvent& DamageEvent,
                                   class AController* EventInstigator,
                                   AActor* DamageCauser) {
    const float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= FMath::Floor(DamageToApply);
    if (HUDWidget) {
        HUDWidget->UpdateHealth(CurrentHealth / 100.f);
    }

    if (CurrentHealth <= 0) {
        Die();
    }

    return DamageToApply;
}

void ASeetheCharacter::EquipRevolver() {
    if (RevolverClass) {
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.Owner      = this;
        SpawnInfo.Instigator = GetInstigator();

        RevolverInstance = GetWorld()->SpawnActor<ARevolver>(RevolverClass,
                                                             GetActorLocation(),
                                                             GetActorRotation(),
                                                             SpawnInfo);

        if (RevolverInstance) {
            RevolverInstance->Equip(FirstPersonArms, true);
            UpdateHolstered(true);
        }
    }
}

bool ASeetheCharacter::GetRevolverHolstered() const {
    return RevolverInstance ? RevolverInstance->GetHolstered() : false;
}

void ASeetheCharacter::UpdateHolstered(const bool Holstered) {
    GetCharacterMovement()->MaxWalkSpeed = Holstered ? KWalkSpeedHolstered : KWalkSpeedUnholstered;
    BobFrequency                         = Holstered ? KBobFreqHolstered : KBobFreqUnholstered;

    Flashlight->SetVisibility(Holstered);

    if (HUDWidget) {
        HUDWidget->SetCrosshairType(Holstered ? ECrosshairType::Default : ECrosshairType::Revolver);
    }
}