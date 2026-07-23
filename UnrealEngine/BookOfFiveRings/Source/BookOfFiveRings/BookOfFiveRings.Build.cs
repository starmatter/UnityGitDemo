using UnrealBuildTool;

public class BookOfFiveRings : ModuleRules
{
    public BookOfFiveRings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Niagara",
            "UMG",
            "Slate",
            "SlateCore",
            "ProceduralMeshComponent"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Allow Blueprint access to all public headers
        PublicIncludePaths.AddRange(new string[]
        {
            "BookOfFiveRings/Data",
            "BookOfFiveRings/Core",
            "BookOfFiveRings/Characters",
            "BookOfFiveRings/Animation",
            "BookOfFiveRings/UI"
        });
    }
}
