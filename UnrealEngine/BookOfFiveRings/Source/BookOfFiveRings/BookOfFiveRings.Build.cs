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
            "EnhancedInput",
            "Niagara",
            "UMG",
            "Slate",
            "SlateCore",
            "ProceduralMeshComponent"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Add the module root so subdirectory includes like "Data/Foo.h" resolve correctly
        PublicIncludePaths.Add(ModuleDirectory);
    }
}
