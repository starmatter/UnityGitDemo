using UnityEngine;
using System.Collections;

/// <summary>
/// Drives all sword-trail and body-motion animations for a fighter.
/// Dispatches to per-move-type coroutines; each creates its own
/// LineRenderer trail and destroys it when done.
/// </summary>
[RequireComponent(typeof(FighterController))]
public class SwordplayAnimator : MonoBehaviour
{
    [Header("Trail Settings")]
    public float trailStartWidth = 0.12f;
    public float trailEndWidth   = 0.02f;
    public float trailFadeTime   = 0.35f;

    // Entry point called by FighterController.PerformMove
    public IEnumerator ExecuteMove(SwordplayMove move, FighterController fighter)
    {
        switch (move.moveType)
        {
            case SwordplayMove.MoveType.HorizontalSlash: yield return HorizontalSlash(move, fighter); break;
            case SwordplayMove.MoveType.VerticalSlash:   yield return VerticalSlash  (move, fighter); break;
            case SwordplayMove.MoveType.DiagonalSlash:   yield return DiagonalSlash  (move, fighter); break;
            case SwordplayMove.MoveType.Thrust:           yield return Thrust         (move, fighter); break;
            case SwordplayMove.MoveType.Combo:            yield return ComboSequence  (move, fighter); break;
            case SwordplayMove.MoveType.PowerStrike:      yield return PowerStrike    (move, fighter); break;
            case SwordplayMove.MoveType.AerialAssault:    yield return AerialAssault  (move, fighter); break;
            case SwordplayMove.MoveType.EnergyBlade:      yield return EnergyBlade    (move, fighter); break;
            default:                                       yield return HorizontalSlash(move, fighter); break;
        }
    }

    // ── Individual move animations ──────────────────────────────────────────

    IEnumerator HorizontalSlash(SwordplayMove move, FighterController fighter)
    {
        var trail = MakeTrail(move.effectColor);
        Vector3 pivot = fighter.SwordRoot;
        float radius  = 1.6f;
        float elapsed = 0f;

        while (elapsed < move.duration)
        {
            elapsed += Time.deltaTime;
            float t     = elapsed / move.duration;
            float angle = Mathf.Lerp(-90f, 90f, t) * Mathf.Deg2Rad;

            Vector3 tip = pivot + new Vector3(Mathf.Cos(angle) * radius, 0f, Mathf.Sin(angle) * radius);
            UpdateTrail(trail, pivot, tip, t, move.effectColor);

            fighter.transform.rotation = Quaternion.Euler(0f, Mathf.Sin(t * Mathf.PI) * 20f, 0f);
            yield return null;
        }

        fighter.transform.rotation = Quaternion.identity;
        yield return FadeAndDestroy(trail, trailFadeTime);
    }

    IEnumerator VerticalSlash(SwordplayMove move, FighterController fighter)
    {
        var trail  = MakeTrail(move.effectColor);
        Vector3 pivot = fighter.SwordRoot;
        float radius  = 1.9f;
        float elapsed = 0f;

        while (elapsed < move.duration)
        {
            elapsed += Time.deltaTime;
            float t     = elapsed / move.duration;
            float angle = Mathf.Lerp(90f, -90f, t) * Mathf.Deg2Rad;

            Vector3 tip = pivot + new Vector3(0f, Mathf.Sin(angle) * radius, Mathf.Cos(angle) * radius * 0.3f);
            UpdateTrail(trail, pivot, tip, t, move.effectColor);
            yield return null;
        }

        yield return FadeAndDestroy(trail, trailFadeTime);
    }

    IEnumerator DiagonalSlash(SwordplayMove move, FighterController fighter)
    {
        var trail  = MakeTrail(move.effectColor);
        Vector3 pivot = fighter.SwordRoot;
        Vector3 start = pivot + new Vector3(-1.3f, 1.6f, 0f);
        Vector3 end   = pivot + new Vector3( 1.3f, -0.5f, 0.4f);
        float elapsed = 0f;

        while (elapsed < move.duration)
        {
            elapsed += Time.deltaTime;
            float t   = elapsed / move.duration;
            Vector3 tip = Vector3.Lerp(start, end, Mathf.SmoothStep(0f, 1f, t));
            UpdateTrail(trail, pivot, tip, t, move.effectColor);
            yield return null;
        }

        yield return FadeAndDestroy(trail, trailFadeTime);
    }

