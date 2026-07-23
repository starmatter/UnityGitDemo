#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/CharacterDataAsset.h"
#include "NiagaraComponent.h"
#include "Components/PointLightComponent.h"
#include "FighterActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEntranceComplete);

/**
 * AFighterActor
 * Scene representation of one fighter.
 * Uses primitive meshes (capsule body + box sword) as stand-ins for proper
 * character models — swap these for Skeletal Meshes in full production.
 */
UCLASS()
class BOOKOFFIVERINGS_API AFighterActor : public AActor
{
    GENERATED_BODY()

public:
    AFighterActor();

    // ── Public API ────────────────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Fighter")
    void SetCharacter(UCharacterDataAsset* Data);

    UFUNCTION(BlueprintCallable, Category = "Fighter")
    void PlayEntrance();

    UFUNCTION(BlueprintCallable, Category = "Fighter")
    void PerformMove(const FSwordplayMove& Move);

    UPROPERTY(BlueprintAssignable, Category = "Fighter")
    FOnEntranceComplete OnEntranceComplete;

    // Exposed so PresentationManager can bind to move-complete events
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USwordplayComponent* SwordplayComp;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // ── Scene components ──────────────────────────────────────────────────────
    UPROPERTY(VisibleAnywhere) USceneComponent*      RootScene;
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* BodyMesh;
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* SwordMesh;
    UPROPERTY(VisibleAnywhere) UStaticMeshComponent* GuardMesh;
    UPROPERTY(VisibleAnywhere) UNiagaraComponent*    AuraFX;
    UPROPERTY(VisibleAnywhere) UPointLightComponent* CharLight;

    // ── Data ──────────────────────────────────────────────────────────────────
    UPROPERTY() UCharacterDataAsset* CurrentData = nullptr;

    // ── Entrance animation state ──────────────────────────────────────────────
    bool  bEntrancePlaying  = false;
    float EntranceElapsed   = 0.f;
    const float EntranceDuration = 0.75f;
    FVector EntranceTargetScale;

    // ── Power pulse ───────────────────────────────────────────────────────────
    bool  bPulsing         = false;
    float PulseElapsed     = 0.f;
    float PulseBaseLumen   = 0.f;
    const float PulseDuration = 1.4f;

    void ApplyCharacterColors(UCharacterDataAsset* Data);
};
