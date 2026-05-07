// Copyright (C) 2026 Jake Rieger


#include "SeetheSaveGame.h"
#include "Kismet/KismetSystemLibrary.h"

USeetheSaveGame::USeetheSaveGame() {
    SaveSlotName = UKismetSystemLibrary::GetPlatformUserName();
}