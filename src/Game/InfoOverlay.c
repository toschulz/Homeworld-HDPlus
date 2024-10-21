/*=============================================================================
    Name    : InfoOverlay.c
    Purpose : This file contains the logic for the information overlay.

    Created 6/8/1998 by ddunlop
    Copyright Relic Entertainment, Inc.  All rights reserved.
=============================================================================*/

#include "InfoOverlay.h"

#include <stdio.h>

#include "FontReg.h"
#include "Globals.h"
#include "Key.h"
#include "Select.h"
#include "StatScript.h"
#include "StringSupport.h"
#include "Task.h"
#include "Tutor.h"
#include "utility.h"


/*=============================================================================
    Defines:
=============================================================================*/

#define numOverlays             (TOTAL_NUM_SHIPS)
#define numButtons              TOTAL_NUM_SHIPS
#define IO_VertSpacing          4
#define IO_HorzSpacing          (fontWidth(" "))
//#define ioShipListWidth            250

#define IO_DefaultFont          "HW_EuroseCond_11.hff"


/*=============================================================================
    Data:
=============================================================================*/


fonthandle   ioShipListFont=0;
char         ioShipListFontName[64] = IO_DefaultFont;
int          ioShipListWidth = 150;

ShipListInfo  listinfo[numOverlays + 1]=
{
    { 0, 0, LightInterceptor,      NULL },
    { 0, 0, LightDefender,         NULL },
    { 0, 0, HeavyDefender,         NULL },
    { 0, 0, HeavyInterceptor,      NULL },
    { 0, 0, AttackBomber,          NULL },
    { 0, 0, CloakedFighter,        NULL },
    { 0, 0, DefenseFighter,        NULL },
    { 0, 0, P1Fighter,             NULL },
    { 0, 0, P2Swarmer,             NULL },
    { 0, 0, P2AdvanceSwarmer,      NULL },
    { 0, 0, LightCorvette,         NULL },
    { 0, 0, RepairCorvette,        NULL },
    { 0, 0, SalCapCorvette,        NULL },
    { 0, 0, MultiGunCorvette,      NULL },
    { 0, 0, HeavyCorvette,         NULL },
    { 0, 0, MinelayerCorvette,     NULL },
    { 0, 0, P1StandardCorvette,    NULL },
    { 0, 0, P1MissileCorvette,     NULL },
    { 0, 0, AdvanceSupportFrigate, NULL },
    { 0, 0, StandardFrigate,       NULL },
    { 0, 0, DDDFrigate,            NULL },
    { 0, 0, DFGFrigate,            NULL },
    { 0, 0, IonCannonFrigate,      NULL },
    { 0, 0, P1IonArrayFrigate,     NULL },
    { 0, 0, P2FuelPod,             NULL },
    { 0, 0, P2MultiBeamFrigate,    NULL },
    { 0, 0, P3Frigate,             NULL },
    { 0, 0, StandardDestroyer,     NULL },
    { 0, 0, MissileDestroyer,      NULL },
    { 0, 0, Carrier,               NULL },
    { 0, 0, HeavyCruiser,          NULL },
    { 0, 0, Probe,                 NULL },
    { 0, 0, ProximitySensor,       NULL },
    { 0, 0, SensorArray,           NULL },
    { 0, 0, ResourceCollector,     NULL },
    { 0, 0, ResourceController,    NULL },
    { 0, 0, ResearchShip,          NULL },
    { 0, 0, GravWellGenerator,     NULL },
    { 0, 0, CloakGenerator,        NULL },
    { 0, 0, Mothership,            NULL },
    { 0, 0, DefaultShip,           NULL },   // target drone
    { 0, 0, DefaultShip,           NULL },   // drone
    { 0, 0, DefaultShip,           NULL },   // headshot asteroid
    { 0, 0, CryoTray,              NULL },
    { 0, 0, P1Mothership,          NULL },
    { 0, 0, P2Mothership,          NULL },
    { 0, 0, P3Destroyer,           NULL },
    { 0, 0, P3Megaship,            NULL },
    { 0, 0, FloatingCity,          NULL },
    { 0, 0, CargoBarge,            NULL },
    { 0, 0, MiningBase,            NULL },
    { 0, 0, ResearchStation,       NULL },
    { 0, 0, JunkYardDawg,          NULL },
    { 0, 0, JunkYardHQ,            NULL },
    { 0, 0, Ghostship,             NULL },
    { 0, 0, Junk_LGun,             NULL },
    { 0, 0, Junk_SGun,             NULL },
    { 0, 0, ResearchStationBridge, NULL },
    {-1, 0, ResearchStationTower , NULL }
};

