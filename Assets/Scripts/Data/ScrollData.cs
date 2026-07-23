using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// Miyamoto Musashi's Go Rin No Sho — one scroll per ScriptableObject instance.
/// Five elements: Earth (Chi), Water (Sui), Fire (Ka), Wind (Fu), Void (Ku).
/// </summary>
[CreateAssetMenu(fileName = "NewScrollData", menuName = "Book of Five Rings/Scroll Data")]
public class ScrollData : ScriptableObject
{
    public enum ScrollType { Earth, Water, Fire, Wind, Void }

    [Header("Identity")]
    public ScrollType scrollType;
    public string scrollName;
    public string japaneseCharacter;   // e.g. "地\nChi"
    public string associatedCharacterName;

    [Header("Visual Theme")]
    public Color primaryColor   = Color.white;
    public Color secondaryColor = Color.gray;
    public Color textColor      = Color.white;
    public Color accentColor    = Color.yellow;

    [Header("Narration")]
    [TextArea(3, 8)]
    public string openingNarration;

    [TextArea(2, 5)]
    public string musashiQuote;

    [Header("Key Principles (shown as animated bullet list)")]
    public List<string> principles = new List<string>();

    [Header("Combat Philosophy")]
    [TextArea(2, 6)]
    public string combatPhilosophy;
    public List<string> keyTechniques = new List<string>();

    [Header("Timing")]
    public float displayDuration = 10f;
    public AnimationCurve transitionCurve = AnimationCurve.EaseInOut(0, 0, 1, 1);
}
