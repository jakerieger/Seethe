// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/RectLightComponent.h"
#include "InventoryItem3dPreview.generated.h"

UCLASS()
class SEETHE_API AInventoryItem3dPreview : public AActor {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> ItemMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneCaptureComponent2D> SceneCapture;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<URectLightComponent> SceneLight;

public:
    AInventoryItem3dPreview();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
    float RotateSpeed = 75.f;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void SetupPreview(UStaticMesh* Mesh, const FTransform& PreviewTransform) const;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void SetVisible(bool bVisible) const;

    UFUNCTION(BlueprintCallable, Category="Inventory")
    USceneCaptureComponent2D* GetSceneCapture() const;
};