ShipOverlayInfo overlayinfo[numButtons];

sdword          lookupforlist[TOTAL_NUM_SHIPS + 1];

color ioListTextColor     = IO_ListTextColor;
color ioSelectedTextColor = IO_SelectedTextColor;

bool32            ioRunning=TRUE;

int io_largest_y_cord = 0;
int io_largest_x_cord = 0;
int io_smallest_x_cord = 0;
int io_smallest_y_cord = 0;

color io_bg_color_a = colRGBA(0  , 100, 160, 100);
color io_bg_color_b = colRGBA(0  , 100, 160, 135);
color io_border_color = colRGBA(0  , 100, 160, 255);

//int

/*=============================================================================
    Logic:
=============================================================================*/

// Function callback for maks of region
udword ioListClick(regionhandle reg, sdword ID, udword event, udword data);

// Draw callback for list of ships selected
void ioShipListDraw(regionhandle region);

bool32 crapthing(udword num, void *data, struct BabyCallBack *baby)
{
    ioUpdateShipTotals();

    return(TRUE);
}

udword ioListClick(regionhandle reg, sdword ID, udword event, udword data)
{
	if (keyIsHit(SHIFTKEY))
	{
		//dbgMessagef("ioListClick, shift");
		overlayinfo[ID].listinfo->bSelected = (bool16)!overlayinfo[ID].listinfo->bSelected;
	} else if (keyIsHit(CONTROLKEY))
	{
		//dbgMessagef("Alt IF_O, id: %d", ID);
		overlayinfo[ID].listinfo->bSelected = (bool16)!overlayinfo[ID].listinfo->bSelected;
	}
	else
	{
		overlayinfo[ID].listinfo->bSelected = TRUE;
		ioSetSelection(FALSE);
	}
	tutGameMessage("Game_InfoOverlayClick");

	return(0);
}

void getioShipListHieght()
{
	int counter = 0;
	for (int i = 0; i < numButtons; i++)
	{
		if (overlayinfo[i].listinfo != NULL && overlayinfo[i].inlist != 0)
		{
			counter++;
		}
	}

	if (counter != 0)
	{
		rectangle my_rec;
		my_rec.x0 = ghMainRegion->rect.x1-ioShipListWidth-9;
		my_rec.y0 = 0;
		my_rec.x1 = MAIN_WindowWidth - 2;
		my_rec.y1 = my_rec.y0+counter*(fontHeight(" ")+4)+8;
		color io_border_color = colRGBA(0  , 100, 160, 255);
		primRectOutline2(&my_rec, 2, io_border_color);

		int old_y1 =my_rec.y0;
		int old_y2 = my_rec.y1;

		//bottom fill
		my_rec.x0 = ghMainRegion->rect.x1-ioShipListWidth-9;
		my_rec.y0 = old_y2-4;
		my_rec.x1 = ghMainRegion->rect.x1+ioShipListWidth;
		my_rec.y1 = old_y2;
		if (counter%2 == 0)
		{
			primRectTranslucent2(&my_rec,io_bg_color_b);
		} else
		{
			primRectTranslucent2(&my_rec,io_bg_color_a);
		}

		//top fill
		my_rec.x0 = ghMainRegion->rect.x1-ioShipListWidth-9;
		my_rec.y0 = old_y1;
		my_rec.x1 = ghMainRegion->rect.x1+ioShipListWidth;
		my_rec.y1 = ghMainRegion->rect.y0+IO_VertSpacing;
		primRectTranslucent2(&my_rec,io_bg_color_a);
	}
}

