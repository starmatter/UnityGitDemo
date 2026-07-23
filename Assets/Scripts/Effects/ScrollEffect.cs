using UnityEngine;

/// <summary>
/// Manages element-specific ambient particle effects.
/// Each scroll element (Earth/Water/Fire/Wind/Void) has its own particle system.
/// Call SetScrollEffect() when transitioning between scrolls.
/// </summary>
public class ScrollEffect : MonoBehaviour
{
    [Header("Element Particle Systems")]
    public ParticleSystem earthEffect;   // dust / stone shards
    public ParticleSystem waterEffect;   // rippling droplets
    public ParticleSystem fireEffect;    // embers and sparks
    public ParticleSystem windEffect;    // leaves / energy wisps
    public ParticleSystem voidEffect;    // dark energy motes

    [Header("Principle Burst")]
    public int principleBurstCount = 12;

    private ParticleSystem _active;

    // ── Public API ───────────────────────────────────────────────────────────

    public void SetScrollEffect(ScrollData.ScrollType scrollType)
    {
        if (_active) _active.Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);

        _active = scrollType switch
        {
            ScrollData.ScrollType.Earth => earthEffect,
            ScrollData.ScrollType.Water => waterEffect,
            ScrollData.ScrollType.Fire  => fireEffect,
            ScrollData.ScrollType.Wind  => windEffect,
            ScrollData.ScrollType.Void  => voidEffect,
            _                           => null
        };

        if (_active) _active.Play();
    }

    /// <summary>Burst a small spray of particles each time a principle is revealed.</summary>
    public void BurstForPrinciple(Color color)
    {
        if (!_active) return;

        var mainModule       = _active.main;
        mainModule.startColor = color;

        var emitParams = new ParticleSystem.EmitParams();
        emitParams.startColor = color;
        _active.Emit(emitParams, principleBurstCount);
    }

    public void StopAllEffects()
    {
        if (_active) _active.Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);
        _active = null;
    }
}
