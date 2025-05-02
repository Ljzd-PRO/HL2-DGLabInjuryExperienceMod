"GameMenu"
{
	"2"
	{
		"label" "#GameUI_GameMenu_ResumeGame"
		"command" "ResumeGame"
		"InGameOrder" "10"
		"OnlyInGame" "1"
	}
	"5"	
	{
		"label" "#GameUI_GameMenu_NewGame"
		"command" "OpenNewGameDialog"
		"InGameOrder" "20"
		"notmulti" "1"
		"NotInGame" "1"
	}
	"6"
	{
		"label" "#GameUI_GameMenu_LoadGame"
		"command" "OpenLoadGameDialog"
		"InGameOrder" "30"
		"notmulti" "1"
	}
	"7"
	{
		"label" "#GameUI_GameMenu_SaveGame"
		"command" "OpenSaveGameDialog"
		"InGameOrder" "40"
		"notmulti" "1"
		"OnlyInGame" "1"
	}
	"7_5"
	{
		"label" "#GameUI_GameMenu_ActivateVR"
		"command" "engine vr_activate"
		"InGameOrder" "50"
		"OnlyWhenVREnabled" "1"
		"OnlyWhenVRInactive" "1"
	}
	"7_6"
	{
		"label" "#GameUI_GameMenu_DeactivateVR"
		"command" "engine vr_deactivate"
		"InGameOrder" "60"
		"OnlyWhenVREnabled" "1"
		"OnlyWhenVRActive" "1"
	}
	"8"
	{
		"label" "#GameUI_GameMenu_Achievements"
		"command" "OpenAchievementsDialog"
		"InGameOrder" "70"
	}
	"9"
	{
		"label"	"#GameUI_LoadCommentary"
		"command" "OpenLoadSingleplayerCommentaryDialog"
		"InGameOrder" "71"
		"NotInGame" "1"
	}
	"10"
	{
		"label" "#GameUI_Controller"
		"command" "OpenControllerDialog"
		"InGameOrder" "80"
		"ConsoleOnly" "1"
	}
	"11"
	{
		"label" "#GameUI_GameMenu_Options"
		"command" "OpenOptionsDialog"
		"InGameOrder" "90"
	}
	"12"
	{
		"label" "#GameUI_GameMenu_ReturnToMainMenu"
		"command" "OpenMainMenuDialog"
		"OnlyInGame" "1"
		"InGameOrder" "99"
	}
	"13"
	{
		"label" "DGLab Injury Experience Mod"
		"command" "engine OpenDGLabIEModPanel"
		"notmulti" "1"
	}
	"14"
	{
		"label" "#GameUI_GameMenu_Quit"
		"command" "Quit"
		"InGameOrder" "100"
	}
}

