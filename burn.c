// Burn - Drivers module // ok

#include "burnint.h"
//#include "saturn/ovl.h"

//char toto[0xF00] ={'0','0'};
#define NB_DRV 54
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
/*	if (nBurnDrvSelect >= nBurnDrvCount) {
		return 1;
	}*/
	return pDriver[nBurnDrvSelect]->Init();	// Forward to drivers function
}

// Exit game emulation
void BurnDrvExit()
{
//	if (pDriver[nBurnDrvSelect]->Exit) {
		pDriver[nBurnDrvSelect]->Exit();			// Forward to drivers function

		extern unsigned int shared, shared_end;
		extern unsigned int _start_ram;		
		ShutdownSlave();
		for( unsigned char *dst = (unsigned char *)&_start_ram; dst < (unsigned char *)0x060FF000; dst++ ) 
		{
			*dst = 0;
		}
		
		for( unsigned char *dst = (unsigned char *)&shared; dst < (unsigned char *)&shared_end; dst++ ) 
		{
			*dst = 0;
		}
		
//		SPR_InitSlaveSH();
//	}
}

void BurnDrvAssignList()
{
	nBurnDrvCount = NB_DRV;
	static struct BurnDriver BurnDrvbankp = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvcombh = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNewsa = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNews = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGberet = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvMrgoemon = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvhigemaru = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvpkunwar = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvPang = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvSpang = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvGnga = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvBlockgal = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWboyu = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvChplftb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGardia = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvFlickys2 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvMyhero = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvTeddybb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvPitfall2u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWbdeluxe = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvpengo2u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvStarjack = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvRaflesia = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvAtetris = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_akmw = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_cz80 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_gg = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsms_ggcz = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvCongo = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvZaxxon = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvZaxxonb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvSzaxxon = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvVigilant = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvsg1k_wboy = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvBombjack = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNova2001u = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNinjakun = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvRaiders5 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWbml = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvWbmlb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWbmlvc = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvpuckman = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvAppoooh = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvRobowres = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvBlktiger = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvWiz	= {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvKungfut = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvStinger = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvScion = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvTigerHB1 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvSlapFigh = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};

	static struct BurnDriver BurnDrvPbillrd = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvFreekickb1 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvCountrunb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGigasb = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvGigasm2 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvOmega = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvFM = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvMSX_1942 = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};

//	static struct BurnDriver BurnDrvHangonjr = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvTetrisse = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvTransfrm = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvFantzn2  = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvAstrofl = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvOpaopa = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
//	static struct BurnDriver BurnDrvSlapshtr = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};

	static struct BurnDriver BurnDrvSolomon = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvSidearms = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvRobokid = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};
	static struct BurnDriver BurnDrvNinjakd2a = {NULL, NULL,NULL,NULL, NULL, NULL, NULL,	NULL, NULL, NULL};	

BurnDrvsms_akmw.szShortName="sms";
BurnDrvsms_akmw.szFullNameA="Sega Master System (Faze)";
//BurnDrvsms_akmw.szParent=NULL;

BurnDrvsms_cz80.szShortName="smscz";
BurnDrvsms_cz80.szFullNameA="Sega Master System (CZ80)";
//BurnDrvsms_cz80.szParent=NULL;

BurnDrvsms_gg.szShortName="gg";
BurnDrvsms_gg.szFullNameA="Sega Game Gear (Faze)";
//BurnDrvsms_gg.szParent=NULL;

BurnDrvsms_ggcz.szShortName="ggcz";
BurnDrvsms_ggcz.szFullNameA="Sega Game Gear (CZ80)";
//BurnDrvsms_ggcz.szParent=NULL;

BurnDrvsg1k_wboy.szShortName="sg1000";
BurnDrvsg1k_wboy.szFullNameA="Sega SG-1000";
//BurnDrvsg1k_wboy.szParent=NULL;

BurnDrvbankp.szShortName="bankp";
BurnDrvbankp.szFullNameA="Bank Panic";
//BurnDrvbankp.szParent=NULL;

BurnDrvcombh.szShortName="combh";
BurnDrvcombh.szFullNameA="Combat Hawk";
BurnDrvcombh.szParent="bankp";

