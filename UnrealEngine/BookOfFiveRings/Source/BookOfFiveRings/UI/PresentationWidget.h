#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "PresentationWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTypewriterComplete);

/**
 * UPresentationWidget
 * Main UMG widget for the entire Book of Five Rings animated presentation.
 *
 * Bind the named slots below in the Widget Blueprint (WBP_Presentation):
 *   Background          → UImage  (full-screen colour fill)
 *   ScrollBorder        → UImage  (decorative border)
 *   ScrollTitleText     → UTextBlock
 *   JapaneseCharText    → UTextBlock
 *   MusashiQuoteText    → UTextBlock
 *   CharacterNameText   → UTextBlock
 *   PrinciplesText      → UTextBlock
 *   PhilosophyText      → UTextBlock
 *   HUDPanel            → UCanvasPanel  (kanji progress dots — populated at runtime)
 */
UCLASS(Abstract)
class BOOKOFFIVERINGS_API UPresentationWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ── Bound slots (set meta = (BindWidget) so UMG validates at compile time) ─
    UPROPERTY(meta = (BindWidget)) UImage*      Background       = nullptr;
    UPROPERTY(meta = (BindWidget)) UImage*      ScrollBorder     = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock*  ScrollTitleText  = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock*  JapaneseCharText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock*  MusashiQuoteText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock*  CharacterNameText= nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock*  PrinciplesText   = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock*  PhilosophyText   = nullptr;

    // ── Typewriter API ────────────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void StartTypewriter(UTextBlock* Target, const FString& FullText,
                         float CharDelay, bool bFastChars = true);

    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void CancelTypewriter();

    UPROPERTY(BlueprintAssignable, Category = "Presentation")
    FOnTypewriterComplete OnTypewriterComplete;

    // ── Convenience setters ───────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void SetBackgroundColor(const FLinearColor& Color);

    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void SetBorderColor(const FLinearColor& Color);

    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void ClearAllText();

    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void SetTextSafe(UTextBlock* Block, const FString& Text,
                     const FLinearColor& Color, int32 FontSize = 0);

    // ── Background transition ─────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void BeginBackgroundTransition(const FLinearColor& TargetColor, float Duration);

    // ── Flash ─────────────────────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void Flash(const FLinearColor& FlashColor, float Duration);

    // ── Fade canvas ───────────────────────────────────────────────────────────
    UFUNCTION(BlueprintCallable, Category = "Presentation")
    void FadeCanvas(float TargetAlpha, float Duration);

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    // Typewriter state
    FString      TypewriterFull;
    int32        TypewriterIndex = 0;
    float        TypewriterDelay = 0.03f;
    float        TypewriterAccum = 0.f;
    bool         bTypewriterActive = false;
    UTextBlock*  TypewriterTarget  = nullptr;

    // Background transition state
    bool         bBGTransition  = false;
    float        BGTransElapsed = 0.f;
    float        BGTransDuration= 1.f;
    FLinearColor BGStartColor;
    FLinearColor BGTargetColor;

    // Flash state
    bool         bFlashing      = false;
    float        FlashElapsed   = 0.f;
    float        FlashDuration  = 0.4f;
    FLinearColor FlashColor;
    FLinearColor FlashBaseColor;

    // Canvas fade state
    bool         bFading        = false;
    float        FadeElapsed    = 0.f;
    float        FadeDuration   = 1.f;
    float        FadeStart      = 1.f;
    float        FadeTarget     = 0.f;
};
