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

#include "Seethe.h"
#include "Weapons/BaseWeapon.h"
#include "BaseEquipable.h"
#include "DeathScreenWidget.h"
#include "FirstPersonAnimInstance.h"
#include "SeetheGameMode.h"
#include "Inventory/InventoryComponent.h"
#include "UI/HUDBase.h"
#include "UI/HUDWidget.h"
#include "UI/InventoryWidget.h"
#include "CharacterInputData.h"

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

void ASeetheCharacter::BeginPlay() {
    Super::BeginPlay();

    DefaultCameraLocation = FirstPersonCamera->GetRelativeLocation();
    CurrentHealth         = 100;

    if (GetInventoryWidget()) {
        GetInventoryWidget()->InitializeWidget(GetInventory());
    }

    if (GetHUDWidget()) {
        GetHUDWidget()->UpdateHealth(GetHealthPercent());
        GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::Dead);
    }
}

void ASeetheCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    Super::EndPlay(EndPlayReason);
}

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

void ASeetheCharacter::Tick(const float DeltaTime) {
    Super::Tick(DeltaTime);

    Mesh1PSway(DeltaTime);
    Mesh1PAvoidClipping(DeltaTime);
    CameraBob(DeltaTime);

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

        if (GetInventoryWidget()) {
            GetInventoryWidget()->InitializeWidget(GetInventory());
        }

        if (GetHUDWidget()) {
            GetHUDWidget()->UpdateHealth(GetHealthPercent());
            GetHUDWidget()->UpdateBatteryChargeState(EBatteryChargeState::Dead);
        }
    }
}

USkeletalMeshComponent* ASeetheCharacter::GetMesh1P() const { return FirstPersonArms; }
UCameraComponent* ASeetheCharacter::GetCamera1P() const { return FirstPersonCamera; }
UInventoryComponent* ASeetheCharacter::GetInventory() const { return InventoryComponent; }
ABaseEquipable* ASeetheCharacter::GetCurrentEquipable() const { return CurrentEquipable; }
ABaseWeapon* ASeetheCharacter::GetCurrentWeapon() const { return Cast<ABaseWeapon>(CurrentEquipable); }
bool ASeetheCharacter::HasEquippedItem() const { return CurrentEquipable != nullptr; }
float ASeetheCharacter::GetHealthPercent() const { return CurrentHealth / 100.0f; }
bool ASeetheCharacter::IsSprinting() const { return bSprinting; }
bool ASeetheCharacter::IsGrounded() const { return !GetMovementComponent()->IsFalling(); }
bool ASeetheCharacter::IsFalling() const { return FMath::IsNegative(GetVelocity().Z); }
float ASeetheCharacter::GetWalkSpeed() const { return WalkSpeed; }
float ASeetheCharacter::GetSprintSpeed() const { return SprintSpeed; }

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

UFirstPersonAnimInstance* ASeetheCharacter::GetAnimInstance1P() const {
    return Cast<UFirstPersonAnimInstance>(GetMesh1P()->GetAnimInstance());
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

float ASeetheCharacter::TakeDamage(const float DamageAmount,
                                   const FDamageEvent& DamageEvent,
                                   AController* EventInstigator,
                                   AActor* DamageCauser) {
    const float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= FMath::Floor(DamageToApply);
    if (GetHUDWidget()) {
        GetHUDWidget()->UpdateHealth(CurrentHealth / 100.0f);
    }

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

void ASeetheCharacter::Equip(const UInventoryItemEquipable* Item) {
    if (CurrentEquipable && CurrentEquipable->GetClass() == Item->EquipableClass) {
        return;
    }

    if (CurrentEquipable) {
        CurrentEquipable->SetActorHiddenInGame(true);
        GetHUDWidget()->HideCrosshair();
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

    if (CurrentEquipable->CrosshairTexture) {
        GetHUDWidget()
            ->SetCrosshairTexture(CurrentEquipable->CrosshairTexture)
            ->ShowCrosshair();
    }

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
    CurrentEquipable = nullptr;

    if (UHUDWidget* HUD = GetHUDWidget()) {
        HUD->HideCrosshair();
    }
}

void ASeetheCharacter::Drop() {
    if (!CurrentEquipable) { return; }

    CachedEquipables.Remove(CurrentEquipable->GetClass());

    CurrentEquipable->Drop(this);
    CurrentEquipable = nullptr;

    GetHUDWidget()->HideCrosshair();
}

void ASeetheCharacter::UseItem(const int32 Index, const EInventoryCategory& Category) {
    if (Index < 0) { return; }
    GetInventory()->UseItem(Index, Category);
}

IEquipableInterface* ASeetheCharacter::GetEquipableInterface() const {
    return Cast<IEquipableInterface>(CurrentEquipable);
}

IWeaponInterface* ASeetheCharacter::GetWeaponInterface() const {
    return Cast<IWeaponInterface>(CurrentEquipable);
}

void ASeetheCharacter::Mesh1PSway(const float DeltaTime) {
    const APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || PC->IsLookInputIgnored()) { return; }

    FRotator TargetSway;
    TargetSway.Pitch = FMath::Clamp(LookAxisY * SwayAmount, -MaxSway, MaxSway);
    TargetSway.Yaw   = FMath::Clamp(LookAxisX * SwayAmount, -MaxSway, MaxSway);
    TargetSway.Roll  = TargetSway.Yaw * 0.4f;

    EquipSwayRotation = FMath::RInterpTo(EquipSwayRotation, TargetSway, DeltaTime, SwaySmoothing);

    LookAxisX = FMath::FInterpTo(LookAxisX, 0.0f, DeltaTime, 10.0f);
    LookAxisY = FMath::FInterpTo(LookAxisY, 0.0f, DeltaTime, 10.0f);
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

                GetHUDWidget()->SetCrosshairColor(FColor::Yellow);
            }
            return;
        }
    }

    if (CurrentInteractable) {
        CurrentInteractable->LookAway();
        CurrentInteractable = nullptr;

        GetHUDWidget()->SetCrosshairColor(FColor::White);
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

    GetHUDWidget()->SetCrosshairColor(bHit && Hit.GetActor() ? FColor::Red : FColor::White);
}