// Copyright (C) 2026 Jake Rieger


#include "CharacterAnimInstance.h"

#include "SeetheCharacter.h"

void UCharacterAnimInstance::UpdateLocomotionSequences(UCharacterLocomotionData* AnimData) {
    TransitionToNewLocomotion();
    CurrentIdleSequence         = AnimData->IdleSequence;
    CurrentIdleSequenceInactive = AnimData->IdleSequenceInactive;
    CurrentWalkSequence         = AnimData->WalkSequence;
    CurrentSprintSequence       = AnimData->SprintSequence;
    CurrentJumpSequence_Start   = AnimData->JumpSequence_Start;
    CurrentJumpSequence_Falling = AnimData->JumpSequence_Falling;
    CurrentJumpSequence_End     = AnimData->JumpSequence_End;
}

void UCharacterAnimInstance::SetDefaultLocomotion() {
    TransitionToNewLocomotion();
    CurrentIdleSequence         = DefaultLocomotion->IdleSequence;
    CurrentIdleSequenceInactive = DefaultLocomotion->IdleSequenceInactive;
    CurrentWalkSequence         = DefaultLocomotion->WalkSequence;
    CurrentSprintSequence       = DefaultLocomotion->SprintSequence;
    CurrentJumpSequence_Start   = DefaultLocomotion->JumpSequence_Start;
    CurrentJumpSequence_Falling = DefaultLocomotion->JumpSequence_Falling;
    CurrentJumpSequence_End     = DefaultLocomotion->JumpSequence_End;
}

void UCharacterAnimInstance::NativeBeginPlay() {
    Super::NativeBeginPlay();
    SetDefaultLocomotion();
}

void UCharacterAnimInstance::NativeThreadSafeUpdateAnimation(const float DeltaSeconds) {
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
    LocomotionState = PendingLocomotionState;
}

void UCharacterAnimInstance::NativeUpdateAnimation(const float DeltaSeconds) {
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (ASeetheCharacter* Character = Cast<ASeetheCharacter>(TryGetPawnOwner())) {
        Character->SetLocomotionState(LocomotionState);
    }
}

void UCharacterAnimInstance::TransitionToNewLocomotion() {
    StopAllMontages(0.0f);
}