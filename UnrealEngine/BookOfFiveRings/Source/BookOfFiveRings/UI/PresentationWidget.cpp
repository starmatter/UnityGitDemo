#include "PresentationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/SlateTypes.h"

// ── Programmatic widget construction (no Blueprint required) ──────────────────

void UPresentationWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // If BindWidget didn't populate our slots (no WBP Blueprint was used),
    // build the entire widget hierarchy in C++.
    if (Background || !WidgetTree) return;

    UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(
        UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
    WidgetTree->RootWidget = Canvas;

    auto AddImage = [&](FName Name, float MinX, float MinY, float MaxX, float MaxY,
                        const FLinearColor& Col) -> UImage*
    {
        UImage* Img = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), Name);
        Img->SetColorAndOpacity(Col);
        UCanvasPanelSlot* S = Canvas->AddChildToCanvas(Img);
        S->SetAnchors(FAnchors(MinX, MinY, MaxX, MaxY));
        S->SetOffsets(FMargin(0.f));
        return Img;
    };

    auto AddText = [&](FName Name, float MinX, float MinY, float MaxX, float MaxY,
                       ETextJustify::Type Justify = ETextJustify::Center) -> UTextBlock*
    {
        UTextBlock* B = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
        B->SetJustification(Justify);
        B->SetAutoWrapText(true);
        UCanvasPanelSlot* S = Canvas->AddChildToCanvas(B);
        S->SetAnchors(FAnchors(MinX, MinY, MaxX, MaxY));
        S->SetOffsets(FMargin(0.f));
        return B;
    };

    // Background covers the full screen; border sits inside it
    Background    = AddImage(TEXT("Background"),  0.f,   0.f,  1.f,  1.f,
                              FLinearColor(0.04f, 0.02f, 0.01f, 1.f));
    ScrollBorder  = AddImage(TEXT("ScrollBorder"),0.04f, 0.03f, 0.96f, 0.97f,
                              FLinearColor(0.55f, 0.42f, 0.10f, 0.12f));

    // Text layout — arranged top-to-bottom so all six can coexist without overlap:
    //   [3-13%]  Scroll / presentation title   (large, centred)
    //   [13-27%] Japanese character + romanji  (medium, centred)
    //   [27-50%] Musashi quote                 (wrap-safe region)
    //   [50-62%] Character name / battle cry   (large, centred)
    //   [48-66%] Principles / combat moves     (left-aligned list)
    //   [66-94%] Philosophy / move description (centred, smaller)
    ScrollTitleText   = AddText(TEXT("ScrollTitleText"),  0.05f, 0.03f, 0.95f, 0.13f);
    JapaneseCharText  = AddText(TEXT("JapaneseCharText"), 0.05f, 0.13f, 0.95f, 0.27f);
    MusashiQuoteText  = AddText(TEXT("MusashiQuoteText"), 0.07f, 0.27f, 0.93f, 0.50f);
    CharacterNameText = AddText(TEXT("CharacterNameText"),0.05f, 0.50f, 0.95f, 0.62f);
    PrinciplesText    = AddText(TEXT("PrinciplesText"),   0.07f, 0.48f, 0.93f, 0.66f, ETextJustify::Left);
    PhilosophyText    = AddText(TEXT("PhilosophyText"),   0.07f, 0.66f, 0.93f, 0.94f);
}

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
