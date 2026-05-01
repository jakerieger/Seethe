// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "FirstPersonAnimData.h"
#include "Animation/AnimInstance.h"
#include "FirstPersonAnimInstance.generated.h"

UCLASS()
class SEETHE_API UFirstPersonAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    /** Set locomotion sequences. **/
    UFUNCTION(BlueprintCallable)
    void UpdateLocomotionSequences(UFirstPersonAnimData* AnimData);

    /** Reset locomotion sequences to character default. **/
    UFUNCTION(BlueprintCallable)
    void SetDefaultLocomotion();

    virtual void NativeBeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> DefaultIdleSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> DefaultWalkSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> DefaultSprintSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentIdleSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentWalkSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentSprintSequence;

private:
    void TransitionToNewLocomotion();
};