void ioShipListDraw(regionhandle region)
{
    fonthandle oldfont;
    char       tempstr[64];
    color      col;

    oldfont = fontMakeCurrent(ioShipListFont);

    if (overlayinfo[region->userID].listinfo->bSelected)
        col = ioSelectedTextColor;
    else
        col = ioListTextColor;

	if (ext_info_overlay)
	{
		rectangle my_rec;
		my_rec.x0 = region->rect.x0-9;
		my_rec.y0 = region->rect.y0;
		my_rec.x1 = region->rect.x1;
		my_rec.y1 = region->rect.y1;
		if (region->userID%2 == 0)
		{
			primRectTranslucent2(&my_rec,io_bg_color_a);
		} else
		{
			primRectTranslucent2(&my_rec,io_bg_color_b);
		}
	}

    sprintf(tempstr, " %i",overlayinfo[region->userID].listinfo->nShips);
    fontPrintf(region->rect.x0+fontWidth(" 55 ")-fontWidth(tempstr), region->rect.y0, col,"%s",tempstr);
	if (homeworld_hdplus == TRUE)
	{
		char* io_string = strGetString(overlayinfo[region->userID].listinfo->shipnum + strShipAbrevOffset);
		capitalize(io_string);
		fontPrintf(region->rect.x0+fontWidth(" 55 "), region->rect.y0+2, col," x %s", io_string);
	} else
	{
		//fontPrintf(region->rect.x0+fontWidth(" 55 ")-fontWidth(tempstr), region->rect.y0, col,"%s",tempstr);
		fontPrintf(region->rect.x0+fontWidth(" 55 "), region->rect.y0, col," x %s", strGetString(overlayinfo[region->userID].listinfo->shipnum + strShipAbrevOffset));
	}
	//dbgMessagef("ioShipListDraw, y:%d", region->rect.y0);
	//getioShipListHieght();
    fontMakeCurrent(oldfont);
}

void ioSetSelection(bool32 shiftrelease)
{
    MaxSelection temp;
    sdword       index;
    bool32         ships=FALSE;

    temp.numShips = 0;

    for (index=0;index<selSelected.numShips;index++)
    {
        if (selSelected.ShipPtr[index]->shiptype < TOTAL_NUM_SHIPS)
        {
            if (listinfo[lookupforlist[selSelected.ShipPtr[index]->shiptype]].bSelected)
            {
//                if (selSelected.ShipPtr[index]->shiptype!=Mothership)
//                {
                    selSelectionAddSingleShip(&temp,selSelected.ShipPtr[index]);
                    ships=TRUE;
//                }
            }
        }
    }

    if (shiftrelease)
    {
        if (ships)
        {
            selSelectionCopy((MaxAnySelection *)&selSelected, (MaxAnySelection *)&temp);

            taskCallBackRegister(crapthing, 0, NULL, (real32)0.25);
            tutGameMessage("Game_InfoOverlayShiftSelect");
        }
    }
    else
    {
        selSelectionCopy((MaxAnySelection *)&selSelected, (MaxAnySelection *)&temp);

        taskCallBackRegister(crapthing, 0, NULL, (real32)0.25);
        tutGameMessage("Game_InfoOverlaySelect");
    }
}

void ioCtrlSetSelection(bool32 shiftrelease)
{
	MaxSelection temp;
	sdword       index;
	bool32         ships=FALSE;

	temp.numShips = 0;

	for (index=0;index<selSelected.numShips;index++)
	{
		if (selSelected.ShipPtr[index]->shiptype < TOTAL_NUM_SHIPS)
		{
			if (listinfo[lookupforlist[selSelected.ShipPtr[index]->shiptype]].bSelected == FALSE)
			{
				selSelectionAddSingleShip(&temp,selSelected.ShipPtr[index]);
				ships=TRUE;
			}
		}
	}

	if (shiftrelease)
	{
		if (ships)
		{
			selSelectionCopy((MaxAnySelection *)&selSelected, (MaxAnySelection *)&temp);

			taskCallBackRegister(crapthing, 0, NULL, (real32)0.25);
			tutGameMessage("Game_InfoOverlayShiftSelect");
		}
	}
	else
	{
		selSelectionCopy((MaxAnySelection *)&selSelected, (MaxAnySelection *)&temp);

		taskCallBackRegister(crapthing, 0, NULL, (real32)0.25);
		tutGameMessage("Game_InfoOverlaySelect");
	}
}