BurnDrvNewsa.szShortName="newsa";
BurnDrvNewsa.szFullNameA="News (set 2)";
BurnDrvNewsa.szParent="news";

BurnDrvNews.szShortName="news";
BurnDrvNews.szFullNameA="News (set 1)";
//BurnDrvNews.szParent=NULL;

BurnDrvGberet.szShortName="gberet";
BurnDrvGberet.szFullNameA="Green Beret";
//BurnDrvGberet.szParent=NULL;

BurnDrvMrgoemon.szShortName="mrgoemon";
BurnDrvMrgoemon.szFullNameA="Mr. Goemon (Japan)";
BurnDrvMrgoemon.szParent="gberet";

BurnDrvhigemaru.szShortName="higema";
BurnDrvhigemaru.szFullNameA="Pirate Ship Higemaru";
//BurnDrvhigemaru.szParent=NULL;

BurnDrvpkunwar.szShortName="pkunw";
BurnDrvpkunwar.szFullNameA="Penguin-Kun Wars (US)";
//BurnDrvpkunwar.szParent=NULL;

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
//BurnDrvZaxxon.szParent=NULL;

//BurnDrvZaxxonb.szShortName="zaxxonb";
//BurnDrvZaxxonb.szFullNameA="Jackson";
//BurnDrvZaxxonb.szParent="zaxxon";

BurnDrvSzaxxon.szShortName="szaxxon";
BurnDrvSzaxxon.szFullNameA="Super Zaxxon";
BurnDrvSzaxxon.szParent="zaxxon";			

BurnDrvVigilant.szShortName="vigil";
BurnDrvVigilant.szFullNameA="Vigilante (World)";
//BurnDrvVigilant.szParent=NULL;

BurnDrvBombjack.szShortName="bombja";
BurnDrvBombjack.szFullNameA="Bomb Jack (set 1)";
//BurnDrvBombjack.szParent=NULL;

BurnDrvNova2001u.szShortName="nova2001";
BurnDrvNova2001u.szFullNameA="Nova 2001 (US)";
BurnDrvNova2001u.szParent="pkunw";

BurnDrvNinjakun.szShortName="ninjakun";
BurnDrvNinjakun.szFullNameA="Ninjakun Majou no Bouken";
BurnDrvNinjakun.szParent="pkunw";
/*
BurnDrvRaiders5.szShortName="raiders5";
BurnDrvRaiders5.szFullNameA="Raiders5";
BurnDrvRaiders5.szParent="pkunw";
*/
BurnDrvWbml.szShortName="wbml";
BurnDrvWbml.szFullNameA="Wonder Boy in Monster Land (Jap New)";
BurnDrvWbml.szParent="sys2";

BurnDrvWbmlvc.szShortName="wbmlvc";
BurnDrvWbmlvc.szFullNameA="Wonder Boy in Monster Land (EN VC)";
BurnDrvWbmlvc.szParent="sys2";

BurnDrvpuckman.szShortName="puckman";
BurnDrvpuckman.szFullNameA="Puck Man (Japan set 1)";
BurnDrvpuckman.szParent="pacm";

BurnDrvAppoooh.szShortName="appoooh";
BurnDrvAppoooh.szFullNameA="Appoooh";
BurnDrvAppoooh.szParent="appooo";

BurnDrvRobowres.szShortName="robowres";
BurnDrvRobowres.szFullNameA="Robo Wres 2001";
BurnDrvRobowres.szParent="appooo";

BurnDrvBlktiger.szShortName="blktiger";
BurnDrvBlktiger.szFullNameA="Black Tiger";
BurnDrvBlktiger.szParent="blktgr";

BurnDrvWiz.szShortName="wiz";
BurnDrvWiz.szFullNameA="Wiz";
//BurnDrvWiz.szParent=NULL;

BurnDrvKungfut.szShortName="kungfut";
BurnDrvKungfut.szFullNameA="Kung-Fu Taikun";
BurnDrvKungfut.szParent="wiz";
/*
BurnDrvStinger.szShortName="stinger";
BurnDrvStinger.szFullNameA="Stinger";
BurnDrvStinger.szParent="wiz";

BurnDrvScion.szShortName="scion";
BurnDrvScion.szFullNameA="Scion";
BurnDrvScion.szParent="wiz";
*/
BurnDrvTigerHB1.szShortName="tigerhb1";
BurnDrvTigerHB1.szFullNameA="Tiger Heli (bootleg, set 1)";
BurnDrvTigerHB1.szParent="slpfgh";