    IEnumerator Thrust(SwordplayMove move, FighterController fighter)
    {
        Vector3 origin = fighter.transform.position;
        Vector3 target = origin + fighter.transform.forward * 2.5f;
        var trail      = MakeTrail(move.effectColor);
        float half     = move.duration * 0.5f;
        float elapsed  = 0f;

        // lunge forward
        while (elapsed < half)
        {
            elapsed += Time.deltaTime;
            float t = elapsed / half;
            fighter.transform.position = Vector3.Lerp(origin, target, Mathf.SmoothStep(0f, 1f, t));
            UpdateTrail(trail, fighter.SwordRoot, fighter.SwordRoot + fighter.transform.forward * 1.5f, t, move.effectColor);
            yield return null;
        }

        // pull back
        elapsed = 0f;
        while (elapsed < half)
        {
            elapsed += Time.deltaTime;
            float t = elapsed / half;
            fighter.transform.position = Vector3.Lerp(target, origin, Mathf.SmoothStep(0f, 1f, t));
            UpdateTrail(trail, fighter.SwordRoot, fighter.SwordRoot + fighter.transform.forward * (1.5f - t), t, move.effectColor);
            yield return null;
        }

        fighter.transform.position = origin;
        yield return FadeAndDestroy(trail, trailFadeTime);
    }

    IEnumerator ComboSequence(SwordplayMove move, FighterController fighter)
    {
        int hits       = 5;
        float hitTime  = move.duration / hits;

        for (int i = 0; i < hits; i++)
        {
            float startAngle = (i % 2 == 0) ? -65f : 65f;
            float endAngle   = -startAngle;
            Color hitColor   = Color.Lerp(move.effectColor, Color.white, i * 0.1f);
            var trail        = MakeTrail(hitColor);
            float elapsed    = 0f;
            float yOff       = i * 0.22f - 0.44f;
            Vector3 pivot    = fighter.SwordRoot + Vector3.up * yOff;

            while (elapsed < hitTime)
            {
                elapsed += Time.deltaTime;
                float t     = elapsed / hitTime;
                float angle = Mathf.Lerp(startAngle, endAngle, t) * Mathf.Deg2Rad;
                Vector3 tip = pivot + new Vector3(Mathf.Cos(angle) * 1.5f, Mathf.Sin(angle) * 0.5f, 0f);
                UpdateTrail(trail, pivot, tip, t, hitColor);
                yield return null;
            }

            yield return FadeAndDestroy(trail, 0.08f);
        }
    }

    IEnumerator PowerStrike(SwordplayMove move, FighterController fighter)
    {
        // ── Wind-up ──────────────────────────────
        float windUp  = 0.45f;
        float elapsed = 0f;

        while (elapsed < windUp)
        {
            elapsed += Time.deltaTime;
            float t = elapsed / windUp;
            fighter.transform.localScale = Vector3.one * Mathf.Lerp(1f, 0.68f, t);
            yield return null;
        }

        // ── Explosive strike ──────────────────────
        var trail    = MakeTrail(move.effectColor);
        trail.startWidth = trailStartWidth * 2.5f;
        trail.endWidth   = trailStartWidth * 0.8f;
        elapsed = 0f;
        float strikeTime = move.duration * 0.45f;

        while (elapsed < strikeTime)
        {
            elapsed += Time.deltaTime;
            float t     = elapsed / strikeTime;
            float scale = Mathf.Lerp(0.68f, 1.35f, Mathf.Pow(t, 0.25f));
            fighter.transform.localScale = Vector3.one * scale;

            float angle = (t * 270f - 135f) * Mathf.Deg2Rad;
            Vector3 tip = fighter.SwordRoot + new Vector3(Mathf.Cos(angle) * 2.2f, Mathf.Sin(angle) * 1.6f, 0f);
            UpdateTrail(trail, fighter.SwordRoot, tip, t, move.effectColor);
            yield return null;
        }

        // ── Settle ────────────────────────────────
        elapsed = 0f;
        float settleTime = 0.28f;
        while (elapsed < settleTime)
        {
            elapsed += Time.deltaTime;
            fighter.transform.localScale = Vector3.one * Mathf.Lerp(1.35f, 1f, elapsed / settleTime);
            yield return null;
        }

        fighter.transform.localScale = Vector3.one;
        yield return FadeAndDestroy(trail, 0.5f);
    }

    IEnumerator AerialAssault(SwordplayMove move, FighterController fighter)
    {
        Vector3 groundPos = fighter.transform.position;
        Vector3 peakPos   = groundPos + Vector3.up * 3.2f;

        // leap up
        float elapsed = 0f;
        while (elapsed < 0.38f)
        {
            elapsed += Time.deltaTime;
            float t = elapsed / 0.38f;
            fighter.transform.position = Vector3.Lerp(groundPos, peakPos, Mathf.Sin(t * Mathf.PI * 0.5f));
            yield return null;
        }

        // aerial spin-slashes
        var trail   = MakeTrail(move.effectColor);
        elapsed = 0f;
        float spinTime = move.duration * 0.6f;

        while (elapsed < spinTime)
        {
            elapsed += Time.deltaTime;
            float t = elapsed / spinTime;

            fighter.transform.rotation = Quaternion.Euler(0f, t * 720f, 0f);
            fighter.transform.position = Vector3.Lerp(peakPos, groundPos, t * t);

            float spinRad = t * 720f * Mathf.Deg2Rad;
            Vector3 tip   = fighter.transform.position
                          + new Vector3(Mathf.Cos(spinRad) * 1.6f, 0.4f - t * 0.8f, Mathf.Sin(spinRad) * 0.5f);

            UpdateTrail(trail, fighter.transform.position + Vector3.up * 0.8f, tip, t, move.effectColor);
            yield return null;
        }

        // land
        elapsed = 0f;
        while (elapsed < 0.28f)
        {
            elapsed += Time.deltaTime;
            float t = elapsed / 0.28f;
            fighter.transform.position = Vector3.Lerp(fighter.transform.position, groundPos, t);
            yield return null;
        }

        fighter.transform.position = groundPos;
        fighter.transform.rotation = Quaternion.identity;
        yield return FadeAndDestroy(trail, 0.4f);
    }

