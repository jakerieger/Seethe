// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "CrosshairWidget.h"
#include "CharacterLocomotionData.h"
#include "EquipMontageNotify.h"
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

    /** IEquipableInterface overrides **/
    virtual void Equip(ASeetheCharacter* Character) override;
    virtual void UnEquip(ASeetheCharacter* Character) override;
    virtual void Drop(ASeetheCharacter* Character) override;
    virtual void Use(ASeetheCharacter* Character) override {};
    virtual FString GetName() override { return {}; }
    void OnEquipNotify(EEquipMontageNotifyAction EquipAction);

    /** Properties **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform AttachOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName LeftHandSocket {"S_Grip_L"};

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UCharacterLocomotionData> Locomotion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<class UEquipableAnimationData> EquipAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UEquipableAnimationData> UnEquipAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UEquipableAnimationData> DropAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

    /** Helpers **/
    UFUNCTION(BlueprintPure)
    USkeletalMeshComponent* GetMesh1P() const;

    UFUNCTION(BlueprintPure)
    bool HasLeftHandSocket() const;

    UFUNCTION(BlueprintPure)
    FTransform GetLeftHandSocketTransform() const;

protected:
    virtual void OnEquipNotify_Equip();
    virtual void OnEquipNotify_UnEquip();
    virtual void OnEquipNotify_Drop();

    virtual void OnMontageEnded_Equip(UAnimMontage*,
                                      bool);
    virtual void OnMontageEnded_UnEquip(UAnimMontage*,
                                        bool);
    virtual void OnMontageEnded_Drop(UAnimMontage*,
                                     bool);
};