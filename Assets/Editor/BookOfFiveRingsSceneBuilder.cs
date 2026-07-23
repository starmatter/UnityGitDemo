#if UNITY_EDITOR
using UnityEngine;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine.UI;
using System.IO;

/// <summary>
/// Editor utility that builds the Book of Five Rings demo scene from scratch.
///
/// Usage:  Menu → Book of Five Rings → Build Scene
///
/// What it creates:
///   • Main Camera  (dark cinematic background)
///   • DirectionalLight + RimLight
///   • CharacterStage  (capsule body + sword primitive + point light + aura PS)
///   • MainCanvas      (Background, border, all Text elements)
///   • ScaffoldEffects (five element ParticleSystems)
///   • Managers        (BookOfFiveRingsManager + GameBootstrapper on one GameObject)
///   • AmbientParticles
///
/// The scene is saved to Assets/Scenes/BookOfFiveRings.unity and added to
/// Build Settings automatically.
/// </summary>
public static class BookOfFiveRingsSceneBuilder
{
    const string ScenePath = "Assets/Scenes/BookOfFiveRings.unity";

    // ─────────────────────────────────────────────────────────────────────────

    [MenuItem("Book of Five Rings/Build Scene #&B")]
    public static void BuildScene()
    {
        // New empty scene
        var scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);

        SetupRenderSettings();
        var cam              = SetupCamera();
        SetupLighting();
        var characterStage   = SetupCharacterStage();
        var mainCanvas       = SetupCanvas();
        var scrollEffectsObj = SetupScrollEffects();
        var ambientPs        = SetupAmbientParticles();

        SetupManagers(mainCanvas, characterStage, scrollEffectsObj, ambientPs);

        // Ensure Scenes folder exists
        Directory.CreateDirectory("Assets/Scenes");
        EditorSceneManager.SaveScene(scene, ScenePath);

        // Add to build settings
        AddSceneToBuildSettings(ScenePath);

        Debug.Log("[Book of Five Rings] Scene built → " + ScenePath);
        EditorUtility.DisplayDialog("Done", "Scene created at:\n" + ScenePath +
            "\n\nPress Play to run the animated presentation.", "OK");
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Render settings
    // ─────────────────────────────────────────────────────────────────────────