    IEnumerator EnergyBlade(SwordplayMove move, FighterController fighter)
    {
        var trail  = MakeTrail(move.effectColor);
        trail.startWidth = 0.05f;

        // ── Energy gather (spiralling in) ────────
        float buildUp = 0.55f;
        float elapsed = 0f;
        Vector3 swordRoot = fighter.SwordRoot;

        while (elapsed < buildUp)
        {
            elapsed += Time.deltaTime;
            float t           = elapsed / buildUp;
            float spiralAngle = t * 720f * Mathf.Deg2Rad;
            float radius      = Mathf.Lerp(2.2f, 0.05f, t);

            Vector3 gather = swordRoot + new Vector3(
                Mathf.Cos(spiralAngle) * radius,
                Mathf.Sin(spiralAngle) * radius * 0.5f,
                0f
            );

            trail.startWidth = Mathf.Lerp(0.05f, 0.55f, t);
            trail.SetPosition(0, swordRoot);
            trail.SetPosition(1, gather);
            SetTrailAlpha(trail, t);
            yield return null;
        }

        // ── Release beam ─────────────────────────
        trail.startWidth = 0.55f;
        trail.endWidth   = 0.08f;
        elapsed = 0f;
        float releaseTime  = move.duration * 0.5f;
        Vector3 releaseEnd = swordRoot + fighter.transform.forward * 9f + Vector3.up * 0.4f;

        while (elapsed < releaseTime)
        {
            elapsed += Time.deltaTime;
            float t = elapsed / releaseTime;

            trail.startWidth = Mathf.Lerp(0.55f, 0.02f, t);
            trail.SetPosition(0, swordRoot);
            trail.SetPosition(1, Vector3.Lerp(swordRoot + Vector3.up, releaseEnd, t));

            // colour pulse
            Color pulse = Color.Lerp(move.effectColor, Color.white, Mathf.Abs(Mathf.Sin(t * Mathf.PI * 5f)));
            trail.startColor = pulse;
            trail.endColor   = new Color(pulse.r, pulse.g, pulse.b, 0f);
            yield return null;
        }

        yield return FadeAndDestroy(trail, 0.6f);
    }

    // ── Trail helpers ────────────────────────────────────────────────────────

    LineRenderer MakeTrail(Color color)
    {
        var go = new GameObject("SwordTrail");
        var lr = go.AddComponent<LineRenderer>();
        lr.positionCount = 2;
        lr.startWidth    = trailStartWidth;
        lr.endWidth      = trailEndWidth;
        lr.startColor    = color;
        lr.endColor      = new Color(color.r, color.g, color.b, 0f);
        lr.material      = new Material(Shader.Find("Sprites/Default"));
        lr.useWorldSpace = true;
        return lr;
    }

    void UpdateTrail(LineRenderer lr, Vector3 start, Vector3 end, float t, Color color)
    {
        if (!lr) return;
        lr.SetPosition(0, start);
        lr.SetPosition(1, end);
        float alpha = Mathf.Sin(t * Mathf.PI);
        lr.startColor = new Color(color.r, color.g, color.b, alpha);
        lr.endColor   = new Color(color.r, color.g, color.b, 0f);
    }

    void SetTrailAlpha(LineRenderer lr, float alpha)
    {
        Color s = lr.startColor; s.a = alpha; lr.startColor = s;
        Color e = lr.endColor;   e.a = 0f;    lr.endColor   = e;
    }

    IEnumerator FadeAndDestroy(LineRenderer lr, float duration)
    {
        if (!lr) yield break;
        float elapsed     = 0f;
        Color startColor  = lr.startColor;

        while (elapsed < duration)
        {
            elapsed += Time.deltaTime;
            float a = Mathf.Lerp(startColor.a, 0f, elapsed / duration);
            lr.startColor = new Color(startColor.r, startColor.g, startColor.b, a);
            yield return null;
        }

        if (lr && lr.gameObject) Destroy(lr.gameObject);
    }
}
