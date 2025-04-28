class IDGLabEEModPanel
{
public:
    virtual void Create(vgui::VPANEL parent) = 0;
    virtual void Destroy(void) = 0;
    virtual void Activate(void) = 0;
};

extern IDGLabEEModPanel* dglab_ee_mod_panel;
