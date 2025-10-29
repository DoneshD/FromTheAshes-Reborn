using UnrealBuildTool;

public class FromTheAshesReborn : ModuleRules
{
	public FromTheAshesReborn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"Slate",
			"SlateCore",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"HeadMountedDisplay", 
			"EnhancedInput",
			"GameplayTasks", 
			"GameplayTags",
			"GameplayAbilities", 
			"AIModule", 
			"AnimGraphRuntime", 
			"MotionWarping", 
			"MoviePlayer",
			"Niagara",
			"NavigationSystem",
			"ContextualAnimation",
			"UMG"
		});
	}
}
