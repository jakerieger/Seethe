// Copyright (C) 2026 Jake Rieger

// ReSharper disable CppMemberFunctionMayBeConst
#include "SeetheCharacter.h"
#include "BaseEquipable.h"
#include "BaseWeapon.h"
#include "Camera/CameraComponent.h"
#include "CharacterAnimInstance.h"
#include "CharacterInputData.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DeathScreenWidget.h"
#include "EnhancedInputComponent.h"
#include "FootstepAudioData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "HUDBase.h"
#include "HUDWidget.h"
#include "InputActionValue.h"
#include "InventoryComponent.h"
#include "InventoryWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Seethe.h"
#include "SeetheGameMode.h"

ASeetheCharacter::ASeetheCharacter() {
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh());
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    FirstPersonCamera->FirstPersonFieldOfView  = 70.0f;
    FirstPersonCamera->FirstPersonScale        = 0.6f;
    FirstPersonCamera->SetEnableFirstPersonFieldOfView(true);
    FirstPersonCamera->SetEnableFirstPersonScale(true);

    FirstPersonArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonArms"));
    FirstPersonArms->SetupAttachment(FirstPersonCamera);
    FirstPersonArms->SetOnlyOwnerSee(true);
    FirstPersonArms->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

    GetCharacterMovement()->MaxWalkSpeed  = WalkSpeed;
    GetCharacterMovement()->JumpZVelocity = JumpHeight;
}

#pragma region Getters
USkeletalMeshComponent* ASeetheCharacter::GetMesh1P() const { return FirstPersonArms; }
UCameraComponent* ASeetheCharacter::GetCamera1P() const { return FirstPersonCamera; }
UInventoryComponent* ASeetheCharacter::GetInventory() const { return InventoryComponent; }
ABaseEquipable* ASeetheCharacter::GetCurrentEquipable() const { return CurrentEquipable; }
ABaseWeapon* ASeetheCharacter::GetCurrentWeapon() const { return Cast<ABaseWeapon>(CurrentEquipable); }
bool ASeetheCharacter::HasEquippedItem() const { return CurrentEquipable != nullptr; }
float ASeetheCharacter::GetHealthPercent() const { return CurrentHealth / 100.0f; }
bool ASeetheCharacter::IsSprinting() const { return bSprinting; }
bool ASeetheCharacter::IsMoving() const { return GetVelocity().SizeSquared() > 10.0f; }
bool ASeetheCharacter::IsGrounded() const { return !GetMovementComponent()->IsFalling(); }
bool ASeetheCharacter::IsFalling() const { return GetVelocity().Z < 0.0f; }
float ASeetheCharacter::GetWalkSpeed() const { return WalkSpeed; }
float ASeetheCharacter::GetSprintSpeed() const { return SprintSpeed; }
float ASeetheCharacter::GetMovementSpeed() const { return GetMovementComponent()->Velocity.Length(); }
ELocomotionState ASeetheCharacter::GetLocomotionState() const { return CurrentLocomotionState; }

AHUDBase* ASeetheCharacter::GetHUDInstance() const {
    if (const APlayerController* PC = Cast<APlayerController>(GetController())) {
        return Cast<AHUDBase>(PC->GetHUD());
    }
    return nullptr;
}

UHUDWidget* ASeetheCharacter::GetHUDWidget() const {
    if (const APlayerController* PC = Cast<APlayerController>(GetController())) {
        if (const AHUDBase* HUD = Cast<AHUDBase>(PC->GetHUD())) {
            return HUD->GetHUDWidget();
        }
    }

    return nullptr;
}

UInventoryWidget* ASeetheCharacter::GetInventoryWidget() const {
    if (const APlayerController* PC = Cast<APlayerController>(GetController())) {
        if (const AHUDBase* HUD = Cast<AHUDBase>(PC->GetHUD())) {
            return HUD->GetInventoryWidget();
        }
    }

    return nullptr;
}

APlayerCameraManager* ASeetheCharacter::GetPlayerCameraManager() const {
    if (const APlayerController* PC = Cast<APlayerController>(GetController())) {
        return PC->PlayerCameraManager;
    }

    return nullptr;
}

