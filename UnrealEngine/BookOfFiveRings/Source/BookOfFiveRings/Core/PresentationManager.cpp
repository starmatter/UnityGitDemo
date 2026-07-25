#include "PresentationManager.h"
#include "Animation/SwordplayComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"

APresentationManager::APresentationManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APresentationManager::BeginPlay()
{
    Super::BeginPlay();

    // Create and add widget to viewport — fall back to base C++ class if no Blueprint set
    {
        UClass* WClass = WidgetClass ? WidgetClass.Get() : UPresentationWidget::StaticClass();
        Widget = CreateWidget<UPresentationWidget>(GetWorld(), WClass);
        if (Widget)
        {
            Widget->AddToViewport(10);
            Widget->SetBackgroundColor(FLinearColor(0.04f, 0.02f, 0.01f));

            // Wire typewriter-complete → advance step queue
            Widget->OnTypewriterComplete.AddDynamic(this, &APresentationManager::RunNextStep);
        }
    }

    // Wire fighter move-complete → advance step queue
    if (Fighter && Fighter->SwordplayComp)
        Fighter->SwordplayComp->OnMoveComplete.AddDynamic(this, &APresentationManager::RunNextStep);

    // Build the entire presentation as a flat step queue
    BuildTitleCard();
    for (UScrollDataAsset* Scroll : Scrolls)
        BuildScrollSection(Scroll);
    BuildClosingMeditation();

    // Start
    StepIndex = 0;
    RunNextStep();
}

// ── Enhanced Input handlers ───────────────────────────────────────────────────

void APresentationManager::SkipSection()
{
    // Cancel any pending timer and advance immediately to the next step
    GetWorldTimerManager().ClearTimer(StepTimer);
    RunNextStep();
}

