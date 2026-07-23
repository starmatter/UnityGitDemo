using UnityEngine;
using System.Collections.Generic;

/// <summary>
/// Fighter profile — one per character (Broly, Superman, Vegeta, Goku).
/// Each fighter embodies the philosophy of a specific scroll.
/// </summary>
[CreateAssetMenu(fileName = "NewCharacterData", menuName = "Book of Five Rings/Character Data")]
public class CharacterData : ScriptableObject
{
    [Header("Identity")]
    public string characterName;
    public string title;
    public string origin;
    public ScrollData.ScrollType associatedScroll;

    [Header("Visual")]
    public Color bodyColor    = Color.white;
    public Color auraColor    = Color.cyan;
    public Color hairColor    = Color.black;
    public float height       = 2f;

    [Header("Combat Stats (0–10)")]
    public float powerLevel   = 1f;
    public float speed        = 5f;
    public float technique    = 5f;

    [Header("Philosophy")]
    public string styleDescription;
    [TextArea(2, 5)]
    public string philosophyStatement;
    public string battleCry;
    public Color  battleCryColor = Color.yellow;

    [Header("Swordplay Move List")]
    public List<SwordplayMove> signatureMoves = new List<SwordplayMove>();
}

// ─────────────────────────────────────────────
//  Swordplay move descriptor (used by SwordplayAnimator)
// ─────────────────────────────────────────────
[System.Serializable]
public class SwordplayMove
{
    public string moveName;
    [TextArea(1, 3)]
    public string description;          // scroll principle tied to this move
    public Color  effectColor = Color.white;
    public float  duration    = 1.5f;
    public MoveType moveType  = MoveType.HorizontalSlash;

    public enum MoveType
    {
        HorizontalSlash,
        VerticalSlash,
        DiagonalSlash,
        Thrust,
        Combo,
        PowerStrike,
        AerialAssault,
        EnergyBlade
    }
}
