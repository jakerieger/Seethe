// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/EquipableInterface.h"
#include "BaseEquipable.generated.h"

UCLASS()
class SEETHE_API ABaseEquipable : public AActor, public IEquipableInterface {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> Mesh1P;

public:
    ABaseEquipable();

    /** IEquipableInterface overrides **/
    virtual void Equip(ASeetheCharacter* Character) override;
    virtual void Drop(ASeetheCharacter* Character) override;
    virtual void Use(ASeetheCharacter* Character) override {};

    /** Properties **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipable")
    FTransform AttachOffset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<USoundBase> EquipSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<USoundBase> DropSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<UAnimMontage> EquipMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<UAnimMontage> DropMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TSubclassOf<UAnimInstance> EquipableAnimLayer;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<UTexture2D> CrosshairTexture;

    /** Helpers **/
    UFUNCTION(BlueprintPure, Category="Equipable")
    USkeletalMeshComponent* GetMesh1P() const;
};