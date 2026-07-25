#include "PresentationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Styling/SlateTypes.h"

// ── Typewriter ────────────────────────────────────────────────────────────────

void UPresentationWidget::StartTypewriter(UTextBlock* Target, const FString& FullText,
                                           float CharDelay, bool bFastChars)
{
    if (!Target) return;

    CancelTypewriter();

    TypewriterTarget  = Target;
    TypewriterFull    = FullText;
    TypewriterIndex   = 0;
    TypewriterDelay   = CharDelay;
    TypewriterAccum   = 0.f;
    bTypewriterActive = true;

    Target->SetText(FText::GetEmpty());
}

void UPresentationWidget::CancelTypewriter()
{
    bTypewriterActive = false;
    if (TypewriterTarget)
        TypewriterTarget->SetText(FText::FromString(TypewriterFull));
    TypewriterTarget = nullptr;
}

// ── Convenience ───────────────────────────────────────────────────────────────

void UPresentationWidget::SetBackgroundColor(const FLinearColor& Color)
{
    if (Background) Background->SetColorAndOpacity(Color);
}

void UPresentationWidget::SetBorderColor(const FLinearColor& Color)
{
    if (ScrollBorder)
    {
        FLinearColor BorderCol = Color;
        BorderCol.A = 0.25f;
        ScrollBorder->SetColorAndOpacity(BorderCol);
    }
}

void UPresentationWidget::ClearAllText()
{
    for (UTextBlock* B : { ScrollTitleText, JapaneseCharText, MusashiQuoteText,
                            CharacterNameText, PrinciplesText, PhilosophyText })
    {
        if (B) B->SetText(FText::GetEmpty());
    }
}

void UPresentationWidget::SetTextSafe(UTextBlock* Block, const FString& Text,
                                       const FLinearColor& Color, int32 FontSize)
{
    if (!Block) return;
    Block->SetText(FText::FromString(Text));
    Block->SetColorAndOpacity(FSlateColor(Color));
    if (FontSize > 0)
    {
        FSlateFontInfo Font = Block->GetFont();
        Font.Size = FontSize;
        Block->SetFont(Font);
    }
}

// ── Background transition ─────────────────────────────────────────────────────

void UPresentationWidget::BeginBackgroundTransition(const FLinearColor& TargetColor, float Duration)
{
    if (Background)
        BGStartColor = Background->GetColorAndOpacity();
    else
        BGStartColor = FLinearColor::Black;

    BGTargetColor  = TargetColor;
    BGTransDuration= FMath::Max(Duration, 0.01f);
    BGTransElapsed = 0.f;
    bBGTransition  = true;
}

// ── Flash ─────────────────────────────────────────────────────────────────────

void UPresentationWidget::Flash(const FLinearColor& InFlashColor, float Duration)
{
    if (Background)
        FlashBaseColor = Background->GetColorAndOpacity();
    else
        FlashBaseColor = FLinearColor::Black;

    FlashColor    = InFlashColor;
    FlashDuration = FMath::Max(Duration, 0.01f);
    FlashElapsed  = 0.f;
    bFlashing     = true;
}

// ── Canvas fade ───────────────────────────────────────────────────────────────

void UPresentationWidget::FadeCanvas(float TargetAlpha, float Duration)
{
    FadeStart    = GetRenderOpacity();
    FadeTarget   = TargetAlpha;
    FadeDuration = FMath::Max(Duration, 0.01f);
    FadeElapsed  = 0.f;
    bFading      = true;
}

// ── NativeTick — drives all per-frame state ───────────────────────────────────

void UPresentationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // ── Typewriter ────────────────────────────────────────────────────────────
    if (bTypewriterActive && TypewriterTarget)
    {
        TypewriterAccum += InDeltaTime;
        while (TypewriterAccum >= TypewriterDelay && TypewriterIndex < TypewriterFull.Len())
        {
            TypewriterAccum -= TypewriterDelay;
            TypewriterTarget->SetText(FText::FromString(TypewriterFull.Left(++TypewriterIndex)));

            // Space/newline: no delay
            if (TypewriterIndex < TypewriterFull.Len())
            {
                TCHAR Next = TypewriterFull[TypewriterIndex];
                if (Next == TEXT(' ') || Next == TEXT('\n')) ++TypewriterIndex;
            }
        }

        if (TypewriterIndex >= TypewriterFull.Len())
        {
            bTypewriterActive = false;
            OnTypewriterComplete.Broadcast();
        }
    }

    // ── Background transition ─────────────────────────────────────────────────
    if (bBGTransition && Background)
    {
        BGTransElapsed += InDeltaTime;
        float T = FMath::Clamp(BGTransElapsed / BGTransDuration, 0.f, 1.f);
        Background->SetColorAndOpacity(FLinearColor::LerpUsingHSV(BGStartColor, BGTargetColor, T));
        if (T >= 1.f) bBGTransition = false;
    }

    // ── Flash ─────────────────────────────────────────────────────────────────
    if (bFlashing && Background)
    {
        FlashElapsed += InDeltaTime;
        float T    = FMath::Clamp(FlashElapsed / FlashDuration, 0.f, 1.f);
        float Wave = FMath::Sin(T * PI);
        Background->SetColorAndOpacity(FLinearColor::LerpUsingHSV(FlashBaseColor, FlashColor, Wave));
        if (T >= 1.f)
        {
            bFlashing = false;
            Background->SetColorAndOpacity(FlashBaseColor);
        }
    }

    // ── Canvas fade ───────────────────────────────────────────────────────────
    if (bFading)
    {
        FadeElapsed += InDeltaTime;
        float T = FMath::Clamp(FadeElapsed / FadeDuration, 0.f, 1.f);
        SetRenderOpacity(FMath::Lerp(FadeStart, FadeTarget, T));
        if (T >= 1.f) bFading = false;
    }
}
