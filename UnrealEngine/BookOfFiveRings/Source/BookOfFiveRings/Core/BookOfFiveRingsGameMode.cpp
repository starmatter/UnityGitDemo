#include "BookOfFiveRingsGameMode.h"
#include "UI/PresentationWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

ABookOfFiveRingsGameMode::ABookOfFiveRingsGameMode()
{
    // Defaults — override these in Blueprints or DefaultEngine.ini
}

void ABookOfFiveRingsGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Hide mouse cursor during cinematic
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC) PC->bShowMouseCursor = false;

    // ── Build runtime data ────────────────────────────────────────────────────
    TArray<UScrollDataAsset*>    Scrolls    = BuildScrollData();
    TArray<UCharacterDataAsset*> Characters = BuildCharacterData();

    // ── Spawn fighter stage actor ─────────────────────────────────────────────
    FActorSpawnParameters SP;
    SP.Owner = this;
    AFighterActor* Fighter = GetWorld()->SpawnActor<AFighterActor>(
        FighterClass ? FighterClass : AFighterActor::StaticClass(),
        FVector(0.f, 200.f, 0.f), FRotator::ZeroRotator, SP);

    // ── Spawn presentation manager ────────────────────────────────────────────
    APresentationManager* Mgr = GetWorld()->SpawnActor<APresentationManager>(
        ManagerClass ? ManagerClass : APresentationManager::StaticClass(),
        FVector::ZeroVector, FRotator::ZeroRotator, SP);

    if (!Mgr) return;

    Mgr->Scrolls     = Scrolls;
    Mgr->Fighter     = Fighter;
    Mgr->WidgetClass = WidgetClass;

    // Assign characters by scroll type
    for (UCharacterDataAsset* C : Characters)
    {
        switch (C->AssociatedScroll)
        {
            case EScrollType::Earth: Mgr->BrolyData    = C; break;
            case EScrollType::Water: Mgr->SupermanData = C; break;
            case EScrollType::Fire:  Mgr->VegetaData   = C; break;
            case EScrollType::Void:  Mgr->GokuData     = C; break;
            default: break;
        }
    }

    // ── Wire Enhanced Input (UE 5.8) ──────────────────────────────────────────
    SetupEnhancedInput(PC, Mgr);
}

