// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Seethe/Interfaces/EquipableInterface.h"
#include "BaseEquipable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipped, ABaseEquipable*, Equipable);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnEquipped, ABaseEquipable*, Equipable);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDropped, ABaseEquipable*, Equipable);

UCLASS()
class SEETHE_API ABaseEquipable : public AActor, public IEquipableInterface {
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> Mesh1P;

public:
    ABaseEquipable();

    /** IEquipableInterface overrides **/
    virtual void Equip(ASeetheCharacter* Character) override;
    virtual void UnEquip(ASeetheCharacter* Character) override;
    virtual void Drop(ASeetheCharacter* Character) override;
    virtual void Use(ASeetheCharacter* Character) override {};

    /** Delegates **/
    UPROPERTY(BlueprintAssignable)
    FOnEquipped OnEquipped;

    UPROPERTY(BlueprintAssignable)
    FOnUnEquipped OnUnEquipped;

    UPROPERTY(BlueprintAssignable)
    FOnDropped OnDropped;

    /** Properties **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipable")
    FTransform AttachOffset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<UAnimMontage> EquipMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<UAnimMontage> UnEquipMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<UAnimMontage> DropMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TSubclassOf<UAnimInstance> EquipableAnimLayer;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
    TObjectPtr<UTexture2D> CrosshairTexture;

    /** Helpers **/
    UFUNCTION(BlueprintPure, Category="Equipable")
    USkeletalMeshComponent* GetMesh1P() const;

protected:
    UFUNCTION()
    void OnEquipMontageEnded(UAnimMontage* InMontage,
                             bool bInterrupted);

    UFUNCTION()
    void OnUnEquipMontageEnded(UAnimMontage* InMontage,
                               bool bInterrupted,
                               UAnimInstance* InAnimInstance);

    UFUNCTION()
    void OnDropMontageEnded(UAnimMontage* InMontage,
                            bool bInterrupted,
                            UAnimInstance* InAnimInstance);
};