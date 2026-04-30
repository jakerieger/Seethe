// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "FirstPersonAnimData.h"
#include "GameFramework/Actor.h"
#include "Seethe/Interfaces/EquipableInterface.h"
#include "BaseEquipable.generated.h"

UCLASS()
class SEETHE_API ABaseEquipable : public AActor, public IEquipableInterface {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> Mesh1P;

public:
    ABaseEquipable();
    virtual void Tick(float DeltaSeconds) override;

    /** IEquipableInterface overrides **/
    virtual void Equip(ASeetheCharacter* Character) override;
    virtual void UnEquip(ASeetheCharacter* Character) override;
    virtual void Drop(ASeetheCharacter* Character) override;
    virtual void Use(ASeetheCharacter* Character) override {};

    /** Properties **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipable")
    FTransform AttachOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipable")
    FName LeftHandSocket {"S_Grip_L"};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipable")
    TObjectPtr<UFirstPersonAnimData> AnimData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable")
    TObjectPtr<UAnimMontage> EquipMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable")
    TObjectPtr<UAnimMontage> UnEquipMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable")
    TObjectPtr<UAnimMontage> DropMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable")
    TObjectPtr<UTexture2D> CrosshairTexture;

    /** Helpers **/
    UFUNCTION(BlueprintPure, Category="Equipable")
    USkeletalMeshComponent* GetMesh1P() const;

    UFUNCTION(BlueprintPure, Category="Equipable")
    bool HasLeftHandSocket() const;

    UFUNCTION(BlueprintPure, Category="Equipable")
    FTransform GetLeftHandSocketTransform() const;

protected:
    UFUNCTION()
    void OnEquipMontageEnded(UAnimMontage* InMontage,
                             bool bInterrupted);

    UFUNCTION()
    void OnUnEquipMontageEnded(UAnimMontage* InMontage,
                               bool bInterrupted,
                               class UFirstPersonAnimInstance* InAnimInstance);

    UFUNCTION()
    void OnDropMontageEnded(UAnimMontage* InMontage,
                            bool bInterrupted,
                            UFirstPersonAnimInstance* InAnimInstance);
};