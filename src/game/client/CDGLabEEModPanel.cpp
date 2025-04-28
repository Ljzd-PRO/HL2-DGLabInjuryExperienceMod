#include "cbase.h"
#include "IDGLabEEModPanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Label.h>
#include "dglab_ws_client.h"

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
    SetSize(600, 500);

    SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

    // Create controls
    // Connection Title
    m_pConnectionTitleLabel = new vgui::Label(this, "ConnectionTitleLabel", "Connect To PyDGLab-WS Connector");
    m_pConnectionTitleLabel->SetPos(50, 30);
    m_pConnectionTitleLabel->SetSize(500, 20);
    m_pConnectionTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pConnectionTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Hostname Label
    m_pHostnameLabel = new vgui::Label(this, "HostnameLabel", "Hostname:");
    m_pHostnameLabel->SetPos(50, 60);
    m_pHostnameLabel->SetSize(300, 20);
    m_pHostnameLabel->SetContentAlignment(vgui::Label::a_west);

    m_pHostnameEntry = new vgui::TextEntry(this, "HostnameEntry");
    m_pHostnameEntry->SetPos(50, 80);
    m_pHostnameEntry->SetSize(300, 30);
    m_pHostnameEntry->SetAllowNonAsciiCharacters(true);
    m_pHostnameEntry->SetText("127.0.0.1");

    // Port Label
    m_pPortLabel = new vgui::Label(this, "PortLabel", "Port:");
    m_pPortLabel->SetPos(50, 120);
    m_pPortLabel->SetSize(300, 20);
    m_pPortLabel->SetContentAlignment(vgui::Label::a_west);

    m_pPortEntry = new vgui::TextEntry(this, "PortEntry");
    m_pPortEntry->SetPos(50, 140);
    m_pPortEntry->SetSize(300, 30);
    m_pPortEntry->SetAllowNonAsciiCharacters(true);
    m_pPortEntry->SetText("5679");

    m_pConnectButton = new vgui::Button(this, "ConnectButton", "Connect", this, "ToggleConnection");
    m_pConnectButton->SetPos(50, 180);
    m_pConnectButton->SetSize(120, 40);

    // Settings Title
    m_pSettingsTitleLabel = new vgui::Label(this, "SettingsTitleLabel", "DGLab Settings");
    m_pSettingsTitleLabel->SetPos(50, 230);
    m_pSettingsTitleLabel->SetSize(500, 20);
    m_pSettingsTitleLabel->SetContentAlignment(vgui::Label::a_west);
    m_pSettingsTitleLabel->SetFont(vgui::scheme()->GetIScheme(GetScheme())->GetFont("DefaultBold"));

    // Max Strength Label
    m_pMaxStrengthLabel = new vgui::Label(this, "MaxStrengthLabel", "Max Strength:");
    m_pMaxStrengthLabel->SetPos(50, 260);
    m_pMaxStrengthLabel->SetSize(300, 20);
    m_pMaxStrengthLabel->SetContentAlignment(vgui::Label::a_west);

    m_pMaxStrengthEntry = new vgui::TextEntry(this, "MaxStrengthEntry");
    m_pMaxStrengthEntry->SetPos(50, 280);
    m_pMaxStrengthEntry->SetSize(300, 30);
    m_pMaxStrengthEntry->SetAllowNonAsciiCharacters(true);
    m_pMaxStrengthEntry->SetText("50");

    m_pSaveButton = new vgui::Button(this, "SaveButton", "Save", this, "SaveSettings");
    m_pSaveButton->SetPos(50, 320);
    m_pSaveButton->SetSize(120, 40);

    // Output Label
    m_pOutputLabel = new vgui::Label(this, "OutputLabel", "Output:");
    m_pOutputLabel->SetPos(50, 370);
    m_pOutputLabel->SetSize(300, 20);
    m_pOutputLabel->SetContentAlignment(vgui::Label::a_west);

    m_pOutputText = new vgui::RichText(this, "OutputText");
    m_pOutputText->SetPos(50, 390);
    m_pOutputText->SetSize(500, 100);
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
    bool isConnected = dglab_ws_is_connected() != 0;
    m_pConnectButton->SetText(isConnected ? "Disconnect" : "Connect");
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
        bool isConnected = dglab_ws_is_connected() != 0;

        if (!isConnected)
        {
            // Try to connect
            char hostname[256];
            char port[256];
            m_pHostnameEntry->GetText(hostname, sizeof(hostname));
            m_pPortEntry->GetText(port, sizeof(port));

            char ws_url[512];
            Q_snprintf(ws_url, sizeof(ws_url), "ws://%s:%s", hostname, port);

            AppendLog("Connecting to server...");
            int result = dglab_ws_connect(ws_url);

            if (result == 0)
            {
                AppendLog("Connection successful!");
            }
            else
            {
                AppendLog("Connection failed!");
            }
        }
        else
        {
            // Disconnect
            AppendLog("Disconnecting...");
            dglab_ws_disconnect();
            AppendLog("Disconnected");
        }

        UpdateConnectionStatus();
    }
    else if (!Q_stricmp(pcCommand, "SaveSettings"))
    {
        char maxStrength[32];
        m_pMaxStrengthEntry->GetText(maxStrength, sizeof(maxStrength));
        AppendLog("Settings saved!");
    }
    else if (!Q_stricmp(pcCommand, "Close"))
    {
        cl_show_dglab_ee_mod_panel.SetValue(0);
        SetVisible(false);
    }
}
