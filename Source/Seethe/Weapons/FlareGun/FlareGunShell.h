// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "FlareGunShell.generated.h"

UCLASS()
class SEETHE_API AFlareGunShell : public AActor {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCapsuleComponent> CapsuleCollider;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> ShellMesh;

protected:
    virtual void BeginPlay() override;

public:
    AFlareGunShell();

    UStaticMeshComponent* GetMesh();

    UPROPERTY(EditAnywhere)
    float Lifetime {3.0f};

private:
    FTimerHandle SelfDestructTimer;

    UFUNCTION()
    void SelfDestruct();
};