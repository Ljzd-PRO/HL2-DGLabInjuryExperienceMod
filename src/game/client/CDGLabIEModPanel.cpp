#include "cbase.h"
#include <string>
#include "IDGLabIEModPanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Slider.h>
#include <vgui/ILocalize.h>

// Default values
#define DEFAULT_HOSTNAME "127.0.0.1"
#define DEFAULT_PORT "5679"
#define DGLAB_IE_MOD_VERSION "v1.0.1"

// Panel dimensions
#define PANEL_WIDTH 600
#define PANEL_HEIGHT 1050

// Control dimensions
#define LABEL_WIDTH 300
#define LABEL_HEIGHT 20
#define ENTRY_WIDTH 300
#define ENTRY_HEIGHT 30
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define OUTPUT_WIDTH 500
#define OUTPUT_HEIGHT 150
#define CHECKBOX_WIDTH 300
#define CHECKBOX_HEIGHT 20

// Control positions
#define START_X 50
#define TITLE_Y 30
#define HOSTNAME_LABEL_Y 60
#define HOSTNAME_ENTRY_Y 80
#define PORT_LABEL_Y 120
#define PORT_ENTRY_Y 140
#define CONNECT_BUTTON_Y 180
#define CONNECTION_NOTE_Y 230
#define SETTINGS_TITLE_Y 280
#define ENEMY_EXPERIENCE_CHECKBOX_Y 310
#define SELF_EXPERIENCE_CHECKBOX_Y 340
#define SELF_STRENGTH_SLIDER_Y 370
#define SELF_STRENGTH_LABEL_Y 400
#define MAX_STRENGTH_A_LABEL_Y 440
#define MAX_STRENGTH_A_ENTRY_Y 460
#define MIN_STRENGTH_A_LABEL_Y 500
#define MIN_STRENGTH_A_ENTRY_Y 520
#define MAX_STRENGTH_B_LABEL_Y 560
#define MAX_STRENGTH_B_ENTRY_Y 580
#define MIN_STRENGTH_B_LABEL_Y 620
#define MIN_STRENGTH_B_ENTRY_Y 640
#define SAVE_BUTTON_Y 680
#define OUTPUT_LABEL_Y 730
#define OUTPUT_TEXT_Y 750
#define VERSION_LABEL_Y 920
#define COPYRIGHT_LABEL_Y 940
#define GITHUB_BUTTON_Y 960
#define PANEL_BOTTOM_MARGIN 50

// Default values
#define DGLAB_IE_MOD_PANEL_DEFAULT_MAX_STRENGTH 100
#define DGLAB_IE_MOD_PANEL_DEFAULT_MIN_STRENGTH 0
#define DGLAB_IE_MOD_PANEL_DEFAULT_SELF_STRENGTH 30

class CDGLabIEModPanel : public vgui::Frame
{
    DECLARE_CLASS_SIMPLE(CDGLabIEModPanel, vgui::Frame);
    CDGLabIEModPanel(vgui::VPANEL parent);
    ~CDGLabIEModPanel() override
    {
    };

protected:
    void OnTick() override;
    void OnCommand(const char* pcCommand) override;

private:
    //Other used VGUI control Elements:
    vgui::TextEntry* m_pHostnameEntry;
    vgui::TextEntry* m_pPortEntry;
    vgui::TextEntry* m_pMaxStrengthAEntry;
    vgui::TextEntry* m_pMinStrengthAEntry;
    vgui::TextEntry* m_pMaxStrengthBEntry;
    vgui::TextEntry* m_pMinStrengthBEntry;
    vgui::RichText* m_pOutputText;
    vgui::Button* m_pConnectButton;
    vgui::Button* m_pSaveButton;
    vgui::Button* m_pGitHubButton;
    vgui::Label* m_pCopyrightLabel;
    vgui::Label* m_pVersionLabel;
    vgui::Label* m_pConnectionNoteLabel;
    vgui::CheckButton* m_pEnemyExperienceCheckbox;
    vgui::CheckButton* m_pSelfExperienceCheckbox;
    vgui::Label* m_pHostnameLabel;
    vgui::Label* m_pPortLabel;
    vgui::Label* m_pOutputLabel;
    vgui::Label* m_pMaxStrengthALabel;
    vgui::Label* m_pMinStrengthALabel;
    vgui::Label* m_pMaxStrengthBLabel;
    vgui::Label* m_pMinStrengthBLabel;
    vgui::Label* m_pConnectionTitleLabel;
    vgui::Label* m_pSettingsTitleLabel;
    vgui::Slider* m_pSelfStrengthSlider;
    vgui::Label* m_pSelfStrengthLabel;

