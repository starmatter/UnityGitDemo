#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ScrollDataAsset.h"
#include "Data/CharacterDataAsset.h"
#include "Characters/FighterActor.h"
#include "UI/PresentationWidget.h"
#include "PresentationManager.generated.h"

/**
 * APresentationManager
 * Master sequence controller — UE5 equivalent of Unity's BookOfFiveRingsManager.
 *
 * Presentation flow per scroll:
 *   1. Background colour transition
 *   2. Scroll title + kanji slide in
 *   3. Opening narration (typewriter)
 *   4. Key principles (one per step, with delay)
 *   5. Musashi quote (flash → typewriter)
 *   6. Fighter entrance + philosophy
 *   7. Swordplay demonstration (each move, sequenced)
 *   8. Battle cry
 *   9. Fade out → next scroll
 *
 * Sequencing uses a TArray<FStep> step-queue executed via FTimerHandle.
 * Typewriter completion fires OnTypewriterComplete which advances to the next step.
 */
UCLASS()
class BOOKOFFIVERINGS_API APresentationManager : public AActor
{
    GENERATED_BODY()

public:
    APresentationManager();

    // ── Enhanced Input callbacks (bound by BookOfFiveRingsGameMode) ───────────
    UFUNCTION()
    void SkipSection();

    UFUNCTION()
    void QuitPresentation();

    // ── References (set in Blueprint or via GameMode spawn) ───────────────────
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TArray<UScrollDataAsset*> Scrolls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UCharacterDataAsset* BrolyData    = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UCharacterDataAsset* SupermanData = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UCharacterDataAsset* VegetaData   = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    UCharacterDataAsset* GokuData     = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
    AFighterActor* Fighter = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UPresentationWidget> WidgetClass;

    // ── Timing ────────────────────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
    float TypewriterCharDelay  = 0.035f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
    float TypewriterFastDelay  = 0.016f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
    float PrincipleHoldTime    = 0.9f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
    float TransitionDuration   = 1.2f;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void RunNextStep();

private:
    // ── Step queue ────────────────────────────────────────────────────────────
    struct FStep
    {
        TFunction<void()> Action;
        float             DelayAfter = 0.f;
        bool              bWaitForTypewriter = false;  // if true, ignore DelayAfter
    };

    TArray<FStep> StepQueue;
    int32         StepIndex = 0;
    FTimerHandle  StepTimer;

    UPROPERTY() UPresentationWidget* Widget = nullptr;

    void Enqueue(TFunction<void()> Action, float DelayAfter = 0.f,
                 bool bWaitForTypewriter = false);

    // ── Sequence builders ─────────────────────────────────────────────────────
    void BuildTitleCard();
    void BuildScrollSection(UScrollDataAsset* Scroll);
    void BuildClosingMeditation();

    // ── Helpers ───────────────────────────────────────────────────────────────
    UCharacterDataAsset* CharacterForScroll(EScrollType Type) const;

    int32 CurrentScrollIndex  = 0;
    int32 CurrentMoveIndex    = 0;
    UScrollDataAsset*    ActiveScroll = nullptr;
    UCharacterDataAsset* ActiveChar   = nullptr;
};
