using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;

/// <summary>
/// Master sequence controller for the animated Book of Five Rings presentation.
///
/// Flow per scroll:
///   1. Background colour transition (element theme)
///   2. Scroll title + Japanese kanji animate in
///   3. Opening narration (typewriter reveal)
///   4. Key principles appear one by one with particle bursts
///   5. Musashi quote (dramatic flash-then-reveal)
///   6. Fighter enters with aura and power-up pulse
///   7. Fighter philosophy statement
///   8. Swordplay demonstration (each signature move)
///   9. Battle cry finale
///  10. Fade out → next scroll
/// </summary>
public class BookOfFiveRingsManager : MonoBehaviour
{
    // ── Scroll & character data (filled by GameBootstrapper at Awake) ────────
    [Header("Data (set by GameBootstrapper)")]
    public List<ScrollData> scrolls = new List<ScrollData>();
    public CharacterData    brolyData;
    public CharacterData    supermanData;
    public CharacterData    vegetaData;
    public CharacterData    gokuData;

    // ── UI references (wired in scene by BookOfFiveRingsSceneBuilder) ────────
    [Header("UI")]
    public Canvas     mainCanvas;
    public Image      backgroundImage;
    public Image      scrollBorderImage;
    public Text       scrollTitleText;
    public Text       japaneseCharText;
    public Text       musashiQuoteText;
    public Text       characterNameText;
    public Text       principlesText;
    public Text       philosophyText;

    // ── 3-D scene objects ────────────────────────────────────────────────────
    [Header("Scene")]
    public FighterController fighterController;
    public ScrollEffect       scrollEffects;
    public ParticleSystem     ambientParticles;

    // ── Timing ───────────────────────────────────────────────────────────────
    [Header("Timing (seconds)")]
    public float typewriterCharDelay  = 0.035f;
    public float typewriterFastDelay  = 0.015f;
    public float principleHoldTime   = 0.9f;
    public float transitionDuration  = 1.2f;

    private CanvasGroup _canvasGroup;

