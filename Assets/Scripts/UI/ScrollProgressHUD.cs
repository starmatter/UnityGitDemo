using UnityEngine;
using UnityEngine.UI;
using System.Collections;

/// <summary>
/// Displays a five-dot progress indicator at the top of the screen,
/// one dot per scroll (Earth → Water → Fire → Wind → Void).
/// The active dot pulses in the scroll's accent colour.
/// </summary>
public class ScrollProgressHUD : MonoBehaviour
{
    [Header("Layout")]
    public float dotSize       = 22f;
    public float dotSpacing    = 48f;
    public Color inactiveColor = new Color(0.35f, 0.30f, 0.25f, 0.6f);

    private Image[]  _dots   = new Image[5];
    private Text[]   _labels = new Text[5];
    private Canvas   _canvas;
    private int      _current = -1;

    static readonly string[] Labels = { "地", "水", "火", "風", "空" };

    // ─────────────────────────────────────────────────────────────────────────
    void Awake()
    {
        _canvas = GetComponent<Canvas>();
        if (!_canvas) _canvas = gameObject.AddComponent<Canvas>();
        _canvas.renderMode   = RenderMode.ScreenSpaceOverlay;
        _canvas.sortingOrder = 20;

        if (!GetComponent<CanvasScaler>())
        {
            var cs = gameObject.AddComponent<CanvasScaler>();
            cs.uiScaleMode        = CanvasScaler.ScaleMode.ScaleWithScreenSize;
            cs.referenceResolution = new Vector2(1080f, 1920f);
            cs.matchWidthOrHeight  = 0.5f;
        }

        BuildDots();
    }

    // ─────────────────────────────────────────────────────────────────────────

    void BuildDots()
    {
        var container = new GameObject("DotContainer");
        container.transform.SetParent(transform, false);
        var rt = container.AddComponent<RectTransform>();
        rt.anchorMin        = new Vector2(0.5f, 1f);
        rt.anchorMax        = new Vector2(0.5f, 1f);
        rt.pivot            = new Vector2(0.5f, 1f);
        rt.anchoredPosition = new Vector2(0f, -20f);
        rt.sizeDelta        = new Vector2(dotSpacing * 5f, dotSize + 28f);

        for (int i = 0; i < 5; i++)
        {
            float x = (i - 2f) * dotSpacing;

            // Dot circle
            var dotGO = new GameObject("Dot_" + i);
            dotGO.transform.SetParent(container.transform, false);
            var drt = dotGO.AddComponent<RectTransform>();
            drt.anchoredPosition = new Vector2(x, -dotSize * 0.5f);
            drt.sizeDelta        = new Vector2(dotSize, dotSize);

            _dots[i]       = dotGO.AddComponent<Image>();
            _dots[i].color = inactiveColor;

            // Kanji label below dot
            var labelGO = new GameObject("Label_" + i);
            labelGO.transform.SetParent(container.transform, false);
            var lrt = labelGO.AddComponent<RectTransform>();
            lrt.anchoredPosition = new Vector2(x, -dotSize * 1.8f);
            lrt.sizeDelta        = new Vector2(dotSpacing, 26f);

            _labels[i]           = labelGO.AddComponent<Text>();
            _labels[i].text      = Labels[i];
            _labels[i].font      = Resources.GetBuiltinResource<Font>("Arial.ttf");
            _labels[i].fontSize  = 20;
            _labels[i].alignment = TextAnchor.MiddleCenter;
            _labels[i].color     = inactiveColor;
        }
    }

    // ─────────────────────────────────────────────────────────────────────────

    /// <summary>Advance the HUD to show scrollIndex as the active scroll.</summary>
    public void SetActiveScroll(int scrollIndex, Color accentColor)
    {
        if (_current >= 0 && _current < 5)
        {
            StopAllCoroutines();
            _dots[_current].color   = inactiveColor;
            _labels[_current].color = inactiveColor;
        }

        _current = Mathf.Clamp(scrollIndex, 0, 4);
        _labels[_current].color = accentColor;
        StartCoroutine(PulseDot(_current, accentColor));
    }

    IEnumerator PulseDot(int index, Color color)
    {
        while (true)
        {
            float elapsed = 0f;
            while (elapsed < 1.2f)
            {
                elapsed += Time.deltaTime;
                float t = Mathf.Sin(elapsed / 1.2f * Mathf.PI * 2f) * 0.5f + 0.5f;
                _dots[index].color = Color.Lerp(color * 0.5f, color, t);
                yield return null;
            }
        }
    }
}
