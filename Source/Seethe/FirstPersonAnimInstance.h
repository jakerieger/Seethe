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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> DefaultIdleSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> DefaultWalkSequence;

    UFUNCTION(BlueprintCallable)
    void UpdateLocomotionSequences(UFirstPersonAnimData* AnimData);

    UFUNCTION(BlueprintCallable)
    void SetDefaultLocomotion();

    virtual void NativeBeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UAnimSequenceBase> CurrentIdleSequence;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UAnimSequenceBase> CurrentWalkSequence;
};