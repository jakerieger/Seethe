// Copyright (C) 2026 Jake Rieger


#include "InventoryItem3dPreview.h"
#include "Components/SceneCaptureComponent2D.h"


AInventoryItem3dPreview::AInventoryItem3dPreview() {
    PrimaryActorTick.bCanEverTick = true;

    SceneRoot     = CreateDefaultSubobject<USceneComponent>("SceneRoot");
    RootComponent = SceneRoot;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
    ItemMesh->SetupAttachment(SceneRoot);

    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapture");
    SceneCapture->SetupAttachment(SceneRoot);

    SceneLight = CreateDefaultSubobject<URectLightComponent>("SceneLight");
    SceneLight->SetupAttachment(SceneRoot);
}

void AInventoryItem3dPreview::Tick(const float DeltaTime) {
    Super::Tick(DeltaTime);

    if (ItemMesh) {
        const float RotYaw = RotateSpeed * DeltaTime;
        ItemMesh->AddLocalRotation(FRotator(0.0f, RotYaw, 0.0f));
    }
}

void AInventoryItem3dPreview::BeginPlay() {
    Super::BeginPlay();

    if (ItemMesh) {
        SceneCapture->ShowOnlyComponent(ItemMesh);
    }
}

void AInventoryItem3dPreview::SetupPreview(UStaticMesh* Mesh, const FTransform& PreviewTransform) const {
    if (ItemMesh) {
        ItemMesh->SetStaticMesh(Mesh);
        ItemMesh->SetRelativeTransform(PreviewTransform);
        ItemMesh->SetVisibility(true);
    } else {
        ItemMesh->SetStaticMesh(nullptr);
        ItemMesh->SetVisibility(false);
    }
}

void AInventoryItem3dPreview::SetVisible(const bool bVisible) const {
    SceneRoot->SetVisibility(bVisible);
}

USceneCaptureComponent2D* AInventoryItem3dPreview::GetSceneCapture() const {
    return SceneCapture;
}