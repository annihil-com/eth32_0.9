// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "../eth32.h"

/*********************
* CWeaponConfigWindow
*********************/

#define VIRT_WEAP_SIZE		100
#define REAL_WEAP_SIZE		(VIRT_WEAP_SIZE * eth32.cg.screenXScale)

CWeaponConfigWindow::CWeaponConfigWindow(char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	CControl *ctrl;

	int yPos = (GUI_FONTPIXELHEIGHT * 2) + VIRT_WEAP_SIZE + 20;

	autoPoints = new CDropbox("Auto Aimpoints", 5, yPos, ww-10, 23, 0, AUTOPT_MAX-1, &eth32.settings.autoPoints, autoPointText);
	AddControl((CControl*)autoPoints);
	yPos += 28;

	headTraces = new CIntSlider("Head Points", 5, yPos, ww-10, 20, 0, 64, NULL);
	AddControl((CControl*)headTraces);
	yPos += 25;

	bodyTraces = new CIntSlider("Body Points", 5, yPos, ww-10, 20, 0, 64, NULL);
	AddControl((CControl*)bodyTraces);
	yPos += 25;

	range = new CIntSlider("Range", 5, yPos, ww-10, 20, 0, 10000, NULL);
	AddControl((CControl*)range);
	yPos += 25;

	delay = new CIntSlider("Delay", 5, yPos, ww-10, 20, 0, 1000, NULL);
	AddControl((CControl*)delay);
	yPos += 25;

	autofire = new CCheckBox("Autofire", 5, yPos, ww-10, 8, NULL);
	AddControl((CControl*)autofire);
	yPos += 13;

	ctrl = new CButton("Apply to all similar type", 5, yPos, ww-10, 12, funcApplyToType);
	AddControl(ctrl);
	yPos += 17;

	ctrl = new CButton("<<", 5, yPos, (ww-20)/3 , 12, funcPrevWeapon);
	AddControl(ctrl);
	setCurrent = new CButton("Current", (ww-20)/3+10, yPos, (ww-20)/3, 12, funcViewCurrent);
	AddControl((CControl*)setCurrent);
	ctrl = new CButton(">>", 2*(ww-20)/3+15, yPos, (ww-20)/3, 12, funcNextWeapon);
	AddControl(ctrl);
	yPos += 17;

	Resize(w, GUI_TITLEBAR_SIZE + (yPos * eth32.cg.screenYScale));		
}

