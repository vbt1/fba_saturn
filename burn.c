// Burn - Drivers module

#include "burnint.h"
#include "saturn/ovl.h"

/*static*/ struct BurnDriver BurnDrvbankp = {
	"bankp", NULL,
	"Bank Panic\0", 
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvcombh = {
	"combh", "bankp", 
	"Combat Hawk\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvNewsa = {
	"newsa", "news", 
	"News (set 2)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvNews = {
	"news", NULL,
	"News (set 1)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvGberet = {
	"gberet", NULL,
	"Green Beret\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvMrgoemon = {
	"mrgoemon", "gberet",
	"Mr. Goemon (Japan)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvhigemaru = {
	"higema", NULL,
	"Pirate Ship Higemaru\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvpkunwar = {
	"pkunw", NULL,
	"Penguin-Kun Wars (US)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvPang = {
	"pang", "mitch",
	"Pang (World)\0", 
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvSpang = {
	"spang", "mitch",
	"Super Pang (World 900914)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};
  
/*static*/ struct BurnDriver BurnDrvGnga = {
	"gnga", "gng",
	"Ghosts'n Goblins (World? set 2)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvBlockgal = {
	"blockgal", "sys1h",
	"Block Gal (MC-8123B, 317-0029)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};
	
/*static*/ struct BurnDriver BurnDrvWboyu = {
	"wboyu", "sys1",
	"Wonder Boy (not encrypted)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvChplftb = {
	"chplftb", "sys2",
	"Choplifter (Alternate)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvGardia = {
	"gardia", "sys1h",
	"Gardia (317-0006)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvFlickys2 = {
	"flickys2", "sys1",
	"Flicky (128k, System 2, not encrypted)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvMyhero = {
	"myhero", "sys1", 
	"My Hero (US, not encrypted)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};
 
/*static*/ struct BurnDriver BurnDrvTeddybb = {
	"teddybb", "sys1",
	"TeddyBoy Blues (315-5115, New Ver.)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvPitfall2u = {
	"pitfal2u", "sys1",
	"Pitfall II (not encrypted)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvWbdeluxe = {
	"wbdeluxe", "sys1",
	"Wonder Boy Deluxe\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvpengo2u = {
	"pengo2u", "pacm",
	"Pengo (set 2 not encrypted)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvStarjack = {
	"starjack", "sys1h",
	"Star Jacker (Sega)\0", 
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvRaflesia = {
	"raflesia", "sys1h",
	"Rafflesia (315-5162)\0", 
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};
/*
struct BurnDriver BurnDrvpacman = {
	"pacman", "pacm",
	"Pac-Man (Midway)\0",
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};	  */

/*static*/ struct BurnDriver BurnDrvAtetris = {
		"atetris", "tetris",
		"Tetris (set 1)\0",
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL
};

/*static*/ struct BurnDriver BurnDrvsms_akmw = {
		"sms", NULL,
		"Sega Master System\0",
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL
};

/*static*//* struct BurnDriver BurnDrvFourdwarrio = {
		"4dwarrio", "sys1",
		"4-D Warriors (315-5162)\0",
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL
	};	  */
/*
	struct BurnDriver BurnDrvmspacman = {
		"mspacman", "pacm",
		"MS Pacman\0",
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL
};

	struct BurnDriver BurnDrvpuckman = {
		"puckman", "pacm",
		"Puck Man (Japan set 1)\0",
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL
};
*/
struct BurnDriver* pDriver[] __attribute__((aligned (4)))
	/*__attribute__ ((section(".pdriver")))*/ = {
//&BurnDrvFourdwarrio,
&BurnDrvsms_akmw,
&BurnDrvbankp,
&BurnDrvBlockgal,
&BurnDrvChplftb,
&BurnDrvcombh,
&BurnDrvFlickys2,
&BurnDrvGardia,
&BurnDrvGnga,
&BurnDrvGberet,
&BurnDrvMrgoemon,
&BurnDrvMyhero,
&BurnDrvNews,
&BurnDrvNewsa,
&BurnDrvPang,
&BurnDrvSpang,
&BurnDrvpkunwar,	   
&BurnDrvpengo2u,
&BurnDrvhigemaru,
&BurnDrvPitfall2u,
&BurnDrvRaflesia,
&BurnDrvStarjack,
//&BurnDrvGnga//
//&BurnDrvpuckman,
//&BurnDrvmspacman,
&BurnDrvTeddybb,
&BurnDrvAtetris,
//&BurnDrvpacman,
&BurnDrvWboyu,
&BurnDrvWbdeluxe
//&BurnDrvBlckgalb,
//&BurnDrvChaknpop, 
/* 
//&BurnDrvNprincesu, */
//&BurnDrvSeganinju,
//&BurnDrvVigilant,
//&BurnDrvWbmljb,
//&BurnDrvPengo2u
};
int BurnLibInit()
{
	nBurnDrvSelect=0;
	nBurnDrvCount = sizeof(pDriver) / sizeof(pDriver[0]);	// count available drivers

	BurnDrvWbdeluxe.szShortName = "wbdeluxe";
	BurnDrvWbdeluxe.szParent = "sys1";
	BurnDrvWbdeluxe.szFullNameA = "Wonder Boy Deluxe\0";

	BurnDrvGardia.szShortName = "gardia";
	BurnDrvGardia.szParent = "sys1h";
	BurnDrvGardia.szFullNameA = "Gardia (317-0006)\0";

	return 0;
}
// ----------------------------------------------------------------------------
// Static functions which forward to each driver's data and functions

// Get the ASCII text fields for the driver in ASCII format;
char* BurnDrvGetTextA(unsigned int i)
{
	switch (i) {
		case DRV_NAME:
			return pDriver[nBurnDrvSelect]->szShortName;
		case DRV_FULLNAME:
			return pDriver[nBurnDrvSelect]->szFullNameA;
		case DRV_PARENT:
			return pDriver[nBurnDrvSelect]->szParent;
		default:
			return NULL;
	}
}
int BurnDrvGetRomInfo(struct BurnRomInfo* pri, unsigned int i)		// Forward to drivers function
{
	return pDriver[nBurnDrvSelect]->GetRomInfo(pri, i);
}
int BurnDrvGetRomName(char** pszName, unsigned int i, int nAka)		// Forward to drivers function
{
	return pDriver[nBurnDrvSelect]->GetRomName(pszName, i, nAka);
}

int BurnDrvGetInputInfo(struct BurnInputInfo* pii, unsigned int i)	// Forward to drivers function
{
	return pDriver[nBurnDrvSelect]->GetInputInfo(pii, i);
}

int BurnDrvGetDIPInfo(struct BurnDIPInfo* pdi, unsigned int i)
{
	if (pDriver[nBurnDrvSelect]->GetDIPInfo) {									// Forward to drivers function
		return pDriver[nBurnDrvSelect]->GetDIPInfo(pdi, i);
	}

	return 1;																	// Fail automatically
}

// Init game emulation (loading any needed roms)
int BurnDrvInit()
{
	int nReturnValue;
	if (nBurnDrvSelect >= nBurnDrvCount) {
		return 1;
	}
	nReturnValue = pDriver[nBurnDrvSelect]->Init();	// Forward to drivers function

	return nReturnValue;
}

// Exit game emulation
int BurnDrvExit()
{
	if (pDriver[nBurnDrvSelect]->Exit) {
		return pDriver[nBurnDrvSelect]->Exit();			// Forward to drivers function
	}
}
// Do one frame of game emulation
int BurnDrvFrame()
{
	return pDriver[nBurnDrvSelect]->Frame();		// Forward to drivers function
}
// Force redraw of the screen
/*
int BurnDrvRedraw()
{
	if (pDriver[nBurnDrvSelect]->Redraw) {
		return pDriver[nBurnDrvSelect]->Redraw();	// Forward to drivers function
	}

	return 1;										// No funtion provide, so simply return
}
*/
// Application-defined rom loading function:
int (__cdecl *BurnExtLoadRom)(unsigned char *Dest,int *pnWrote,int i, int nGap,int bXor) = NULL;
