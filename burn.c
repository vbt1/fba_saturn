// Burn - Drivers module

#include "burnint.h"
//#include "saturn/ovl.h"

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
struct BurnDriver* pDriver[25] __attribute__((aligned (4)));

/*	 =
		{
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
//&BurnDrvSeganinju,
//&BurnDrvVigilant,
//&BurnDrvWbmljb,
//&BurnDrvPengo2u
};
*/
int BurnLibInit()
{
	nBurnDrvSelect=0;
	nBurnDrvCount = sizeof(pDriver) / sizeof(pDriver[0]);	// count available drivers
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
void BurnDrvAssignList()
{
	static struct BurnDriver BurnDrvbankp = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvcombh = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNewsa = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNews = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGberet = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvMrgoemon = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvhigemaru = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvpkunwar = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvPang = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvSpang = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGnga = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvBlockgal = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWboyu = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvChplftb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGardia = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvFlickys2 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvMyhero = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvTeddybb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvPitfall2u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWbdeluxe = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvpengo2u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvStarjack = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvRaflesia = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvAtetris = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_akmw = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};

BurnDrvsms_akmw.szShortName="sms";
BurnDrvsms_akmw.szFullNameA="Sega Master System\0";
BurnDrvsms_akmw.szParent=NULL;

BurnDrvbankp.szShortName="bankp";
BurnDrvbankp.szFullNameA="Bank Panic\0";
BurnDrvbankp.szParent=NULL;

BurnDrvcombh.szShortName="combh";
BurnDrvcombh.szFullNameA="Combat Hawk\0";
BurnDrvcombh.szParent="bankp";

BurnDrvNewsa.szShortName="newsa";
BurnDrvNewsa.szFullNameA="News (set 2)\0";
BurnDrvNewsa.szParent="news";

BurnDrvNews.szShortName="news";
BurnDrvNews.szFullNameA="News (set 1)\0";
BurnDrvNews.szParent=NULL;

BurnDrvGberet.szShortName="gberet";
BurnDrvGberet.szFullNameA="Green Beret\0";
BurnDrvGberet.szParent=NULL;

BurnDrvMrgoemon.szShortName="mrgoemon";
BurnDrvMrgoemon.szFullNameA="Mr. Goemon (Japan)\0";
BurnDrvMrgoemon.szParent="gberet";

BurnDrvhigemaru.szShortName="higema";
BurnDrvhigemaru.szFullNameA="Pirate Ship Higemaru\0";
BurnDrvhigemaru.szParent=NULL;

BurnDrvpkunwar.szShortName="pkunw";
BurnDrvpkunwar.szFullNameA="Penguin-Kun Wars (US)\0";
BurnDrvpkunwar.szParent=NULL;

BurnDrvPang.szShortName="pang";
BurnDrvPang.szFullNameA="Pang (World)\0";
BurnDrvPang.szParent="mitch";

BurnDrvSpang.szShortName="spang";
BurnDrvSpang.szFullNameA="Super Pang (World 900914)\0";
BurnDrvSpang.szParent="mitch";

BurnDrvGnga.szShortName="gnga";
BurnDrvGnga.szFullNameA="Ghosts'n Goblins (World? set 2)\0";
BurnDrvGnga.szParent="gng";

BurnDrvBlockgal.szShortName="blockgal";
BurnDrvBlockgal.szFullNameA="Block Gal (MC-8123B, 317-0029)\0";
BurnDrvBlockgal.szParent="sys1h";

BurnDrvWboyu.szShortName="wboyu";
BurnDrvWboyu.szFullNameA="Wonder Boy (not encrypted)\0";
BurnDrvWboyu.szParent="sys1";

BurnDrvChplftb.szShortName="chplftb";
BurnDrvChplftb.szFullNameA="Choplifter (Alternate)\0";
BurnDrvChplftb.szParent="sys2";

BurnDrvGardia.szShortName="gardia";
BurnDrvGardia.szFullNameA="Gardia (317-0006)\0";
BurnDrvGardia.szParent="sys1h";

BurnDrvFlickys2.szShortName="flickys2";
BurnDrvFlickys2.szFullNameA="Flicky (128k, System 2, not encrypted)\0";
BurnDrvFlickys2.szParent="sys1";

BurnDrvMyhero.szShortName="myhero";
BurnDrvMyhero.szFullNameA="My Hero (US, not encrypted)\0";
BurnDrvMyhero.szParent="sys1";

BurnDrvTeddybb.szShortName="teddybb";
BurnDrvTeddybb.szFullNameA="TeddyBoy Blues (315-5115, New Ver.)\0";
BurnDrvTeddybb.szParent="sys1";

BurnDrvPitfall2u.szShortName="pitfal2u";
BurnDrvPitfall2u.szFullNameA="Pitfall II (not encrypted)\0";
BurnDrvPitfall2u.szParent="sys1";

BurnDrvWbdeluxe.szShortName="wbdeluxe";
BurnDrvWbdeluxe.szFullNameA="Wonder Boy Deluxe\0";
BurnDrvWbdeluxe.szParent="sys1";

BurnDrvpengo2u.szShortName="pengo2u";
BurnDrvpengo2u.szFullNameA="Pengo (set 2 not encrypted)\0";
BurnDrvpengo2u.szParent="pacm";

BurnDrvStarjack.szShortName="starjack";
BurnDrvStarjack.szFullNameA="Star Jacker (Sega)\0";
BurnDrvStarjack.szParent="sys1h";

BurnDrvRaflesia.szShortName="raflesia";
BurnDrvRaflesia.szFullNameA="Rafflesia (315-5162)\0";
BurnDrvRaflesia.szParent="sys1h";

BurnDrvAtetris.szShortName="atetris";
BurnDrvAtetris.szFullNameA="Tetris (set 1)\0";
BurnDrvAtetris.szParent="tetris";

int i=0;
pDriver[i++] = &BurnDrvsms_akmw;
pDriver[i++] = &BurnDrvbankp;
pDriver[i++] = &BurnDrvBlockgal;
pDriver[i++] = &BurnDrvChplftb;
pDriver[i++] = &BurnDrvcombh;
pDriver[i++] = &BurnDrvFlickys2;
pDriver[i++] = &BurnDrvGardia;
pDriver[i++] = &BurnDrvGnga;
pDriver[i++] = &BurnDrvGberet;
pDriver[i++] = &BurnDrvMrgoemon;
pDriver[i++] = &BurnDrvMyhero;
pDriver[i++] = &BurnDrvNews;
pDriver[i++] = &BurnDrvNewsa;
pDriver[i++] = &BurnDrvPang;
pDriver[i++] = &BurnDrvSpang;
pDriver[i++] = &BurnDrvpkunwar;
pDriver[i++] = &BurnDrvpengo2u;
pDriver[i++] = &BurnDrvhigemaru;
pDriver[i++] = &BurnDrvPitfall2u;
pDriver[i++] = &BurnDrvRaflesia;
pDriver[i++] = &BurnDrvStarjack;
pDriver[i++] = &BurnDrvTeddybb;
pDriver[i++] = &BurnDrvAtetris;
pDriver[i++] = &BurnDrvWboyu;
pDriver[i++] = &BurnDrvWbdeluxe;
}



















// Application-defined rom loading function:
int (__cdecl *BurnExtLoadRom)(unsigned char *Dest,int *pnWrote,int i, int nGap,int bXor) = NULL;
