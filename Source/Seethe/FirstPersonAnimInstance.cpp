// Copyright (C) 2026 Jake Rieger


#include "FirstPersonAnimInstance.h"

void UFirstPersonAnimInstance::UpdateLocomotionSequences(UFirstPersonAnimData* AnimData) {
    TransitionToNewLocomotion();
    CurrentIdleSequence   = AnimData->IdleSequence;
    CurrentWalkSequence   = AnimData->WalkSequence;
    CurrentSprintSequence = AnimData->SprintSequence;
}

void UFirstPersonAnimInstance::SetDefaultLocomotion() {
    TransitionToNewLocomotion();
    CurrentIdleSequence   = DefaultIdleSequence;
    CurrentWalkSequence   = DefaultWalkSequence;
    CurrentSprintSequence = DefaultSprintSequence;
}

void UFirstPersonAnimInstance::NativeBeginPlay() {
    Super::NativeBeginPlay();
    SetDefaultLocomotion();
}

void UFirstPersonAnimInstance::TransitionToNewLocomotion() {
    StopAllMontages(0.2f);
}