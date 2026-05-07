// Copyright (C) 2026 Jake Rieger


#include "ItemPickupBase.h"

#include "CharacterAnimInstance.h"
#include "InventoryComponent.h"
#include "PickupAnimData.h"
#include "Seethe.h"
#include "Seethe/SeetheCharacter.h"
#include "Seethe/UI/PickupInteractWidget.h"
#include "Seethe/UI/HUDWidget.h"

AItemPickupBase::AItemPickupBase() {
    PrimaryActorTick.bCanEverTick = false;

    ItemMesh      = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
    RootComponent = ItemMesh;
    ItemMesh->SetupAttachment(InteractCollider);
    ItemMesh->SetRenderCustomDepth(false);
    ItemMesh->SetCustomDepthStencilValue(2);
    ItemMesh->CanCharacterStepUpOn = ECB_No;
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetCollisionObjectType(ECC_PhysicsBody);
    ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
    ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
    ItemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    ItemMesh->SetCollisionResponseToChannel(ECC_ENEMY, ECR_Ignore);
    ItemMesh->SetCollisionResponseToChannel(ECC_NIAGARA_ONLY, ECR_Ignore);

    InteractCollider = CreateDefaultSubobject<USphereComponent>("InteractCollider");
    InteractCollider->SetupAttachment(ItemMesh);
    InteractCollider->CanCharacterStepUpOn = ECB_No;
    InteractCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractCollider->SetCollisionObjectType(ECC_WorldDynamic);
    InteractCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    PickupWidget = CreateDefaultSubobject<UWidgetComponent>("InteractWidget");
    PickupWidget->SetVisibility(false);
    PickupWidget->SetupAttachment(ItemMesh);
}

bool AItemPickupBase::Interact(ASeetheCharacter* Character) {
    if (Character) {
        UAnimMontage* PickupMontage = AnimData->GetPickupMontage(Character->HasEquippedItem());

        if (PickupMontage) {
            if (auto* AI = Character->GetAnimInstance1P()) {
                if (const auto Duration = AI->Montage_Play(PickupMontage); Duration > 0.0f) {
                    FOnMontageEnded EndDelegate;
                    EndDelegate.BindUObject(this, &AItemPickupBase::OnPickupMontageEnded, Character);
                    AI->Montage_SetEndDelegate(EndDelegate, PickupMontage);

                    return true;
                }
            }
        }
    }

    return false;
}

void AItemPickupBase::LookAt() {
    ItemMesh->SetRenderCustomDepth(true);
    ShowInteractWidget();
}

void AItemPickupBase::LookAway() {
    ItemMesh->SetRenderCustomDepth(false);
    HideInteractWidget();
}

FText AItemPickupBase::GetInteractMessage() {
    const auto MsgTxt = FText::Format(
        NSLOCTEXT("UI", "World", "Equip {0} (x{1})"),
        InventoryItem->ItemName,
        InventoryItem->Quantity);
    return MsgTxt;
}

UStaticMeshComponent* AItemPickupBase::GetMesh() const {
    return ItemMesh;
}

void AItemPickupBase::ShowInteractWidget() const {
    PickupWidget->SetVisibility(true);
}

void AItemPickupBase::HideInteractWidget() const {
    PickupWidget->SetVisibility(false);
}

void AItemPickupBase::BeginPlay() {
    Super::BeginPlay();

    const UPickupInteractWidget* InteractPopupWidget = Cast<UPickupInteractWidget>(PickupWidget->GetWidget());
    if (InteractPopupWidget) {
        InteractPopupWidget->SetItem(InventoryItem);
    }
}

void AItemPickupBase::OnPickupMontageEnded(UAnimMontage* Montage, bool bInterrupted, ASeetheCharacter* Character) {
    Character->GetInventory()->AddItem(InventoryItem);
    OnItemPickedUp(Character);

    if (UHUDWidget* HUD = Character->GetHUDWidget()) {
        FConfirmNotification Notification;
        Notification.Icon    = InventoryItem->ItemIcon;
        Notification.Title   = InventoryItem->ItemName;
        Notification.Message = FText::Format(NSLOCTEXT("UI", "Notification", "x{0}"), InventoryItem->Quantity);
        HUD->PostConfirmNotification(Notification);

        FToastNotification ToastNotification;
        ToastNotification.Icon    = InventoryItem->ItemIcon;
        ToastNotification.Message = FText::Format(
            NSLOCTEXT("UI", "Notification", "Picked up {0} (x{1})"),
            InventoryItem->ItemName,
            InventoryItem->Quantity);
        HUD->PostToastNotification(ToastNotification, 1.5f);
    }

    Destroy();
}