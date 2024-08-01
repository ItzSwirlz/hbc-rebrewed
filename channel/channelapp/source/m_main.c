#include <malloc.h>
#include <stdio.h>

#include <network.h>
#include <ogcsys.h>
#include <ogc/isfs.h>

#include "../config.h"
#include "i18n.h"
#include "loader.h"
#include "panic.h"
#include "theme.h"
#include "title.h"
#include "view.h"

#define TITLE_UPPER(x) (u32)(x >> 32)
#define TITLE_LOWER(x) (u32)(x & 0xFFFFFFFF)

#include "m_main.h"

static view *v_m_main;

static const char *text_no_ip;
static const char *text_has_ip;
static const char *text_number_apps;

static bool priiloader = false;
static bool vwii = false;

static bool priiloader_is_installed(u64 title_id) {

	u32 tmd_size;
	static u8 tmd_buf[MAX_SIGNED_TMD_SIZE] ATTRIBUTE_ALIGN(32);
	static signed_blob *mTMD;
	static tmd *rTMD;
	char TMD_Path[ISFS_MAXPATH];
	char TMD_Path2[ISFS_MAXPATH];

	//IOS Shit Done
	//read TMD so we can get the main booting dol
	s32 fd = 0;
	u32 id = 0;
	bool ret = false;
	memset(TMD_Path,0,64);
	memset(TMD_Path2,0,64);
	sprintf(TMD_Path, "/title/%08x/%08x/content/title.tmd",TITLE_UPPER(title_id),TITLE_LOWER(title_id));
	sprintf(TMD_Path2, "/title/%08x/%08x/content/title_or.tmd",TITLE_UPPER(title_id),TITLE_LOWER(title_id));
	fd = ES_GetStoredTMDSize(title_id,&tmd_size);
	if (fd < 0)
	{
		gprintf("Unable to get stored tmd size");
	}
	mTMD = (signed_blob *)tmd_buf;
	fd = ES_GetStoredTMD(title_id,mTMD,tmd_size);
	if (fd < 0)
	{
		gprintf("Unable to get stored tmd");
	}
	rTMD = (tmd*)SIGNATURE_PAYLOAD(mTMD);
	for(u8 i=0; i < rTMD->num_contents; ++i)
	{
		if (rTMD->contents[i].index == rTMD->boot_index)
		{
			id = rTMD->contents[i].cid;
			break;
		}
	}
	if (id == 0)
	{
		gprintf("Unable to retrieve title booting app");
	}

	ret = id;
	return ret;
}

static bool inited_widgets = false;

view *m_main_init(void) {
    priiloader = priiloader_is_installed(TITLEID_SYSMENU);
    vwii = is_vwii();

    v_m_main = view_new(9, NULL, 0, 0, 0, 0);

    m_main_theme_reinit();
    m_main_update();

    view_set_focus(v_m_main, 0);

    return v_m_main;
}

void m_main_deinit(void) {
    view_free(v_m_main);
    inited_widgets = false;
    v_m_main = NULL;
}

void m_main_theme_reinit(void) {
    u16 x, y, yadd;
    int i;
    char buffer[20];

    text_no_ip = _("Network not initialized");
    text_has_ip = _("Your Wii's IP is %u.%u.%u.%u");
    text_number_apps = _("%d applications installed");

    if (inited_widgets)
        for (i = 0; i < v_m_main->widget_count; ++i)
            widget_free(&v_m_main->widgets[i]);

    if (vwii)
        yadd = 8;
    else
        yadd = 16;

    x = (view_width - theme_gfx[THEME_BUTTON]->w) / 2;
    y = 80;

    widget_button(&v_m_main->widgets[0], x, y, 0, BTN_NORMAL, _("Back"));
    y += theme_gfx[THEME_BUTTON]->h + yadd;
    widget_button(&v_m_main->widgets[1], x, y, 0, BTN_NORMAL, _("About"));
    y += theme_gfx[THEME_BUTTON]->h + yadd;

	widget_button(&v_m_main->widgets[2], x, y, 0, BTN_NORMAL,
					_("Launch Priiloader"));
	y += theme_gfx[THEME_BUTTON]->h + yadd;

    if (vwii) {
        widget_button(&v_m_main->widgets[3], x, y, 0, BTN_NORMAL,
                      _("Exit to vWii System Menu"));
        y += theme_gfx[THEME_BUTTON]->h + yadd;

        widget_button(&v_m_main->widgets[4], x, y, 0, BTN_NORMAL,
                      _("Reboot to Wii U Menu"));
    } else {
        widget_button(&v_m_main->widgets[3], x, y, 0, BTN_NORMAL,
                      _("Exit to System Menu"));
    }
    y += theme_gfx[THEME_BUTTON]->h + yadd; // the widget will always be added

    widget_button(&v_m_main->widgets[5], x, y, 0, BTN_NORMAL, _("Shutdown"));

    widget_label(&v_m_main->widgets[6], view_width / 3 * 2 - 16, 32, 0,
                 CHANNEL_VERSION_STR, view_width / 3 - 32, FA_RIGHT,
                 FA_ASCENDER, FONT_LABEL);

    sprintf(buffer, "IOS%d v%d.%d", IOS_GetVersion(), IOS_GetRevisionMajor(),
            IOS_GetRevisionMinor());

    widget_label(&v_m_main->widgets[7], view_width / 3 * 2 - 16,
                 32 + font_get_y_spacing(FONT_LABEL), 0, buffer,
                 view_width / 3 - 32, FA_RIGHT, FA_ASCENDER, FONT_LABEL);

    inited_widgets = true;
}

void m_main_update(void) {
    u32 ip;
    char buffer[64];
    char buffer2[64];

    if (loader_tcp_initialized()) {
        ip = net_gethostip();
        sprintf(buffer, text_has_ip, (ip >> 24) & 0xff, (ip >> 16) & 0xff,
                (ip >> 8) & 0xff, ip & 0xff);
        widget_label(&v_m_main->widgets[7], 48, 32, 0, buffer,
                     view_width / 3 * 2 - 32, FA_LEFT, FA_ASCENDER, FONT_LABEL);
    } else {
        widget_label(&v_m_main->widgets[7], 48, 32, 0, text_no_ip,
                     view_width / 3 * 2 - 32, FA_LEFT, FA_ASCENDER, FONT_LABEL);
    }

    sprintf(buffer2, text_number_apps, entry_count);
    widget_label(&v_m_main->widgets[8], 48, 48, 0, buffer2,
                 view_width / 3 * 2 - 48, FA_LEFT, FA_ASCENDER, FONT_LABEL);
}
