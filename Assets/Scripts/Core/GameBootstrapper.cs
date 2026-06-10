using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// Creates all ScrollData and CharacterData ScriptableObjects at runtime
/// and feeds them into BookOfFiveRingsManager.
///
/// Each of the five scrolls is mapped to a fighter whose combat style
/// embodies that scroll's philosophy:
///
///   地 Earth  → Broly          (raw foundation, limitless physical might)
///   水 Water  → Superman/Kal-El (persistent hope, fluid adaptability)
///   火 Fire   → Vegeta          (aggressive timing, royal pride)
///   風 Wind   → Goku SSJ/Base   (mastery of all schools)
///   空 Void   → Goku Ultra Inst.(beyond thought — pure perfect response)
/// </summary>
public class GameBootstrapper : MonoBehaviour
{
    public BookOfFiveRingsManager manager;

    void Awake()
    {
        if (!manager) manager = GetComponent<BookOfFiveRingsManager>();

        var scrolls    = BuildScrollData();
        var characters = BuildCharacterData();

        manager.scrolls = scrolls;

        foreach (var c in characters)
        {
            switch (c.associatedScroll)
            {
                case ScrollData.ScrollType.Earth: manager.brolyData   = c; break;
                case ScrollData.ScrollType.Water: manager.supermanData = c; break;
                case ScrollData.ScrollType.Fire:  manager.vegetaData   = c; break;
                case ScrollData.ScrollType.Void:  manager.gokuData     = c; break;
                // Wind also maps to Goku (same instance)
            }
        }
        // Wind scroll uses Goku data
        manager.gokuData = manager.gokuData ?? characters.Find(c => c.associatedScroll == ScrollData.ScrollType.Void);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  SCROLL DATA
    // ─────────────────────────────────────────────────────────────────────────

    static List<ScrollData> BuildScrollData()
    {
        var list = new List<ScrollData>();

        // ── 地  EARTH ─────────────────────────────────────────────────────────
        var earth = SO<ScrollData>();
        earth.scrollType              = ScrollData.ScrollType.Earth;
        earth.scrollName              = "EARTH";
        earth.japaneseCharacter       = "地\nChi";
        earth.associatedCharacterName = "Broly";
        earth.primaryColor            = new Color(0.38f, 0.22f, 0.08f);
        earth.secondaryColor          = new Color(0.18f, 0.10f, 0.03f);
        earth.textColor               = new Color(0.92f, 0.82f, 0.60f);
        earth.accentColor             = new Color(0.55f, 0.85f, 0.25f);
        earth.openingNarration        =
            "The Earth Scroll lays the foundation of all strategy.\n" +
            "Before technique, before speed, before power —\n" +
            "a warrior must know the ground on which he stands.";
        earth.musashiQuote            =
            "There is nothing outside of yourself that can ever enable you\n" +
            "to get better, stronger, richer, quicker, or smarter.\n" +
            "Everything is within. Everything exists.\n" +
            "Seek nothing outside of yourself.";
        earth.principles              = new List<string>
        {
            "Master the fundamentals before all advanced technique",
            "Know your own strengths — and your own limits",
            "The body itself is the first and greatest weapon",
            "Strength without form is energy wasted",
            "True power is born from complete self-mastery"
        };
        earth.combatPhilosophy        =
            "Strike from the earth. Every technique must be rooted. " +
            "An unstable warrior falls before he can defeat his enemy.";
        earth.keyTechniques           = new List<string>
            { "Earth Crusher", "Mountain Stance", "Bedrock Fist", "Foundation Strike" };
        earth.displayDuration         = 11f;
        list.Add(earth);

        // ── 水  WATER ─────────────────────────────────────────────────────────
        var water = SO<ScrollData>();
        water.scrollType              = ScrollData.ScrollType.Water;
        water.scrollName              = "WATER";
        water.japaneseCharacter       = "水\nSui";
        water.associatedCharacterName = "Superman / Kal-El";
        water.primaryColor            = new Color(0.08f, 0.26f, 0.58f);
        water.secondaryColor          = new Color(0.04f, 0.12f, 0.36f);
        water.textColor               = new Color(0.80f, 0.92f, 1.00f);
        water.accentColor             = new Color(0.35f, 0.70f, 1.00f);
        water.openingNarration        =
            "Water takes the shape of any vessel it enters.\n" +
            "The warrior's spirit must be the same — calm, clear,\n" +
            "and impossibly persistent.";
        water.musashiQuote            =
            "A river cuts through rock\n" +
            "not because of its power,\n" +
            "but because of its persistence.";
        water.principles              = new List<string>
        {
            "Adapt your technique to the moment and the enemy",
            "Persistence overcomes what force cannot",
            "A calm, untroubled spirit sees every opening",
            "Flow around resistance — never clash with it head-on",
            "Hope is the most persistent force in the universe"
        };
        water.combatPhilosophy        =
            "Flow like water. Where there is an opening, fill it. " +
            "Where there is a wall, find the crack. Water always finds its way.";
        water.keyTechniques           = new List<string>
            { "Hope Slash", "Flowing Justice", "Solar Surge", "Aerial Persistence" };
        water.displayDuration         = 11f;
        list.Add(water);

        // ── 火  FIRE ──────────────────────────────────────────────────────────
        var fire = SO<ScrollData>();
        fire.scrollType              = ScrollData.ScrollType.Fire;
        fire.scrollName              = "FIRE";
        fire.japaneseCharacter       = "火\nKa";
        fire.associatedCharacterName = "Vegeta";
        fire.primaryColor            = new Color(0.75f, 0.18f, 0.04f);
        fire.secondaryColor          = new Color(0.45f, 0.08f, 0.00f);
        fire.textColor               = new Color(1.00f, 0.90f, 0.55f);
        fire.accentColor             = new Color(1.00f, 0.60f, 0.10f);
        fire.openingNarration        =
            "The Fire Scroll governs the heat and rhythm of actual combat.\n" +
            "In battle, timing is everything.\n" +
            "Hesitation is defeat. A prince does not hesitate.";
        fire.musashiQuote            =
            "Accept your past without regret,\n" +
            "handle your present with confidence,\n" +
            "and face your future without fear.";
        fire.principles              = new List<string>
        {
            "Timing is the very essence of combat",
            "Attack with absolute commitment — no half-measures",
            "Never give the enemy space to breathe or recover",
            "Pride is the warrior's most powerful fuel",
            "A prince falls only when he stops believing he is a prince"
        };
        fire.combatPhilosophy        =
            "A true warrior attacks with the ferocity of fire. " +
            "Fire does not defend — fire only consumes.";
        fire.keyTechniques           = new List<string>
            { "Galick Slash", "Royal Volley", "Final Flash Cut", "Prince's Wrath" };
        fire.displayDuration         = 11f;
        list.Add(fire);

        // ── 風  WIND ──────────────────────────────────────────────────────────
        var wind = SO<ScrollData>();
        wind.scrollType              = ScrollData.ScrollType.Wind;
        wind.scrollName              = "WIND";
        wind.japaneseCharacter       = "風\nFū";
        wind.associatedCharacterName = "Goku";
        wind.primaryColor            = new Color(0.18f, 0.46f, 0.18f);
        wind.secondaryColor          = new Color(0.08f, 0.26f, 0.08f);
        wind.textColor               = new Color(0.80f, 1.00f, 0.80f);
        wind.accentColor             = new Color(0.40f, 0.90f, 0.40f);
        wind.openingNarration        =
            "The Wind Scroll teaches knowledge of other schools.\n" +
            "To know every style is to be boundless.\n" +
            "The wind touches every place and knows every path.";
        wind.musashiQuote            =
            "Master the divine techniques of the Art of Peace\n" +
            "and no enemy will dare to challenge you.";
        wind.principles              = new List<string>
        {
            "Study all schools, but commit fully to none",
            "Identify the weakness within every style of fighting",
            "The wind warrior adapts the instant contact is made",
            "Your mind is your most dangerous weapon",
            "Master yourself and you automatically master all styles"
        };
        wind.combatPhilosophy        =
            "The wind warrior has studied every technique. " +
            "In combat, he uses whatever the moment demands — unlimited adaptability.";
        wind.keyTechniques           = new List<string>
            { "Kaio-ken Slash", "Spirit Bomb Blade", "Dragon Fist Cut", "All-Style Mastery" };
        wind.displayDuration         = 11f;
        list.Add(wind);

        // ── 空  VOID ──────────────────────────────────────────────────────────
        var voidScroll = SO<ScrollData>();
        voidScroll.scrollType              = ScrollData.ScrollType.Void;
        voidScroll.scrollName              = "VOID";
        voidScroll.japaneseCharacter       = "空\nKū";
        voidScroll.associatedCharacterName = "Goku — Ultra Instinct";
        voidScroll.primaryColor            = new Color(0.10f, 0.07f, 0.18f);
        voidScroll.secondaryColor          = new Color(0.04f, 0.02f, 0.08f);
        voidScroll.textColor               = new Color(0.92f, 0.92f, 1.00f);
        voidScroll.accentColor             = new Color(0.75f, 0.75f, 1.00f);
        voidScroll.openingNarration        =
            "The Void Scroll transcends all the others.\n" +
            "Beyond technique. Beyond thought. Beyond the self.\n" +
            "The Void contains all possibilities — and none.";
        voidScroll.musashiQuote            = "Accept everything just the way it is.";
        voidScroll.principles              = new List<string>
        {
            "True mastery acts without conscious thought",
            "The Void contains all possibilities simultaneously",
            "Transcend the self and every limit dissolves",
            "When ego disappears, perfect technique emerges",
            "Ultra Instinct — the body moves before the mind can slow it"
        };
        voidScroll.combatPhilosophy        =
            "In the Void there is no technique — only perfect response. " +
            "The body acts before thought can slow it. " +
            "This is Ultra Instinct. This is the Void.";
        voidScroll.keyTechniques           = new List<string>
            { "Ultra Instinct Strike", "Autonomous Response", "Limitless Void Slash", "Perfect Transcendence" };
        voidScroll.displayDuration         = 13f;
        list.Add(voidScroll);

        return list;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  CHARACTER DATA
    // ─────────────────────────────────────────────────────────────────────────

    static List<CharacterData> BuildCharacterData()
    {
        var list = new List<CharacterData>();

        // ── BROLY — Earth ──────────────────────────────────────────────────────
        var broly = SO<CharacterData>();
        broly.characterName       = "BROLY";
        broly.title               = "The Legendary Super Saiyan";
        broly.origin              = "Dragon Ball Z";
        broly.associatedScroll    = ScrollData.ScrollType.Earth;
        broly.bodyColor           = new Color(0.55f, 0.38f, 0.18f);
        broly.auraColor           = new Color(0.20f, 0.92f, 0.20f);
        broly.hairColor           = new Color(0.15f, 0.80f, 0.15f);
        broly.height              = 2.4f;
        broly.powerLevel          = 10f;
        broly.speed               = 7f;
        broly.technique           = 5f;
        broly.styleDescription    = "Overwhelming ground-based force";
        broly.philosophyStatement =
            "\"There is no power greater than the power that was always\n" +
            "already inside you.\"\n\n" +
            "The Earth never yields. The foundation never breaks.\n" +
            "Broly IS the Earth Scroll made flesh.";
        broly.battleCry           = "BROLY!!!";
        broly.battleCryColor      = new Color(0.20f, 1.00f, 0.20f);
        broly.signatureMoves      = new List<SwordplayMove>
        {
            new SwordplayMove
            {
                moveName     = "Earth Crusher",
                description  = "Foundation Principle: Strike from rooted earth.\nThe mountain does not chase you — it simply falls on you.",
                effectColor  = new Color(0.35f, 0.85f, 0.10f),
                duration     = 1.9f,
                moveType     = SwordplayMove.MoveType.PowerStrike
            },
            new SwordplayMove
            {
                moveName     = "Bedrock Fist",
                description  = "Self-Knowledge: Raw power born from knowing one's limitless potential.\nEvery strike acknowledges the depth of the earth below.",
                effectColor  = new Color(0.25f, 0.92f, 0.30f),
                duration     = 1.6f,
                moveType     = SwordplayMove.MoveType.VerticalSlash
            },
            new SwordplayMove
            {
                moveName     = "Mountain Stance Combo",
                description  = "Form Before Power: Even the mightiest blow must flow from perfect form.\nFive strikes — each one is the mountain falling.",
                effectColor  = new Color(0.20f, 0.75f, 0.40f),
                duration     = 2.6f,
                moveType     = SwordplayMove.MoveType.Combo
            }
        };
        list.Add(broly);

        // ── SUPERMAN / KAL-EL — Water ──────────────────────────────────────────
        var superman = SO<CharacterData>();
        superman.characterName       = "KAL-EL";
        superman.title               = "Superman — The Man of Steel";
        superman.origin              = "DC Universe · Krypton";
        superman.associatedScroll    = ScrollData.ScrollType.Water;
        superman.bodyColor           = new Color(0.10f, 0.20f, 0.72f);
        superman.auraColor           = new Color(0.30f, 0.55f, 1.00f);
        superman.hairColor           = new Color(0.10f, 0.10f, 0.30f);
        superman.height              = 1.92f;
        superman.powerLevel          = 9f;
        superman.speed               = 9f;
        superman.technique           = 8f;
        superman.styleDescription    = "Fluid hope-driven adaptability";
        superman.philosophyStatement =
            "\"It's not an 'S'. On my world it means Hope.\"\n\n" +
            "Like water, hope is not the hardest force —\n" +
            "but it is the most persistent.\n" +
            "It finds every crack. It never stops.";
        superman.battleCry           = "HOPE NEVER DIES!";
        superman.battleCryColor      = new Color(0.50f, 0.75f, 1.00f);
        superman.signatureMoves      = new List<SwordplayMove>
        {
            new SwordplayMove
            {
                moveName     = "Hope Slash",
                description  = "Water Principle: Not the hardest blow — the one that never stops.\nHope flows around every obstacle until it finds the way through.",
                effectColor  = new Color(0.30f, 0.60f, 1.00f),
                duration     = 1.7f,
                moveType     = SwordplayMove.MoveType.HorizontalSlash
            },
            new SwordplayMove
            {
                moveName     = "Flowing Justice",
                description  = "Adaptability: Flow around your enemy's strength.\nFind the opening the way water finds cracks in solid rock.",
                effectColor  = new Color(0.45f, 0.82f, 1.00f),
                duration     = 2.0f,
                moveType     = SwordplayMove.MoveType.DiagonalSlash
            },
            new SwordplayMove
            {
                moveName     = "Solar Surge",
                description  = "Persistent Energy: Kryptonian power channels endlessly, like a river.\nThe sun always rises — and so does the Man of Steel.",
                effectColor  = new Color(0.70f, 0.90f, 1.00f),
                duration     = 2.4f,
                moveType     = SwordplayMove.MoveType.EnergyBlade
            },
            new SwordplayMove
            {
                moveName     = "Aerial Justice",
                description  = "Clarity of Spirit: Height brings perspective — a calm mind sees\nwhat others cannot. Strike from clarity, not from anger.",
                effectColor  = new Color(0.20f, 0.50f, 0.92f),
                duration     = 2.6f,
                moveType     = SwordplayMove.MoveType.AerialAssault
            }
        };
        list.Add(superman);

        // ── VEGETA — Fire ──────────────────────────────────────────────────────
        var vegeta = SO<CharacterData>();
        vegeta.characterName       = "VEGETA";
        vegeta.title               = "Prince of All Saiyans";
        vegeta.origin              = "Dragon Ball Z · Planet Vegeta";
        vegeta.associatedScroll    = ScrollData.ScrollType.Fire;
        vegeta.bodyColor           = new Color(0.28f, 0.18f, 0.38f);
        vegeta.auraColor           = new Color(0.60f, 0.12f, 0.80f);
        vegeta.hairColor           = new Color(0.08f, 0.08f, 0.10f);
        vegeta.height              = 1.65f;
        vegeta.powerLevel          = 9f;
        vegeta.speed               = 8f;
        vegeta.technique           = 9.5f;
        vegeta.styleDescription    = "Aggressive tactical precision with royal pride";
        vegeta.philosophyStatement =
            "\"I am the Prince of all Saiyans.\n" +
            "My pride is not ego — it is the fire that drives me\n" +
            "past every limit that should have stopped me.\"\n\n" +
            "Fire does not negotiate. Fire only consumes.";
        vegeta.battleCry           = "IT'S OVER 9000!!!";
        vegeta.battleCryColor      = new Color(1.00f, 0.70f, 0.10f);
        vegeta.signatureMoves      = new List<SwordplayMove>
        {
            new SwordplayMove
            {
                moveName     = "Galick Slash",
                description  = "Fire Timing: Strike at the exact moment of vulnerability.\nThe Prince sees weakness before the enemy knows it exists.",
                effectColor  = new Color(0.72f, 0.10f, 0.92f),
                duration     = 1.4f,
                moveType     = SwordplayMove.MoveType.DiagonalSlash
            },
            new SwordplayMove
            {
                moveName     = "Royal Volley",
                description  = "Rhythm of Battle: Five strikes in three seconds.\nThe fire warrior never allows a breath between blows.",
                effectColor  = new Color(0.85f, 0.22f, 0.72f),
                duration     = 2.1f,
                moveType     = SwordplayMove.MoveType.Combo
            },
            new SwordplayMove
            {
                moveName     = "Final Flash Cut",
                description  = "Total Commitment: Sacrifice all defense for absolute destruction.\nMusashi's lesson — commit completely, or do not act at all.",
                effectColor  = new Color(1.00f, 0.90f, 0.10f),
                duration     = 2.9f,
                moveType     = SwordplayMove.MoveType.EnergyBlade
            },
            new SwordplayMove
            {
                moveName     = "Prince's Wrath",
                description  = "Pride as Fuel: Royal Saiyan pride converts every defeat into\nmore power. A prince does not fall — he transforms.",
                effectColor  = new Color(0.90f, 0.28f, 0.10f),
                duration     = 2.3f,
                moveType     = SwordplayMove.MoveType.PowerStrike
            }
        };
        list.Add(vegeta);

        // ── GOKU — Wind + Void ─────────────────────────────────────────────────
        var goku = SO<CharacterData>();
        goku.characterName       = "SON GOKU";
        goku.title               = "Ultra Instinct · Migatte no Gokui";
        goku.origin              = "Dragon Ball Z · Saiyan raised on Earth";
        goku.associatedScroll    = ScrollData.ScrollType.Void;
        goku.bodyColor           = new Color(0.82f, 0.76f, 0.72f);
        goku.auraColor           = new Color(0.72f, 0.72f, 1.00f);
        goku.hairColor           = Color.white;
        goku.height              = 1.75f;
        goku.powerLevel          = 10f;
        goku.speed               = 10f;
        goku.technique           = 10f;
        goku.styleDescription    = "Void mastery — acts beyond conscious thought";
        goku.philosophyStatement =
            "\"I never fight for power or pride.\n" +
            "I fight because I always want to be stronger\n" +
            "than I was the day before.\"\n\n" +
            "The Void contains no ego.\n" +
            "Only perfect, unthinking response.";
        goku.battleCry           = "KAAAAAMEEEEHAAAAMEEEEHAAAAA!!!";
        goku.battleCryColor      = new Color(0.50f, 0.72f, 1.00f);
        goku.signatureMoves      = new List<SwordplayMove>
        {
            new SwordplayMove
            {
                moveName     = "Kaio-ken Slash",
                description  = "Wind Mastery: Having studied every school, Goku multiplies technique itself.\nThe wind warrior knows all paths — and takes the best one.",
                effectColor  = new Color(1.00f, 0.22f, 0.10f),
                duration     = 1.9f,
                moveType     = SwordplayMove.MoveType.HorizontalSlash
            },
            new SwordplayMove
            {
                moveName     = "Dragon Fist Cut",
                description  = "All-Style Mastery: A technique that combines every school into one strike.\nThis is what the Wind scroll teaches — and what it becomes.",
                effectColor  = new Color(0.20f, 0.62f, 1.00f),
                duration     = 2.2f,
                moveType     = SwordplayMove.MoveType.AerialAssault
            },
            new SwordplayMove
            {
                moveName     = "Ultra Instinct Strike",
                description  = "Void Principle: The body moves before the mind can slow it.\nPerfect technique requires no thought — only being.",
                effectColor  = new Color(0.85f, 0.85f, 1.00f),
                duration     = 1.3f,
                moveType     = SwordplayMove.MoveType.PowerStrike
            },
            new SwordplayMove
            {
                moveName     = "Limitless Void Slash",
                description  = "True Mastery: Beyond Ultra Instinct — the Void where all technique\ndissolves into pure, perfect, limitless action.",
                effectColor  = Color.white,
                duration     = 3.2f,
                moveType     = SwordplayMove.MoveType.EnergyBlade
            }
        };
        list.Add(goku);

        return list;
    }

    // Shortcut — avoids writing ScriptableObject.CreateInstance<T>() repeatedly
    static T SO<T>() where T : ScriptableObject => ScriptableObject.CreateInstance<T>();
}