UCharacterAnimInstance* ASeetheCharacter::GetAnimInstance1P() const {
    return Cast<UCharacterAnimInstance>(GetMesh1P()->GetAnimInstance());
}

FLeftHandSocketResult ASeetheCharacter::GetLeftHandSocketTransform() const {
    FLeftHandSocketResult Result;

    if (HasEquippedItem()) {
        const auto* Equipped   = GetCurrentEquipable();
        Result.bHasSocket      = Equipped->HasLeftHandSocket();
        Result.SocketTransform = Equipped->GetLeftHandSocketTransform();
    }

    return Result;
}

float ASeetheCharacter::GetStrafeFactor() const {
    const FVector RightVector        = GetActorRightVector();
    const FVector VelocityNormalized = GetVelocity().GetSafeNormal();
    return FVector::DotProduct(VelocityNormalized, RightVector);
}

float ASeetheCharacter::GetStrafeBlendAlpha() const {
    return FMath::Abs(SmoothedStrafeFactor);
}

FRotator ASeetheCharacter::GetStrafeRotation() const {
    FRotator Rotation {FRotator::ZeroRotator};

    if (SmoothedStrafeFactor < 0) {
        Rotation.Pitch = -StrafeRotationAmount;
    } else {
        Rotation.Pitch = StrafeRotationAmount;
    }

    return Rotation;
}

FVector ASeetheCharacter::GetStrafeTranslation() const {
    FVector Translation {FVector::ZeroVector};

    if (FMath::Abs(SmoothedStrafeFactor) > 0) {
        Translation.Y = -StrafeTranslationAmount;
    }

    return Translation;
}

IEquipableInterface* ASeetheCharacter::GetEquipableInterface() const {
    return Cast<IEquipableInterface>(CurrentEquipable);
}

IWeaponInterface* ASeetheCharacter::GetWeaponInterface() const {
    return Cast<IWeaponInterface>(CurrentEquipable);
}
#pragma endregion

#pragma region Setters
void ASeetheCharacter::SetHealth(const int32 Health) {
    CurrentHealth = Health;
    UpdateHealth();
}

void ASeetheCharacter::SetCurrentEquipable(ABaseEquipable* NewCurrentEquipable) {
    CurrentEquipable = NewCurrentEquipable;
    UpdateEquipable();
}

void ASeetheCharacter::SetLocomotionState(const ELocomotionState& NewLocomotionState) {
    CurrentLocomotionState = NewLocomotionState;
}
#pragma endregion

#pragma region Class Overrides
void ASeetheCharacter::BeginPlay() {
    Super::BeginPlay();

    DefaultCameraLocation = FirstPersonCamera->GetRelativeLocation();
    CurrentHealth         = 100;

    UpdateHealth();
    UpdateEquipable();
}

void ASeetheCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    Super::EndPlay(EndPlayReason);
}

void ASeetheCharacter::Jump() {
    Super::Jump();

    HandleFootstep(EFootstepType::Jump);

    if (BobShakeJump) {
        auto* PC = Cast<APlayerController>(GetController());
        if (PC) {
            PC->ClientStartCameraShake(BobShakeJump, 1.0f);
        }
    }
}

void ASeetheCharacter::Landed(const FHitResult& Hit) {
    Super::Landed(Hit);

    HandleFootstep(EFootstepType::Land);

    if (BobShakeJumpLand) {
        auto* PC = Cast<APlayerController>(GetController());
        if (PC) {
            PC->ClientStartCameraShake(BobShakeJumpLand, 1.0f);
        }
    }
}

void ASeetheCharacter::Tick(const float DeltaTime) {
    Super::Tick(DeltaTime);

    Mesh1PSway(DeltaTime);
    Mesh1PAvoidClipping(DeltaTime);
    UpdateIdleStatus(DeltaTime);
    UpdateStrafeFactor(DeltaTime);
    UpdateCurveDrivenEffects(DeltaTime);

    TraceForInteractables();
    TraceForEnemies();
}

void ASeetheCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent); EIC && InputData) {
        EIC->BindAction(InputData->MoveAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::OnMove);
        EIC->BindAction(InputData->LookAction, ETriggerEvent::Triggered, this, &ASeetheCharacter::OnLook);
        EIC->BindAction(InputData->UseAction, ETriggerEvent::Started, this, &ASeetheCharacter::OnUse);
        EIC->BindAction(InputData->ReloadAction, ETriggerEvent::Started, this, &ASeetheCharacter::OnReload);
        EIC->BindAction(InputData->InteractAction,
                        ETriggerEvent::Started,
                        this,
                        &ASeetheCharacter::OnInteract);
        EIC->BindAction(InputData->InventoryAction,
                        ETriggerEvent::Started,
                        this,
                        &ASeetheCharacter::OnToggleInventory);
        EIC->BindAction(InputData->UnEquipAction, ETriggerEvent::Started, this, &ASeetheCharacter::OnUnEquip);

        EIC->BindAction(InputData->LookAction, ETriggerEvent::Completed, this, &ASeetheCharacter::OnStopLook);
        EIC->BindAction(InputData->LookAction, ETriggerEvent::Canceled, this, &ASeetheCharacter::OnStopLook);

        EIC->BindAction(InputData->SprintAction, ETriggerEvent::Started, this, &ASeetheCharacter::OnSprintStarted);
        EIC->BindAction(InputData->SprintAction, ETriggerEvent::Completed, this, &ASeetheCharacter::OnSprintEnded);

        EIC->BindAction(InputData->JumpAction, ETriggerEvent::Started, this, &ASeetheCharacter::Jump);
        EIC->BindAction(InputData->JumpAction, ETriggerEvent::Completed, this, &ASeetheCharacter::StopJumping);
    }
}

void ASeetheCharacter::PossessedBy(AController* NewController) {
    Super::PossessedBy(NewController);

    APlayerController* PC = Cast<APlayerController>(NewController);
    if (PC) {
        PC->SetInputMode(FInputModeGameOnly {});
    }
}

float ASeetheCharacter::TakeDamage(const float DamageAmount,
                                   const FDamageEvent& DamageEvent,
                                   AController* EventInstigator,
                                   AActor* DamageCauser) {
    const float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= FMath::Floor(DamageToApply);
    UpdateHealth();

    if (GetPlayerCameraManager()) {
        GetPlayerCameraManager()->StartCameraShake(HitCameraShake);
    }

    if (auto* PC = Cast<APlayerController>(GetController()); PC && HitFFB) {
        PC->ClientPlayForceFeedback(HitFFB);
    }

    if (CurrentHealth <= 0) {
        Die();
    }

    return DamageToApply;
}
#pragma endregion

#pragma region Input Handlers
void ASeetheCharacter::OnMove(const FInputActionValue& Value) {
    const FVector2D MovementVector = Value.Get<FVector2D>();
    if (GetController()) {
        AddMovementInput(GetActorRightVector(), MovementVector.X);
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
    }
}

void ASeetheCharacter::OnLook(const FInputActionValue& Value) {
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (GetController()) {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);

        LookAxisX = LookAxisVector.X;
        LookAxisY = LookAxisVector.Y;

        // TODO: Convert these to delegates
        GetHUDWidget()->UpdateLastLookInput(LookAxisVector);
        GetInventoryWidget()->UpdateLookAxes(LookAxisX, LookAxisY);
    }
}

void ASeetheCharacter::OnStopLook() {
    GetHUDWidget()->UpdateLastLookInput(FVector2D::ZeroVector);
}

void ASeetheCharacter::OnUse() {
    if (IEquipableInterface* Equipable = GetEquipableInterface()) {
        Equipable->Use(this);
    }
}

void ASeetheCharacter::OnReload() {
    if (IWeaponInterface* Weapon = GetWeaponInterface()) {
        Weapon->Reload(this);
    }
}

