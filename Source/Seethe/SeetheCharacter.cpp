// Copyright (C) 2026 Jake Rieger

// ReSharper disable CppMemberFunctionMayBeConst
#include "SeetheCharacter.h"

#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameModeBase.h"

#include "BaseWeapon.h"
#include "WeaponPickup.h"
#include "HUDBase.h"
#include "HUDWidget.h"
#include "Flashlight.h"
#include "GameUtils.h"
#include "Seethe.h"

ASeetheCharacter::ASeetheCharacter() {
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    FirstPersonCamera->FirstPersonFieldOfView  = 70.f;
    FirstPersonCamera->FirstPersonScale        = 0.6f;
    FirstPersonCamera->SetEnableFirstPersonFieldOfView(true);
    FirstPersonCamera->SetEnableFirstPersonScale(true);

    FirstPersonArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArms"));
    FirstPersonArms->SetupAttachment(FirstPersonCamera);
    FirstPersonArms->SetOnlyOwnerSee(true);
    FirstPersonArms->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

    GetCharacterMovement()->MaxWalkSpeed = 200.0f;
}

void ASeetheCharacter::BeginPlay() {
    Super::BeginPlay();

    // Attach flashlight
    if (FlashlightClass) {
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner      = this;
        SpawnParameters.Instigator = GetInstigator();

        AFlashlight* SpawnedFlashlight = GetWorld()->SpawnActor<AFlashlight>(
            FlashlightClass,
            {},
            {},
            SpawnParameters);

        if (SpawnedFlashlight) {
            Flashlight = SpawnedFlashlight;

            FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
            Flashlight->AttachToComponent(GetMesh1P(), AttachRules, FName("S_Attach"));
            // Flashlight->SetActorRelativeTransform(FlashlightOffset);
            Flashlight->SetOn(true);

            if (UAnimInstance* AnimInst = GetMesh1P()->GetAnimInstance(); AnimInst && FlashlightAnimLayer) {
                AnimInst->LinkAnimClassLayers(FlashlightAnimLayer);
            }
        }
    }

    DefaultCameraLocation = FirstPersonCamera->GetRelativeLocation();

    GetHUD()->UpdateHealth(GetHealthPercent())
            ->UpdateBatteryChargeState(EBatteryChargeState::BCS_Full);
}

void ASeetheCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    Super::EndPlay(EndPlayReason);
}

ABaseWeapon* ASeetheCharacter::GetCurrentWeapon() const { return CurrentWeapon; }

AFlashlight* ASeetheCharacter::GetFlashlight() const {
    return Flashlight;
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

        LookAxisX = LookAxisVector.X;
        LookAxisY = LookAxisVector.Y;

        GetHUD()->UpdateLastLookInput(LookAxisVector);
    }
}

void ASeetheCharacter::StopLook() {
    GetHUD()->UpdateLastLookInput(FVector2D::ZeroVector);
}

void ASeetheCharacter::Attack(const FInputActionValue&) {
    if (IWeaponInterface* Weapon = GetWeaponInterface()) {
        APlayerController* PC = Cast<APlayerController>(GetController());
        Weapon->Attack(PC, GetMesh1P());
    }
}

void ASeetheCharacter::Holster(const FInputActionValue&) {
    DropWeapon();

    // if (IWeaponInterface* Weapon = GetWeaponInterface()) {
    //     APlayerController* PC = Cast<APlayerController>(GetController());
    //     Weapon->ToggleHolster(PC, GetMesh1P());
    // }
}

void ASeetheCharacter::Reload(const FInputActionValue&) {
    if (IWeaponInterface* Weapon = GetWeaponInterface()) {
        APlayerController* PC = Cast<APlayerController>(GetController());
        Weapon->Reload(PC, GetMesh1P());
    }
}

void ASeetheCharacter::Inspect(const FInputActionValue&) {
    if (IWeaponInterface* Weapon = GetWeaponInterface()) {
        APlayerController* PC = Cast<APlayerController>(GetController());
        Weapon->Inspect(PC, GetMesh1P());
    }
}

