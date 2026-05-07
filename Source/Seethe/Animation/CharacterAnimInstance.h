// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "CharacterLocomotionData.h"
#include "LocomotionState.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

UCLASS()
class SEETHE_API UCharacterAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    /** Read-only, updated on game thread **/
    UPROPERTY(BlueprintReadOnly)
    ELocomotionState LocomotionState {ELocomotionState::Idle};

    /** Called by the ABP via OnStateEntry **/
    UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
    void StageLocomotionState(const ELocomotionState InState) { PendingLocomotionState = InState; }

    /** Set locomotion sequences. **/
    UFUNCTION(BlueprintCallable)
    void UpdateLocomotionSequences(UCharacterLocomotionData* AnimData);

    /** Reset locomotion sequences to character default. **/
    UFUNCTION(BlueprintCallable)
    void SetDefaultLocomotion();

    virtual void NativeBeginPlay() override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UCharacterLocomotionData> DefaultLocomotion;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentIdleSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentIdleSequenceInactive;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentWalkSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentSprintSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentJumpSequence_Start;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentJumpSequence_Falling;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> CurrentJumpSequence_End;

private:
    void TransitionToNewLocomotion();

    ELocomotionState PendingLocomotionState {ELocomotionState::Idle};
};