// Copyright (C) 2026 Jake Rieger


#include "FirstPersonAnimInstance.h"

void UFirstPersonAnimInstance::UpdateLocomotionSequences(UFirstPersonAnimData* AnimData) {
    CurrentIdleSequence = AnimData->IdleSequence;
    CurrentWalkSequence = AnimData->WalkSequence;
}

void UFirstPersonAnimInstance::SetDefaultLocomotion() {
    CurrentIdleSequence = DefaultIdleSequence;
    CurrentWalkSequence = DefaultWalkSequence;
}

void UFirstPersonAnimInstance::NativeBeginPlay() {
    Super::NativeBeginPlay();
    SetDefaultLocomotion();
}