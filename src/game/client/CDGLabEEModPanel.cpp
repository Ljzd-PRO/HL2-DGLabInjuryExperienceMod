#include "cbase.h"
#include <string>
#include "IDGLabEEModPanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Label.h>
#include <vgui/ILocalize.h>

// Default values
#define DEFAULT_HOSTNAME "127.0.0.1"
#define DEFAULT_PORT "5679"

// Panel dimensions
#define PANEL_WIDTH 600
#define PANEL_HEIGHT 700

// Control dimensions
#define LABEL_WIDTH 300
#define LABEL_HEIGHT 20
#define ENTRY_WIDTH 300
#define ENTRY_HEIGHT 30
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define OUTPUT_WIDTH 500
#define OUTPUT_HEIGHT 100

// Control positions
#define START_X 50
#define TITLE_Y 30
#define HOSTNAME_LABEL_Y 60
#define HOSTNAME_ENTRY_Y 80
#define PORT_LABEL_Y 120
#define PORT_ENTRY_Y 140
#define CONNECT_BUTTON_Y 180
#define SETTINGS_TITLE_Y 230
#define MAX_STRENGTH_A_LABEL_Y 260
#define MAX_STRENGTH_A_ENTRY_Y 280
#define MIN_STRENGTH_A_LABEL_Y 320
#define MIN_STRENGTH_A_ENTRY_Y 340
#define MAX_STRENGTH_B_LABEL_Y 380
#define MAX_STRENGTH_B_ENTRY_Y 400
#define MIN_STRENGTH_B_LABEL_Y 440
#define MIN_STRENGTH_B_ENTRY_Y 460
#define SAVE_BUTTON_Y 500
#define OUTPUT_LABEL_Y 550
#define OUTPUT_TEXT_Y 570

// Default values
#define DGLAB_EE_MOD_PANEL_DEFAULT_MAX_STRENGTH 100
#define DGLAB_EE_MOD_PANEL_DEFAULT_MIN_STRENGTH 0

class CDGLabEEModPanel : public vgui::Frame
{
    DECLARE_CLASS_SIMPLE(CDGLabEEModPanel, vgui::Frame);
    CDGLabEEModPanel(vgui::VPANEL parent);
    ~CDGLabEEModPanel() override
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
    vgui::Label* m_pHostnameLabel;
    vgui::Label* m_pPortLabel;
    vgui::Label* m_pOutputLabel;
    vgui::Label* m_pMaxStrengthALabel;
    vgui::Label* m_pMinStrengthALabel;
    vgui::Label* m_pMaxStrengthBLabel;
    vgui::Label* m_pMinStrengthBLabel;
    vgui::Label* m_pConnectionTitleLabel;
    vgui::Label* m_pSettingsTitleLabel;

    void UpdateConnectionStatus();
    void AppendLog(const char* message);
};

