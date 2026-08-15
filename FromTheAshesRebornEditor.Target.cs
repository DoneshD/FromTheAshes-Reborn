using UnrealBuildTool;
using System.Collections.Generic;

public class FromTheAshesRebornEditorTarget : TargetRules
{
    public FromTheAshesRebornEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.Add("FromTheAshesReborn");
    }
}