// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FootstepAudioData.generated.h"

UENUM(BlueprintType)
enum class EFootstepType : uint8 {
    Jog,
    Jump,
    Land,
    Sprint,
    Walk,
};

UCLASS()
class SEETHE_API UFootstepAudioData : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> JogSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> JogStopSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> JumpSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> LandSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> RunSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> RunStopSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> WalkSound;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USoundBase> WalkStopSound;

    USoundBase* GetSoundForType(EFootstepType Type);
};