// Copyright (C) 2026 Jake Rieger


#include "FootstepAudioData.h"

USoundBase* UFootstepAudioData::GetSoundForType(const EFootstepType Type) {
    switch (Type) {
        case EFootstepType::Walk:
            return WalkSound;
        case EFootstepType::Sprint:
            return RunSound;
        case EFootstepType::Jog:
            return JogSound;
        case EFootstepType::Jump:
            return JumpSound;
        case EFootstepType::Land:
            return LandSound;
    }

    return nullptr;
}