void ASeetheCharacter::OnToggleInventory() {
    APlayerController* PC = Cast<APlayerController>(GetController());
    const AHUDBase* HUD   = Cast<AHUDBase>(PC->GetHUD());

    if (PC && HUD) {
        if (HUD->IsInventoryOpen()) {
            HUD->HideInventory();

            PC->ResetIgnoreLookInput();
            PC->ResetIgnoreMoveInput();
            PC->SetShowMouseCursor(false);

            const FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        } else {
            HUD->ShowInventory();

            PC->SetIgnoreLookInput(true);
            PC->SetIgnoreMoveInput(true);
            PC->SetShowMouseCursor(true);

            FInputModeUIOnly InputMode;
            if (UUserWidget* InvWidget = HUD->GetInventoryWidget()) {
                InputMode.SetWidgetToFocus(InvWidget->TakeWidget());
            }
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
            PC->SetInputMode(InputMode);
        }
    }
}

void ASeetheCharacter::OnInteract() {
    if (auto* HUDWidget = GetHUDWidget()) {
        if (auto* ConfirmWidget = HUDWidget->GetCurrentConfirmWidget()) {
            ConfirmWidget->ConfirmNotification(false);
            return;
        }
    }

    if (CurrentInteractable) {
        CurrentInteractable->Interact(this);
    }
}

void ASeetheCharacter::OnUnEquip() {
    UnEquip();
}

void ASeetheCharacter::OnSprintStarted() {
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    bSprinting                           = true;
}

void ASeetheCharacter::OnSprintEnded() {
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    bSprinting                           = false;
}
#pragma endregion

#pragma region Tick Handlers
void ASeetheCharacter::UpdateStrafeFactor(const float DeltaTime) {
    const float TargetStrafeFactor = GetStrafeFactor();
    SmoothedStrafeFactor = FMath::FInterpTo(SmoothedStrafeFactor, TargetStrafeFactor, DeltaTime, StrafeInterpSpeed);
}

void ASeetheCharacter::UpdateCurveDrivenEffects(const float DeltaTime) {
    if (IsMoving() && IsGrounded()) {
        const bool bIsSprinting = IsSprinting();

        const float TargetBobSpeed = bIsSprinting ? 1.5f : 1.0f;
        const float TargetAmpZ     = bIsSprinting ? BobAmplitudeSprintingZ : BobAmplitudeZ;
        const float TargetAmpY     = bIsSprinting ? BobAmplitudeSprintingY : BobAmplitudeY;

        CurrentBobSpeed   = FMath::FInterpTo(CurrentBobSpeed, TargetBobSpeed, DeltaTime, BobTransitionSpeed);
        CurrentAmplitudeZ = FMath::FInterpTo(CurrentAmplitudeZ, TargetAmpZ, DeltaTime, BobTransitionSpeed);
        CurrentAmplitudeY = FMath::FInterpTo(CurrentAmplitudeY, TargetAmpY, DeltaTime, BobTransitionSpeed);

        CurveTime += DeltaTime * CurrentBobSpeed;

        if (!BobCurveZ || !BobCurveY || !FootstepCurve) {
            UE_LOG(LogTemp, Error, TEXT("UpdateHeadBob() : Missing BobCurveZ and/or BobCurveY"))
            return;
        }

        float MinZ, MaxZ;
        BobCurveZ->GetTimeRange(MinZ, MaxZ);
        const FFloatInterval RangeZ {MinZ, MaxZ};

        float MinY, MaxY;
        BobCurveY->GetTimeRange(MinY, MaxY);
        const FFloatInterval RangeY {MinY, MaxY};

        // Curves have different lengths
        if (RangeZ != RangeY) {
            UE_LOG(LogTemp, Error, TEXT("UpdateHeadBob() : RangeZ != RangeY"))
            return;
        }

        // TODO: Cache this in BeginPlay
        const float MaxTime = RangeZ.Max;
        const auto Duration = MaxTime - RangeZ.Min;

        if (CurveTime > MaxTime) {
            CurveTime -= Duration;
        }

        const auto BobZ           = BobCurveZ->GetFloatValue(CurveTime);
        const auto BobY           = BobCurveY->GetFloatValue(CurveTime);
        const auto FootstepsValue = FootstepCurve->GetFloatValue(CurveTime);

        // Check for and trigger footstep sound
        {
            const bool bWasBelow = FootstepsPrevious < FootstepsThreshold;
            const bool bIsAbove  = FootstepsValue >= FootstepsThreshold;

            if (bWasBelow && bIsAbove) {
                HandleFootstep(bIsSprinting ? EFootstepType::Sprint : EFootstepType::Walk);
            }

            FootstepsPrevious = FootstepsValue;
        }

        FVector CameraLocation = GetCamera1P()->GetRelativeLocation();
        CameraLocation.Z       = DefaultCameraLocation.Z + (BobZ * CurrentAmplitudeZ);
        CameraLocation.Y       = DefaultCameraLocation.Y + (BobY * CurrentAmplitudeY);

        GetCamera1P()->SetRelativeLocation(CameraLocation);

        return;
    }

    // Smooth reset amplitudes
    CurrentAmplitudeZ = FMath::FInterpTo(CurrentAmplitudeZ, 0.f, DeltaTime, BobTransitionSpeed);
    CurrentAmplitudeY = FMath::FInterpTo(CurrentAmplitudeY, 0.f, DeltaTime, BobTransitionSpeed);

    // Smooth reset camera
    const auto TargetCameraLocation = FMath::VInterpTo(GetCamera1P()->GetRelativeLocation(),
                                                       DefaultCameraLocation,
                                                       DeltaTime,
                                                       BobTransitionSpeed);
    GetCamera1P()->SetRelativeLocation(TargetCameraLocation);
}