    // ─────────────────────────────────────────────────────────────────────────
    void Start()
    {
        _canvasGroup = mainCanvas.GetComponent<CanvasGroup>();
        if (!_canvasGroup) _canvasGroup = mainCanvas.gameObject.AddComponent<CanvasGroup>();

        StartCoroutine(RunPresentation());
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Master sequence
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator RunPresentation()
    {
        yield return ShowTitleCard();

        foreach (var scroll in scrolls)
            yield return PresentScroll(scroll);

        yield return ShowClosingMeditation();
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Title card
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator ShowTitleCard()
    {
        SetBackground(new Color(0.07f, 0.04f, 0.01f), new Color(0.25f, 0.14f, 0.03f));

        ClearAll();
        SetText(scrollTitleText, "THE BOOK OF FIVE RINGS", Color.white, 72);
        yield return TypeReveal(scrollTitleText, typewriterCharDelay * 0.8f);

        yield return new WaitForSeconds(0.6f);

        SetText(japaneseCharText, "五輪書\nGo Rin No Sho", new Color(0.9f, 0.7f, 0.2f), 48);
        yield return TypeReveal(japaneseCharText, typewriterFastDelay);

        yield return new WaitForSeconds(0.7f);

        SetText(musashiQuoteText,
            "\"There is nothing outside of yourself that can ever enable you\n" +
            "to get better, stronger, richer, quicker, or smarter.\n" +
            "Everything is within. Everything exists.\n" +
            "Seek nothing outside of yourself.\"\n\n" +
            "— Miyamoto Musashi, 1643",
            new Color(0.88f, 0.82f, 0.68f), 26);
        yield return TypeReveal(musashiQuoteText, typewriterFastDelay);

        yield return new WaitForSeconds(3.5f);
        yield return FadeCanvas(0f, transitionDuration);
        ClearAll();
        yield return FadeCanvas(1f, 0.3f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Scroll section
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator PresentScroll(ScrollData scroll)
    {
        // 1 ─ Environment
        yield return TransitionBackground(scroll.primaryColor, scroll.secondaryColor, transitionDuration);
        if (scrollEffects) scrollEffects.SetScrollEffect(scroll.scrollType);

        ClearAll();

        // 2 ─ Scroll title
        yield return AnimateScrollTitle(scroll);
        yield return new WaitForSeconds(0.5f);

        // 3 ─ Opening narration
        SetText(principlesText, scroll.openingNarration, scroll.textColor, 26);
        yield return TypeReveal(principlesText, typewriterFastDelay);
        yield return new WaitForSeconds(1f);

        // 4 ─ Key principles (bullet by bullet)
        yield return ShowPrinciples(scroll);

        // 5 ─ Musashi quote
        yield return ShowMusashiQuote(scroll);

        // 6-7 ─ Fighter introduction
        yield return IntroduceFighter(scroll);

        // 8-9 ─ Swordplay demonstration
        yield return DemonstrateSwordplay(scroll);

        // 10 ─ Transition out
        yield return FadeCanvas(0f, transitionDuration);
        ClearAll();
        if (scrollEffects) scrollEffects.StopAllEffects();
        yield return FadeCanvas(1f, 0.3f);
        yield return new WaitForSeconds(0.3f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Scroll title animation
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator AnimateScrollTitle(ScrollData scroll)
    {
        SetText(scrollTitleText, scroll.scrollName + " SCROLL", scroll.textColor, 72);
        SetText(japaneseCharText, scroll.japaneseCharacter, scroll.accentColor, 56);

        // Slide in from left
        RectTransform rt = scrollTitleText.GetComponent<RectTransform>();
        Vector2 finalPos = rt.anchoredPosition;
        Vector2 offLeft  = new Vector2(finalPos.x - Screen.width, finalPos.y);

        float elapsed = 0f;
        const float dur = 0.7f;

        while (elapsed < dur)
        {
            elapsed += Time.deltaTime;
            float t  = scroll.transitionCurve.Evaluate(elapsed / dur);
            rt.anchoredPosition = Vector2.Lerp(offLeft, finalPos, t);
            yield return null;
        }

        rt.anchoredPosition = finalPos;

        // Kanji pulse
        yield return PulsateColor(japaneseCharText, scroll.accentColor, Color.white, 1.8f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Key principles
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator ShowPrinciples(ScrollData scroll)
    {
        principlesText.text  = "Key Principles:\n\n";
        principlesText.color = scroll.textColor;

        foreach (string principle in scroll.principles)
        {
            principlesText.text += "◆  " + principle + "\n";
            if (scrollEffects) scrollEffects.BurstForPrinciple(scroll.accentColor);
            yield return new WaitForSeconds(principleHoldTime);
        }

        yield return new WaitForSeconds(0.8f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Musashi quote
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator ShowMusashiQuote(ScrollData scroll)
    {
        // Flash
        yield return FlashBackground(scroll.accentColor, 0.35f);

        SetText(musashiQuoteText,
            "\"" + scroll.musashiQuote + "\"\n\n— Miyamoto Musashi",
            new Color(0.95f, 0.9f, 0.75f), 28);

        yield return TypeReveal(musashiQuoteText, typewriterFastDelay);
        yield return new WaitForSeconds(2.5f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Fighter introduction
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator IntroduceFighter(ScrollData scroll)
    {
        CharacterData character = GetCharacterForScroll(scroll.scrollType);
        if (character == null) yield break;

        // Fighter name – bold, large
        SetText(characterNameText, character.characterName, character.auraColor, 58);
        yield return TypeReveal(characterNameText, typewriterCharDelay * 1.2f);

        // Title + origin
        SetText(philosophyText, character.title + "\n" + character.origin, scroll.textColor, 24);
        yield return TypeReveal(philosophyText, typewriterFastDelay);

        // Spawn fighter
        if (fighterController)
        {
            fighterController.SetCharacter(character);
            fighterController.PlayEntrance();
        }

        yield return new WaitForSeconds(1.8f);

        // Philosophy statement
        SetText(philosophyText, character.philosophyStatement, new Color(0.9f, 0.88f, 1f), 22);
        yield return TypeReveal(philosophyText, typewriterFastDelay);
        yield return new WaitForSeconds(2.2f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Swordplay demonstration
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator DemonstrateSwordplay(ScrollData scroll)
    {
        CharacterData character = GetCharacterForScroll(scroll.scrollType);
        if (character == null || !fighterController) yield break;

        SetText(principlesText, "Combat Demonstration:", scroll.accentColor, 30);
        yield return new WaitForSeconds(0.6f);

        foreach (SwordplayMove move in character.signatureMoves)
        {
            // Move name banner
            SetText(principlesText, "► " + move.moveName, move.effectColor, 32);

            // Execute animation
            yield return fighterController.PerformMove(move);

            // Scroll principle tied to this move
            SetText(philosophyText, move.description, new Color(0.88f, 0.85f, 1f), 22);
            yield return TypeReveal(philosophyText, typewriterFastDelay);
            yield return new WaitForSeconds(1.2f);
        }

        // Battle cry finale
        if (!string.IsNullOrEmpty(character.battleCry))
        {
            SetText(characterNameText, character.battleCry, character.battleCryColor, 60);
            yield return TypeReveal(characterNameText, typewriterCharDelay);
            yield return new WaitForSeconds(1.8f);
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Closing meditation
    // ─────────────────────────────────────────────────────────────────────────

    IEnumerator ShowClosingMeditation()
    {
        SetBackground(new Color(0.06f, 0.04f, 0.02f), new Color(0.18f, 0.12f, 0.04f));
        ClearAll();

        SetText(scrollTitleText, "THE WAY OF THE SWORD", Color.white, 64);
        yield return TypeReveal(scrollTitleText, typewriterCharDelay);

        yield return new WaitForSeconds(0.8f);

        SetText(musashiQuoteText,
            "\"Accept everything just the way it is.\"\n\n" +
            "— Miyamoto Musashi\n" +
            "Go Rin No Sho · Written 1643 CE",
            new Color(0.9f, 0.85f, 0.7f), 30);
        yield return TypeReveal(musashiQuoteText, typewriterFastDelay);

        yield return new WaitForSeconds(3f);

        SetText(principlesText,
            "地  Earth  —  BROLY\n" +
            "水  Water  —  KAL-EL  /  SUPERMAN\n" +
            "火  Fire   —  VEGETA\n" +
            "風  Wind   —  SON GOKU\n" +
            "空  Void   —  GOKU  ULTRA INSTINCT\n\n" +
            "\"Seek nothing outside of yourself.\"",
            new Color(0.8f, 0.9f, 0.8f), 26);
        yield return TypeReveal(principlesText, typewriterFastDelay);

        yield return new WaitForSeconds(6f);
        yield return FadeCanvas(0f, 2f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Utilities
    // ─────────────────────────────────────────────────────────────────────────

    CharacterData GetCharacterForScroll(ScrollData.ScrollType type)
    {
        return type switch
        {
            ScrollData.ScrollType.Earth => brolyData,
            ScrollData.ScrollType.Water => supermanData,
            ScrollData.ScrollType.Fire  => vegetaData,
            ScrollData.ScrollType.Wind  => gokuData,
            ScrollData.ScrollType.Void  => gokuData,
            _                           => null
        };
    }

    void SetText(Text t, string content, Color color, int size = 0)
    {
        if (!t) return;
        t.text  = content;
        t.color = color;
        if (size > 0) t.fontSize = size;
    }

    void ClearAll()
    {
        foreach (Text t in new[] { scrollTitleText, japaneseCharText, musashiQuoteText,
                                   characterNameText, principlesText, philosophyText })
        {
            if (t) t.text = "";
        }
    }

    IEnumerator TypeReveal(Text textComponent, float charDelay)
    {
        if (!textComponent) yield break;

        string full      = textComponent.text;
        textComponent.text = "";

        foreach (char c in full)
        {
            textComponent.text += c;
            if (c != ' ' && c != '\n')
                yield return new WaitForSeconds(charDelay);
        }
    }

    IEnumerator PulsateColor(Text t, Color a, Color b, float duration)
    {
        if (!t) yield break;
        float elapsed = 0f;

        while (elapsed < duration)
        {
            elapsed += Time.deltaTime;
            t.color  = Color.Lerp(a, b, Mathf.PingPong(elapsed * 2f, 1f));
            yield return null;
        }

        t.color = a;
    }

    IEnumerator FadeCanvas(float targetAlpha, float duration)
    {
        if (!_canvasGroup) yield break;

        float startAlpha = _canvasGroup.alpha;
        float elapsed    = 0f;

        while (elapsed < duration)
        {
            elapsed           += Time.deltaTime;
            _canvasGroup.alpha = Mathf.Lerp(startAlpha, targetAlpha, elapsed / duration);
            yield return null;
        }

        _canvasGroup.alpha = targetAlpha;
    }

    void SetBackground(Color primary, Color secondary)
    {
        if (backgroundImage) backgroundImage.color = primary;
        if (scrollBorderImage)
        {
            Color border = secondary;
            border.a = 0.25f;
            scrollBorderImage.color = border;
        }
    }

    IEnumerator TransitionBackground(Color primary, Color secondary, float duration)
    {
        if (!backgroundImage) yield break;

        Color startColor = backgroundImage.color;
        float elapsed    = 0f;

        while (elapsed < duration)
        {
            elapsed += Time.deltaTime;
            backgroundImage.color = Color.Lerp(startColor, primary, elapsed / duration);
            yield return null;
        }

        backgroundImage.color = primary;
        if (scrollBorderImage)
        {
            Color border = secondary;
            border.a = 0.28f;
            scrollBorderImage.color = border;
        }
    }

    IEnumerator FlashBackground(Color flashColor, float duration)
    {
        if (!backgroundImage) yield break;

        Color original = backgroundImage.color;
        float half     = duration * 0.5f;
        float elapsed  = 0f;

        while (elapsed < half)
        {
            elapsed += Time.deltaTime;
            backgroundImage.color = Color.Lerp(original, flashColor, elapsed / half);
            yield return null;
        }

        elapsed = 0f;
        while (elapsed < half)
        {
            elapsed += Time.deltaTime;
            backgroundImage.color = Color.Lerp(flashColor, original, elapsed / half);
            yield return null;
        }

        backgroundImage.color = original;
    }
}
