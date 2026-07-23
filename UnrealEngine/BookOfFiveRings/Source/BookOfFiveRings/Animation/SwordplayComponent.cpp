#include "SwordplayComponent.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

USwordplayComponent::USwordplayComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USwordplayComponent::BeginPlay()
{
    Super::BeginPlay();

    // Create the procedural mesh for sword trails
    TrailMesh = NewObject<UProceduralMeshComponent>(GetOwner(), TEXT("SwordTrailMesh"));
    TrailMesh->RegisterComponent();
    TrailMesh->AttachToComponent(GetOwner()->GetRootComponent(),
                                  FAttachmentTransformRules::KeepWorldTransform);
    TrailMesh->bUseAsyncCooking = true;
    TrailMesh->SetCastShadow(false);
}

// ── Entry point ───────────────────────────────────────────────────────────────

void USwordplayComponent::ExecuteMove(const FSwordplayMove& Move, USceneComponent* InSwordRoot)
{
    if (bMovePlaying) return;

    ActiveMove    = Move;
    SwordRoot     = InSwordRoot;
    MoveElapsed   = 0.f;
    ComboHitIndex = 0;
    FadeAlpha     = 1.f;
    bMovePlaying  = true;

    GetOwner()->GetWorldTimerManager().ClearTimer(FadeTimer);
    SetComponentTickEnabled(true);
}

// ── Tick dispatcher ───────────────────────────────────────────────────────────

void USwordplayComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bMovePlaying) return;

    MoveElapsed += DeltaTime;
    float T = FMath::Clamp(MoveElapsed / ActiveMove.Duration, 0.f, 1.f);

    FVector Pivot = SwordRoot
        ? SwordRoot->GetComponentLocation() + SwordRoot->GetUpVector() * 60.f
        : GetOwner()->GetActorLocation() + FVector::UpVector * 130.f;

    switch (ActiveMove.MoveType)
    {
        case EMoveType::HorizontalSlash: TickHorizontalSlash(T, Pivot); break;
        case EMoveType::VerticalSlash:   TickVerticalSlash  (T, Pivot); break;
        case EMoveType::DiagonalSlash:   TickDiagonalSlash  (T, Pivot); break;
        case EMoveType::Thrust:           TickThrust         (T, Pivot); break;
        case EMoveType::Combo:            TickCombo          (T, Pivot); break;
        case EMoveType::PowerStrike:      TickPowerStrike    (T, Pivot); break;
        case EMoveType::AerialAssault:    TickAerialAssault  (T, Pivot); break;
        case EMoveType::EnergyBlade:      TickEnergyBlade    (T, Pivot); break;
        default:                          TickHorizontalSlash(T, Pivot); break;
    }

    if (T >= 1.f)
    {
        bMovePlaying = false;
        SetComponentTickEnabled(false);
        StartFade();
    }
}

// ── Per-move implementations ──────────────────────────────────────────────────

void USwordplayComponent::TickHorizontalSlash(float T, const FVector& Pivot)
{
    float Angle = FMath::Lerp(-90.f, 90.f, T);
    float Rad   = FMath::DegreesToRadians(Angle);
    float R     = 160.f;

    FVector Tip = Pivot + FVector(FMath::Cos(Rad) * R, FMath::Sin(Rad) * R, 0.f);
    float Alpha = FMath::Sin(T * PI);
    BuildTrailMesh(Pivot, Tip, ActiveMove.EffectColor, Alpha);
}

void USwordplayComponent::TickVerticalSlash(float T, const FVector& Pivot)
{
    float Angle = FMath::Lerp(90.f, -90.f, T);
    float Rad   = FMath::DegreesToRadians(Angle);
    float R     = 190.f;

    FVector Tip = Pivot + FVector(0.f, FMath::Cos(Rad) * R * 0.3f, FMath::Sin(Rad) * R);
    float Alpha = FMath::Sin(T * PI);
    BuildTrailMesh(Pivot, Tip, ActiveMove.EffectColor, Alpha);
}

void USwordplayComponent::TickDiagonalSlash(float T, const FVector& Pivot)
{
    FVector Start = Pivot + FVector(-130.f, 0.f,  160.f);
    FVector End   = Pivot + FVector( 130.f, 40.f, -50.f);

    float Smooth = FMath::SmoothStep(0.f, 1.f, T);
    FVector Tip  = FMath::Lerp(Start, End, Smooth);
    float Alpha  = FMath::Sin(T * PI);
    BuildTrailMesh(Pivot, Tip, ActiveMove.EffectColor, Alpha);
}

