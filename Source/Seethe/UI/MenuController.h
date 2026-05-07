// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "MainMenuWidget.h"
#include "GameFramework/PlayerController.h"
#include "MenuController.generated.h"

UCLASS()
class SEETHE_API AMenuController : public APlayerController {
    GENERATED_BODY()

public:
    AMenuController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UMainMenuWidget> MainMenuClass;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<UMainMenuWidget> MainMenuWidget;
};