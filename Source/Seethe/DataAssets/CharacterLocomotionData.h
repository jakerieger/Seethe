// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterLocomotionData.generated.h"

UCLASS()
class SEETHE_API UCharacterLocomotionData : public UDataAsset {
    GENERATED_BODY()

public:
    /** The idle sequence to play when the character first stops moving (usually no animation at all). **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> IdleSequence;

    /** The idle sequence to play after a period of inactivity from the player. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> IdleSequenceInactive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> WalkSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> SprintSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> JumpSequence_Start;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> JumpSequence_Falling;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimSequenceBase> JumpSequence_End;
};