void USwordplayComponent::TickThrust(float T, const FVector& Pivot)
{
    float Half = 0.5f;
    FVector Forward = GetOwner()->GetActorForwardVector();

    FVector Tip;
    if (T <= Half)
    {
        float LocalT = T / Half;
        Tip = Pivot + Forward * FMath::Lerp(30.f, 250.f, FMath::SmoothStep(0.f, 1.f, LocalT));
    }
    else
    {
        float LocalT = (T - Half) / Half;
        Tip = Pivot + Forward * FMath::Lerp(250.f, 30.f, FMath::SmoothStep(0.f, 1.f, LocalT));
    }

    float Alpha = FMath::Sin(T * PI);
    BuildTrailMesh(Pivot, Tip, ActiveMove.EffectColor, Alpha);
}

void USwordplayComponent::TickCombo(float T, const FVector& Pivot)
{
    const int32 Hits     = 5;
    float PerHit         = 1.f / Hits;
    int32 CurrentHit     = FMath::FloorToInt(T / PerHit);
    float LocalT         = FMath::Fmod(T, PerHit) / PerHit;

    float StartAngle     = (CurrentHit % 2 == 0) ? -65.f : 65.f;
    float EndAngle       = -StartAngle;
    float Angle          = FMath::Lerp(StartAngle, EndAngle, LocalT);
    float Rad            = FMath::DegreesToRadians(Angle);
    float YOff           = CurrentHit * 22.f - 44.f;

    FVector HitPivot     = Pivot + FVector(0.f, 0.f, YOff);
    FVector Tip          = HitPivot + FVector(FMath::Cos(Rad) * 150.f, 0.f, FMath::Sin(Rad) * 50.f);
    float Alpha          = FMath::Sin(LocalT * PI);

    FLinearColor HitColor = FLinearColor::LerpUsingHSV(ActiveMove.EffectColor, FLinearColor::White,
                                                         CurrentHit * 0.08f);
    BuildTrailMesh(HitPivot, Tip, HitColor, Alpha);
}

void USwordplayComponent::TickPowerStrike(float T, const FVector& Pivot)
{
    // Wind-up first 35%, explosive strike the rest
    float Scale = 1.f;
    FVector ActorLoc = GetOwner()->GetActorLocation();

    if (T < 0.35f)
    {
        Scale = FMath::Lerp(1.f, 0.68f, T / 0.35f);
    }
    else
    {
        float StrikeT = (T - 0.35f) / 0.65f;
        Scale = FMath::Lerp(0.68f, 1.35f, FMath::Pow(StrikeT, 0.25f));

        float Angle = (StrikeT * 270.f - 135.f);
        float Rad   = FMath::DegreesToRadians(Angle);
        FVector Tip = Pivot + FVector(FMath::Cos(Rad) * 220.f, 0.f, FMath::Sin(Rad) * 160.f);
        float Alpha = FMath::Sin(StrikeT * PI);
        BuildTrailMesh(Pivot, Tip, ActiveMove.EffectColor, Alpha);
    }

    GetOwner()->SetActorScale3D(FVector(Scale));
}

void USwordplayComponent::TickAerialAssault(float T, const FVector& Pivot)
{
    FVector GroundPos = GetOwner()->GetActorLocation();
    GroundPos.Z       = 0.f;
    FVector PeakPos   = GroundPos + FVector::UpVector * 320.f;

    AActor* Owner = GetOwner();

    if (T < 0.2f)
    {
        // Rise
        float RT = T / 0.2f;
        FVector NewLoc = FMath::Lerp(GroundPos, PeakPos, FMath::Sin(RT * PI * 0.5f));
        Owner->SetActorLocation(NewLoc);
    }
    else if (T < 0.8f)
    {
        // Aerial spin-slashes
        float ST    = (T - 0.2f) / 0.6f;
        float Spin  = ST * 720.f;
        float RadS  = FMath::DegreesToRadians(Spin);
        FVector NewLoc = FMath::Lerp(PeakPos, GroundPos, ST * ST);
        Owner->SetActorLocation(NewLoc);
        Owner->SetActorRotation(FRotator(0.f, Spin, 0.f));

        FVector Tip = NewLoc + FVector(FMath::Cos(RadS) * 160.f, FMath::Sin(RadS) * 50.f, 40.f - ST * 80.f);
        BuildTrailMesh(NewLoc + FVector::UpVector * 80.f, Tip, ActiveMove.EffectColor, FMath::Sin(ST * PI));
    }
    else
    {
        // Land
        float LT    = (T - 0.8f) / 0.2f;
        FVector CurLoc = Owner->GetActorLocation();
        Owner->SetActorLocation(FMath::Lerp(CurLoc, GroundPos, LT));
        Owner->SetActorRotation(FRotator::ZeroRotator);
    }
}