void ASeetheCharacter::Mesh1PSway(const float DeltaTime) {
    const APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || PC->IsLookInputIgnored()) { return; }

    FRotator TargetSway;
    TargetSway.Pitch = FMath::Clamp(LookAxisY * SwayAmount, -MaxSway, MaxSway);
    TargetSway.Yaw   = FMath::Clamp(LookAxisX * SwayAmount, -MaxSway, MaxSway);
    TargetSway.Roll  = TargetSway.Yaw * 0.4f;

    EquipSwayRotation = FMath::RInterpTo(EquipSwayRotation, TargetSway, DeltaTime, SwaySmoothing);

    LookAxisX = FMath::FInterpTo(LookAxisX, 0.0f, DeltaTime, SwayStopSmoothing);
    LookAxisY = FMath::FInterpTo(LookAxisY, 0.0f, DeltaTime, SwayStopSmoothing);
}

void ASeetheCharacter::Mesh1PAvoidClipping(const float DeltaTime) {
    const APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || PC->IsMoveInputIgnored()) { return; }

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
                                                       FCollisionShape::MakeSphere(10.0f),
                                                       Params);
    const float TargetDisplacement = bHit ? (End - WallHit.Location).Size() : 0.0f;
    DrawbackDisplacement           = FMath::FInterpTo(DrawbackDisplacement,
                                                      TargetDisplacement,
                                                      DeltaTime,
                                                      DrawbackSpeed);
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
            }
            return;
        }
    }

    if (CurrentInteractable) {
        CurrentInteractable->LookAway();
        CurrentInteractable = nullptr;
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
}

void ASeetheCharacter::UpdateIdleStatus(const float DeltaTime) {
    const FVector Velocity = GetVelocity();
    const bool bMoving     = Velocity.SizeSquared() > 10.0f;

    if (bMoving) {
        IdleTime        = 0.0f;
        bIsIdleInactive = false;
    } else {
        IdleTime += DeltaTime;

        if (!bIsIdleInactive && IdleTime >= IdleInactiveThreshold) {
            bIsIdleInactive = true;

            if (OnPlayerIdleInactive.IsBound()) {
                OnPlayerIdleInactive.Broadcast();
            }
        }
    }
}
#pragma endregion

#pragma region Gameplay
void ASeetheCharacter::Die() {
    // Drop all our inventory items in place
    if (HasEquippedItem()) {
        Drop();
    }

    InventoryComponent->ResetInventory();

    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    SetActorEnableCollision(false);

    if (APlayerController* PC = Cast<APlayerController>(GetController())) {
        PC->SetInputMode(FInputModeUIOnly {});
        // Show death screen
        if (const ASeetheGameMode* GM = Cast<ASeetheGameMode>(GetWorld()->GetAuthGameMode())) {
            GM->ShowDeathScreen(PC);
        }
    }

    GetWorldTimerManager().SetTimer(
        RespawnHandle,
        this,
        &ASeetheCharacter::Respawn,
        6.0f,
        false);
}