    static void SetupRenderSettings()
    {
        RenderSettings.ambientMode  = UnityEngine.Rendering.AmbientMode.Flat;
        RenderSettings.ambientLight = new Color(0.08f, 0.06f, 0.10f);
        RenderSettings.fog          = false;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Camera
    // ─────────────────────────────────────────────────────────────────────────

    static Camera SetupCamera()
    {
        var go  = new GameObject("Main Camera");
        go.tag  = "MainCamera";
        var cam = go.AddComponent<Camera>();
        cam.clearFlags       = CameraClearFlags.SolidColor;
        cam.backgroundColor  = new Color(0.04f, 0.02f, 0.01f);
        cam.nearClipPlane    = 0.1f;
        cam.farClipPlane     = 100f;
        cam.fieldOfView      = 60f;
        go.transform.position = new Vector3(0f, 1.4f, -5.5f);
        go.transform.rotation = Quaternion.Euler(6f, 0f, 0f);
        go.AddComponent<AudioListener>();
        return cam;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Lighting
    // ─────────────────────────────────────────────────────────────────────────

    static void SetupLighting()
    {
        // Key light
        var key = new GameObject("KeyLight");
        var kl  = key.AddComponent<Light>();
        kl.type      = LightType.Directional;
        kl.color     = new Color(0.88f, 0.78f, 0.55f);
        kl.intensity = 1.6f;
        key.transform.rotation = Quaternion.Euler(45f, -30f, 0f);

        // Rim light
        var rim = new GameObject("RimLight");
        var rl  = rim.AddComponent<Light>();
        rl.type      = LightType.Directional;
        rl.color     = new Color(0.35f, 0.45f, 0.75f);
        rl.intensity = 0.7f;
        rim.transform.rotation = Quaternion.Euler(30f, 150f, 0f);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Character stage
    // ─────────────────────────────────────────────────────────────────────────

    static FighterController SetupCharacterStage()
    {
        var root = new GameObject("CharacterStage");
        root.transform.position = new Vector3(0f, 0f, 1.5f);

        // Body (capsule placeholder)
        var body = GameObject.CreatePrimitive(PrimitiveType.Capsule);
        body.name = "Body";
        body.transform.SetParent(root.transform, false);
        body.transform.localPosition = new Vector3(0f, 0.6f, 0f);
        body.transform.localScale    = new Vector3(0.75f, 1.15f, 0.75f);

        // Sword (thin cube)
        var sword = GameObject.CreatePrimitive(PrimitiveType.Cube);
        sword.name = "Sword";
        sword.transform.SetParent(root.transform, false);
        sword.transform.localPosition = new Vector3(0.55f, 1.25f, 0f);
        sword.transform.localScale    = new Vector3(0.055f, 1.25f, 0.035f);
        sword.transform.localRotation = Quaternion.Euler(0f, 0f, 20f);

        // Sword guard
        var guard = GameObject.CreatePrimitive(PrimitiveType.Cube);
        guard.name = "Guard";
        guard.transform.SetParent(sword.transform, false);
        guard.transform.localPosition = new Vector3(0f, -0.5f, 0f);
        guard.transform.localScale    = new Vector3(3f, 0.08f, 1.8f);

        // Character light (point)
        var lightGO = new GameObject("CharacterLight");
        lightGO.transform.SetParent(root.transform, false);
        lightGO.transform.localPosition = new Vector3(0f, 2.2f, -1.2f);
        var ptLight      = lightGO.AddComponent<Light>();
        ptLight.type     = LightType.Point;
        ptLight.color    = Color.white;
        ptLight.intensity = 3.5f;
        ptLight.range     = 12f;

        // Aura particle system
        var auraGO = new GameObject("Aura");
        auraGO.transform.SetParent(root.transform, false);
        auraGO.transform.localPosition = Vector3.zero;
        var aura = auraGO.AddComponent<ParticleSystem>();

        var am              = aura.main;
        am.startLifetime    = new ParticleSystem.MinMaxCurve(0.8f, 2.2f);
        am.startSpeed       = new ParticleSystem.MinMaxCurve(1.5f, 4f);
        am.startSize        = new ParticleSystem.MinMaxCurve(0.05f, 0.35f);
        am.startColor       = new Color(0.5f, 0.8f, 1f, 0.55f);
        am.maxParticles     = 250;
        am.simulationSpace  = ParticleSystemSimulationSpace.World;

        var ae = aura.emission;
        ae.rateOverTime = 55f;

        var shape = aura.shape;
        shape.shapeType = ParticleSystemShapeType.Sphere;
        shape.radius    = 0.6f;

        var col = aura.colorOverLifetime;
        col.enabled = true;
        var grad = new Gradient();
        grad.SetKeys(
            new[] { new GradientColorKey(Color.white, 0f), new GradientColorKey(Color.white, 1f) },
            new[] { new GradientAlphaKey(0.6f, 0f), new GradientAlphaKey(0f, 1f) }
        );
        col.color = new ParticleSystem.MinMaxGradient(grad);

        // Wire up FighterController
        var fc = root.AddComponent<FighterController>();
        fc.bodyObject    = body;
        fc.swordObject   = sword;
        fc.auraEffect    = aura;
        fc.characterLight = ptLight;

        // SwordplayAnimator (required by FighterController)
        root.AddComponent<SwordplayAnimator>();

        return fc;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Canvas
    // ─────────────────────────────────────────────────────────────────────────

    static Canvas SetupCanvas()
    {
        var canvasGO = new GameObject("MainCanvas");
        var canvas   = canvasGO.AddComponent<Canvas>();
        canvas.renderMode   = RenderMode.ScreenSpaceOverlay;
        canvas.sortingOrder = 10;

        var scaler = canvasGO.AddComponent<CanvasScaler>();
        scaler.uiScaleMode        = CanvasScaler.ScaleMode.ScaleWithScreenSize;
        scaler.referenceResolution = new Vector2(1080f, 1920f);
        scaler.matchWidthOrHeight  = 0.5f;

        canvasGO.AddComponent<GraphicRaycaster>();
        canvasGO.AddComponent<CanvasGroup>();

        // Background fill
        var bg   = CreateUIImage(canvasGO.transform, "Background",
            Vector2.zero, Vector2.one, Vector2.zero, new Color(0.04f, 0.02f, 0.01f));

        // Decorative border (semi-transparent)
        var border = CreateUIImage(canvasGO.transform, "ScrollBorder",
            new Vector2(0.03f, 0.03f), new Vector2(0.97f, 0.97f), Vector2.zero,
            new Color(0.55f, 0.38f, 0.08f, 0.22f));

        // Text elements — (name, anchorCenter, sizeDelta, fontSize, fontStyle, alignment, colour)
        MakeText(canvasGO.transform, "ScrollTitleText",
            new Vector2(0.50f, 0.82f), new Vector2(960f, 130f),
            "", 68, FontStyle.Bold, TextAnchor.MiddleCenter, Color.white);

        MakeText(canvasGO.transform, "JapaneseCharText",
            new Vector2(0.50f, 0.70f), new Vector2(440f, 130f),
            "", 52, FontStyle.Normal, TextAnchor.MiddleCenter, new Color(0.85f, 0.65f, 0.18f));

        MakeText(canvasGO.transform, "MusashiQuoteText",
            new Vector2(0.50f, 0.54f), new Vector2(920f, 170f),
            "", 26, FontStyle.Italic, TextAnchor.MiddleCenter, new Color(0.90f, 0.84f, 0.68f));

        MakeText(canvasGO.transform, "CharacterNameText",
            new Vector2(0.50f, 0.37f), new Vector2(920f, 90f),
            "", 54, FontStyle.Bold, TextAnchor.MiddleCenter, Color.white);

        MakeText(canvasGO.transform, "PrinciplesText",
            new Vector2(0.27f, 0.18f), new Vector2(520f, 320f),
            "", 24, FontStyle.Normal, TextAnchor.UpperLeft, new Color(0.80f, 0.92f, 0.80f));

        MakeText(canvasGO.transform, "PhilosophyText",
            new Vector2(0.73f, 0.18f), new Vector2(520f, 240f),
            "", 22, FontStyle.Italic, TextAnchor.UpperLeft, new Color(0.90f, 0.86f, 1.00f));

        return canvas;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Element particle effects
    // ─────────────────────────────────────────────────────────────────────────

    static ScrollEffect SetupScrollEffects()
    {
        var root = new GameObject("ScrollEffects");
        root.transform.position = new Vector3(0f, 2f, 3f);
        var se = root.AddComponent<ScrollEffect>();

        se.earthEffect = MakeElementParticles(root, "EarthPS",
            new Color(0.55f, 0.40f, 0.18f), 0.25f, 2.5f, 30f, 0.15f);

        se.waterEffect = MakeElementParticles(root, "WaterPS",
            new Color(0.25f, 0.60f, 1.00f), 0.18f, 3.0f, 45f, 0.12f);

        se.fireEffect  = MakeElementParticles(root, "FirePS",
            new Color(1.00f, 0.55f, 0.10f), 0.12f, 4.5f, 60f, 0.10f);

        se.windEffect  = MakeElementParticles(root, "WindPS",
            new Color(0.45f, 0.85f, 0.45f), 0.10f, 5.0f, 40f, 0.08f);

        se.voidEffect  = MakeElementParticles(root, "VoidPS",
            new Color(0.65f, 0.55f, 1.00f), 0.20f, 2.0f, 25f, 0.18f);

        // Stop all on creation; ScrollEffect.SetScrollEffect() plays the right one
        foreach (var ps in root.GetComponentsInChildren<ParticleSystem>())
            ps.Stop(true, ParticleSystemStopBehavior.StopEmittingAndClear);

        return se;
    }

    static ParticleSystem MakeElementParticles(GameObject parent, string name,
        Color color, float size, float speed, float rate, float lifetime)
    {
        var go = new GameObject(name);
        go.transform.SetParent(parent.transform, false);
        go.transform.localPosition = Vector3.zero;
        var ps = go.AddComponent<ParticleSystem>();

        var m = ps.main;
        m.startColor    = color;
        m.startSize     = new ParticleSystem.MinMaxCurve(size * 0.5f, size * 1.5f);
        m.startSpeed    = new ParticleSystem.MinMaxCurve(speed * 0.5f, speed);
        m.startLifetime = new ParticleSystem.MinMaxCurve(lifetime * 0.5f, lifetime * 1.5f);
        m.maxParticles  = 300;
        m.simulationSpace = ParticleSystemSimulationSpace.World;

        var e = ps.emission;
        e.rateOverTime = rate;

        var s = ps.shape;
        s.shapeType = ParticleSystemShapeType.Sphere;
        s.radius    = 4f;

        return ps;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Ambient dust / sparkle
    // ─────────────────────────────────────────────────────────────────────────

    static ParticleSystem SetupAmbientParticles()
    {
        var go = new GameObject("AmbientParticles");
        go.transform.position = new Vector3(0f, 3f, 4f);
        var ps = go.AddComponent<ParticleSystem>();

        var m = ps.main;
        m.startColor    = new Color(0.80f, 0.65f, 0.28f, 0.38f);
        m.startSize     = new ParticleSystem.MinMaxCurve(0.02f, 0.09f);
        m.startSpeed    = new ParticleSystem.MinMaxCurve(0.1f, 0.5f);
        m.startLifetime = new ParticleSystem.MinMaxCurve(3f, 6f);
        m.maxParticles  = 120;
        m.simulationSpace = ParticleSystemSimulationSpace.World;

        var e = ps.emission;
        e.rateOverTime = 14f;

        var s = ps.shape;
        s.shapeType = ParticleSystemShapeType.Box;
        s.scale     = new Vector3(12f, 5f, 4f);

        return ps;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Managers GameObject
    // ─────────────────────────────────────────────────────────────────────────

    static void SetupManagers(Canvas canvas, FighterController fighter,
                               ScrollEffect effects, ParticleSystem ambient)
    {
        var go = new GameObject("Managers");
        var mgr = go.AddComponent<BookOfFiveRingsManager>();
        go.AddComponent<GameBootstrapper>().manager = mgr;

        // Wire canvas references
        mgr.mainCanvas       = canvas;
        mgr.fighterController = fighter;
        mgr.scrollEffects    = effects;
        mgr.ambientParticles = ambient;

        var ct = canvas.transform;
        mgr.backgroundImage  = ct.Find("Background").GetComponent<Image>();
        mgr.scrollBorderImage = ct.Find("ScrollBorder").GetComponent<Image>();
        mgr.scrollTitleText  = ct.Find("ScrollTitleText").GetComponent<Text>();
        mgr.japaneseCharText = ct.Find("JapaneseCharText").GetComponent<Text>();
        mgr.musashiQuoteText = ct.Find("MusashiQuoteText").GetComponent<Text>();
        mgr.characterNameText = ct.Find("CharacterNameText").GetComponent<Text>();
        mgr.principlesText   = ct.Find("PrinciplesText").GetComponent<Text>();
        mgr.philosophyText   = ct.Find("PhilosophyText").GetComponent<Text>();
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  UI helpers
    // ─────────────────────────────────────────────────────────────────────────

    static Image CreateUIImage(Transform parent, string name,
        Vector2 anchorMin, Vector2 anchorMax, Vector2 sizeDelta, Color color)
    {
        var go  = new GameObject(name);
        go.transform.SetParent(parent, false);
        var img = go.AddComponent<Image>();
        img.color = color;

        var rt = go.GetComponent<RectTransform>();
        rt.anchorMin   = anchorMin;
        rt.anchorMax   = anchorMax;
        rt.sizeDelta   = sizeDelta;
        rt.anchoredPosition = Vector2.zero;
        return img;
    }

    static Text MakeText(Transform parent, string name,
        Vector2 anchorCenter, Vector2 sizeDelta,
        string content, int fontSize, FontStyle style,
        TextAnchor alignment, Color color)
    {
        var go = new GameObject(name);
        go.transform.SetParent(parent, false);

        var t = go.AddComponent<Text>();
        t.text      = content;
        t.fontSize  = fontSize;
        t.fontStyle = style;
        t.alignment = alignment;
        t.color     = color;
        t.font      = Resources.GetBuiltinResource<Font>("Arial.ttf");
        t.supportRichText = true;

        var rt = go.GetComponent<RectTransform>();
        rt.anchorMin        = anchorCenter;
        rt.anchorMax        = anchorCenter;
        rt.pivot            = new Vector2(0.5f, 0.5f);
        rt.anchoredPosition = Vector2.zero;
        rt.sizeDelta        = sizeDelta;
        return t;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Build settings
    // ─────────────────────────────────────────────────────────────────────────

    static void AddSceneToBuildSettings(string scenePath)
    {
        var existing = EditorBuildSettings.scenes;
        foreach (var s in existing)
            if (s.path == scenePath) return;   // already present

        var updated = new EditorBuildSettingsScene[existing.Length + 1];
        for (int i = 0; i < existing.Length; i++) updated[i] = existing[i];
        updated[existing.Length] = new EditorBuildSettingsScene(scenePath, true);
        EditorBuildSettings.scenes = updated;
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Info dialog
    // ─────────────────────────────────────────────────────────────────────────

    [MenuItem("Book of Five Rings/About This Project")]
    public static void ShowAbout()
    {
        EditorUtility.DisplayDialog(
            "Book of Five Rings — Animated Unity Demo",
            "An animated presentation of Miyamoto Musashi's\n" +
            "Go Rin No Sho (五輪書), written 1643 CE.\n\n" +
            "Each scroll is embodied by a legendary fighter:\n\n" +
            "  地  Earth Scroll  →  BROLY\n" +
            "      Foundation · Self-knowledge · Form\n\n" +
            "  水  Water Scroll  →  KAL-EL / SUPERMAN\n" +
            "      Adaptability · Persistence · Hope\n\n" +
            "  火  Fire Scroll   →  VEGETA\n" +
            "      Timing · Aggression · Royal Pride\n\n" +
            "  風  Wind Scroll   →  SON GOKU\n" +
            "      Versatility · Mastery of all schools\n\n" +
            "  空  Void Scroll   →  GOKU ULTRA INSTINCT\n" +
            "      Transcendence · Beyond thought · Perfect response\n\n" +
            "Run:  Book of Five Rings → Build Scene\n" +
            "then press Play.",
            "Got it"
        );
    }
}
#endif