void ioUpdateShipTotals(void)
{
    sdword index, screen;

    if (ioRunning || (tutorial==TUTORIAL_ONLY))
    {
        for (index=0;listinfo[index].nShips!=-1;index++)
        {
            listinfo[index].nShips = 0;
            listinfo[index].bSelected   = FALSE;
        }

        for (index=0;index<selSelected.numShips;index++)
        {
            if (selSelected.ShipPtr[index]->shiptype < TOTAL_NUM_SHIPS )
            {
                listinfo[lookupforlist[selSelected.ShipPtr[index]->shiptype]].nShips++;
            }
        }

        for (index=0,screen=0;listinfo[index].nShips!=-1;index++)
        {
            if (listinfo[index].nShips>0)
            {
                overlayinfo[screen].listinfo = &listinfo[index];
                if (!overlayinfo[screen].inlist)
                {
                    regChildInsert(overlayinfo[screen].region,ghMainRegion);
                    overlayinfo[screen].inlist = TRUE;
                }
                screen++;
            }
        }

        for (;screen<numButtons;screen++)
        {
            if (overlayinfo[screen].inlist)
            {
                regLinkRemove(overlayinfo[screen].region);
                overlayinfo[screen].inlist = FALSE;
            }
        }
    }
}

bool32 ioDisable(void)
{
    sdword index;
    bool32   save;

    save = ioRunning;
    ioRunning = FALSE;
    for (index=0;index<numButtons;index++)
    {
        if (overlayinfo[index].inlist)
        {
            regLinkRemove(overlayinfo[index].region);
            overlayinfo[index].inlist = FALSE;
        }
    }
    return (save);
}

void ioEnable(void)
{
    ioRunning = TRUE;
    ioUpdateShipTotals();
}

void ioStartup(void)
{
    sdword index, x, y;
    fonthandle oldfont;

    ioShipListFont = frFontRegister(ioShipListFontName);
    oldfont        = fontMakeCurrent(ioShipListFont);

    y = ghMainRegion->rect.y0+IO_VertSpacing;
    x = ghMainRegion->rect.x1-ioShipListWidth;

    for (index=0;index<numButtons;index++)
    {
        overlayinfo[index].region = regChildAlloc(ghMainRegion, index,
                                                  x, y, ioShipListWidth, fontHeight(" ")+4,
                                                  0, RPE_PressLeft);
        regDrawFunctionSet(overlayinfo[index].region, ioShipListDraw);
        regFunctionSet(overlayinfo[index].region, (regionfunction) ioListClick);
        regLinkRemove(overlayinfo[index].region);
        overlayinfo[index].inlist = FALSE;
        y+=fontHeight(" ")+4;
    }
    io_largest_y_cord = y;
	dbgMessagef("Y Cord: %d", io_largest_y_cord);

    for (index=0;index<TOTAL_NUM_SHIPS;index++)
    {
        lookupforlist[index] = numOverlays;
    }

    for (index=0;listinfo[index].nShips!=-1;index++)
    {
        lookupforlist[listinfo[index].shipnum] = index;
    }

    fontMakeCurrent(oldfont);
}

void ioShutdown(void)
{
    sdword index;

    for (index=0;index<numButtons;index++)
    {
        regRegionDelete(overlayinfo[index].region);
    }
}

/*-----------------------------------------------------------------------------
    Name        : ioResolutionChange
    Description : Called when there is a change in screen resolution.
    Inputs      :
    Outputs     :
    Return      :
----------------------------------------------------------------------------*/
void ioResolutionChange(void)
{
    sdword index, x, diff;

    x = ghMainRegion->rect.x1-ioShipListWidth;
    diff = x - overlayinfo[0].region->rect.x0;

    for (index=0;index<numButtons;index++)
    {
        overlayinfo[index].region->rect.x0 += diff;
        overlayinfo[index].region->rect.x1 += diff;
    }

}