    void UpdateConnectionStatus();
    void AppendLog(const char* message);
};

CDGLabIEModPanel::CDGLabIEModPanel(vgui::VPANEL parent)
    : BaseClass(nullptr, "DGLabIEModPanel")
{
    SetParent(parent);

    SetKeyBoardInputEnabled(true);
    SetMouseInputEnabled(true);

    SetProportional(false);
    SetTitleBarVisible(true);
    SetMinimizeButtonVisible(false);
    SetMaximizeButtonVisible(false);
    SetCloseButtonVisible(true);
    SetSizeable(false);
    SetMoveable(true);
    SetVisible(true);

    // Set panel size
    SetSize(PANEL_WIDTH, PANEL_HEIGHT);

    // Set title
    SetTitle("#DGLabIEMod_Title", false);

    // SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

    // Create controls
    // Connection Title
    m_pConnectionTitleLabel = new vgui::Label(this, "ConnectionTitleLabel", "#DGLabIEMod_ConnectionTitle");
    m_pConnectionTitleLabel->SetPos(START_X, TITLE_Y);
    m_pConnectionTitleLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pConnectionTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pConnectionTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Hostname Label
    m_pHostnameLabel = new vgui::Label(this, "HostnameLabel", "#DGLabIEMod_Hostname");
    m_pHostnameLabel->SetPos(START_X, HOSTNAME_LABEL_Y);
    m_pHostnameLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pHostnameLabel->SetContentAlignment(vgui::Label::a_west);

    m_pHostnameEntry = new vgui::TextEntry(this, "HostnameEntry");
    m_pHostnameEntry->SetPos(START_X, HOSTNAME_ENTRY_Y);
    m_pHostnameEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pHostnameEntry->SetAllowNonAsciiCharacters(true);
    m_pHostnameEntry->SetText(DEFAULT_HOSTNAME);

    // Port Label
    m_pPortLabel = new vgui::Label(this, "PortLabel", "#DGLabIEMod_Port");
    m_pPortLabel->SetPos(START_X, PORT_LABEL_Y);
    m_pPortLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pPortLabel->SetContentAlignment(vgui::Label::a_west);

    m_pPortEntry = new vgui::TextEntry(this, "PortEntry");
    m_pPortEntry->SetPos(START_X, PORT_ENTRY_Y);
    m_pPortEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pPortEntry->SetAllowNonAsciiCharacters(true);
    m_pPortEntry->SetText(DEFAULT_PORT);

    m_pConnectButton = new vgui::Button(this, "ConnectButton", "#DGLabIEMod_Connect", this, "ToggleConnection");
    m_pConnectButton->SetPos(START_X, CONNECT_BUTTON_Y);
    m_pConnectButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // Connection Note Label
    m_pConnectionNoteLabel = new vgui::Label(this, "ConnectionNoteLabel", "#DGLabIEMod_ConnectionNote");
    m_pConnectionNoteLabel->SetPos(START_X, CONNECTION_NOTE_Y);
    m_pConnectionNoteLabel->SetSize(LABEL_WIDTH * 2, LABEL_HEIGHT * 2);
    m_pConnectionNoteLabel->SetContentAlignment(vgui::Label::a_west);
    m_pConnectionNoteLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultSmall"));

    // Settings Title
    m_pSettingsTitleLabel = new vgui::Label(this, "SettingsTitleLabel", "#DGLabIEMod_SettingsTitle");
    m_pSettingsTitleLabel->SetPos(START_X, SETTINGS_TITLE_Y);
    m_pSettingsTitleLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pSettingsTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pSettingsTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Enemy Experience Checkbox
    m_pEnemyExperienceCheckbox = new vgui::CheckButton(this, "EnemyExperienceCheckbox", "#DGLabIEMod_EnemyExperience");
    m_pEnemyExperienceCheckbox->SetPos(START_X, ENEMY_EXPERIENCE_CHECKBOX_Y);
    m_pEnemyExperienceCheckbox->SetSize(CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
    m_pEnemyExperienceCheckbox->SetSelected(true);

    // Self Experience Checkbox
    m_pSelfExperienceCheckbox = new vgui::CheckButton(this, "SelfExperienceCheckbox", "#DGLabIEMod_SelfExperience");
    m_pSelfExperienceCheckbox->SetPos(START_X, SELF_EXPERIENCE_CHECKBOX_Y);
    m_pSelfExperienceCheckbox->SetSize(CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
    m_pSelfExperienceCheckbox->SetSelected(false);

    // Self Strength Slider
    m_pSelfStrengthSlider = new vgui::Slider(this, "SelfStrengthSlider");
    m_pSelfStrengthSlider->SetPos(START_X, SELF_STRENGTH_SLIDER_Y);
    m_pSelfStrengthSlider->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pSelfStrengthSlider->SetRange(0, 100);
    m_pSelfStrengthSlider->SetValue(DGLAB_IE_MOD_PANEL_DEFAULT_SELF_STRENGTH);
    m_pSelfStrengthSlider->AddActionSignalTarget(this);

    m_pSelfStrengthLabel = new vgui::Label(this, "SelfStrengthLabel", "#DGLabIEMod_SelfStrength");
    m_pSelfStrengthLabel->SetPos(START_X, SELF_STRENGTH_LABEL_Y);
    m_pSelfStrengthLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pSelfStrengthLabel->SetContentAlignment(vgui::Label::a_west);

    // Max Strength A Label
    m_pMaxStrengthALabel = new vgui::Label(this, "MaxStrengthALabel", "#DGLabIEMod_MaxStrengthA");
    m_pMaxStrengthALabel->SetPos(START_X, MAX_STRENGTH_A_LABEL_Y);
    m_pMaxStrengthALabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMaxStrengthALabel->SetContentAlignment(vgui::Label::a_west);

    m_pMaxStrengthAEntry = new vgui::TextEntry(this, "MaxStrengthAEntry");
    m_pMaxStrengthAEntry->SetPos(START_X, MAX_STRENGTH_A_ENTRY_Y);
    m_pMaxStrengthAEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMaxStrengthAEntry->SetAllowNonAsciiCharacters(true);
    m_pMaxStrengthAEntry->SetText(std::to_string(DGLAB_IE_MOD_PANEL_DEFAULT_MAX_STRENGTH).c_str());

    // Min Strength A Label
    m_pMinStrengthALabel = new vgui::Label(this, "MinStrengthALabel", "#DGLabIEMod_MinStrengthA");
    m_pMinStrengthALabel->SetPos(START_X, MIN_STRENGTH_A_LABEL_Y);
    m_pMinStrengthALabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMinStrengthALabel->SetContentAlignment(vgui::Label::a_west);

    m_pMinStrengthAEntry = new vgui::TextEntry(this, "MinStrengthAEntry");
    m_pMinStrengthAEntry->SetPos(START_X, MIN_STRENGTH_A_ENTRY_Y);
    m_pMinStrengthAEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMinStrengthAEntry->SetAllowNonAsciiCharacters(true);
    m_pMinStrengthAEntry->SetText(std::to_string(DGLAB_IE_MOD_PANEL_DEFAULT_MIN_STRENGTH).c_str());

    // Max Strength B Label
    m_pMaxStrengthBLabel = new vgui::Label(this, "MaxStrengthBLabel", "#DGLabIEMod_MaxStrengthB");
    m_pMaxStrengthBLabel->SetPos(START_X, MAX_STRENGTH_B_LABEL_Y);
    m_pMaxStrengthBLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMaxStrengthBLabel->SetContentAlignment(vgui::Label::a_west);

    m_pMaxStrengthBEntry = new vgui::TextEntry(this, "MaxStrengthBEntry");
    m_pMaxStrengthBEntry->SetPos(START_X, MAX_STRENGTH_B_ENTRY_Y);
    m_pMaxStrengthBEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMaxStrengthBEntry->SetAllowNonAsciiCharacters(true);
    m_pMaxStrengthBEntry->SetText(std::to_string(DGLAB_IE_MOD_PANEL_DEFAULT_MAX_STRENGTH).c_str());

    // Min Strength B Label
    m_pMinStrengthBLabel = new vgui::Label(this, "MinStrengthBLabel", "#DGLabIEMod_MinStrengthB");
    m_pMinStrengthBLabel->SetPos(START_X, MIN_STRENGTH_B_LABEL_Y);
    m_pMinStrengthBLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMinStrengthBLabel->SetContentAlignment(vgui::Label::a_west);

    m_pMinStrengthBEntry = new vgui::TextEntry(this, "MinStrengthBEntry");
    m_pMinStrengthBEntry->SetPos(START_X, MIN_STRENGTH_B_ENTRY_Y);
    m_pMinStrengthBEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMinStrengthBEntry->SetAllowNonAsciiCharacters(true);
    m_pMinStrengthBEntry->SetText(std::to_string(DGLAB_IE_MOD_PANEL_DEFAULT_MIN_STRENGTH).c_str());

    m_pSaveButton = new vgui::Button(this, "SaveButton", "#DGLabIEMod_Save", this, "SaveSettings");
    m_pSaveButton->SetPos(START_X, SAVE_BUTTON_Y);
    m_pSaveButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // Output Label
    m_pOutputLabel = new vgui::Label(this, "OutputLabel", "#DGLabIEMod_Output");
    m_pOutputLabel->SetPos(START_X, OUTPUT_LABEL_Y);
    m_pOutputLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pOutputLabel->SetContentAlignment(vgui::Label::a_west);

    m_pOutputText = new vgui::RichText(this, "OutputText");
    m_pOutputText->SetPos(START_X, OUTPUT_TEXT_Y);
    m_pOutputText->SetSize(OUTPUT_WIDTH, OUTPUT_HEIGHT);
    m_pOutputText->SetPaintBorderEnabled(true);
    m_pOutputText->SetVerticalScrollbar(true);

    // Version Label
    m_pVersionLabel = new vgui::Label(this, "VersionLabel", DGLAB_IE_MOD_VERSION);
    m_pVersionLabel->SetPos(START_X, VERSION_LABEL_Y);
    m_pVersionLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pVersionLabel->SetContentAlignment(vgui::Label::a_west);
    m_pVersionLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultSmall"));

    // Copyright Label
    m_pCopyrightLabel = new vgui::Label(this, "CopyrightLabel", "#DGLabIEMod_Copyright");
    m_pCopyrightLabel->SetPos(START_X, COPYRIGHT_LABEL_Y);
    m_pCopyrightLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pCopyrightLabel->SetContentAlignment(vgui::Label::a_west);
    m_pCopyrightLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultSmall"));

    // GitHub Link Button
    m_pGitHubButton = new vgui::Button(this, "GitHubButton", "#DGLabIEMod_GitHub", this, "OpenGitHub");
    m_pGitHubButton->SetPos(START_X, GITHUB_BUTTON_Y);
    m_pGitHubButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

    DevMsg("DGLabIEMod> DGLabIEModPanel has been constructed\n");
}

void CDGLabIEModPanel::AppendLog(const char* message)
{
    char currentText[4096];
    m_pOutputText->GetText(0, currentText, sizeof(currentText));

    char newText[4096];
    Q_snprintf(newText, sizeof(newText), "%s\n%s", currentText, message);

    m_pOutputText->SetText(newText);
    m_pOutputText->GotoTextEnd();
}

void CDGLabIEModPanel::UpdateConnectionStatus()
{
    bool isConnected = cvar->FindVar("dglab_ws_connected")->GetBool();
    bool isServerLoaded = engine->IsConnected();
    
    m_pConnectButton->SetText(isServerLoaded ? (isConnected ? "#DGLabIEMod_Disconnect" : "#DGLabIEMod_Connect") : "#DGLabIEMod_EnterGameFirst");
    m_pConnectButton->SetEnabled(isServerLoaded);
    
    m_pSaveButton->SetText(isServerLoaded ? "#DGLabIEMod_Save" : "#DGLabIEMod_EnterGameFirst");
    m_pSaveButton->SetEnabled(isServerLoaded);
    
    m_pHostnameEntry->SetEnabled(isServerLoaded);
    m_pPortEntry->SetEnabled(isServerLoaded);
    m_pMaxStrengthAEntry->SetEnabled(isServerLoaded);
    m_pMinStrengthAEntry->SetEnabled(isServerLoaded);
    m_pMaxStrengthBEntry->SetEnabled(isServerLoaded);
    m_pMinStrengthBEntry->SetEnabled(isServerLoaded);
    m_pEnemyExperienceCheckbox->SetEnabled(isServerLoaded);
    m_pSelfExperienceCheckbox->SetEnabled(isServerLoaded);
    m_pSelfStrengthSlider->SetEnabled(isServerLoaded);
}

class CDGLabIEModPanelInterface : public IDGLabIEModPanel
{
    CDGLabIEModPanel* DGLabIEModPanel;

public:
    CDGLabIEModPanelInterface()
    {
        DGLabIEModPanel = nullptr;
    }

    void Create(vgui::VPANEL parent) override
    {
        DGLabIEModPanel = new CDGLabIEModPanel(parent);
    }

    void Destroy() override
    {
        if (DGLabIEModPanel)
        {
            DGLabIEModPanel->SetParent(static_cast<vgui::Panel*>(nullptr));
            delete DGLabIEModPanel;
        }
    }

    void Activate(void) override
    {
        if (DGLabIEModPanel)
        {
            DGLabIEModPanel->Activate();
        }
    }
};

static CDGLabIEModPanelInterface g_DGLabIEModPanel;
IDGLabIEModPanel* dglab_ie_mod_panel = &g_DGLabIEModPanel;

// ConVar to control panel visibility
ConVar cl_show_dglab_ie_mod_panel("cl_show_dglab_ie_mod_panel", "0", FCVAR_CLIENTDLL, "Sets the state of myPanel <state>");

void CDGLabIEModPanel::OnTick()
{
    BaseClass::OnTick();
    SetVisible(cl_show_dglab_ie_mod_panel.GetBool());

    UpdateConnectionStatus();

    if (!engine->IsConnected()) return;

    static bool lastConnected = false;
    static int lastMaxStrengthA = DGLAB_IE_MOD_PANEL_DEFAULT_MAX_STRENGTH;
    static int lastMinStrengthA = DGLAB_IE_MOD_PANEL_DEFAULT_MIN_STRENGTH;
    static int lastMaxStrengthB = DGLAB_IE_MOD_PANEL_DEFAULT_MAX_STRENGTH;
    static int lastMinStrengthB = DGLAB_IE_MOD_PANEL_DEFAULT_MIN_STRENGTH;
    static bool lastEnemyExperience = true;
    static bool lastSelfExperience = false;
    static float lastSelfStrength = DGLAB_IE_MOD_PANEL_DEFAULT_SELF_STRENGTH / 100.0f;
    
    bool currentConnected = cvar->FindVar("dglab_ws_connected")->GetBool();
    int currentMaxStrengthA = cvar->FindVar("dglab_ws_max_strength_a")->GetInt();
    int currentMinStrengthA = cvar->FindVar("dglab_ws_min_strength_a")->GetInt();
    int currentMaxStrengthB = cvar->FindVar("dglab_ws_max_strength_b")->GetInt();
    int currentMinStrengthB = cvar->FindVar("dglab_ws_min_strength_b")->GetInt();
    bool currentEnemyExperience = cvar->FindVar("dglab_ws_enemy_experience")->GetBool();
    bool currentSelfExperience = cvar->FindVar("dglab_ws_self_experience")->GetBool();
    float currentSelfStrength = cvar->FindVar("dglab_ws_self_strength_percentage")->GetFloat();

    // Update connection status
    if (currentConnected != lastConnected)
    {
        if (currentConnected)
        {
            AppendLog("DGLab WebSocket connected successfully!");
        }
        else
        {
            AppendLog("DGLab WebSocket disconnected.");
        }
        lastConnected = currentConnected;
    }

    // Update enemy experience checkbox
    if (currentEnemyExperience != lastEnemyExperience)
    {
        m_pEnemyExperienceCheckbox->SetSelected(currentEnemyExperience);
        lastEnemyExperience = currentEnemyExperience;
        AppendLog(VarArgs("Enemy experience updated to: %d", currentEnemyExperience));
    }

    // Update self experience checkbox
    if (currentSelfExperience != lastSelfExperience)
    {
        m_pSelfExperienceCheckbox->SetSelected(currentSelfExperience);
        lastSelfExperience = currentSelfExperience;
        AppendLog(VarArgs("Self experience updated to: %d", currentSelfExperience));
    }

    // Update max strength A
    if (currentMaxStrengthA != lastMaxStrengthA)
    {
        char maxStrength[32];
        Q_snprintf(maxStrength, sizeof(maxStrength), "%d", currentMaxStrengthA);
        m_pMaxStrengthAEntry->SetText(maxStrength);
        AppendLog(VarArgs("A channel max strength updated to: %d", currentMaxStrengthA));
        lastMaxStrengthA = currentMaxStrengthA;
    }

    // Update min strength A
    if (currentMinStrengthA != lastMinStrengthA)
    {
        char minStrength[32];
        Q_snprintf(minStrength, sizeof(minStrength), "%d", currentMinStrengthA);
        m_pMinStrengthAEntry->SetText(minStrength);
        AppendLog(VarArgs("A channel min strength updated to: %d", currentMinStrengthA));
        lastMinStrengthA = currentMinStrengthA;
    }

    // Update max strength B
    if (currentMaxStrengthB != lastMaxStrengthB)
    {
        char maxStrength[32];
        Q_snprintf(maxStrength, sizeof(maxStrength), "%d", currentMaxStrengthB);
        m_pMaxStrengthBEntry->SetText(maxStrength);
        AppendLog(VarArgs("B channel max strength updated to: %d", currentMaxStrengthB));
        lastMaxStrengthB = currentMaxStrengthB;
    }

    // Update min strength B
    if (currentMinStrengthB != lastMinStrengthB)
    {
        char minStrength[32];
        Q_snprintf(minStrength, sizeof(minStrength), "%d", currentMinStrengthB);
        m_pMinStrengthBEntry->SetText(minStrength);
        AppendLog(VarArgs("B channel min strength updated to: %d", currentMinStrengthB));
        lastMinStrengthB = currentMinStrengthB;
    }

    // Update self strength slider
    if (currentSelfStrength != lastSelfStrength)
    {
        m_pSelfStrengthSlider->SetValue(currentSelfStrength * 100);
        lastSelfStrength = currentSelfStrength;
        AppendLog(VarArgs("Self strength percentage updated to: %.2f", currentSelfStrength));
    }
}

// Command to toggle panel visibility
CON_COMMAND(OpenDGLabIEModPanel, "Toggles myPanel on or off")
{
    cl_show_dglab_ie_mod_panel.SetValue(!cl_show_dglab_ie_mod_panel.GetBool());
    dglab_ie_mod_panel->Activate();
}

void CDGLabIEModPanel::OnCommand(const char* pcCommand)
{
    BaseClass::OnCommand(pcCommand);

    if (!Q_stricmp(pcCommand, "ToggleConnection"))
    {
        bool isConnected = cvar->FindVar("dglab_ws_connected")->GetBool();
        
        if (isConnected)
        {
            engine->ServerCmd("dglab_disconnect");
        }
        else
        {
            char hostname[256];
            char port[256];
            m_pHostnameEntry->GetText(hostname, sizeof(hostname));
            m_pPortEntry->GetText(port, sizeof(port));

            char ws_url[512];
            Q_snprintf(ws_url, sizeof(ws_url), "ws://%s:%s", hostname, port);

            // Encode the URL to prevent command parsing issues
            char encoded_url[1024];
            Q_StrSubst(ws_url, ":", "_COLON_", encoded_url, sizeof(encoded_url));
            
            engine->ServerCmd(VarArgs("dglab_connect %s", encoded_url));
        }
    }
    else if (!Q_stricmp(pcCommand, "SaveSettings"))
    {
        char maxStrengthA[32];
        char minStrengthA[32];
        char maxStrengthB[32];
        char minStrengthB[32];
        
        m_pMaxStrengthAEntry->GetText(maxStrengthA, sizeof(maxStrengthA));
        m_pMinStrengthAEntry->GetText(minStrengthA, sizeof(minStrengthA));
        m_pMaxStrengthBEntry->GetText(maxStrengthB, sizeof(maxStrengthB));
        m_pMinStrengthBEntry->GetText(minStrengthB, sizeof(minStrengthB));
        
        // Send settings to server using ServerCmd
        engine->ServerCmd(VarArgs("dglab_set_max_strength_a %s", maxStrengthA));
        engine->ServerCmd(VarArgs("dglab_set_min_strength_a %s", minStrengthA));
        engine->ServerCmd(VarArgs("dglab_set_max_strength_b %s", maxStrengthB));
        engine->ServerCmd(VarArgs("dglab_set_min_strength_b %s", minStrengthB));
        
        // Send experience settings
        engine->ServerCmd(VarArgs("dglab_set_enemy_experience %d", m_pEnemyExperienceCheckbox->IsSelected() ? 1 : 0));
        engine->ServerCmd(VarArgs("dglab_set_self_experience %d", m_pSelfExperienceCheckbox->IsSelected() ? 1 : 0));
        
        // Send self strength percentage
        float selfStrength = m_pSelfStrengthSlider->GetValue() / 100.0f;
        engine->ServerCmd(VarArgs("dglab_set_self_strength_percentage %.2f", selfStrength));
    }
    else if (!Q_stricmp(pcCommand, "OpenGitHub"))
    {
        system("start https://github.com/Ljzd-PRO/HL2-DGLabInjuryExperienceMod");
    }
    else if (!Q_stricmp(pcCommand, "Close"))
    {
        cl_show_dglab_ie_mod_panel.SetValue(0);
        SetVisible(false);
    }
}
