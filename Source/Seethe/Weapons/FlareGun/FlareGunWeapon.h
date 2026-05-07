// Copyright (C) 2026 Jake Rieger

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "FlareGunProjectile.h"
#include "FlareGunShell.h"
#include "LocomotionState.h"
#include "FlareGunWeapon.generated.h"

UCLASS()
class SEETHE_API AFlareGunWeapon : public ABaseWeapon {
    GENERATED_BODY()

public:
    AFlareGunWeapon();
    virtual void Attack(ASeetheCharacter* Character) override;
    virtual void Reload(ASeetheCharacter* Character) override;
    virtual bool CanAttack() override;
    virtual FString GetName() override { return "Flare Gun"; }
    virtual int32 GetCurrentAmmo() override { return bLoaded; }
    virtual int32 GetTotalAmmo() override;
    virtual void Equip(ASeetheCharacter* Character) override;
    void SetActionCocked(bool bCocked);

    /** Spawns a shell actor at the location of the animated shell during reload. **/
    void SpawnEjectedShellActor();

    UPROPERTY(EditAnywhere)
    FName MuzzleSocketName {"S_Muzzle"};

    UPROPERTY(EditAnywhere)
    TSubclassOf<AFlareGunShell> ShellClass;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AFlareGunProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UEquipableAnimationData> DryFireAnimations;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UEquipableAnimationData> SimpleEquipAnimations;

    UPROPERTY(EditAnywhere)
    TMap<ELocomotionState, FFloatInterval> SpreadRanges_Y;

    UPROPERTY(EditAnywhere)
    TMap<ELocomotionState, FFloatInterval> SpreadRanges_Z;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bLoaded {false};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bActionCocked {false};

private:
    struct FSpreadRanges {
        FFloatInterval RangeZ;
        FFloatInterval RangeY;
        FSpreadRanges() = default;
        FSpreadRanges(const FFloatInterval InZ, const FFloatInterval InY) : RangeZ(InZ), RangeY(InY) {}
    };

    FSpreadRanges GetSpreadRanges(ELocomotionState State) const;
    static FRotator CalculateSpread(const FRotator& BaseRotation,
                                    const FFloatInterval RangeZ,
                                    const FFloatInterval RangeY);
};