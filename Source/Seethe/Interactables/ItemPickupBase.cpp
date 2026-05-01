// Copyright (C) 2026 Jake Rieger


#include "ItemPickupBase.h"
#include "InventoryComponent.h"
#include "PickupAnimData.h"
#include "Seethe/SeetheCharacter.h"
#include "Seethe/UI/InteractPopupWidget.h"
#include "Seethe/UI/HUDWidget.h"

AItemPickupBase::AItemPickupBase() {
    PrimaryActorTick.bCanEverTick = false;

    ItemMesh      = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
    RootComponent = ItemMesh;
    ItemMesh->SetRenderCustomDepth(false);
    ItemMesh->SetCustomDepthStencilValue(2);

    InteractWidget = CreateDefaultSubobject<UWidgetComponent>("InteractWidget");
    InteractWidget->SetVisibility(false);
    InteractWidget->SetupAttachment(ItemMesh);
}

bool AItemPickupBase::Interact(ASeetheCharacter* Character) {
    if (Character) {
        UAnimMontage* PickupMontage = AnimData->GetPickupMontage(Character->HasEquippedItem());

        if (PickupMontage) {
            if (USkeletalMeshComponent* Arms = Character->GetMesh1P()) {
                if (UAnimInstance* Anim = Arms->GetAnimInstance()) {
                    const auto Duration = Anim->Montage_Play(PickupMontage);

                    if (Duration > 0.0f) {
                        FOnMontageEnded EndDelegate;
                        EndDelegate.BindUObject(this, &AItemPickupBase::OnPickupMontageEnded, Character);
                        Anim->Montage_SetEndDelegate(EndDelegate, PickupMontage);

                        return true;
                    }
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
    InteractWidget->SetVisibility(true);
}

void AItemPickupBase::HideInteractWidget() const {
    InteractWidget->SetVisibility(false);
}

void AItemPickupBase::BeginPlay() {
    Super::BeginPlay();

    const UInteractPopupWidget* InteractPopupWidget = Cast<UInteractPopupWidget>(InteractWidget->GetWidget());
    if (InteractPopupWidget) {
        InteractPopupWidget->SetupPrompt(InteractIcon, GetInteractMessage());
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