void APresentationManager::QuitPresentation()
{
    GetWorldTimerManager().ClearTimer(StepTimer);
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

// ── Step queue machinery ──────────────────────────────────────────────────────

void APresentationManager::Enqueue(TFunction<void()> Action, float DelayAfter, bool bWaitForTypewriter)
{
    StepQueue.Add({ MoveTemp(Action), DelayAfter, bWaitForTypewriter });
}

void APresentationManager::RunNextStep()
{
    GetWorldTimerManager().ClearTimer(StepTimer);

    if (StepIndex >= StepQueue.Num()) return;

    FStep& Step = StepQueue[StepIndex++];
    Step.Action();

    if (!Step.bWaitForTypewriter && Step.DelayAfter > 0.f)
    {
        GetWorldTimerManager().SetTimer(StepTimer, this,
            &APresentationManager::RunNextStep, Step.DelayAfter, false);
    }
    // If bWaitForTypewriter == true, RunNextStep is called by OnTypewriterComplete
    // If bWaitForTypewriter == false && DelayAfter == 0, caller is responsible for triggering next
}

// ── Title card ────────────────────────────────────────────────────────────────

void APresentationManager::BuildTitleCard()
{
    if (!Widget) return;

    Enqueue([this]()
    {
        Widget->ClearAllText();
        Widget->SetBackgroundColor(FLinearColor(0.07f, 0.04f, 0.01f));
        Widget->SetTextSafe(Widget->ScrollTitleText,
            TEXT("THE BOOK OF FIVE RINGS"), FLinearColor::White, 68);
        Widget->StartTypewriter(Widget->ScrollTitleText,
            TEXT("THE BOOK OF FIVE RINGS"), TypewriterCharDelay * 0.8f);
    }, 0.f, true);

    Enqueue([this]()
    {
        Widget->SetTextSafe(Widget->JapaneseCharText,
            TEXT("五輪書\nGo Rin No Sho"), FLinearColor(0.9f, 0.7f, 0.2f), 48);
        Widget->StartTypewriter(Widget->JapaneseCharText,
            TEXT("五輪書\nGo Rin No Sho"), TypewriterFastDelay);
    }, 0.f, true);

    Enqueue([this]()
    {
        const FString Quote =
            TEXT("\"There is nothing outside of yourself that can ever enable you\n")
            TEXT("to get better, stronger, richer, quicker, or smarter.\n")
            TEXT("Everything is within. Everything exists.\n")
            TEXT("Seek nothing outside of yourself.\"\n\n")
            TEXT("— Miyamoto Musashi, 1643");

        Widget->SetTextSafe(Widget->MusashiQuoteText,
            Quote, FLinearColor(0.88f, 0.82f, 0.68f), 26);
        Widget->StartTypewriter(Widget->MusashiQuoteText, Quote, TypewriterFastDelay);
    }, 0.f, true);

    Enqueue([this]() { /* pause after title card */ }, 3.5f);

    Enqueue([this]()
    {
        Widget->FadeCanvas(0.f, TransitionDuration);
    }, TransitionDuration + 0.3f);

    Enqueue([this]()
    {
        Widget->ClearAllText();
        Widget->FadeCanvas(1.f, 0.3f);
    }, 0.5f);
}

// ── Per-scroll section ────────────────────────────────────────────────────────

void APresentationManager::BuildScrollSection(UScrollDataAsset* Scroll)
{
    if (!Scroll || !Widget) return;

    // 1 ─ Background
    Enqueue([this, Scroll]()
    {
        Widget->BeginBackgroundTransition(Scroll->PrimaryColor, TransitionDuration);
        Widget->SetBorderColor(Scroll->AccentColor);
    }, TransitionDuration);

    // 2 ─ Scroll title
    Enqueue([this, Scroll]()
    {
        FString Title = Scroll->ScrollName + TEXT(" SCROLL");
        Widget->SetTextSafe(Widget->ScrollTitleText, Title, Scroll->TextColor, 68);
        Widget->SetTextSafe(Widget->JapaneseCharText, Scroll->JapaneseCharacter, Scroll->AccentColor, 52);
        Widget->StartTypewriter(Widget->ScrollTitleText, Title, TypewriterCharDelay);
    }, 0.f, true);

    Enqueue([this](){}, 0.8f);

    // 3 ─ Opening narration
    Enqueue([this, Scroll]()
    {
        Widget->SetTextSafe(Widget->PrinciplesText,
            Scroll->OpeningNarration, Scroll->TextColor, 26);
        Widget->StartTypewriter(Widget->PrinciplesText,
            Scroll->OpeningNarration, TypewriterFastDelay);
    }, 0.f, true);

    Enqueue([this](){}, 1.f);

    // 4 ─ Principles (one per step)
    Enqueue([this, Scroll]()
    {
        Widget->SetTextSafe(Widget->PrinciplesText,
            TEXT("Key Principles:\n"), Scroll->TextColor, 24);
    }, 0.3f);

    for (const FString& Principle : Scroll->Principles)
    {
        Enqueue([this, Scroll, Principle]()
        {
            UTextBlock* B = Widget->PrinciplesText;
            if (B)
            {
                FString Current = B->GetText().ToString();
                Current += TEXT("◆  ") + Principle + TEXT("\n");
                B->SetText(FText::FromString(Current));
            }
        }, PrincipleHoldTime);
    }

    Enqueue([this](){}, 0.8f);

    // 5 ─ Musashi quote
    Enqueue([this, Scroll]()
    {
        Widget->Flash(Scroll->AccentColor, 0.35f);
    }, 0.4f);

    Enqueue([this, Scroll]()
    {
        FString Q = TEXT("\"") + Scroll->MusashiQuote + TEXT("\"\n\n— Miyamoto Musashi");
        Widget->SetTextSafe(Widget->MusashiQuoteText, Q, FLinearColor(0.95f, 0.9f, 0.75f), 28);
        Widget->StartTypewriter(Widget->MusashiQuoteText, Q, TypewriterFastDelay);
    }, 0.f, true);

    Enqueue([this](){}, 2.5f);

    // 6 ─ Fighter introduction
    UCharacterDataAsset* Char = CharacterForScroll(Scroll->ScrollType);
    if (Char)
    {
        Enqueue([this, Char, Scroll]()
        {
            Widget->SetTextSafe(Widget->CharacterNameText,
                Char->CharacterName, Char->AuraColor, 58);
            Widget->StartTypewriter(Widget->CharacterNameText,
                Char->CharacterName, TypewriterCharDelay * 1.2f);
        }, 0.f, true);

        Enqueue([this, Char, Scroll]()
        {
            Widget->SetTextSafe(Widget->PhilosophyText,
                Char->Title + TEXT("\n") + Char->Origin, Scroll->TextColor, 24);
            Widget->StartTypewriter(Widget->PhilosophyText,
                Char->Title + TEXT("\n") + Char->Origin, TypewriterFastDelay);
        }, 0.f, true);

        Enqueue([this, Char]()
        {
            if (Fighter) { Fighter->SetCharacter(Char); Fighter->PlayEntrance(); }
        }, 2.f);

        Enqueue([this, Char]()
        {
            Widget->SetTextSafe(Widget->PhilosophyText,
                Char->PhilosophyStatement, FLinearColor(0.9f, 0.88f, 1.f), 22);
            Widget->StartTypewriter(Widget->PhilosophyText,
                Char->PhilosophyStatement, TypewriterFastDelay);
        }, 0.f, true);

        Enqueue([this](){}, 2.2f);

        // 7 ─ Swordplay moves
        Enqueue([this, Char, Scroll]()
        {
            Widget->SetTextSafe(Widget->PrinciplesText,
                TEXT("Combat Demonstration:"), Scroll->AccentColor, 30);
        }, 0.6f);

        for (const FSwordplayMove& Move : Char->SignatureMoves)
        {
            Enqueue([this, Move]()
            {
                Widget->SetTextSafe(Widget->PrinciplesText,
                    TEXT("► ") + Move.MoveName, Move.EffectColor, 32);
                if (Fighter) Fighter->PerformMove(Move);
                // Move completion fires RunNextStep via SwordplayComp->OnMoveComplete
            }, 0.f, false);

            // Wait for move to finish (OnMoveComplete triggers RunNextStep)
            // Then show description
            Enqueue([this, Move]()
            {
                Widget->SetTextSafe(Widget->PhilosophyText,
                    Move.Description, FLinearColor(0.88f, 0.85f, 1.f), 22);
                Widget->StartTypewriter(Widget->PhilosophyText,
                    Move.Description, TypewriterFastDelay);
            }, 0.f, true);

            Enqueue([this](){}, 1.2f);
        }

        // 8 ─ Battle cry
        if (!Char->BattleCry.IsEmpty())
        {
            Enqueue([this, Char]()
            {
                Widget->SetTextSafe(Widget->CharacterNameText,
                    Char->BattleCry, Char->BattleCryColor, 60);
                Widget->StartTypewriter(Widget->CharacterNameText,
                    Char->BattleCry, TypewriterCharDelay);
            }, 0.f, true);

            Enqueue([this](){}, 1.8f);
        }
    }

    // 9 ─ Fade out
    Enqueue([this]() { Widget->FadeCanvas(0.f, TransitionDuration); }, TransitionDuration + 0.2f);
    Enqueue([this]() { Widget->ClearAllText(); Widget->FadeCanvas(1.f, 0.3f); }, 0.5f);
}

// ── Closing meditation ────────────────────────────────────────────────────────

void APresentationManager::BuildClosingMeditation()
{
    if (!Widget) return;

    Enqueue([this]()
    {
        Widget->BeginBackgroundTransition(FLinearColor(0.06f, 0.04f, 0.02f), 1.f);
    }, 1.2f);

    Enqueue([this]()
    {
        Widget->SetTextSafe(Widget->ScrollTitleText,
            TEXT("THE WAY OF THE SWORD"), FLinearColor::White, 64);
        Widget->StartTypewriter(Widget->ScrollTitleText,
            TEXT("THE WAY OF THE SWORD"), TypewriterCharDelay);
    }, 0.f, true);

    Enqueue([this](){}, 0.8f);

    Enqueue([this]()
    {
        const FString Closing =
            TEXT("\"Accept everything just the way it is.\"\n\n")
            TEXT("— Miyamoto Musashi\n")
            TEXT("Go Rin No Sho  ·  Written 1643 CE");

        Widget->SetTextSafe(Widget->MusashiQuoteText,
            Closing, FLinearColor(0.9f, 0.85f, 0.7f), 30);
        Widget->StartTypewriter(Widget->MusashiQuoteText, Closing, TypewriterFastDelay);
    }, 0.f, true);

    Enqueue([this](){}, 3.f);

    Enqueue([this]()
    {
        const FString Summary =
            TEXT("地  Earth  —  BROLY\n")
            TEXT("水  Water  —  KAL-EL  /  SUPERMAN\n")
            TEXT("火  Fire   —  VEGETA\n")
            TEXT("風  Wind   —  SON GOKU\n")
            TEXT("空  Void   —  GOKU  ULTRA INSTINCT\n\n")
            TEXT("\"Seek nothing outside of yourself.\"");

        Widget->SetTextSafe(Widget->PrinciplesText,
            Summary, FLinearColor(0.8f, 0.9f, 0.8f), 26);
        Widget->StartTypewriter(Widget->PrinciplesText, Summary, TypewriterFastDelay);
    }, 0.f, true);

    Enqueue([this](){}, 6.f);

    Enqueue([this]() { Widget->FadeCanvas(0.f, 2.f); }, 2.5f);
}

// ── Helper ────────────────────────────────────────────────────────────────────

UCharacterDataAsset* APresentationManager::CharacterForScroll(EScrollType Type) const
{
    switch (Type)
    {
        case EScrollType::Earth: return BrolyData;
        case EScrollType::Water: return SupermanData;
        case EScrollType::Fire:  return VegetaData;
        case EScrollType::Wind:
        case EScrollType::Void:  return GokuData;
        default:                 return nullptr;
    }
}
