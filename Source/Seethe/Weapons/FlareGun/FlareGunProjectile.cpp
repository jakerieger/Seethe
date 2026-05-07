// Copyright (C) 2026 Jake Rieger


#include "FlareGunProjectile.h"

#include "Kismet/GameplayStatics.h"

AFlareGunProjectile::AFlareGunProjectile() {
    PrimaryActorTick.bCanEverTick = true;

    ImpactCollider = CreateDefaultSubobject<USphereComponent>("ImpactCollider");
    ImpactCollider->SetSphereRadius(8.0f);
    RootComponent = ImpactCollider;

    TrailCollider = CreateDefaultSubobject<UCapsuleComponent>("TrailCollider");
    TrailCollider->SetupAttachment(ImpactCollider);

    FlareMesh = CreateDefaultSubobject<UStaticMeshComponent>("FlareMesh");
    FlareMesh->SetupAttachment(ImpactCollider);

    FlareLight = CreateDefaultSubobject<UPointLightComponent>("FlareLight");
    FlareLight->SetupAttachment(ImpactCollider);

    ProjectileMovement  = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
    BrightnessTimeline  = CreateDefaultSubobject<UTimelineComponent>("BrightnessTimeline");
    AttenuationTimeline = CreateDefaultSubobject<UTimelineComponent>("AttenuationTimeline");
    SpeedTimeline       = CreateDefaultSubobject<UTimelineComponent>("SpeedTimeline");
    GravityTimeline     = CreateDefaultSubobject<UTimelineComponent>("GravityTimeline");
}

void AFlareGunProjectile::BeginPlay() {
    Super::BeginPlay();

    if (BrightnessCurve) {
        BrightnessUpdateDelegate.BindUFunction(this, FName("BrightnessTimelineUpdate"));
        BrightnessFinishedDelegate.BindUFunction(this, FName("BrightnessTimelineFinished"));
        BrightnessTimeline->AddInterpFloat(BrightnessCurve, BrightnessUpdateDelegate);
        BrightnessTimeline->SetTimelineFinishedFunc(BrightnessFinishedDelegate);
        BrightnessTimeline->PlayFromStart();
    }

    if (AttenuationCurve) {
        AttenuationUpdateDelegate.BindUFunction(this, FName("AttenuationTimelineUpdate"));
        AttenuationTimeline->AddInterpFloat(AttenuationCurve, AttenuationUpdateDelegate);
        AttenuationTimeline->PlayFromStart();
    }

    if (SpeedCurve) {
        SpeedUpdateDelegate.BindUFunction(this, FName("SpeedTimelineUpdate"));
        SpeedTimeline->AddInterpFloat(SpeedCurve, SpeedUpdateDelegate);
        SpeedTimeline->PlayFromStart();
    }

    if (GravityCurve) {
        GravityUpdateDelegate.BindUFunction(this, FName("GravityTimelineUpdate"));
        GravityTimeline->AddInterpFloat(GravityCurve, GravityUpdateDelegate);
        GravityTimeline->PlayFromStart();
    }

    ImpactCollider->OnComponentHit.AddDynamic(this, &AFlareGunProjectile::OnProjectileHit);

    TrailCollider->OnComponentBeginOverlap.AddDynamic(this, &AFlareGunProjectile::OnTrailBeginOverlap);
    TrailCollider->OnComponentEndOverlap.AddDynamic(this, &AFlareGunProjectile::OnTrailEndOverlap);

    ProjectileMovement->Velocity = GetActorForwardVector() * InitialSpeed;
}

void AFlareGunProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    if (!TickDamageMap.IsEmpty()) {
        for (auto [Pawn, TimerHandle] : TickDamageMap) {
            GetWorldTimerManager().ClearTimer(TimerHandle);
            Pawn = nullptr;
        }

        TickDamageMap.Empty();
    }

    Super::EndPlay(EndPlayReason);
}

void AFlareGunProjectile::BrightnessTimelineUpdate(const float Brightness) const {
    FlareLight->SetIntensity(Brightness);
}

void AFlareGunProjectile::AttenuationTimelineUpdate(const float Radius) const {
    FlareLight->SetAttenuationRadius(Radius);
}

void AFlareGunProjectile::SpeedTimelineUpdate(const float SpeedDelta) const {
    // SpeedDelta is normalized [0, 1]
    const float CurrentSpeed     = FMath::Lerp(InitialSpeed, MaxSpeed, SpeedDelta);
    ProjectileMovement->MaxSpeed = CurrentSpeed;
    const FVector NewVelocity    = ProjectileMovement->Velocity.GetSafeNormal() * CurrentSpeed;
    ProjectileMovement->Velocity = NewVelocity;
    ProjectileMovement->UpdateComponentVelocity();
}

void AFlareGunProjectile::GravityTimelineUpdate(const float GravityScale) const {
    ProjectileMovement->ProjectileGravityScale = GravityScale;
    ProjectileMovement->UpdateComponentVelocity();
}

void AFlareGunProjectile::BrightnessTimelineFinished() {
    SelfDestruct();
}

void AFlareGunProjectile::SelfDestruct() {
    Destroy();
}

void AFlareGunProjectile::OnProjectileHit(UPrimitiveComponent* HitComp,
                                          AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp,
                                          FVector NormalImpulse,
                                          const FHitResult& Hit) {
    if (OtherActor && OtherActor->IsA<APawn>()) {
        UGameplayStatics::ApplyPointDamage(
            OtherActor,
            HitDamage,
            -Hit.ImpactNormal,
            Hit,
            GetInstigatorController(),
            this,
            UDamageType::StaticClass());
    }
}

void AFlareGunProjectile::OnTrailBeginOverlap(UPrimitiveComponent* OverlappedComp,
                                              AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp,
                                              int32 OtherBodyIndex,
                                              bool bFromSweep,
                                              const FHitResult& SweepResult) {
    if (OtherActor && OtherActor->IsA<APawn>()) {
        APawn* Pawn = Cast<APawn>(OtherActor);
        if (!TickDamageMap.Contains(Pawn)) {
            auto& TimerHandle = TickDamageMap.Add(Pawn);
            FTimerDelegate TimerDelegate;
            TimerDelegate.BindUObject(this, &AFlareGunProjectile::ApplyTickDamage, Pawn);
            GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, TickRate, true);
        }
    }
}

void AFlareGunProjectile::OnTrailEndOverlap(UPrimitiveComponent* OverlappedComponent,
                                            AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex) {
    if (OtherActor && OtherActor->IsA<APawn>()) {
        const APawn* Pawn = Cast<APawn>(OtherActor);
        if (TickDamageMap.Contains(Pawn)) {
            FTimerHandle* TimerHandle = TickDamageMap.Find(Pawn);
            GetWorldTimerManager().ClearTimer(*TimerHandle);
            TickDamageMap.Remove(Pawn);
        }
    }
}

void AFlareGunProjectile::ApplyTickDamage(APawn* DamageReceiver) {
    if (!DamageReceiver) { return; }

    UGameplayStatics::ApplyDamage(DamageReceiver,
                                  TrailDamage,
                                  GetInstigatorController(),
                                  this,
                                  UDamageType::StaticClass());
}