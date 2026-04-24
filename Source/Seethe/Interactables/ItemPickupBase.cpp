// Copyright (C) 2026 Jake Rieger


#include "ItemPickupBase.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../SeetheCharacter.h"
#include "Seethe/UI/InteractPopupWidget.h"

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

void AItemPickupBase::Interact(ASeetheCharacter* Character) {
    if (Character) {
        Character->GetInventory()->AddItem(InventoryItem);
        if (EquipSound) {
            UGameplayStatics::PlaySoundAtLocation(this, EquipSound, Character->GetActorLocation());
        }
        Destroy();
    }
}

void AItemPickupBase::LookAt() {
    ItemMesh->SetRenderCustomDepth(true);
    ShowInteractWidget();
}

void AItemPickupBase::LookAway() {
    ItemMesh->SetRenderCustomDepth(false);
    HideInteractWidget();
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