void ABookOfFiveRingsGameMode::SetupEnhancedInput(APlayerController* PC, APresentationManager* Mgr)
{
    if (!PC || !Mgr) return;

    // Build input actions at runtime — no content assets required
    IA_Skip = NewObject<UInputAction>(this, TEXT("IA_Skip"));
    IA_Skip->ValueType = EInputActionValueType::Boolean;

    IA_Quit = NewObject<UInputAction>(this, TEXT("IA_Quit"));
    IA_Quit->ValueType = EInputActionValueType::Boolean;

    // Build mapping context
    PresentationIMC = NewObject<UInputMappingContext>(this, TEXT("IMC_Presentation"));
    PresentationIMC->MapKey(IA_Skip, EKeys::SpaceBar);
    PresentationIMC->MapKey(IA_Skip, EKeys::Enter);
    PresentationIMC->MapKey(IA_Quit, EKeys::Escape);

    // Register the mapping context with priority 0
    if (ULocalPlayer* LP = PC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Subsystem->AddMappingContext(PresentationIMC, 0);
        }
    }

    // Bind actions on the player's Enhanced Input component
    if (UEnhancedInputComponent* EIC =
            Cast<UEnhancedInputComponent>(PC->InputComponent))
    {
        EIC->BindAction(IA_Skip, ETriggerEvent::Started, Mgr,
                        &APresentationManager::SkipSection);
        EIC->BindAction(IA_Quit, ETriggerEvent::Started, Mgr,
                        &APresentationManager::QuitPresentation);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Scroll data factory
// ─────────────────────────────────────────────────────────────────────────────

TArray<UScrollDataAsset*> ABookOfFiveRingsGameMode::BuildScrollData()
{
    TArray<UScrollDataAsset*> Out;

    auto Make = [this]() { return NewObject<UScrollDataAsset>(this); };

    // ── 地  EARTH ──────────────────────────────────────────────────────────────
    {
        auto* S = Make();
        S->ScrollType              = EScrollType::Earth;
        S->ScrollName              = TEXT("EARTH");
        S->JapaneseCharacter       = TEXT("地\nChi");
        S->AssociatedCharacterName = TEXT("Broly");
        S->PrimaryColor            = FLinearColor(0.38f, 0.22f, 0.08f);
        S->SecondaryColor          = FLinearColor(0.18f, 0.10f, 0.03f);
        S->TextColor               = FLinearColor(0.92f, 0.82f, 0.60f);
        S->AccentColor             = FLinearColor(0.55f, 0.85f, 0.25f);
        S->OpeningNarration        = TEXT("The Earth Scroll lays the foundation of all strategy.\nBefore technique, before speed, before power —\na warrior must know the ground on which he stands.");
        S->MusashiQuote            = TEXT("There is nothing outside of yourself that can ever enable you\nto get better, stronger, richer, quicker, or smarter.\nEverything is within. Everything exists.\nSeek nothing outside of yourself.");
        S->Principles              = {
            TEXT("Master the fundamentals before all advanced technique"),
            TEXT("Know your own strengths — and your own limits"),
            TEXT("The body itself is the first and greatest weapon"),
            TEXT("Strength without form is energy wasted"),
            TEXT("True power is born from complete self-mastery")
        };
        S->CombatPhilosophy = TEXT("Strike from the earth. Every technique must be rooted.");
        S->DisplayDuration  = 11.f;
        Out.Add(S);
    }

    // ── 水  WATER ──────────────────────────────────────────────────────────────
    {
        auto* S = Make();
        S->ScrollType              = EScrollType::Water;
        S->ScrollName              = TEXT("WATER");
        S->JapaneseCharacter       = TEXT("水\nSui");
        S->AssociatedCharacterName = TEXT("Superman / Kal-El");
        S->PrimaryColor            = FLinearColor(0.08f, 0.26f, 0.58f);
        S->SecondaryColor          = FLinearColor(0.04f, 0.12f, 0.36f);
        S->TextColor               = FLinearColor(0.80f, 0.92f, 1.00f);
        S->AccentColor             = FLinearColor(0.35f, 0.70f, 1.00f);
        S->OpeningNarration        = TEXT("Water takes the shape of any vessel it enters.\nThe warrior's spirit must be the same — calm, clear,\nand impossibly persistent.");
        S->MusashiQuote            = TEXT("A river cuts through rock\nnot because of its power,\nbut because of its persistence.");
        S->Principles              = {
            TEXT("Adapt your technique to the moment and the enemy"),
            TEXT("Persistence overcomes what force cannot"),
            TEXT("A calm, untroubled spirit sees every opening"),
            TEXT("Flow around resistance — never clash with it head-on"),
            TEXT("Hope is the most persistent force in the universe")
        };
        S->DisplayDuration = 11.f;
        Out.Add(S);
    }

    // ── 火  FIRE ───────────────────────────────────────────────────────────────
    {
        auto* S = Make();
        S->ScrollType              = EScrollType::Fire;
        S->ScrollName              = TEXT("FIRE");
        S->JapaneseCharacter       = TEXT("火\nKa");
        S->AssociatedCharacterName = TEXT("Vegeta");
        S->PrimaryColor            = FLinearColor(0.75f, 0.18f, 0.04f);
        S->SecondaryColor          = FLinearColor(0.45f, 0.08f, 0.00f);
        S->TextColor               = FLinearColor(1.00f, 0.90f, 0.55f);
        S->AccentColor             = FLinearColor(1.00f, 0.60f, 0.10f);
        S->OpeningNarration        = TEXT("The Fire Scroll governs the heat and rhythm of actual combat.\nIn battle, timing is everything.\nHesitation is defeat. A prince does not hesitate.");
        S->MusashiQuote            = TEXT("Accept your past without regret,\nhandle your present with confidence,\nand face your future without fear.");
        S->Principles              = {
            TEXT("Timing is the very essence of combat"),
            TEXT("Attack with absolute commitment — no half-measures"),
            TEXT("Never give the enemy space to breathe or recover"),
            TEXT("Pride is the warrior's most powerful fuel"),
            TEXT("A prince falls only when he stops believing he is a prince")
        };
        S->DisplayDuration = 11.f;
        Out.Add(S);
    }

    // ── 風  WIND ───────────────────────────────────────────────────────────────
    {
        auto* S = Make();
        S->ScrollType              = EScrollType::Wind;
        S->ScrollName              = TEXT("WIND");
        S->JapaneseCharacter       = TEXT("風\nFū");
        S->AssociatedCharacterName = TEXT("Goku");
        S->PrimaryColor            = FLinearColor(0.18f, 0.46f, 0.18f);
        S->SecondaryColor          = FLinearColor(0.08f, 0.26f, 0.08f);
        S->TextColor               = FLinearColor(0.80f, 1.00f, 0.80f);
        S->AccentColor             = FLinearColor(0.40f, 0.90f, 0.40f);
        S->OpeningNarration        = TEXT("The Wind Scroll teaches knowledge of other schools.\nTo know every style is to be boundless.\nThe wind touches every place and knows every path.");
        S->MusashiQuote            = TEXT("Master the divine techniques of the Art of Peace\nand no enemy will dare to challenge you.");
        S->Principles              = {
            TEXT("Study all schools, but commit fully to none"),
            TEXT("Identify the weakness within every fighting style"),
            TEXT("The wind warrior adapts the instant contact is made"),
            TEXT("Your mind is your most dangerous weapon"),
            TEXT("Master yourself and you automatically master all styles")
        };
        S->DisplayDuration = 11.f;
        Out.Add(S);
    }

    // ── 空  VOID ───────────────────────────────────────────────────────────────
    {
        auto* S = Make();
        S->ScrollType              = EScrollType::Void;
        S->ScrollName              = TEXT("VOID");
        S->JapaneseCharacter       = TEXT("空\nKū");
        S->AssociatedCharacterName = TEXT("Goku — Ultra Instinct");
        S->PrimaryColor            = FLinearColor(0.10f, 0.07f, 0.18f);
        S->SecondaryColor          = FLinearColor(0.04f, 0.02f, 0.08f);
        S->TextColor               = FLinearColor(0.92f, 0.92f, 1.00f);
        S->AccentColor             = FLinearColor(0.75f, 0.75f, 1.00f);
        S->OpeningNarration        = TEXT("The Void Scroll transcends all the others.\nBeyond technique. Beyond thought. Beyond the self.\nThe Void contains all possibilities — and none.");
        S->MusashiQuote            = TEXT("Accept everything just the way it is.");
        S->Principles              = {
            TEXT("True mastery acts without conscious thought"),
            TEXT("The Void contains all possibilities simultaneously"),
            TEXT("Transcend the self and every limit dissolves"),
            TEXT("When ego disappears, perfect technique emerges"),
            TEXT("Ultra Instinct — the body moves before the mind can slow it")
        };
        S->DisplayDuration = 13.f;
        Out.Add(S);
    }

    return Out;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Character data factory
// ─────────────────────────────────────────────────────────────────────────────

TArray<UCharacterDataAsset*> ABookOfFiveRingsGameMode::BuildCharacterData()
{
    TArray<UCharacterDataAsset*> Out;

    auto Make = [this]() { return NewObject<UCharacterDataAsset>(this); };

    // ── BROLY — Earth ──────────────────────────────────────────────────────────
    {
        auto* C = Make();
        C->CharacterName       = TEXT("BROLY");
        C->Title               = TEXT("The Legendary Super Saiyan");
        C->Origin              = TEXT("Dragon Ball Z");
        C->AssociatedScroll    = EScrollType::Earth;
        C->BodyColor           = FLinearColor(0.55f, 0.38f, 0.18f);
        C->AuraColor           = FLinearColor(0.20f, 0.92f, 0.20f);
        C->PowerLevel          = 10.f; C->Speed = 7.f; C->Technique = 5.f;
        C->PhilosophyStatement = TEXT("\"There is no power greater than the power that was\nalways already inside you.\"\n\nThe Earth never yields. The foundation never breaks.\nBroly IS the Earth Scroll made flesh.");
        C->BattleCry           = TEXT("BROLY!!!");
        C->BattleCryColor      = FLinearColor(0.20f, 1.00f, 0.20f);
        C->SignatureMoves = {
            MakeMove(TEXT("Earth Crusher"),
                TEXT("Foundation Principle: Strike from rooted earth.\nThe mountain does not chase you — it simply falls on you."),
                FLinearColor(0.35f, 0.85f, 0.10f), 1.9f, EMoveType::PowerStrike),
            MakeMove(TEXT("Bedrock Fist"),
                TEXT("Self-Knowledge: Raw power born from knowing one's limitless potential.\nEvery strike acknowledges the depth of the earth below."),
                FLinearColor(0.25f, 0.92f, 0.30f), 1.6f, EMoveType::VerticalSlash),
            MakeMove(TEXT("Mountain Stance Combo"),
                TEXT("Form Before Power: Five strikes — each one is the mountain falling."),
                FLinearColor(0.20f, 0.75f, 0.40f), 2.6f, EMoveType::Combo)
        };
        Out.Add(C);
    }

    // ── SUPERMAN / KAL-EL — Water ──────────────────────────────────────────────
    {
        auto* C = Make();
        C->CharacterName       = TEXT("KAL-EL");
        C->Title               = TEXT("Superman — The Man of Steel");
        C->Origin              = TEXT("DC Universe  ·  Krypton");
        C->AssociatedScroll    = EScrollType::Water;
        C->BodyColor           = FLinearColor(0.10f, 0.20f, 0.72f);
        C->AuraColor           = FLinearColor(0.30f, 0.55f, 1.00f);
        C->PowerLevel          = 9.f; C->Speed = 9.f; C->Technique = 8.f;
        C->PhilosophyStatement = TEXT("\"It's not an 'S'. On my world it means Hope.\"\n\nLike water, hope is not the hardest force —\nbut it is the most persistent.\nIt finds every crack. It never stops.");
        C->BattleCry           = TEXT("HOPE NEVER DIES!");
        C->BattleCryColor      = FLinearColor(0.50f, 0.75f, 1.00f);
        C->SignatureMoves = {
            MakeMove(TEXT("Hope Slash"),
                TEXT("Water Principle: Not the hardest blow — the one that never stops.\nHope flows around every obstacle until it finds the way through."),
                FLinearColor(0.30f, 0.60f, 1.00f), 1.7f, EMoveType::HorizontalSlash),
            MakeMove(TEXT("Flowing Justice"),
                TEXT("Adaptability: Flow around your enemy's strength.\nFind the opening the way water finds cracks in solid rock."),
                FLinearColor(0.45f, 0.82f, 1.00f), 2.0f, EMoveType::DiagonalSlash),
            MakeMove(TEXT("Solar Surge"),
                TEXT("Persistent Energy: Kryptonian power channels endlessly.\nThe sun always rises — and so does the Man of Steel."),
                FLinearColor(0.70f, 0.90f, 1.00f), 2.4f, EMoveType::EnergyBlade),
            MakeMove(TEXT("Aerial Justice"),
                TEXT("Clarity of Spirit: Height brings perspective — a calm mind sees\nwhat others cannot. Strike from clarity, not from anger."),
                FLinearColor(0.20f, 0.50f, 0.92f), 2.6f, EMoveType::AerialAssault)
        };
        Out.Add(C);
    }

    // ── VEGETA — Fire ──────────────────────────────────────────────────────────
    {
        auto* C = Make();
        C->CharacterName       = TEXT("VEGETA");
        C->Title               = TEXT("Prince of All Saiyans");
        C->Origin              = TEXT("Dragon Ball Z  ·  Planet Vegeta");
        C->AssociatedScroll    = EScrollType::Fire;
        C->BodyColor           = FLinearColor(0.28f, 0.18f, 0.38f);
        C->AuraColor           = FLinearColor(0.60f, 0.12f, 0.80f);
        C->PowerLevel          = 9.f; C->Speed = 8.f; C->Technique = 9.5f;
        C->PhilosophyStatement = TEXT("\"I am the Prince of all Saiyans.\nMy pride is not ego — it is the fire that drives me\npast every limit that should have stopped me.\"\n\nFire does not negotiate. Fire only consumes.");
        C->BattleCry           = TEXT("IT'S OVER 9000!!!");
        C->BattleCryColor      = FLinearColor(1.00f, 0.70f, 0.10f);
        C->SignatureMoves = {
            MakeMove(TEXT("Galick Slash"),
                TEXT("Fire Timing: Strike at the exact moment of vulnerability.\nThe Prince sees weakness before the enemy knows it exists."),
                FLinearColor(0.72f, 0.10f, 0.92f), 1.4f, EMoveType::DiagonalSlash),
            MakeMove(TEXT("Royal Volley"),
                TEXT("Rhythm of Battle: Five strikes in three seconds.\nThe fire warrior never allows a breath between blows."),
                FLinearColor(0.85f, 0.22f, 0.72f), 2.1f, EMoveType::Combo),
            MakeMove(TEXT("Final Flash Cut"),
                TEXT("Total Commitment: Sacrifice all defense for absolute destruction.\nMusashi's lesson — commit completely, or do not act at all."),
                FLinearColor(1.00f, 0.90f, 0.10f), 2.9f, EMoveType::EnergyBlade),
            MakeMove(TEXT("Prince's Wrath"),
                TEXT("Pride as Fuel: Royal Saiyan pride converts every defeat into power.\nA prince does not fall — he transforms."),
                FLinearColor(0.90f, 0.28f, 0.10f), 2.3f, EMoveType::PowerStrike)
        };
        Out.Add(C);
    }

    // ── GOKU — Wind + Void ─────────────────────────────────────────────────────
    {
        auto* C = Make();
        C->CharacterName       = TEXT("SON GOKU");
        C->Title               = TEXT("Ultra Instinct  ·  Migatte no Gokui");
        C->Origin              = TEXT("Dragon Ball Z  ·  Saiyan raised on Earth");
        C->AssociatedScroll    = EScrollType::Void;
        C->BodyColor           = FLinearColor(0.82f, 0.76f, 0.72f);
        C->AuraColor           = FLinearColor(0.72f, 0.72f, 1.00f);
        C->PowerLevel          = 10.f; C->Speed = 10.f; C->Technique = 10.f;
        C->PhilosophyStatement = TEXT("\"I never fight for power or pride.\nI fight because I always want to be stronger\nthan I was the day before.\"\n\nThe Void contains no ego.\nOnly perfect, unthinking response.");
        C->BattleCry           = TEXT("KAAAAAMEEEEHAAAAMEEEEHAAAAA!!!");
        C->BattleCryColor      = FLinearColor(0.50f, 0.72f, 1.00f);
        C->SignatureMoves = {
            MakeMove(TEXT("Kaio-ken Slash"),
                TEXT("Wind Mastery: Having studied every school, Goku multiplies technique itself.\nThe wind warrior knows all paths — and takes the best one."),
                FLinearColor(1.00f, 0.22f, 0.10f), 1.9f, EMoveType::HorizontalSlash),
            MakeMove(TEXT("Dragon Fist Cut"),
                TEXT("All-Style Mastery: A technique that combines every school into one strike.\nThis is what the Wind scroll teaches — and what it becomes."),
                FLinearColor(0.20f, 0.62f, 1.00f), 2.2f, EMoveType::AerialAssault),
            MakeMove(TEXT("Ultra Instinct Strike"),
                TEXT("Void Principle: The body moves before the mind can slow it.\nPerfect technique requires no thought — only being."),
                FLinearColor(0.85f, 0.85f, 1.00f), 1.3f, EMoveType::PowerStrike),
            MakeMove(TEXT("Limitless Void Slash"),
                TEXT("True Mastery: Beyond Ultra Instinct — the Void where all technique\ndissolves into pure, perfect, limitless action."),
                FLinearColor::White, 3.2f, EMoveType::EnergyBlade)
        };
        Out.Add(C);
    }

    return Out;
}

// ── Helper ────────────────────────────────────────────────────────────────────

FSwordplayMove ABookOfFiveRingsGameMode::MakeMove(const FString& Name, const FString& Desc,
                                                   const FLinearColor& Color, float Duration,
                                                   EMoveType Type)
{
    FSwordplayMove M;
    M.MoveName    = Name;
    M.Description = Desc;
    M.EffectColor = Color;
    M.Duration    = Duration;
    M.MoveType    = Type;
    return M;
}
