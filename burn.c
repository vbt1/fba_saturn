// Burn - Drivers module

#include "burnint.h"
//#include "saturn/ovl.h"

//char toto[0xF00] ={'0','0'};
#define NB_DRV 35
struct BurnDriver* pDriver[NB_DRV] __attribute__((aligned (4)));
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

int BurnDrvGetDIPInfo(struct BurnDIPInfo* pdi, unsigned int i)
{
	if (pDriver[nBurnDrvSelect]->GetDIPInfo) {									// Forward to drivers function
		return pDriver[nBurnDrvSelect]->GetDIPInfo(pdi, i);
	}

	return 1;	// Fail automatically
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

void BurnDrvAssignList()
{
	nBurnDrvCount = NB_DRV;
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
//	static struct BurnDriver BurnDrvGnga = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvBlockgal = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWboyu = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvChplftb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGardia = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvFlickys2 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvMyhero = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvTeddybb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvPitfall2u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWbdeluxe = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvpengo2u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvStarjack = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvRaflesia = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvAtetris = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_akmw = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_cz80 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_gg = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_ggcz = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvCongo = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvZaxxon = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvZaxxonb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvSzaxxon = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvVigilant = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsg1k_wboy = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvBombjack = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNova2001u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNinjakun = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvRaiders5 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWbml = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvWbmlb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvWbmlvc = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvpuckman = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};
	static struct BurnDriver BurnDrvAppoooh = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL, NULL};

BurnDrvsms_akmw.szShortName="sms";
BurnDrvsms_akmw.szFullNameA="Sega Master System (Faze)";
BurnDrvsms_akmw.szParent=NULL;

BurnDrvsms_cz80.szShortName="smscz";
BurnDrvsms_cz80.szFullNameA="Sega Master System (CZ80)";
BurnDrvsms_cz80.szParent=NULL;

BurnDrvsms_gg.szShortName="gg";
BurnDrvsms_gg.szFullNameA="Sega Game Gear (Faze)";
BurnDrvsms_gg.szParent=NULL;

BurnDrvsms_ggcz.szShortName="ggcz";
BurnDrvsms_ggcz.szFullNameA="Sega Game Gear (CZ80)";
BurnDrvsms_ggcz.szParent=NULL;

BurnDrvsg1k_wboy.szShortName="sg1000";
BurnDrvsg1k_wboy.szFullNameA="Sega SG-1000";
BurnDrvsg1k_wboy.szParent=NULL;

BurnDrvbankp.szShortName="bankp";
BurnDrvbankp.szFullNameA="Bank Panic";
BurnDrvbankp.szParent=NULL;

BurnDrvcombh.szShortName="combh";
BurnDrvcombh.szFullNameA="Combat Hawk";
BurnDrvcombh.szParent="bankp";

BurnDrvNewsa.szShortName="newsa";
BurnDrvNewsa.szFullNameA="News (set 2)";
BurnDrvNewsa.szParent="news";

BurnDrvNews.szShortName="news";
BurnDrvNews.szFullNameA="News (set 1)";
BurnDrvNews.szParent=NULL;

BurnDrvGberet.szShortName="gberet";
BurnDrvGberet.szFullNameA="Green Beret";
BurnDrvGberet.szParent=NULL;

BurnDrvMrgoemon.szShortName="mrgoemon";
BurnDrvMrgoemon.szFullNameA="Mr. Goemon (Japan)";
BurnDrvMrgoemon.szParent="gberet";

BurnDrvhigemaru.szShortName="higema";
BurnDrvhigemaru.szFullNameA="Pirate Ship Higemaru";
BurnDrvhigemaru.szParent=NULL;

BurnDrvpkunwar.szShortName="pkunw";
BurnDrvpkunwar.szFullNameA="Penguin-Kun Wars (US)";
BurnDrvpkunwar.szParent=NULL;

BurnDrvPang.szShortName="pang";
BurnDrvPang.szFullNameA="Pang (World)";
BurnDrvPang.szParent="mitch";

BurnDrvSpang.szShortName="spang";
BurnDrvSpang.szFullNameA="Super Pang (World 900914)";
BurnDrvSpang.szParent="mitch";

//BurnDrvGnga.szShortName="gnga";
//BurnDrvGnga.szFullNameA="Ghosts'n Goblins (World? set 2)";
//BurnDrvGnga.szParent="gng";

BurnDrvBlockgal.szShortName="blockgal";
BurnDrvBlockgal.szFullNameA="Block Gal (MC-8123B, 317-0029)";
BurnDrvBlockgal.szParent="sys1h";

BurnDrvWboyu.szShortName="wboyu";
BurnDrvWboyu.szFullNameA="Wonder Boy (not encrypted)";
BurnDrvWboyu.szParent="sys1";

//BurnDrvChplftb.szShortName="chplftb";
//BurnDrvChplftb.szFullNameA="Choplifter (Alternate)";
//BurnDrvChplftb.szParent="sys2";

BurnDrvGardia.szShortName="gardia";
BurnDrvGardia.szFullNameA="Gardia (317-0006)";
BurnDrvGardia.szParent="sys1h";

BurnDrvFlickys2.szShortName="flickys2";
BurnDrvFlickys2.szFullNameA="Flicky (128k, System 2, not encrypted)";
BurnDrvFlickys2.szParent="sys1";

BurnDrvMyhero.szShortName="myhero";
BurnDrvMyhero.szFullNameA="My Hero (US, not encrypted)";
BurnDrvMyhero.szParent="sys1";

BurnDrvTeddybb.szShortName="teddybb";
BurnDrvTeddybb.szFullNameA="TeddyBoy Blues (315-5115, New Ver.)";
BurnDrvTeddybb.szParent="sys1";

BurnDrvPitfall2u.szShortName="pitfal2u";
BurnDrvPitfall2u.szFullNameA="Pitfall II (not encrypted)";
BurnDrvPitfall2u.szParent="sys1";

BurnDrvWbdeluxe.szShortName="wbdeluxe";
BurnDrvWbdeluxe.szFullNameA="Wonder Boy Deluxe";
BurnDrvWbdeluxe.szParent="sys1";

BurnDrvpengo2u.szShortName="pengo2u";
BurnDrvpengo2u.szFullNameA="Pengo (set 2 not encrypted)";
BurnDrvpengo2u.szParent="pacm";

BurnDrvStarjack.szShortName="starjack";
BurnDrvStarjack.szFullNameA="Star Jacker (Sega)";
BurnDrvStarjack.szParent="sys1h";

BurnDrvRaflesia.szShortName="raflesia";
BurnDrvRaflesia.szFullNameA="Rafflesia (315-5162)";
BurnDrvRaflesia.szParent="sys1h";

//BurnDrvAtetris.szShortName="atetris";
//BurnDrvAtetris.szFullNameA="Tetris (set 1)";
//BurnDrvAtetris.szParent="tetris";
			
//BurnDrvCongo.szShortName="congo";
//BurnDrvCongo.szFullNameA="Congo Bongo";
//BurnDrvCongo.szParent="zaxxon";

BurnDrvZaxxon.szShortName="zaxxon";
BurnDrvZaxxon.szFullNameA="Zaxxon (set 1)";
BurnDrvZaxxon.szParent=NULL;

//BurnDrvZaxxonb.szShortName="zaxxonb";
//BurnDrvZaxxonb.szFullNameA="Jackson";
//BurnDrvZaxxonb.szParent="zaxxon";

BurnDrvSzaxxon.szShortName="szaxxon";
BurnDrvSzaxxon.szFullNameA="Super Zaxxon";
BurnDrvSzaxxon.szParent="zaxxon";			

//BurnDrvVigilant.szShortName="vigil";
//BurnDrvVigilant.szFullNameA="Vigilante (World)";
//BurnDrvVigilant.szParent=NULL;

BurnDrvBombjack.szShortName="bombja";
BurnDrvBombjack.szFullNameA="Bomb Jack (set 1)";
BurnDrvBombjack.szParent=NULL;

BurnDrvNova2001u.szShortName="nova2001";
BurnDrvNova2001u.szFullNameA="Nova 2001 (US)";
BurnDrvNova2001u.szParent="pkunw";

BurnDrvNinjakun.szShortName="ninjakun";
BurnDrvNinjakun.szFullNameA="Ninjakun Majou no Bouken";
BurnDrvNinjakun.szParent="pkunw";

BurnDrvRaiders5.szShortName="raiders5";
BurnDrvRaiders5.szFullNameA="Raiders5";
BurnDrvRaiders5.szParent="pkunw";

BurnDrvWbml.szShortName="wbml";
BurnDrvWbml.szFullNameA="Wonder Boy in Monster Land (Jap New)";
BurnDrvWbml.szParent="sys2";

//BurnDrvWbmlvc.szShortName="wbmlvc";
//BurnDrvWbmlvc.szFullNameA="Wonder Boy in Monster Land (EN VC)";
//BurnDrvWbmlvc.szParent="sys2";

BurnDrvpuckman.szShortName="puckman";
BurnDrvpuckman.szFullNameA="Puck Man (Japan set 1)";
BurnDrvpuckman.szParent="pacm";

BurnDrvAppoooh.szShortName="appoooh";
BurnDrvAppoooh.szFullNameA="Appoooh";
BurnDrvAppoooh.szParent="appooo";

unsigned int i=0;
pDriver[i++] = &BurnDrvsms_akmw;
pDriver[i++] = &BurnDrvsms_cz80;
pDriver[i++] = &BurnDrvsms_gg;
pDriver[i++] = &BurnDrvsms_ggcz;
pDriver[i++] = &BurnDrvsg1k_wboy;
pDriver[i++] = &BurnDrvAppoooh;
pDriver[i++] = &BurnDrvbankp;
pDriver[i++] = &BurnDrvBlockgal;
pDriver[i++] = &BurnDrvBombjack;
//pDriver[i++] = &BurnDrvChplftb;
pDriver[i++] = &BurnDrvcombh;
pDriver[i++] = &BurnDrvFlickys2;
pDriver[i++] = &BurnDrvGardia;
pDriver[i++] = &BurnDrvGberet;
pDriver[i++] = &BurnDrvMrgoemon;
pDriver[i++] = &BurnDrvMyhero;
pDriver[i++] = &BurnDrvNews;
pDriver[i++] = &BurnDrvNewsa;
pDriver[i++] = &BurnDrvNinjakun;
pDriver[i++] = &BurnDrvNova2001u;
pDriver[i++] = &BurnDrvPang;
pDriver[i++] = &BurnDrvpkunwar;
pDriver[i++] = &BurnDrvpengo2u;
pDriver[i++] = &BurnDrvhigemaru;
pDriver[i++] = &BurnDrvPitfall2u;
pDriver[i++] = &BurnDrvpuckman;
pDriver[i++] = &BurnDrvRaflesia;
pDriver[i++] = &BurnDrvRaiders5;
pDriver[i++] = &BurnDrvStarjack;
pDriver[i++] = &BurnDrvSpang;
pDriver[i++] = &BurnDrvSzaxxon;	  
pDriver[i++] = &BurnDrvTeddybb;
pDriver[i++] = &BurnDrvWboyu;
pDriver[i++] = &BurnDrvWbdeluxe;
pDriver[i++] = &BurnDrvWbml;
//pDriver[i++] = &BurnDrvWbmlb;
//pDriver[i++] = &BurnDrvWbmlvc;
pDriver[i++] = &BurnDrvZaxxon;
//pDriver[i++] = &BurnDrvVigilant;
//pDriver[i++] = &BurnDrvGnga;
//pDriver[i++] = &BurnDrvAtetris;
//pDriver[i++] = &BurnDrvZaxxonb;
//pDriver[i++] = &BurnDrvCongo;

}

// Application-defined rom loading function:
int (__cdecl *BurnExtLoadRom)(unsigned char *Dest,int *pnWrote,int i, int nGap,int bXor) = NULL;