CDGLabEEModPanel::CDGLabEEModPanel(vgui::VPANEL parent)
    : BaseClass(nullptr, "DGLabEEModPanel")
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
    SetTitle("#DGLabEEMod_Title", false);

    // SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

    // Create controls
    // Connection Title
    m_pConnectionTitleLabel = new vgui::Label(this, "ConnectionTitleLabel", "#DGLabEEMod_ConnectionTitle");
    m_pConnectionTitleLabel->SetPos(START_X, TITLE_Y);
    m_pConnectionTitleLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pConnectionTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pConnectionTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Hostname Label
    m_pHostnameLabel = new vgui::Label(this, "HostnameLabel", "#DGLabEEMod_Hostname");
    m_pHostnameLabel->SetPos(START_X, HOSTNAME_LABEL_Y);
    m_pHostnameLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pHostnameLabel->SetContentAlignment(vgui::Label::a_west);

    m_pHostnameEntry = new vgui::TextEntry(this, "HostnameEntry");
    m_pHostnameEntry->SetPos(START_X, HOSTNAME_ENTRY_Y);
    m_pHostnameEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pHostnameEntry->SetAllowNonAsciiCharacters(true);
    m_pHostnameEntry->SetText(DEFAULT_HOSTNAME);

    // Port Label
    m_pPortLabel = new vgui::Label(this, "PortLabel", "#DGLabEEMod_Port");
    m_pPortLabel->SetPos(START_X, PORT_LABEL_Y);
    m_pPortLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pPortLabel->SetContentAlignment(vgui::Label::a_west);

    m_pPortEntry = new vgui::TextEntry(this, "PortEntry");
    m_pPortEntry->SetPos(START_X, PORT_ENTRY_Y);
    m_pPortEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pPortEntry->SetAllowNonAsciiCharacters(true);
    m_pPortEntry->SetText(DEFAULT_PORT);

    m_pConnectButton = new vgui::Button(this, "ConnectButton", "#DGLabEEMod_Connect", this, "ToggleConnection");
    m_pConnectButton->SetPos(START_X, CONNECT_BUTTON_Y);
    m_pConnectButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // Settings Title
    m_pSettingsTitleLabel = new vgui::Label(this, "SettingsTitleLabel", "#DGLabEEMod_SettingsTitle");
    m_pSettingsTitleLabel->SetPos(START_X, SETTINGS_TITLE_Y);
    m_pSettingsTitleLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pSettingsTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pSettingsTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Max Strength A Label
    m_pMaxStrengthALabel = new vgui::Label(this, "MaxStrengthALabel", "#DGLabEEMod_MaxStrengthA");
    m_pMaxStrengthALabel->SetPos(START_X, MAX_STRENGTH_A_LABEL_Y);
    m_pMaxStrengthALabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMaxStrengthALabel->SetContentAlignment(vgui::Label::a_west);

    m_pMaxStrengthAEntry = new vgui::TextEntry(this, "MaxStrengthAEntry");
    m_pMaxStrengthAEntry->SetPos(START_X, MAX_STRENGTH_A_ENTRY_Y);
    m_pMaxStrengthAEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMaxStrengthAEntry->SetAllowNonAsciiCharacters(true);
    m_pMaxStrengthAEntry->SetText(std::to_string(DGLAB_EE_MOD_PANEL_DEFAULT_MAX_STRENGTH).c_str());

    // Min Strength A Label
    m_pMinStrengthALabel = new vgui::Label(this, "MinStrengthALabel", "#DGLabEEMod_MinStrengthA");
    m_pMinStrengthALabel->SetPos(START_X, MIN_STRENGTH_A_LABEL_Y);
    m_pMinStrengthALabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMinStrengthALabel->SetContentAlignment(vgui::Label::a_west);

    m_pMinStrengthAEntry = new vgui::TextEntry(this, "MinStrengthAEntry");
    m_pMinStrengthAEntry->SetPos(START_X, MIN_STRENGTH_A_ENTRY_Y);
    m_pMinStrengthAEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMinStrengthAEntry->SetAllowNonAsciiCharacters(true);
    m_pMinStrengthAEntry->SetText(std::to_string(DGLAB_EE_MOD_PANEL_DEFAULT_MIN_STRENGTH).c_str());

    // Max Strength B Label
    m_pMaxStrengthBLabel = new vgui::Label(this, "MaxStrengthBLabel", "#DGLabEEMod_MaxStrengthB");
    m_pMaxStrengthBLabel->SetPos(START_X, MAX_STRENGTH_B_LABEL_Y);
    m_pMaxStrengthBLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMaxStrengthBLabel->SetContentAlignment(vgui::Label::a_west);

    m_pMaxStrengthBEntry = new vgui::TextEntry(this, "MaxStrengthBEntry");
    m_pMaxStrengthBEntry->SetPos(START_X, MAX_STRENGTH_B_ENTRY_Y);
    m_pMaxStrengthBEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMaxStrengthBEntry->SetAllowNonAsciiCharacters(true);
    m_pMaxStrengthBEntry->SetText(std::to_string(DGLAB_EE_MOD_PANEL_DEFAULT_MAX_STRENGTH).c_str());

    // Min Strength B Label
    m_pMinStrengthBLabel = new vgui::Label(this, "MinStrengthBLabel", "#DGLabEEMod_MinStrengthB");
    m_pMinStrengthBLabel->SetPos(START_X, MIN_STRENGTH_B_LABEL_Y);
    m_pMinStrengthBLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMinStrengthBLabel->SetContentAlignment(vgui::Label::a_west);

    m_pMinStrengthBEntry = new vgui::TextEntry(this, "MinStrengthBEntry");
    m_pMinStrengthBEntry->SetPos(START_X, MIN_STRENGTH_B_ENTRY_Y);
    m_pMinStrengthBEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMinStrengthBEntry->SetAllowNonAsciiCharacters(true);
    m_pMinStrengthBEntry->SetText(std::to_string(DGLAB_EE_MOD_PANEL_DEFAULT_MIN_STRENGTH).c_str());

    m_pSaveButton = new vgui::Button(this, "SaveButton", "#DGLabEEMod_Save", this, "SaveSettings");
    m_pSaveButton->SetPos(START_X, SAVE_BUTTON_Y);
    m_pSaveButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // Output Label
    m_pOutputLabel = new vgui::Label(this, "OutputLabel", "#DGLabEEMod_Output");
    m_pOutputLabel->SetPos(START_X, OUTPUT_LABEL_Y);
    m_pOutputLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pOutputLabel->SetContentAlignment(vgui::Label::a_west);

    m_pOutputText = new vgui::RichText(this, "OutputText");
    m_pOutputText->SetPos(START_X, OUTPUT_TEXT_Y);
    m_pOutputText->SetSize(OUTPUT_WIDTH, OUTPUT_HEIGHT);
    m_pOutputText->SetPaintBorderEnabled(true);
    m_pOutputText->SetVerticalScrollbar(true);

    vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

    DevMsg("DGLabEEModPanel has been constructed\n");
}