void ASeetheCharacter::Interact(const FInputActionValue&) {
    if (CurrentInteractable) {
        CurrentInteractable->Interact(this);
    }
}

void ASeetheCharacter::FlashlightPower(const FInputActionValue&) {
    Flashlight->SetOn(!Flashlight->IsOn());
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

    WeaponSway(DeltaTime);
    WeaponAvoidClipping(DeltaTime);
    CameraBob(DeltaTime);

    TraceForInteractables();
    TraceForEnemies();
}

void ASeetheCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Look);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Attack);
        EnhancedInputComponent->BindAction(HolsterAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Holster);
        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Reload);
        EnhancedInputComponent->BindAction(InspectAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Inspect);
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::Interact);
        EnhancedInputComponent->BindAction(FlashlightPowerAction,
                                           ETriggerEvent::Triggered,
                                           this,
                                           &ASeetheCharacter::FlashlightPower);

        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ASeetheCharacter::StopLook);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Canceled, this, &ASeetheCharacter::StopLook);
    }
}

USkeletalMeshComponent* ASeetheCharacter::GetMesh1P() const { return FirstPersonArms; }
UCameraComponent* ASeetheCharacter::GetCamera1P() const { return FirstPersonCamera; }
IWeaponInterface* ASeetheCharacter::GetWeaponInterface() const { return Cast<IWeaponInterface>(CurrentWeapon); }

bool ASeetheCharacter::HasWeapon() const {
    return CurrentWeapon != nullptr;
}

float ASeetheCharacter::GetHealthPercent() const {
    return CurrentHealth / 100.f;
}

UHUDWidget* ASeetheCharacter::GetHUD() const {
    if (const APlayerController* PC = Cast<APlayerController>(GetController())) {
        if (AHUDBase* HUD = Cast<AHUDBase>(PC->GetHUD())) {
            return HUD->HUDWidget;
        }
    }

    return nullptr;
}

float ASeetheCharacter::TakeDamage(const float DamageAmount,
                                   const FDamageEvent& DamageEvent,
                                   AController* EventInstigator,
                                   AActor* DamageCauser) {
    const float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= FMath::Floor(DamageToApply);
    GetHUD()->UpdateHealth(CurrentHealth / 100.f);

    if (CurrentHealth <= 0) {
        Die();
    }

    return DamageToApply;
}

void ASeetheCharacter::EquipWeapon(const TSubclassOf<AWeaponPickup>& PickupClass,
                                   const TSubclassOf<ABaseWeapon>& WeaponClass) {
    if (CurrentWeapon) {
        DropWeapon();
    }

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Owner        = this;
    SpawnInfo.Instigator   = GetInstigator();
    ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass,
                                                                 GetActorLocation(),
                                                                 GetActorRotation(),
                                                                 SpawnInfo);
    if (NewWeapon) {
        CurrentWeapon         = NewWeapon;
        CurrentWeaponClass    = WeaponClass;
        LastWeaponPickupClass = PickupClass;

        APlayerController* PC = Cast<APlayerController>(GetController());
        CurrentWeapon->Equip(PC, GetMesh1P());

        GetHUD()->SetWeaponCrosshairTexture(CurrentWeapon->GetCrosshairTexture())
                ->ShowWeaponCrosshair();
    }
}

void ASeetheCharacter::DropWeapon() {
    if (!CurrentWeapon || !CurrentWeaponClass || !LastWeaponPickupClass) {
        return;
    }

    const FVector DropLocation = FirstPersonArms->GetSocketLocation(FName("S_Attach")) + (
                                     GetActorForwardVector() * 100.0f);
    AWeaponPickup* DroppedWeapon = GetWorld()->SpawnActor<AWeaponPickup>(
        LastWeaponPickupClass,
        DropLocation,
        FRotator::ZeroRotator);
    DroppedWeapon->PickupMesh->AddImpulse(FirstPersonCamera->GetForwardVector() * 500.0f);

    if (DroppedWeapon) {
        DroppedWeapon->WeaponClass = CurrentWeaponClass;
    }

    CurrentWeapon->Drop(GetMesh1P());
    CurrentWeapon->Destroy();
    CurrentWeapon         = nullptr;
    CurrentWeaponClass    = nullptr;
    LastWeaponPickupClass = nullptr;

    if (UAnimInstance* AnimInst = GetMesh1P()->GetAnimInstance(); AnimInst && FlashlightAnimLayer) {
        AnimInst->LinkAnimClassLayers(FlashlightAnimLayer);
    }

    GetHUD()->HideWeaponCrosshair();
}

