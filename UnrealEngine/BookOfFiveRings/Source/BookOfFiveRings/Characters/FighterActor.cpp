#include "FighterActor.h"
#include "Animation/SwordplayComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"

AFighterActor::AFighterActor()
{
    PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);

    // ── Body (capsule stand-in) ───────────────────────────────────────────────
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(RootScene);
    BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    BodyMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.15f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded()) BodyMesh->SetStaticMesh(SphereMesh.Object);

    // ── Sword blade ───────────────────────────────────────────────────────────
    SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
    SwordMesh->SetupAttachment(RootScene);
    SwordMesh->SetRelativeLocation(FVector(55.f, 0.f, 125.f));
    SwordMesh->SetRelativeRotation(FRotator(0.f, 0.f, 20.f));
    SwordMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, 1.25f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
        TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        SwordMesh->SetStaticMesh(CubeMesh.Object);

        // ── Sword guard ───────────────────────────────────────────────────────
        GuardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GuardMesh"));
        GuardMesh->SetupAttachment(SwordMesh);
        GuardMesh->SetStaticMesh(CubeMesh.Object);
        GuardMesh->SetRelativeLocation(FVector(0.f, 0.f, -0.5f));
        GuardMesh->SetRelativeScale3D(FVector(3.f, 1.8f, 0.06f));
    }

    // ── Aura Niagara FX ───────────────────────────────────────────────────────
    AuraFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AuraFX"));
    AuraFX->SetupAttachment(RootScene);
    AuraFX->SetRelativeLocation(FVector::ZeroVector);
    AuraFX->bAutoActivate = false;

    // ── Point light ───────────────────────────────────────────────────────────
    CharLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("CharLight"));
    CharLight->SetupAttachment(RootScene);
    CharLight->SetRelativeLocation(FVector(0.f, -120.f, 220.f));
    CharLight->SetIntensity(3500.f);
    CharLight->SetLightColor(FLinearColor::White);
    CharLight->AttenuationRadius = 1200.f;

    // ── Swordplay component ───────────────────────────────────────────────────
    SwordplayComp = CreateDefaultSubobject<USwordplayComponent>(TEXT("SwordplayComp"));

    EntranceTargetScale = FVector(1.f);
    SetActorScale3D(FVector::ZeroVector);
}

void AFighterActor::BeginPlay()
{
    Super::BeginPlay();
    SetActorHiddenInGame(true);
}

// ── Public API ────────────────────────────────────────────────────────────────

void AFighterActor::SetCharacter(UCharacterDataAsset* Data)
{
    if (!Data) return;
    CurrentData = Data;
    ApplyCharacterColors(Data);
}

void AFighterActor::PlayEntrance()
{
    SetActorHiddenInGame(false);
    SetActorScale3D(FVector::ZeroVector);
    bEntrancePlaying = true;
    EntranceElapsed  = 0.f;
}

void AFighterActor::PerformMove(const FSwordplayMove& Move)
{
    if (SwordplayComp)
        SwordplayComp->ExecuteMove(Move, SwordMesh);
}

// ── Tick ──────────────────────────────────────────────────────────────────────

void AFighterActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // ── Entrance pop-in (scale from 0 → 1 with overshoot) ────────────────────
    if (bEntrancePlaying)
    {
        EntranceElapsed += DeltaSeconds;
        float T = FMath::Clamp(EntranceElapsed / EntranceDuration, 0.f, 1.f);

        float Scale;
        if (T < 0.7f)
            Scale = FMath::Sin((T / 0.7f) * PI * 0.5f) * 1.12f;
        else
            Scale = FMath::Lerp(1.12f, 1.f, (T - 0.7f) / 0.3f);

        SetActorScale3D(FVector(Scale));

        if (T >= 1.f)
        {
            bEntrancePlaying = false;
            SetActorScale3D(FVector(1.f));

            if (AuraFX) AuraFX->Activate(true);

            // Kick off power-pulse on the light
            bPulsing       = true;
            PulseElapsed   = 0.f;
            PulseBaseLumen = CharLight ? CharLight->Intensity : 3500.f;

            OnEntranceComplete.Broadcast();
        }
    }

    // ── Power pulse ───────────────────────────────────────────────────────────
    if (bPulsing && CharLight)
    {
        PulseElapsed += DeltaSeconds;
        float T     = FMath::Clamp(PulseElapsed / PulseDuration, 0.f, 1.f);
        float PulseT = FMath::Sin(T * PI);

        CharLight->SetIntensity(PulseBaseLumen + PulseT * 4000.f);

        if (CurrentData)
        {
            FLinearColor PulseColor = FLinearColor::LerpUsingHSV(
                FLinearColor::White, CurrentData->AuraColor, PulseT);
            CharLight->SetLightColor(PulseColor);
        }

        if (T >= 1.f)
        {
            bPulsing = false;
            CharLight->SetIntensity(PulseBaseLumen);
            if (CurrentData) CharLight->SetLightColor(CurrentData->AuraColor);
        }
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void AFighterActor::ApplyCharacterColors(UCharacterDataAsset* Data)
{
    // Body colour
    if (BodyMesh)
    {
        UMaterialInstanceDynamic* BodyMat =
            UMaterialInstanceDynamic::Create(BodyMesh->GetMaterial(0), this);
        if (BodyMat)
        {
            BodyMat->SetVectorParameterValue(TEXT("BaseColor"), Data->BodyColor);
            BodyMesh->SetMaterial(0, BodyMat);
        }
    }

    // Sword tint
    if (SwordMesh)
    {
        UMaterialInstanceDynamic* SwordMat =
            UMaterialInstanceDynamic::Create(SwordMesh->GetMaterial(0), this);
        if (SwordMat)
        {
            FLinearColor SwordCol = Data->AuraColor * 0.4f + FLinearColor(0.6f, 0.6f, 0.7f);
            SwordMat->SetVectorParameterValue(TEXT("BaseColor"), SwordCol);
            SwordMesh->SetMaterial(0, SwordMat);
        }
    }

    // Aura colour via Niagara user parameter
    if (AuraFX)
    {
        AuraFX->SetVariableLinearColor(TEXT("AuraColor"), Data->AuraColor);
        float AuraScale = FMath::Clamp(Data->PowerLevel / 8.f, 0.5f, 5.f);
        AuraFX->SetWorldScale3D(FVector(AuraScale));
    }

    // Light
    if (CharLight)
    {
        CharLight->SetLightColor(Data->AuraColor);
        CharLight->SetIntensity(FMath::Lerp(2000.f, 8000.f, Data->PowerLevel / 10.f));
    }
}