void CDGLabEEModPanel::AppendLog(const char* message)
{
    char currentText[4096];
    m_pOutputText->GetText(0, currentText, sizeof(currentText));

    char newText[4096];
    Q_snprintf(newText, sizeof(newText), "%s\n%s", currentText, message);

    m_pOutputText->SetText(newText);
    m_pOutputText->GotoTextEnd();
}

void CDGLabEEModPanel::UpdateConnectionStatus()
{
    bool isConnected = cvar->FindVar("dglab_ws_connected")->GetBool();
    bool isServerLoaded = engine->IsConnected();
    
    m_pConnectButton->SetText(isServerLoaded ? (isConnected ? "#DGLabEEMod_Disconnect" : "#DGLabEEMod_Connect") : "#DGLabEEMod_EnterGameFirst");
    m_pConnectButton->SetEnabled(isServerLoaded);
    
    m_pSaveButton->SetText(isServerLoaded ? "#DGLabEEMod_Save" : "#DGLabEEMod_EnterGameFirst");
    m_pSaveButton->SetEnabled(isServerLoaded);
    
    m_pHostnameEntry->SetEnabled(isServerLoaded);
    m_pPortEntry->SetEnabled(isServerLoaded);
    m_pMaxStrengthAEntry->SetEnabled(isServerLoaded);
    m_pMinStrengthAEntry->SetEnabled(isServerLoaded);
    m_pMaxStrengthBEntry->SetEnabled(isServerLoaded);
    m_pMinStrengthBEntry->SetEnabled(isServerLoaded);
}

class CDGLabEEModPanelInterface : public IDGLabEEModPanel
{
    CDGLabEEModPanel* DGLabEEModPanel;

public:
    CDGLabEEModPanelInterface()
    {
        DGLabEEModPanel = nullptr;
    }

    void Create(vgui::VPANEL parent) override
    {
        DGLabEEModPanel = new CDGLabEEModPanel(parent);
    }

    void Destroy() override
    {
        if (DGLabEEModPanel)
        {
            DGLabEEModPanel->SetParent(static_cast<vgui::Panel*>(nullptr));
            delete DGLabEEModPanel;
        }
    }

    void Activate(void) override
    {
        if (DGLabEEModPanel)
        {
            DGLabEEModPanel->Activate();
        }
    }
};

static CDGLabEEModPanelInterface g_DGLabEEModPanel;
IDGLabEEModPanel* dglab_ee_mod_panel = &g_DGLabEEModPanel;

// ConVar to control panel visibility
ConVar cl_show_dglab_ee_mod_panel("cl_show_dglab_ee_mod_panel", "0", FCVAR_CLIENTDLL, "Sets the state of myPanel <state>");

void CDGLabEEModPanel::OnTick()
{
    BaseClass::OnTick();
    SetVisible(cl_show_dglab_ee_mod_panel.GetBool());

    UpdateConnectionStatus();

    if (!engine->IsConnected()) return;

    static bool lastConnected = false;
    static int lastMaxStrengthA = DGLAB_EE_MOD_PANEL_DEFAULT_MAX_STRENGTH;
    static int lastMinStrengthA = DGLAB_EE_MOD_PANEL_DEFAULT_MIN_STRENGTH;
    static int lastMaxStrengthB = DGLAB_EE_MOD_PANEL_DEFAULT_MAX_STRENGTH;
    static int lastMinStrengthB = DGLAB_EE_MOD_PANEL_DEFAULT_MIN_STRENGTH;
    
    bool currentConnected = cvar->FindVar("dglab_ws_connected")->GetBool();
    int currentMaxStrengthA = cvar->FindVar("dglab_ws_max_strength_a")->GetInt();
    int currentMinStrengthA = cvar->FindVar("dglab_ws_min_strength_a")->GetInt();
    int currentMaxStrengthB = cvar->FindVar("dglab_ws_max_strength_b")->GetInt();
    int currentMinStrengthB = cvar->FindVar("dglab_ws_min_strength_b")->GetInt();

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
}

// Command to toggle panel visibility
CON_COMMAND(OpenDGLabEEModPanel, "Toggles myPanel on or off")
{
    cl_show_dglab_ee_mod_panel.SetValue(!cl_show_dglab_ee_mod_panel.GetBool());
    dglab_ee_mod_panel->Activate();
}

void CDGLabEEModPanel::OnCommand(const char* pcCommand)
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
    }
    else if (!Q_stricmp(pcCommand, "Close"))
    {
        cl_show_dglab_ee_mod_panel.SetValue(0);
        SetVisible(false);
    }
}
