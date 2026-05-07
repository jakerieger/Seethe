// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ActionInputIconData.generated.h"

UCLASS()
class SEETHE_API UActionInputIconData : public UDataAsset {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> IconMouseKeyboard;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> IconGamepad;
};