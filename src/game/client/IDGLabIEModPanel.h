class IDGLabIEModPanel
{
public:
    virtual void Create(vgui::VPANEL parent) = 0;
    virtual void Destroy(void) = 0;
    virtual void Activate(void) = 0;
};

extern IDGLabIEModPanel* dglab_ie_mod_panel;
