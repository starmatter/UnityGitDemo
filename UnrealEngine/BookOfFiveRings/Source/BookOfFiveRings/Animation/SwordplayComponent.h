#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/CharacterDataAsset.h"
#include "ProceduralMeshComponent.h"
#include "SwordplayComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveComplete);

/**
 * USwordplayComponent
 * Attached to FighterActor. Drives sword-trail animations using a
 * UProceduralMeshComponent updated each tick.
 *
 * Each move type runs as a timer-based state machine:
 *   - MoveElapsed / MoveDuration control progress
 *   - UpdateTrail() rebuilds the mesh each tick
 *   - OnMoveComplete fires when the move finishes
 */
UCLASS(ClassGroup = (BookOfFiveRings), meta = (BlueprintSpawnableComponent))
class BOOKOFFIVERINGS_API USwordplayComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USwordplayComponent();

    // ── Public API ────────────────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Swordplay")
    void ExecuteMove(const FSwordplayMove& Move, USceneComponent* SwordRoot);

    UPROPERTY(BlueprintAssignable, Category = "Swordplay")
    FOnMoveComplete OnMoveComplete;

    // Trail visual settings
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trail")
    float TrailWidth = 8.f;          // cm

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trail")
    int32 TrailSegments = 24;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

private:
    // ── State ─────────────────────────────────────────────────────────────────
    FSwordplayMove  ActiveMove;
    USceneComponent* SwordRoot    = nullptr;
    bool            bMovePlaying  = false;
    float           MoveElapsed   = 0.f;
    int32           ComboHitIndex = 0;

    UPROPERTY() UProceduralMeshComponent* TrailMesh = nullptr;

    // ── Per-move tick functions ───────────────────────────────────────────────
    void TickHorizontalSlash (float T, const FVector& Pivot);
    void TickVerticalSlash   (float T, const FVector& Pivot);
    void TickDiagonalSlash   (float T, const FVector& Pivot);
    void TickThrust          (float T, const FVector& Pivot);
    void TickCombo           (float T, const FVector& Pivot);
    void TickPowerStrike     (float T, const FVector& Pivot);
    void TickAerialAssault   (float T, const FVector& Pivot);
    void TickEnergyBlade     (float T, const FVector& Pivot);

    // ── Mesh helpers ──────────────────────────────────────────────────────────
    void BuildTrailMesh(const FVector& Start, const FVector& End,
                        const FLinearColor& Color, float Alpha);
    void ClearTrailMesh();
    UMaterialInterface* GetTrailMaterial(const FLinearColor& Color);

    FTimerHandle FadeTimer;
    float        FadeAlpha = 1.f;
    void         StartFade();
};
