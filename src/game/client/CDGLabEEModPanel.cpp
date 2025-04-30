#include "cbase.h"
#include "IDGLabEEModPanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Label.h>

// Default values
#define DEFAULT_HOSTNAME "127.0.0.1"
#define DEFAULT_PORT "5679"
#define DEFAULT_MAX_STRENGTH "50"

// Panel dimensions
#define PANEL_WIDTH 600
#define PANEL_HEIGHT 500

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
#define MAX_STRENGTH_LABEL_Y 260
#define MAX_STRENGTH_ENTRY_Y 280
#define SAVE_BUTTON_Y 320
#define OUTPUT_LABEL_Y 370
#define OUTPUT_TEXT_Y 390

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
    vgui::TextEntry* m_pMaxStrengthEntry;
    vgui::RichText* m_pOutputText;
    vgui::Button* m_pConnectButton;
    vgui::Button* m_pSaveButton;
    vgui::Label* m_pHostnameLabel;
    vgui::Label* m_pPortLabel;
    vgui::Label* m_pOutputLabel;
    vgui::Label* m_pMaxStrengthLabel;
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

    SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

    // Create controls
    // Connection Title
    m_pConnectionTitleLabel = new vgui::Label(this, "ConnectionTitleLabel", "Connect To PyDGLab-WS Connector");
    m_pConnectionTitleLabel->SetPos(START_X, TITLE_Y);
    m_pConnectionTitleLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pConnectionTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pConnectionTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Hostname Label
    m_pHostnameLabel = new vgui::Label(this, "HostnameLabel", "Hostname:");
    m_pHostnameLabel->SetPos(START_X, HOSTNAME_LABEL_Y);
    m_pHostnameLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pHostnameLabel->SetContentAlignment(vgui::Label::a_west);

    m_pHostnameEntry = new vgui::TextEntry(this, "HostnameEntry");
    m_pHostnameEntry->SetPos(START_X, HOSTNAME_ENTRY_Y);
    m_pHostnameEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pHostnameEntry->SetAllowNonAsciiCharacters(true);
    m_pHostnameEntry->SetText(DEFAULT_HOSTNAME);

    // Port Label
    m_pPortLabel = new vgui::Label(this, "PortLabel", "Port:");
    m_pPortLabel->SetPos(START_X, PORT_LABEL_Y);
    m_pPortLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pPortLabel->SetContentAlignment(vgui::Label::a_west);

    m_pPortEntry = new vgui::TextEntry(this, "PortEntry");
    m_pPortEntry->SetPos(START_X, PORT_ENTRY_Y);
    m_pPortEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pPortEntry->SetAllowNonAsciiCharacters(true);
    m_pPortEntry->SetText(DEFAULT_PORT);

    m_pConnectButton = new vgui::Button(this, "ConnectButton", "Connect", this, "ToggleConnection");
    m_pConnectButton->SetPos(START_X, CONNECT_BUTTON_Y);
    m_pConnectButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // Settings Title
    m_pSettingsTitleLabel = new vgui::Label(this, "SettingsTitleLabel", "DGLab Settings");
    m_pSettingsTitleLabel->SetPos(START_X, SETTINGS_TITLE_Y);
    m_pSettingsTitleLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pSettingsTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pSettingsTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Max Strength Label
    m_pMaxStrengthLabel = new vgui::Label(this, "MaxStrengthLabel", "Max Strength:");
    m_pMaxStrengthLabel->SetPos(START_X, MAX_STRENGTH_LABEL_Y);
    m_pMaxStrengthLabel->SetSize(LABEL_WIDTH, LABEL_HEIGHT);
    m_pMaxStrengthLabel->SetContentAlignment(vgui::Label::a_west);

    m_pMaxStrengthEntry = new vgui::TextEntry(this, "MaxStrengthEntry");
    m_pMaxStrengthEntry->SetPos(START_X, MAX_STRENGTH_ENTRY_Y);
    m_pMaxStrengthEntry->SetSize(ENTRY_WIDTH, ENTRY_HEIGHT);
    m_pMaxStrengthEntry->SetAllowNonAsciiCharacters(true);
    m_pMaxStrengthEntry->SetText(DEFAULT_MAX_STRENGTH);

    m_pSaveButton = new vgui::Button(this, "SaveButton", "Save", this, "SaveSettings");
    m_pSaveButton->SetPos(START_X, SAVE_BUTTON_Y);
    m_pSaveButton->SetSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    // Output Label
    m_pOutputLabel = new vgui::Label(this, "OutputLabel", "Output:");
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
    
    m_pConnectButton->SetText(isServerLoaded ? (isConnected ? "Disconnect" : "Connect") : "Enter The Game First");
    m_pConnectButton->SetEnabled(isServerLoaded);
    
    m_pSaveButton->SetText(isServerLoaded ? "Save" : "Enter The Game First");
    m_pSaveButton->SetEnabled(isServerLoaded);
    m_pMaxStrengthEntry->SetEnabled(isServerLoaded);
    
    m_pHostnameEntry->SetEnabled(isServerLoaded);
    m_pPortEntry->SetEnabled(isServerLoaded);
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

    static bool lastConnected = false;
    static int lastMaxStrength = 0;
    
    bool currentConnected = cvar->FindVar("dglab_ws_connected")->GetBool();
    int currentMaxStrength = cvar->FindVar("dglab_ws_max_strength")->GetInt();

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

    // Update max strength
    if (currentMaxStrength != lastMaxStrength)
    {
        char maxStrength[32];
        Q_snprintf(maxStrength, sizeof(maxStrength), "%d", currentMaxStrength);
        m_pMaxStrengthEntry->SetText(maxStrength);
        AppendLog(VarArgs("Max strength updated to: %d", currentMaxStrength));
        lastMaxStrength = currentMaxStrength;
    }

    UpdateConnectionStatus();
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
        char maxStrength[32];
        m_pMaxStrengthEntry->GetText(maxStrength, sizeof(maxStrength));
        
        // Send settings to server using ServerCmd
        engine->ServerCmd(VarArgs("dglab_set_max_strength %s", maxStrength));
    }
    else if (!Q_stricmp(pcCommand, "Close"))
    {
        cl_show_dglab_ee_mod_panel.SetValue(0);
        SetVisible(false);
    }
}
