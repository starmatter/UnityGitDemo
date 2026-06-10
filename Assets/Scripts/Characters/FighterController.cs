using UnityEngine;
using System.Collections;

/// <summary>
/// Represents one fighter in the scene.
/// Handles character-swap, entrance animation, aura colour, and delegates
/// move execution to the attached SwordplayAnimator.
/// </summary>
[RequireComponent(typeof(SwordplayAnimator))]
public class FighterController : MonoBehaviour
{
    [Header("Body Parts")]
    public GameObject bodyObject;
    public GameObject swordObject;
    public Transform  swordTipOverride;   // optional; defaults to sword transform

    [Header("VFX")]
    public ParticleSystem auraEffect;
    public ParticleSystem impactEffect;
    public Light          characterLight;

    // Convenience property used by SwordplayAnimator
    public Vector3 SwordRoot
    {
        get
        {
            if (swordTipOverride) return swordTipOverride.position;
            if (swordObject)      return swordObject.transform.position + swordObject.transform.up * 0.6f;
            return transform.position + Vector3.up * 1.3f;
        }
    }

    private SwordplayAnimator  _animator;
    private CharacterData      _current;

    void Awake()
    {
        _animator = GetComponent<SwordplayAnimator>();
    }

    // ── Public API ───────────────────────────────────────────────────────────

    public void SetCharacter(CharacterData data)
    {
        _current = data;

        // Apply body tint
        if (bodyObject)
        {
            foreach (Renderer r in bodyObject.GetComponentsInChildren<Renderer>())
                r.material.color = data.bodyColor;
        }

        // Aura colour
        if (auraEffect)
        {
            var main = auraEffect.main;
            main.startColor = data.auraColor;
            float scale = Mathf.Clamp(data.powerLevel / 8f, 0.5f, 5f);
            auraEffect.transform.localScale = Vector3.one * scale;
        }

        // Character light
        if (characterLight)
        {
            characterLight.color     = data.auraColor;
            characterLight.intensity = Mathf.Clamp(data.powerLevel / 4f, 1f, 8f);
        }
    }

    public void PlayEntrance()
    {
        StartCoroutine(EntranceSequence());
    }

    public IEnumerator PerformMove(SwordplayMove move)
    {
        yield return _animator.ExecuteMove(move, this);
    }

    // ── Private ──────────────────────────────────────────────────────────────

    IEnumerator EntranceSequence()
    {
        // Pop-in from zero scale with overshoot
        transform.localScale = Vector3.zero;
        float elapsed = 0f;
        const float duration = 0.75f;

        while (elapsed < duration)
        {
            elapsed += Time.deltaTime;
            float t     = elapsed / duration;
            float scale = t < 0.7f
                ? Mathf.Sin(t / 0.7f * Mathf.PI * 0.5f) * 1.12f
                : Mathf.Lerp(1.12f, 1f, (t - 0.7f) / 0.3f);
            transform.localScale = Vector3.one * scale;
            yield return null;
        }

        transform.localScale = Vector3.one;

        if (auraEffect && !auraEffect.isPlaying) auraEffect.Play();

        if (_current != null)
            yield return PowerPulse(_current.auraColor, 1.4f);
    }

    IEnumerator PowerPulse(Color pulseColor, float duration)
    {
        if (!characterLight) yield break;

        float baseIntensity = characterLight.intensity;
        float elapsed       = 0f;

        while (elapsed < duration)
        {
            elapsed += Time.deltaTime;
            float t = Mathf.Sin(elapsed / duration * Mathf.PI);
            characterLight.intensity = baseIntensity + t * 4f;
            characterLight.color     = Color.Lerp(Color.white, pulseColor, t);
            yield return null;
        }

        characterLight.intensity = baseIntensity;
    }
}