BurnDrvPbillrd.szShortName="pbillrd";
BurnDrvPbillrd.szFullNameA="Perfect Billiard";
BurnDrvPbillrd.szParent="freek";

BurnDrvFreekickb1.szShortName="freekb1";
BurnDrvFreekickb1.szFullNameA="Free Kick (bootleg set 1)";
BurnDrvFreekickb1.szParent="freek";

BurnDrvCountrunb.szShortName="countrnb";
BurnDrvCountrunb.szFullNameA="Counter Run (bootleg set 1)";
BurnDrvCountrunb.szParent="freek";

BurnDrvGigasb.szShortName="gigasb";
BurnDrvGigasb.szFullNameA="Gigas (bootleg)";
BurnDrvGigasb.szParent="freek";

BurnDrvGigasm2.szShortName="gigasm2b";
BurnDrvGigasm2.szFullNameA="Gigas Mark II";
BurnDrvGigasm2.szParent="freek";

BurnDrvOmega.szShortName="omega";
BurnDrvOmega.szFullNameA="Omega";
BurnDrvOmega.szParent="freek";

BurnDrvSlapFigh.szShortName="slapfib1";
BurnDrvSlapFigh.szFullNameA="Slap Fight (bootleg set 1)";
BurnDrvSlapFigh.szParent="slpfgh";

BurnDrvMSX_1942.szShortName="msx";
BurnDrvMSX_1942.szFullNameA="MSX1 System";
//BurnDrvMSX_1942.szParent=NULL;
/*
BurnDrvHangonjr.szShortName="hangonjr";
BurnDrvHangonjr.szFullNameA="Hang-On Jr.";
BurnDrvHangonjr.szParent="segae"	;
*/
BurnDrvTetrisse.szShortName="tetrisse";
BurnDrvTetrisse.szFullNameA="Tetris (Japan, System E)";
BurnDrvTetrisse.szParent="segae";
/*
BurnDrvTransfrm.szShortName="transfrm";
BurnDrvTransfrm.szFullNameA="Transformer";
BurnDrvTransfrm.szParent="segae";
*/
BurnDrvFantzn2.szShortName="fantzn2";
BurnDrvFantzn2.szFullNameA="Fantasy Zone 2 - The Tears of Opa-Opa";
BurnDrvFantzn2.szParent="segae";
/*
BurnDrvAstrofl.szShortName="astrofl";
BurnDrvAstrofl.szFullNameA="Astro Flash (Japan)";
BurnDrvAstrofl.szParent="segae";	
*/
//BurnDrvOpaopa.szShortName="opaopa";
//BurnDrvOpaopa.szFullNameA="Opa Opa (MC-8123, 317-0042)";
//BurnDrvOpaopa.szParent="segae";	
/*
BurnDrvSlapshtr.szShortName="slapshtr";
BurnDrvSlapshtr.szFullNameA="Slap Shooter";
BurnDrvSlapshtr.szParent="segae";	
*/
BurnDrvSolomon.szShortName="solomon";
BurnDrvSolomon.szFullNameA="Solomon's Key (US)";
BurnDrvSolomon.szParent="solomn";	

BurnDrvSidearms.szShortName="sidarm";
BurnDrvSidearms.szFullNameA="Side Arms - Hyper Dyne (World, 861129)";
//BurnDrvSidearms.szParent=NULL;	

BurnDrvRobokid.szShortName="robokid";
BurnDrvRobokid.szFullNameA="Atomic Robo-kid (World, Type-2)";
BurnDrvRobokid.szParent="ninkd2";	

BurnDrvNinjakd2a.szShortName="ninkd2a";
BurnDrvNinjakd2a.szFullNameA="Ninja-Kid II (set 2, bootleg?)";
BurnDrvNinjakd2a.szParent="ninkd2";

//BurnDrvFM.szShortName="fm";
//BurnDrvFM.szFullNameA="FM Test Driver";
//BurnDrvFM.szParent="blktgr";