void ASeetheCharacter::HandleFootstep(const EFootstepType Type) {
    const UCapsuleComponent* Capsule = GetCapsuleComponent();
    const FVector CapsuleBottom      = GetActorLocation() - FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight());

    const FVector TraceStart = CapsuleBottom + FVector(0, 0, 10.0f); // Start slightly above floor
    const FVector TraceEnd = CapsuleBottom - FVector(0, 0, 20.f); // Check for 10 units beyond the bottom of our capsule

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.bReturnPhysicalMaterial = true;

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
    if (!bHit) { return; }

    const auto Surface = static_cast<EFootstepSurface>(Hit.PhysMaterial->SurfaceType.GetIntValue());
    UFootstepAudioData* AudioData {nullptr};
    if (const auto* Found = FootstepDataMap.Find(Surface)) {
        AudioData = Found->LoadSynchronous();
    }
    // Failed to load or didn't exist
    if (!AudioData) {
        AudioData = FootstepDataMap.Find(EFootstepSurface::Default)->LoadSynchronous();
    }
    // Failed to loud
    if (!AudioData) { return; }

    USoundBase* Sound = AudioData->GetSoundForType(Type);
    if (Sound) {
        UGameplayStatics::SpawnSoundAtLocation(this, Sound, Hit.Location);
    }
}

void ASeetheCharacter::Respawn() {
    AController* Saved = GetController();

    DetachFromControllerPendingDestroy();

    if (Saved) {
        if (AGameModeBase* GM = GetWorld()->GetAuthGameMode()) {
            GM->RestartPlayer(Saved);
        }
    }

    Destroy();
}

void ASeetheCharacter::Equip(const UInventoryItemEquipable* Item) {
    if (CurrentEquipable && CurrentEquipable->GetClass() == Item->EquipableClass) {
        return;
    }

    if (CurrentEquipable) {
        CurrentEquipable->SetActorHiddenInGame(true);
    }

    ABaseEquipable* TargetEquipable = CachedEquipables.FindRef(Item->EquipableClass);
    if (!TargetEquipable) {
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.Owner      = this;
        SpawnInfo.Instigator = GetInstigator();

        TargetEquipable = GetWorld()->SpawnActor<ABaseEquipable>(
            Item->EquipableClass,
            GetActorLocation(),
            GetActorRotation(),
            SpawnInfo);

        CachedEquipables.Add(Item->EquipableClass, TargetEquipable);
    }

    CurrentEquipable = TargetEquipable;
    CurrentEquipable->Equip(this);

    UpdateEquipable();

    if (const UHUDWidget* HUD = GetHUDWidget()) {
        FToastNotification Notification;
        Notification.Icon    = Item->ItemIcon;
        Notification.Message = FText::Format(NSLOCTEXT("UI", "Notification", "Equipped {0}"), Item->ItemName);
        HUD->PostToastNotification(Notification, 1.5f);
    }
}

void ASeetheCharacter::UnEquip() {
    if (!CurrentEquipable) { return; }
    CurrentEquipable->UnEquip(this);

    // Equipable state handled by ABaseEquipable
}

void ASeetheCharacter::Drop() {
    if (!CurrentEquipable) { return; }
    CachedEquipables.Remove(CurrentEquipable->GetClass());
    CurrentEquipable->Drop(this);

    // Equipable state handled by ABaseEquipable
}

void ASeetheCharacter::UseItem(const int32 Index, const EInventoryCategory& Category) const {
    if (Index < 0) { return; }

    if (auto* Inventory = GetInventory()) {
        Inventory->UseItem(Index, Category);
    }
}
#pragma endregion

#pragma region Delegate Handlers
void ASeetheCharacter::UpdateEquipable() {
    if (OnEquippedItemChanged.IsBound()) {
        OnEquippedItemChanged.Broadcast(CurrentEquipable);
    }
}

void ASeetheCharacter::UpdateHealth() {
    if (OnHealthChanged.IsBound()) {
        OnHealthChanged.Broadcast(CurrentHealth);
    }
}
#pragma endregion