void USwordplayComponent::TickEnergyBlade(float T, const FVector& Pivot)
{
    FVector Forward = GetOwner()->GetActorForwardVector();

    if (T < 0.35f)
    {
        // Spiral energy gather
        float GatherT = T / 0.35f;
        float SpiralAngle = GatherT * 720.f;
        float Rad     = FMath::DegreesToRadians(SpiralAngle);
        float Radius  = FMath::Lerp(220.f, 5.f, GatherT);

        FVector Gather = Pivot + FVector(FMath::Cos(Rad) * Radius,
                                          FMath::Sin(Rad) * Radius * 0.5f, 0.f);
        BuildTrailMesh(Pivot, Gather, ActiveMove.EffectColor, GatherT);
    }
    else
    {
        // Beam release
        float BeamT      = (T - 0.35f) / 0.65f;
        float BeamLength = FMath::Lerp(0.f, 900.f, FMath::SmoothStep(0.f, 1.f, BeamT));
        FVector Tip      = Pivot + Forward * BeamLength + FVector::UpVector * 40.f;

        // Colour pulse
        float PulseT = FMath::Abs(FMath::Sin(BeamT * PI * 5.f));
        FLinearColor PulseColor = FLinearColor::LerpUsingHSV(ActiveMove.EffectColor,
                                                               FLinearColor::White, PulseT);
        float Alpha = FMath::Lerp(1.f, 0.f, BeamT);
        BuildTrailMesh(Pivot, Tip, PulseColor, Alpha);
    }
}

// ── Trail mesh helpers ────────────────────────────────────────────────────────

void USwordplayComponent::BuildTrailMesh(const FVector& Start, const FVector& End,
                                          const FLinearColor& Color, float Alpha)
{
    if (!TrailMesh) return;

    FVector Dir  = (End - Start).GetSafeNormal();
    FVector Perp = FVector::CrossProduct(Dir, FVector::UpVector).GetSafeNormal();
    if (Perp.IsNearlyZero()) Perp = FVector::RightVector;

    float HalfW = TrailWidth * 0.5f;

    TArray<FVector>  Verts;
    TArray<int32>    Tris;
    TArray<FVector>  Normals;
    TArray<FVector2D> UVs;
    TArray<FLinearColor> Colors;

    const int32 Segs = TrailSegments;

    for (int32 i = 0; i <= Segs; i++)
    {
        float t    = (float)i / Segs;
        FVector Pt = FMath::Lerp(Start, End, t);
        float W    = FMath::Lerp(HalfW, HalfW * 0.1f, t);

        Verts.Add(Pt + Perp * W);
        Verts.Add(Pt - Perp * W);

        Normals.Add(FVector::UpVector);
        Normals.Add(FVector::UpVector);
        UVs.Add(FVector2D(t, 0.f));
        UVs.Add(FVector2D(t, 1.f));

        float A = Alpha * FMath::Lerp(1.f, 0.f, t);
        FLinearColor C(Color.R, Color.G, Color.B, A);
        Colors.Add(C);
        Colors.Add(C);
    }

    for (int32 i = 0; i < Segs; i++)
    {
        int32 Base = i * 2;
        Tris.Add(Base);     Tris.Add(Base + 2); Tris.Add(Base + 1);
        Tris.Add(Base + 1); Tris.Add(Base + 2); Tris.Add(Base + 3);
    }

    TArray<FVector2D> UV1, UV2, UV3;
    TArray<FProcMeshTangent> Tangents;

    TrailMesh->CreateMeshSection_LinearColor(0, Verts, Tris, Normals, UVs, UV1, UV2, UV3,
                                              Colors, Tangents, false);

    // Unlit translucent material
    UMaterialInterface* Mat = GetTrailMaterial(Color);
    if (Mat) TrailMesh->SetMaterial(0, Mat);
}

void USwordplayComponent::ClearTrailMesh()
{
    if (TrailMesh) TrailMesh->ClearAllMeshSections();
}

UMaterialInterface* USwordplayComponent::GetTrailMaterial(const FLinearColor& /*Color*/)
{
    // Returns the engine default unlit material.
    // In a full project, create M_SwordTrail (unlit, translucent, vertex-color) in Content Browser
    // and reference it here via a UPROPERTY.
    return UMaterial::GetDefaultMaterial(MD_Surface);
}

// ── Fade-out after move completes ─────────────────────────────────────────────

void USwordplayComponent::StartFade()
{
    const float FadeStep = 0.05f;
    const float FadeRate = 12.f;

    GetOwner()->GetWorldTimerManager().SetTimer(FadeTimer, [this, FadeRate, FadeStep]()
    {
        FadeAlpha -= FadeRate * FadeStep;
        if (FadeAlpha <= 0.f)
        {
            GetOwner()->GetWorldTimerManager().ClearTimer(FadeTimer);
            ClearTrailMesh();
            OnMoveComplete.Broadcast();
        }
        // Rebuilding with reduced alpha is lightweight since mesh topology doesn't change
    },
    FadeStep, true);
}