unsigned int i=0;
//pDriver[i++] = &BurnDrvFM;
pDriver[i++] = &BurnDrvsms_akmw;
pDriver[i++] = &BurnDrvsms_cz80;
pDriver[i++] = &BurnDrvsms_gg;
pDriver[i++] = &BurnDrvsms_ggcz;
pDriver[i++] = &BurnDrvsg1k_wboy;
pDriver[i++] = &BurnDrvMSX_1942;
pDriver[i++] = &BurnDrvAppoooh;
//pDriver[i++] = &BurnDrvAstrofl;
pDriver[i++] = &BurnDrvRobokid;
pDriver[i++] = &BurnDrvNinjakd2a;
//pDriver[i++] = &BurnDrvGnga;
//pDriver[i++] = &BurnDrvOpaopa;
//pDriver[i++] = &BurnDrvKungfut;
//pDriver[i++] = &BurnDrvStinger;
//pDriver[i++] = &BurnDrvScion;
pDriver[i++] = &BurnDrvbankp;
pDriver[i++] = &BurnDrvBlktiger;
pDriver[i++] = &BurnDrvBlockgal;
pDriver[i++] = &BurnDrvBombjack;
//pDriver[i++] = &BurnDrvChplftb;
pDriver[i++] = &BurnDrvcombh;
pDriver[i++] = &BurnDrvCountrunb;
pDriver[i++] = &BurnDrvFantzn2;
pDriver[i++] = &BurnDrvFlickys2;
pDriver[i++] = &BurnDrvFreekickb1;
pDriver[i++] = &BurnDrvGardia;
pDriver[i++] = &BurnDrvGigasb;
pDriver[i++] = &BurnDrvGigasm2;
pDriver[i++] = &BurnDrvGberet;
//pDriver[i++] = &BurnDrvHangonjr;
pDriver[i++] = &BurnDrvMrgoemon;
pDriver[i++] = &BurnDrvMyhero;
pDriver[i++] = &BurnDrvNews;
pDriver[i++] = &BurnDrvNewsa;
pDriver[i++] = &BurnDrvNinjakun;
pDriver[i++] = &BurnDrvNova2001u;
pDriver[i++] = &BurnDrvOmega;
pDriver[i++] = &BurnDrvPang;
pDriver[i++] = &BurnDrvpkunwar;
pDriver[i++] = &BurnDrvpengo2u;
pDriver[i++] = &BurnDrvPbillrd;
pDriver[i++] = &BurnDrvhigemaru;
pDriver[i++] = &BurnDrvPitfall2u;
pDriver[i++] = &BurnDrvpuckman;
pDriver[i++] = &BurnDrvRaflesia;
//pDriver[i++] = &BurnDrvRaiders5;
pDriver[i++] = &BurnDrvRobowres;
pDriver[i++] = &BurnDrvSidearms;
pDriver[i++] = &BurnDrvSlapFigh;
//pDriver[i++] = &BurnDrvSlapshtr;
pDriver[i++] = &BurnDrvSolomon;
pDriver[i++] = &BurnDrvStarjack;
pDriver[i++] = &BurnDrvSpang;
pDriver[i++] = &BurnDrvSzaxxon;	  
pDriver[i++] = &BurnDrvTeddybb;
pDriver[i++] = &BurnDrvTetrisse;
pDriver[i++] = &BurnDrvTigerHB1;
//pDriver[i++] = &BurnDrvTransfrm;
pDriver[i++] = &BurnDrvVigilant;
pDriver[i++] = &BurnDrvWiz;
pDriver[i++] = &BurnDrvWboyu;
pDriver[i++] = &BurnDrvWbdeluxe;
pDriver[i++] = &BurnDrvWbml;
//pDriver[i++] = &BurnDrvWbmlb;
pDriver[i++] = &BurnDrvWbmlvc;
pDriver[i++] = &BurnDrvZaxxon;
//
//pDriver[i++] = &BurnDrvGnga;
//pDriver[i++] = &BurnDrvAtetris;
//pDriver[i++] = &BurnDrvZaxxonb;
//pDriver[i++] = &BurnDrvCongo;

}

// Application-defined rom loading function:
//int (__cdecl *BurnExtLoadRom)(unsigned char *Dest,int *pnWrote,int i, int nGap,int bXor) = NULL;