void CWeaponConfigWindow::Display(void)
{
	if (!eth32.cg.weaponList || !show)	// no valid weaponlist, so nothing to modify
		return;

	if (!eth32.cg.modifyWeapon) {	// find our weapon to modify if not set yet
		if (eth32.cg.snap->ps.weapon) {
			eth32.cg.modifyWeapNum = eth32.cg.snap->ps.weapon;
			eth32.cg.modifyWeapon = &eth32.cg.weaponList[eth32.cg.snap->ps.weapon];
			SetWeaponFocus();
		}
		else {
			int findweap = 0;
			// search for user defined weapon
			while (!IS_WEAPATTRIB(findweap, WA_USER_DEFINED) && findweap < eth32.cg.numWeapons)
				findweap++;
			// make sure we are still in weaponlist
			if (findweap < eth32.cg.numWeapons) {
				eth32.cg.modifyWeapNum = findweap;
				eth32.cg.modifyWeapon = &eth32.cg.weaponList[findweap];
				SetWeaponFocus();
			}
			else return;	// no user defined weapons found
		}
	}
	SetWeaponFocus();

	// Only show current button if current weapon is different than showing and current weapon is user defined
	if (eth32.cg.snap->ps.weapon && eth32.cg.modifyWeapNum != eth32.cg.snap->ps.weapon && IS_WEAPATTRIB(eth32.cg.snap->ps.weapon, WA_USER_DEFINED))
		setCurrent->Show(true);
	else setCurrent->Show(false);

	CWindow::Display();

	weapdef_t *weap = eth32.cg.modifyWeapon;

	Engine.RenderWeapon(x+(w>>1)-(REAL_WEAP_SIZE / 2), y+GUI_TITLEBAR_SIZE+(GUI_FONTHEIGHT*2)+(GUI_SPACING*3), REAL_WEAP_SIZE, eth32.cg.modifyWeapNum);

	Draw.TextCenter(x+(w>>1), y+GUI_TITLEBAR_SIZE+GUI_SPACING, GUI_FONTSCALE, weap->name, GUI_FONTCOLOR1, qtrue, qfalse, GUI_FONT, true);
		
	char typetxt[32];
	switch (weap->type)
	{
	case WT_NONE:
		strcpy_s(typetxt, 32, "Non Weapon");
		break;
	case WT_PISTOL:
		strcpy_s(typetxt, 32, "Pistol");
		break;
	case WT_SMG:
		strcpy_s(typetxt, 32, "Sub-Machine Gun");
		break;
	case WT_MG:
		strcpy_s(typetxt, 32, "Machine Gun");
		break;
	case WT_EXPLOSIVE:
		strcpy_s(typetxt, 32, "Explosive");
		break;
	case WT_SNIPER:
		strcpy_s(typetxt, 32, "Sniper");
		break;
	case WT_RIFLE:
		strcpy_s(typetxt, 32, "Rifle");
		break;
	case WT_KNIFE:
		strcpy_s(typetxt, 32, "Knife");
		break;
	case WT_SHOTGUN:
		strcpy_s(typetxt, 32, "Shotgun");
		break;
	case WT_HEAVY:
		strcpy_s(typetxt, 32, "Heavy");
		break;
	default:
		strcpy_s(typetxt, 32, "Unknown");
		break;
	}

	Draw.TextCenter(x+(w>>1), y+GUI_TITLEBAR_SIZE+GUI_SPACING+GUI_FONTHEIGHT+GUI_SPACING, GUI_FONTSCALE, typetxt, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
}

int CWeaponConfigWindow::ProcessMouse(int mx, int my, UINT mevent, CControl **mhook, CControl **khook)
{
	int ret = CWindow::ProcessMouse(mx, my, mevent, mhook, khook);

	if (ret == 0) // a control or window handled event
		SetWeaponFocus();	// update controls, in case our mod weap has changed

	return ret;
}

void CWeaponConfigWindow::SetWeaponFocus(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;

	if (weap) {
		// If modify weapon valid, set all our controls to point to that weapon definition
		headTraces->SetTarget(&weap->headTraces);
		bodyTraces->SetTarget(&weap->bodyTraces);
		range->SetTarget(&weap->range);
		delay->SetTarget(&weap->delay);
		autofire->SetTarget(&weap->autofire);
	}
}

// Helper Functions

void funcApplyToType(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;
	weapdef_t *modweap = eth32.cg.weaponList;

	if (weap && modweap) { // sanity check
		for (int i=0 ; i<eth32.cg.numWeapons ; i++)
		{
			if (IS_WEAPATTRIB(i, WA_USER_DEFINED) && modweap[i].type == weap->type && weap != &modweap[i]) {
				modweap[i].headTraces = weap->headTraces;
				modweap[i].bodyTraces = weap->bodyTraces;
				modweap[i].range = weap->range;
				modweap[i].delay = weap->delay;
				modweap[i].autofire = weap->autofire;
			}
		}
	}
}

void funcPrevWeapon(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;
	int weapnum = eth32.cg.modifyWeapNum - 1;

	if (!weap)
		return;

	if (weapnum < 0) // this shouldn't happen, but to be safe
		weapnum = eth32.cg.numWeapons;

	while (!IS_WEAPATTRIB(weapnum, WA_USER_DEFINED))
	{
		weapnum--;
		if (weapnum < 0)
			weapnum = eth32.cg.numWeapons;

		// We went through the whole list and didn't find a valid weapon def
		if (weap == &eth32.cg.weaponList[weapnum])
			return;
	}
	eth32.cg.modifyWeapNum = weapnum;
	eth32.cg.modifyWeapon = &eth32.cg.weaponList[weapnum];
}

void funcNextWeapon(void)
{
	weapdef_t *weap = eth32.cg.modifyWeapon;
	int weapnum = eth32.cg.modifyWeapNum + 1;

	if (!weap)
		return;

	if (weapnum > eth32.cg.numWeapons) // current modify weapon is last in list
		weapnum = 0;

	while (!IS_WEAPATTRIB(weapnum, WA_USER_DEFINED))
	{
		weapnum++;
		if (weapnum > eth32.cg.numWeapons)
			weapnum = 0;

		// We went through the whole list and didn't find a valid weapon def
		if (weap == &eth32.cg.weaponList[weapnum])
			return;
	}
	eth32.cg.modifyWeapNum = weapnum;
	eth32.cg.modifyWeapon = &eth32.cg.weaponList[weapnum];
}

void funcViewCurrent(void)
{
	if (eth32.cg.snap->ps.weapon && IS_WEAPATTRIB(eth32.cg.snap->ps.weapon, WA_USER_DEFINED)) {
		eth32.cg.modifyWeapNum = eth32.cg.snap->ps.weapon;
		eth32.cg.modifyWeapon = &eth32.cg.weaponList[eth32.cg.snap->ps.weapon];
	}
}

/*********************
* CHitboxWindow
*********************/

const char *hbOffsetText[] = {
	"Standing",
	"Running",
	"Crouching",
	"Crawling",
	"Prone",
};

CHitboxWindow::CHitboxWindow(char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	hitbox = &Aimbot.customHitbox;
	curPart = HB_STAND;

	AddControl(new CDropbox("Offset", 5, 5, ww-10, 23, 0, HB_MAX-1, &curPart, hbOffsetText));

	xOff = new CFloatSlider("X", 5, 33, ww-10, 20, -10, 10, &hitbox->stand_offset[0]);
	yOff = new CFloatSlider("Y", 5, 58, ww-10, 20, -10, 10, &hitbox->stand_offset[1]);
	zOff = new CFloatSlider("Z", 5, 83, ww-10, 20, -10, 10, &hitbox->stand_offset[2]);

	AddControl(xOff);
	AddControl(yOff);
	AddControl(zOff);

	AddControl(new CFloatSlider("Length (x)", 5, 108, ww-10, 20, 0, 15, &hitbox->size[0]));
	AddControl(new CFloatSlider("Width (y)", 5, 133, ww-10, 20, 0, 15, &hitbox->size[1]));
	AddControl(new CFloatSlider("Height (z)", 5, 158, ww-10, 20, 0, 15, &hitbox->size[2]));
}

void CHitboxWindow::Display(void)
{
	if (eth32.settings.hitboxType != HITBOX_CUSTOM)
		return;

	AssignHitboxPart();

	CWindow::Display();
}

void CHitboxWindow::AssignHitboxPart(void)
{
	float *curOffset;

	if (curPart == HB_STAND)
		curOffset = hitbox->stand_offset;
	else if (curPart == HB_RUNNING)
		curOffset = hitbox->stand_offset_moving;
	else if (curPart == HB_CROUCH)
		curOffset = hitbox->crouch_offset;
	else if (curPart == HB_CRAWL)
		curOffset = hitbox->crouch_offset_moving;
	else if (curPart == HB_PRONE)
		curOffset = hitbox->prone_offset;
	else // shouldn't ever get here
		curOffset = hitbox->stand_offset;

	xOff->SetTarget(&curOffset[0]);
	yOff->SetTarget(&curOffset[1]);
	zOff->SetTarget(&curOffset[2]);
}

/*********************
* CWeaponWindow
*********************/

CWeaponWindow::CWeaponWindow(char *wlabel, int wx, int wy, int ww, int wh)
: CWindow(wlabel, wx, wy, ww, wh)
{
	showTitle = false;
	chargeBarSize = 6 * eth32.cg.screenYScale;
	
	int fontSize = h - GUI_FONTHEIGHT - chargeBarSize - (4 * GUI_SPACING);
	ammoFontScale = 2.f;
	
	while (Draw.sizeTextHeight(ammoFontScale, GUI_FONT) > fontSize)
		ammoFontScale -= 0.001;
}

void CWeaponWindow::Display(void)
{
	if (!eth32.cg.snap || !eth32.cg.currentWeapon || !eth32.cg.snap->ps.weapon || eth32.settings.guiOriginal)
		return;

	CWindow::Display();

	int drawX = x + GUI_SPACING;
	int drawY = y + GUI_SPACING;

	weapdef_t *weap = &eth32.cg.weaponList[eth32.cg.snap->ps.weapon];

#ifdef ETH32_DEBUG
	char buf[64];
	sprintf_s(buf, 64, "%s (%i)", weap->name, eth32.cg.snap->ps.weapon);
	Draw.TextCenter(x + ((w-h) >> 1), drawY, GUI_FONTSCALE, buf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
#else
	Draw.TextCenter(x + ((w-h) >> 1), drawY, GUI_FONTSCALE, weap->name, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
#endif

	// get current charge time (ripped and modified from SDK)
	int chargeTime;
	if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
		chargeTime = eth32.cg.chargeTimes.engineer[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
		chargeTime = eth32.cg.chargeTimes.medic[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_FIELDOPS ) {
		chargeTime = eth32.cg.chargeTimes.lt[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( eth32.cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_COVERTOPS ) {
		chargeTime = eth32.cg.chargeTimes.covops[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	} else {
		chargeTime = eth32.cg.chargeTimes.soldier[eth32.cg.snap->ps.persistant[PERS_TEAM]-1];
	}

	float frac;
	if (chargeTime)
		frac = (float)(eth32.cg.time - eth32.cg.snap->ps.classWeaponTime) / chargeTime;
	else
		frac = 1.f;
			
	drawY += GUI_FONTHEIGHT + GUI_SPACING;
	// draw weapon charge
	Draw.Statbar(drawX, drawY, w-h-GUI_SPACING-GUI_SPACING, chargeBarSize, frac, colorWhite);	// this will eventually show weapon charge
	// draw weapon heat
	if (eth32.cg.snap->ps.curWeapHeat)
		Draw.Statbar2(drawX, drawY, w-h-GUI_SPACING-GUI_SPACING, chargeBarSize, (float)eth32.cg.snap->ps.curWeapHeat/255.f, colorRed);

	drawY += chargeBarSize + GUI_SPACING;

	if (GetCurrentAmmo()) {
		char ammobuf[64];
		if (akimbo >= 0) 
			sprintf_s(ammobuf, 64, "%i|%i/%i", akimbo, clip, ammo);
		else if (ammo >= 0)
			sprintf_s(ammobuf, 64, "%i/%i", clip, ammo);
		else 
			sprintf_s(ammobuf, 64, "%i", clip);

		Draw.TextCenter(x + ((w-h) >> 1), drawY, ammoFontScale, ammobuf, GUI_FONTCOLOR2, qtrue, qfalse, GUI_FONT, true);
	}

	Engine.RenderWeapon(x2-h+5, y+5, h-10);
}

bool CWeaponWindow::GetCurrentAmmo(void)
{
	ammo = clip = akimbo = -1;

	if (!orig_BG_FindClipForWeapon || !orig_BG_FindAmmoForWeapon)
		return false;

	if (!eth32.cg.snap || !eth32.cg.snap->ps.weapon)
		return false;

	if (eth32.cg.snap->ps.eFlags & EF_MG42_ACTIVE || eth32.cg.snap->ps.eFlags & EF_MOUNTEDTANK)
		return false;

	int weap = eth32.cg.snap->ps.weapon;

	if (IS_WEAPATTRIB(weap, WA_NO_AMMO))
		return false;

	clip = eth32.cg.snap->ps.ammoclip[orig_BG_FindClipForWeapon(weap)];

	if (IS_WEAPATTRIB(weap, WA_ONLY_CLIP))
		return true;

	ammo = eth32.cg.snap->ps.ammo[orig_BG_FindAmmoForWeapon(weap)];

	if (IS_WEAPATTRIB(weap, WA_AKIMBO))
		akimbo = eth32.cg.snap->ps.ammoclip[orig_BG_FindClipForWeapon(eth32.cg.weaponList[weap].otherAkimbo)];

	return true;
}







