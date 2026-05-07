// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EquipableAnimationData.generated.h"

USTRUCT(BlueprintType)
struct FEquipableAnimationPlayResult {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float EquipableMontageDuration {0.0f};

    UPROPERTY(BlueprintReadOnly)
    float CharacterMontageDuration {0.0f};

    UPROPERTY(BlueprintReadOnly)
    bool bPlayedEquipableMontage {false};

    UPROPERTY(BlueprintReadOnly)
    bool bPlayedCharacterMontage {false};

    FEquipableAnimationPlayResult() = default;

    FEquipableAnimationPlayResult(const float InEquipableMontageDuration,
                                  const float InCharacterMontageDuration,
                                  const bool bInPlayedEquipableMontage,
                                  const bool bInPlayedCharacterMontage)
        : EquipableMontageDuration(InEquipableMontageDuration),
          CharacterMontageDuration(InCharacterMontageDuration),
          bPlayedEquipableMontage(bInPlayedEquipableMontage),
          bPlayedCharacterMontage(bInPlayedCharacterMontage) {}
};

UCLASS()
class SEETHE_API UEquipableAnimationData : public UDataAsset {
    GENERATED_BODY()

public:
    /** The animation to play on the weapon/tool mesh. **/
    UPROPERTY(EditAnywhere)
    TObjectPtr<UAnimMontage> EquipableMontage;

    /** The animation to play on the character mesh (arms). **/
    UPROPERTY(EditAnywhere)
    TObjectPtr<UAnimMontage> CharacterMontage;

    FEquipableAnimationPlayResult PlayAnimations(UAnimInstance* CharacterAI, UAnimInstance* EquipableAI) const;
};