void ASeetheCharacter::WeaponSway(const float DeltaTime) {
    FRotator TargetSway;
    TargetSway.Pitch = FMath::Clamp(LookAxisY * SwayAmount, -MaxSway, MaxSway);
    TargetSway.Yaw   = FMath::Clamp(LookAxisX * SwayAmount, -MaxSway, MaxSway);
    TargetSway.Roll  = TargetSway.Yaw * 0.5f;

    WeaponSwayRotation = FMath::RInterpTo(WeaponSwayRotation, TargetSway, DeltaTime, SwaySmoothing);

    LookAxisX = FMath::FInterpTo(LookAxisX, 0.f, DeltaTime, 10.0f);
    LookAxisY = FMath::FInterpTo(LookAxisY, 0.f, DeltaTime, 10.0f);
}

void ASeetheCharacter::WeaponAvoidClipping(const float DeltaTime) {
    FHitResult WallHit;
    const FVector Start = FirstPersonCamera->GetComponentLocation();
    const FVector End   = Start + (FirstPersonCamera->GetForwardVector() * 100.0f);
    // roughly arm length, may need tweaking

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->SweepSingleByChannel(WallHit,
                                                       Start,
                                                       End,
                                                       FQuat::Identity,
                                                       ECC_Visibility,
                                                       FCollisionShape::MakeSphere(10.f),
                                                       Params);
    const float TargetDisplacement = bHit ? (End - WallHit.Location).Size() : 0.0f;
    WeaponDrawbackDisplacement     = FMath::FInterpTo(WeaponDrawbackDisplacement,
                                                  TargetDisplacement,
                                                  DeltaTime,
                                                  DrawbackSpeed);
}

void ASeetheCharacter::CameraBob(float DeltaTime) {
    FVector Velocity = GetVelocity();
    Velocity.Z       = 0; // Ignore jumping/falling
    if (const float Speed = Velocity.Size(); Speed > 0 && !GetCharacterMovement()->IsFalling()) {
        BobTimer            += DeltaTime * (Speed / 100.0f) * BobFrequency;
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

void ASeetheCharacter::TraceForInteractables() {
    FVector Start;
    FRotator Direction;
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Start, Direction);
    const FVector End = Start + (Direction.Vector() * InteractRange);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    if (bHit) {
        if (Hit.GetActor() && Hit.GetActor()->Implements<UInteractableInterface>()) {
            if (CurrentInteractable.GetObject() != Hit.GetActor()) {
                if (CurrentInteractable) {
                    CurrentInteractable->LookAway();
                }

                CurrentInteractable = Hit.GetActor();
                CurrentInteractable->LookAt();

                GetHUD()->ShowInteractText(CurrentInteractable->GetInteractMessage())
                        ->SetCrosshairColor(FColor::Yellow);
            }
            return;
        }
    }

    if (CurrentInteractable) {
        CurrentInteractable->LookAway();
        CurrentInteractable = nullptr;

        GetHUD()->HideInteractText()
                ->SetCrosshairColor(FColor::White);
    }
}

void ASeetheCharacter::TraceForEnemies() const {
    FHitResult Hit;
    const FVector Start = GetCamera1P()->GetComponentLocation();
    const FVector End   = Start + (FirstPersonCamera->GetForwardVector() * EnemyDetectRange);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,
                                                           Start,
                                                           End,
                                                           ECC_ENEMY,
                                                           Params);

    GetHUD()->SetCrosshairColor(bHit && Hit.GetActor() ? FColor::Red : FColor::White);
}