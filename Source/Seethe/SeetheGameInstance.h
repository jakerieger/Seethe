// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SeetheGameInstance.generated.h"

UCLASS()
class SEETHE_API USeetheGameInstance : public UGameInstance {
    GENERATED_BODY()

public:
    void SaveGame(const int32 Slot) const;
    void LoadGame(const int32 Slot) const;
    void ToggleDevToolsVisibility() const;

protected:
    virtual void OnStart() override;

    UPROPERTY(EditAnywhere)
    bool bEnableDevTools {false};

    UPROPERTY(EditAnywhere)
    TSubclassOf<class UDevToolsWidget> DevToolsWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UDevToolsWidget> DevToolsWidget;
};