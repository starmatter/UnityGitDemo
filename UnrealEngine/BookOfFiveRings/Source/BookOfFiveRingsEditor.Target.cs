using UnrealBuildTool;

public class BookOfFiveRingsEditorTarget : TargetRules
{
    public BookOfFiveRingsEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion  = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("BookOfFiveRings");
    }
}
