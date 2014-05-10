#ifndef _D_SYS1_H_
#define _D_SYS1_H_

#include "d_sys1_common.h"

/*static*/ INT32 WboyuInit();
/*static*/ INT32 TeddybbInit();
/*static*/ INT32 PitfalluInit();
/*static*/ INT32 MyheroInit();
/*static*/ INT32 Wboy2uInit();
///*static*/ int NprincsuInit();
///*static*/ int SeganinuInit();
/*static*/ INT32 Flicks2Init();
///*static*/ INT32 FourdwarrioInit();

/*==============================================================================================
Input Definitions
===============================================================================================*/
/*static*/ struct BurnInputInfo WboyInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System1InputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System1InputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System1InputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System1InputPort2 + 5, "p2 start"  },

	{"P1 Left"           , BIT_DIGITAL  , System1InputPort0 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System1InputPort0 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System1InputPort0 + 1, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System1InputPort0 + 2, "p1 fire 2" },

	{"P2 Left"           , BIT_DIGITAL  , System1InputPort1 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System1InputPort1 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System1InputPort1 + 1, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System1InputPort1 + 2, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &System1Reset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , System1InputPort2 + 3, "service"   },
	{"Test"              , BIT_DIGITAL  , System1InputPort2 + 2, "diag"      },
	{"Dip 1"             , BIT_DIPSWITCH, System1Dip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, System1Dip + 1       , "dip"       },
};

STDINPUTINFO(Wboy)


/*static*/ /*struct BurnInputInfo WbmljbInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System1InputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System1InputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System1InputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System1InputPort2 + 5, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System1InputPort0 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System1InputPort0 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System1InputPort0 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System1InputPort0 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System1InputPort0 + 1, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System1InputPort0 + 2, "p1 fire 2" },

	{"P2 Up"             , BIT_DIGITAL  , System1InputPort1 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System1InputPort1 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System1InputPort1 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System1InputPort1 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System1InputPort1 + 1, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System1InputPort1 + 2, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &System1Reset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , System1InputPort2 + 3, "service"   },
	{"Test"              , BIT_DIGITAL  , System1InputPort2 + 2, "diag"      },
	{"Dip 1"             , BIT_DIPSWITCH, System1Dip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, System1Dip + 1       , "dip"       },
};

STDINPUTINFO(Wbmljb)
*/
/*static*/ /*struct BurnInputInfo SeganinjInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System1InputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System1InputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System1InputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System1InputPort2 + 5, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System1InputPort0 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System1InputPort0 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System1InputPort0 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System1InputPort0 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System1InputPort0 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System1InputPort0 + 1, "p1 fire 2" },
	{"P1 Fire 3"         , BIT_DIGITAL  , System1InputPort0 + 2, "p1 fire 3" },

	{"P2 Up"             , BIT_DIGITAL  , System1InputPort1 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System1InputPort1 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System1InputPort1 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System1InputPort1 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System1InputPort1 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System1InputPort1 + 1, "p2 fire 2" },
	{"P2 Fire 3"         , BIT_DIGITAL  , System1InputPort1 + 2, "p2 fire 3" },

	{"Reset"             , BIT_DIGITAL  , &System1Reset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , System1InputPort2 + 3, "service"   },
	{"Test"              , BIT_DIGITAL  , System1InputPort2 + 2, "diag"      },
	{"Dip 1"             , BIT_DIPSWITCH, System1Dip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, System1Dip + 1       , "dip"       },
};

STDINPUTINFO(Seganinj)
*/
/*static*/ struct BurnInputInfo FlickyInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System1InputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System1InputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System1InputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System1InputPort2 + 5, "p2 start"  },

	{"P1 Left"           , BIT_DIGITAL  , System1InputPort0 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System1InputPort0 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System1InputPort0 + 2, "p1 fire 1" },

	{"P2 Left"           , BIT_DIGITAL  , System1InputPort1 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System1InputPort1 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System1InputPort1 + 2, "p2 fire 1" },

	{"Reset"             , BIT_DIGITAL  , &System1Reset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , System1InputPort2 + 3, "service"   },
	{"Test"              , BIT_DIGITAL  , System1InputPort2 + 2, "diag"      },
	{"Dip 1"             , BIT_DIPSWITCH, System1Dip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, System1Dip + 1       , "dip"       },
};

STDINPUTINFO(Flicky)

/*
/*static*/ /*struct BurnDIPInfo WboyDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xff, NULL                     },
	{0x10, 0xff, 0xff, 0xec, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x0f)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x10, 0x01, 0x01, 0x00, "Upright"                },
	{0x10, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x10, 0x01, 0x02, 0x02, "Off"                    },
	{0x10, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x10, 0x01, 0x0c, 0x0c, "3"                      },
	{0x10, 0x01, 0x0c, 0x08, "4"                      },
	{0x10, 0x01, 0x0c, 0x04, "5"                      },
	{0x10, 0x01, 0x0c, 0x00, "Freeplay"               },
	
	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x10, 0x01, 0x10, 0x10, "30k 100k 170k 240k"     },
	{0x10, 0x01, 0x10, 0x00, "30k 120k 210k 300k"     },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x10, 0x01, 0x20, 0x00, "Off"                    },
	{0x10, 0x01, 0x20, 0x20, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x10, 0x01, 0x40, 0x40, "Easy"                   },
	{0x10, 0x01, 0x40, 0x00, "Hard"                   },
};

STDDIPINFO(Wboy)
 */

/*static*/ struct BurnDIPInfo WboyuDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xbe, NULL                     },
	{0x10, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x0f, 0x01, 0x01, 0x00, "Upright"                },
	{0x0f, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x0f, 0x01, 0x06, 0x00, "2"                      },
	{0x0f, 0x01, 0x06, 0x06, "3"                      },
	{0x0f, 0x01, 0x06, 0x04, "4"                      },
	{0x0f, 0x01, 0x06, 0x02, "5"                      },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0f, 0x01, 0x40, 0x40, "Off"                    },
	{0x0f, 0x01, 0x40, 0x00, "On"                     },
	
	// Dip 2
	{0   , 0xfe, 0   , 8   , "Coinage"                },
	{0x10, 0x01, 0x07, 0x04, "4 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x05, "3 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x00, "4 Coins 2 Credits"      },
	{0x10, 0x01, 0x07, 0x06, "2 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x01, "3 Coins 2 Credits"      },
	{0x10, 0x01, 0x07, 0x02, "2 Coins 1 Credits"      },
	{0x10, 0x01, 0x07, 0x07, "1 Coin  1 Credit"       },
	{0x10, 0x01, 0x07, 0x03, "1 Coin  2 Credits"      },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x10, 0x01, 0x10, 0x00, "Off"                    },
	{0x10, 0x01, 0x10, 0x10, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Mode"                   },
	{0x10, 0x01, 0xc0, 0xc0, "Normal Game"            },
	{0x10, 0x01, 0xc0, 0x80, "Free Play"              },
	{0x10, 0x01, 0xc0, 0x40, "Test Mode"              },
	{0x10, 0x01, 0xc0, 0x00, "Endless Game"           },
};

STDDIPINFO(Wboyu)

/*static*/ struct BurnDIPInfo TeddybbDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0xfe, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x13)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x14, 0x01, 0x01, 0x00, "Upright"                },
	{0x14, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x14, 0x01, 0x02, 0x00, "Off"                    },
	{0x14, 0x01, 0x02, 0x02, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x14, 0x01, 0x0c, 0x08, "2"                      },
	{0x14, 0x01, 0x0c, 0x0c, "3"                      },
	{0x14, 0x01, 0x0c, 0x04, "4"                      },
	{0x14, 0x01, 0x0c, 0x00, "252"                    },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x14, 0x01, 0x30, 0x30, "100k 400k"              },
	{0x14, 0x01, 0x30, 0x20, "200k 600k"              },
	{0x14, 0x01, 0x30, 0x10, "400k 800k"              },
	{0x14, 0x01, 0x30, 0x00, "600k"                   },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x14, 0x01, 0x40, 0x40, "Easy"                   },
	{0x14, 0x01, 0x40, 0x00, "Hard"                   },
};

STDDIPINFO(Teddybb)

/*static*/ struct BurnDIPInfo PitfalluDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0xde, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x13)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x14, 0x01, 0x01, 0x00, "Upright"                },
	{0x14, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x14, 0x01, 0x06, 0x06, "3"                      },
	{0x14, 0x01, 0x06, 0x04, "4"                      },
	{0x14, 0x01, 0x06, 0x02, "5"                      },
	{0x14, 0x01, 0x06, 0x00, "Infinite"               },
	
	{0   , 0xfe, 0   , 4   , "Starting Stage"         },
	{0x14, 0x01, 0x18, 0x18, "1"                      },
	{0x14, 0x01, 0x18, 0x10, "2"                      },
	{0x14, 0x01, 0x18, 0x08, "3"                      },
	{0x14, 0x01, 0x18, 0x00, "4"                      },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x14, 0x01, 0x20, 0x20, "Off"                    },
	{0x14, 0x01, 0x20, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Time"                   },
	{0x14, 0x01, 0x40, 0x00, "2 minutes"              },
	{0x14, 0x01, 0x40, 0x40, "3 minutes"              },
};

STDDIPINFO(Pitfallu)

/*static*/ struct BurnDIPInfo MyheroDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0xfc, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x13)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x14, 0x01, 0x01, 0x00, "Upright"                },
	{0x14, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x14, 0x01, 0x02, 0x02, "Off"                    },
	{0x14, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x14, 0x01, 0x0c, 0x0c, "3"                      },
	{0x14, 0x01, 0x0c, 0x08, "4"                      },
	{0x14, 0x01, 0x0c, 0x04, "5"                      },
	{0x14, 0x01, 0x0c, 0x00, "Infinite"               },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x14, 0x01, 0x10, 0x10, "30000 100000 200000"                    },
	{0x14, 0x01, 0x10, 0x00, "50000 150000 250000"                    },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x14, 0x01, 0x20, 0x20, "Easy"                   },
	{0x14, 0x01, 0x20, 0x00, "Hard"                   },
};

STDDIPINFO(Myhero)

/*
/*static*/ /*struct BurnDIPInfo WbmljbDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xbe, NULL                     },
	{0xf0, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x0f, 0x01, 0x01, 0x00, "Upright"                },
	{0x0f, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x0f, 0x01, 0x0c, 0x04, "3"                      },
	{0x0f, 0x01, 0x0c, 0x0c, "4"                      },
	{0x0f, 0x01, 0x06, 0x08, "5"                      },

	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0f, 0x01, 0x40, 0x40, "Off"                    },
	{0x0f, 0x01, 0x40, 0x00, "On"                     },
	
	// Dip 2
	{0   , 0xfe, 0   , 8   , "Coinage"                },
	{0x10, 0x01, 0x07, 0x04, "4 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x05, "3 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x00, "4 Coins 2 Credits"      },
	{0x10, 0x01, 0x07, 0x06, "2 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x01, "3 Coins 2 Credits"      },
	{0x10, 0x01, 0x07, 0x02, "2 Coins 1 Credits"      },
	{0x10, 0x01, 0x07, 0x07, "1 Coin  1 Credit"       },
	{0x10, 0x01, 0x07, 0x03, "1 Coin  2 Credits"      },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x10, 0x01, 0x10, 0x00, "Off"                    },
	{0x10, 0x01, 0x10, 0x10, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Mode"                   },
	{0x10, 0x01, 0xc0, 0xc0, "Normal Game"            },
	{0x10, 0x01, 0xc0, 0x80, "Free Play"              },
	{0x10, 0x01, 0xc0, 0x40, "Test Mode"              },
	{0x10, 0x01, 0xc0, 0x00, "Endless Game"           },
};

STDDIPINFO(Wbmljb)
*/

/*static*/ /*struct BurnDIPInfo SeganinjDIPList[]=
{
	// Default Values
	{0x15, 0xff, 0xff, 0xff, NULL                     },
	{0x16, 0xff, 0xff, 0xdc, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x15)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x16, 0x01, 0x01, 0x00, "Upright"                },
	{0x16, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x16, 0x01, 0x02, 0x02, "Off"                    },
	{0x16, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x16, 0x01, 0x0c, 0x08, "2"                      },
	{0x16, 0x01, 0x0c, 0x0c, "3"                      },
	{0x16, 0x01, 0x0c, 0x04, "4"                      },
	{0x16, 0x01, 0x0c, 0x00, "240"                    },
	
	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x16, 0x01, 0x10, 0x10, "20k  70k 120k 170k"     },
	{0x16, 0x01, 0x10, 0x00, "50k 100k 150k 200k"     },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x16, 0x01, 0x20, 0x20, "Off"                    },
	{0x16, 0x01, 0x20, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x16, 0x01, 0x40, 0x40, "Easy"                   },
	{0x16, 0x01, 0x40, 0x00, "Hard"                   },
};

STDDIPINFO(Seganinj)
*/

/*static*/ struct BurnDIPInfo FlickyDIPList[]=
{
	// Default Values
	{0x0d, 0xff, 0xff, 0xff, NULL                     },
	{0x0e, 0xff, 0xff, 0xfe, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x0d)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x0e, 0x01, 0x01, 0x00, "Upright"                },
	{0x0e, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x0e, 0x01, 0x0c, 0x0c, "2"                      },
	{0x0e, 0x01, 0x0c, 0x08, "3"                      },
	{0x0e, 0x01, 0x0c, 0x04, "4"                      },
	{0x0e, 0x01, 0x0c, 0x00, "Infinite"               },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x0e, 0x01, 0x30, 0x30, "30k  80k 160k"          },
	{0x0e, 0x01, 0x30, 0x20, "30k 100k 200k"          },
	{0x0e, 0x01, 0x30, 0x10, "40k 120k 240k"          },
	{0x0e, 0x01, 0x30, 0x00, "40k 140k 280k"          },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0e, 0x01, 0x40, 0x40, "Easy"                   },
	{0x0e, 0x01, 0x40, 0x00, "Hard"                   },
};

STDDIPINFO(Flicky)

/*static*/ struct BurnDIPInfo WbdeluxeDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xff, NULL                     },
	{0x10, 0xff, 0xff, 0x7c, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x0f)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x10, 0x01, 0x01, 0x00, "Upright"                },
	{0x10, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x10, 0x01, 0x02, 0x02, "Off"                    },
	{0x10, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x10, 0x01, 0x0c, 0x0c, "3"                      },
	{0x10, 0x01, 0x0c, 0x08, "4"                      },
	{0x10, 0x01, 0x0c, 0x04, "5"                      },
	{0x10, 0x01, 0x0c, 0x00, "Freeplay"               },
	
	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x10, 0x01, 0x10, 0x10, "30k 100k 170k 240k"     },
	{0x10, 0x01, 0x10, 0x00, "30k 120k 210k 300k"     },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x10, 0x01, 0x20, 0x00, "Off"                    },
	{0x10, 0x01, 0x20, 0x20, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x10, 0x01, 0x40, 0x40, "Easy"                   },
	{0x10, 0x01, 0x40, 0x00, "Hard"                   },
	
	{0   , 0xfe, 0   , 2   , "Energy Consumption"     },
	{0x10, 0x01, 0x80, 0x00, "Slow"                   },
	{0x10, 0x01, 0x80, 0x80, "Fast"                   },
};

STDDIPINFO(Wbdeluxe)

/*static*/ /*struct BurnDIPInfo FourdwarrioDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0xfe, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x13)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x14, 0x01, 0x01, 0x00, "Upright"                },
	{0x14, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x14, 0x01, 0x06, 0x06, "3"                      },
	{0x14, 0x01, 0x06, 0x04, "4"                      },
	{0x14, 0x01, 0x06, 0x02, "5"                      },
	{0x14, 0x01, 0x06, 0x00, "Infinite"               },
	
	{0   , 0xfe, 0   , 8   , "Bonus Life"             },
	{0x14, 0x01, 0x38, 0x38, "30k"                    },
	{0x14, 0x01, 0x38, 0x30, "40k"                    },
	{0x14, 0x01, 0x38, 0x28, "50k"                    },
	{0x14, 0x01, 0x38, 0x20, "60k"                    },
	{0x14, 0x01, 0x38, 0x18, "70k"                    },
	{0x14, 0x01, 0x38, 0x10, "80k"                    },
	{0x14, 0x01, 0x38, 0x08, "90k"                    },
	{0x14, 0x01, 0x38, 0x00, "None"                   },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x14, 0x01, 0x40, 0x40, "Easy"                   },
	{0x14, 0x01, 0x40, 0x00, "Hard"                   },
};

STDDIPINFO(Fourdwarrio)
*/
#undef SYSTEM1_COINAGE

/*==============================================================================================
ROM Descriptions
===============================================================================================*/
/*
/*static*/ /*struct BurnRomInfo WboyRomDesc[] = {
	{ "epr7489.116",      0x004000, 0x130f4b70, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr7490.109",      0x004000, 0x9e656733, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr7491.96",       0x004000, 0x1f7d0efe, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr7498.120",      0x002000, 0x78ae1e7b, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr7497.62",       0x002000, 0x08d609ca, BRF_GRA },		  //  4 Tiles
	{ "epr7496.61",       0x002000, 0x6f61fdf1, BRF_GRA },		  //  5 Tiles
	{ "epr7495.64",       0x002000, 0x6a0d2c2d, BRF_GRA },		  //  6 Tiles
	{ "epr7494.63",       0x002000, 0xa8e281c7, BRF_GRA },		  //  7 Tiles
	{ "epr7493.66",       0x002000, 0x89305df4, BRF_GRA },		  //  8 Tiles
	{ "epr7492.65",       0x002000, 0x60f806b1, BRF_GRA },		  //  9 Tiles
	
	{ "epr7485.117",      0x004000, 0xc2891722, BRF_GRA },		  //  10 Sprites
	{ "epr7487.04",       0x004000, 0x2d3a421b, BRF_GRA },		  //  11 Sprites
	{ "epr7486.110",      0x004000, 0x8d622c50, BRF_GRA },		  //  12 Sprites
	{ "epr7488.05",       0x004000, 0x007c2f1b, BRF_GRA },		  //  13 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Wboy)
STD_ROM_FN(Wboy)
*/

/*static*/ struct BurnRomInfo WboyuRomDesc[] = {
	{ "ic11689.bin",      0x004000, 0x73d8cef0, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "ic10990.bin",      0x004000, 0x29546828, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "ic09691.bin",      0x004000, 0xc7145c2a, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr7498.120",      0x002000, 0x78ae1e7b, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr7497.62",       0x002000, 0x08d609ca, BRF_GRA },		  //  4 Tiles
	{ "epr7496.61",       0x002000, 0x6f61fdf1, BRF_GRA },		  //  5 Tiles
	{ "epr7495.64",       0x002000, 0x6a0d2c2d, BRF_GRA },		  //  6 Tiles
	{ "epr7494.63",       0x002000, 0xa8e281c7, BRF_GRA },		  //  7 Tiles
	{ "epr7493.66",       0x002000, 0x89305df4, BRF_GRA },		  //  8 Tiles
	{ "epr7492.65",       0x002000, 0x60f806b1, BRF_GRA },		  //  9 Tiles
	
	{ "ic11785.bin",      0x004000, 0x1ee96ae8, BRF_GRA },		  //  10 Sprites
	{ "ic00487.bin",      0x004000, 0x119735bb, BRF_GRA },		  //  11 Sprites
	{ "ic11086.bin",      0x004000, 0x26d0fac4, BRF_GRA },		  //  12 Sprites
	{ "ic00588.bin",      0x004000, 0x2602e519, BRF_GRA },		  //  13 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Wboyu)
STD_ROM_FN(Wboyu)

/*static*/ struct BurnRomInfo TeddybbRomDesc[] = {
	{ "epr6768.116",      0x004000, 0x5939817e, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr6769.109",      0x004000, 0x14a98ddd, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr6770.96",       0x004000, 0x67b0c7c2, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr6748x.120",      0x002000, 0xc2a1b89d, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr6747.62",       0x002000, 0xa0e5aca7, BRF_GRA },		  //  4 Tiles
	{ "epr6746.61",       0x002000, 0xcdb77e51, BRF_GRA },		  //  5 Tiles
	{ "epr6745.64",       0x002000, 0x0cab75c3, BRF_GRA },		  //  6 Tiles
	{ "epr6744.63",       0x002000, 0x0ef8d2cd, BRF_GRA },		  //  7 Tiles
	{ "epr6743.66",       0x002000, 0xc33062b5, BRF_GRA },		  //  8 Tiles
	{ "epr6742.65",       0x002000, 0xc457e8c5, BRF_GRA },		  //  9 Tiles
	
	{ "epr6735.117",      0x004000, 0x1be35a97, BRF_GRA },		  //  10 Sprites
	{ "epr6737.04",       0x004000, 0x6b53aa7a, BRF_GRA },		  //  11 Sprites
	{ "epr6736.110",      0x004000, 0x565c25d0, BRF_GRA },		  //  12 Sprites
	{ "epr6738.05",       0x004000, 0xe116285f, BRF_GRA },		  //  13 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Teddybb)
STD_ROM_FN(Teddybb)

/*static*/ struct BurnRomInfo Pitfall2uRomDesc[] = {
	{ "epr6623.116",      0x004000, 0xbcb47ed6, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr6624a.109",      0x004000, 0x6e8b09c1, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr6625.96",       0x004000, 0xdc5484ba, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr6462.120",      0x002000, 0x86bb9185, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr6474a.62",       0x002000, 0x9f1711b9, BRF_GRA },		  //  4 Tiles
	{ "epr6473a.61",       0x002000, 0x8e53b8dd, BRF_GRA },		  //  5 Tiles
	{ "epr6472a.64",       0x002000, 0xe0f34a11, BRF_GRA },		  //  6 Tiles
	{ "epr6471a.63",       0x002000, 0xd5bc805c, BRF_GRA },		  //  7 Tiles
	{ "epr6470a.66",       0x002000, 0x1439729f, BRF_GRA },		  //  8 Tiles
	{ "epr6469a.65",       0x002000, 0xe4ac6921, BRF_GRA },		  //  9 Tiles
	
	{ "epr6454a.117",      0x004000, 0xa5d96780, BRF_GRA },		  //  10 Sprites
	{ "epr6455.05",       0x004000, 0x32ee64a1, BRF_GRA },		  //  11 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  12 Timing PROM
};

STD_ROM_PICK(Pitfall2u)
STD_ROM_FN(Pitfall2u)

/*static*/ struct BurnRomInfo MyheroRomDesc[] = {
	{ "epr6963b.116",      0x004000, 0x4daf89d4, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr6964a.109",      0x004000, 0xc26188e5, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr6927.96",       0x004000, 0x3cbbaf64, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr69xx.120",      0x002000, 0x0039e1e9, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr6966.62",       0x002000, 0x157f0401, BRF_GRA },		  //  4 Tiles
	{ "epr6961.61",       0x002000, 0xbe53ce47, BRF_GRA },		  //  5 Tiles
	{ "epr6960.64",       0x002000, 0xbd381baa, BRF_GRA },		  //  6 Tiles
	{ "epr6959.63",       0x002000, 0xbc04e79a, BRF_GRA },		  //  7 Tiles
	{ "epr6958.66",       0x002000, 0x714f2c26, BRF_GRA },		  //  8 Tiles
	{ "epr6957.65",       0x002000, 0x80920112, BRF_GRA },		  //  9 Tiles
	
	{ "epr6921.117",      0x004000, 0xf19e05a1, BRF_GRA },		  //  10 Sprites
	{ "epr6923.04",       0x004000, 0x7988adc3, BRF_GRA },		  //  11 Sprites
	{ "epr6922.110",      0x004000, 0x37f77a78, BRF_GRA },		  //  12 Sprites
	{ "epr6924.05",       0x004000, 0x42bdc8f6, BRF_GRA },		  //  13 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Myhero)
STD_ROM_FN(Myhero)


/*static*/ /*struct BurnRomInfo WbmljbRomDesc[] = {
	{ "wbml.bin",      0x008000, 0x66482638, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "m6.bin",      0x008000, 0x8c08cd11, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "m7.bin",       0x008000, 0x11881703, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr11037.bin",      0x008000, 0x7a4ee585, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr11034.bin",       0x008000, 0x37a2077d, BRF_GRA },		  //  4 Tiles
	{ "epr11035.bin",       0x008000, 0xcdf2a21b, BRF_GRA },		  //  5 Tiles
	{ "epr11036.bin",       0x008000, 0x644687fa, BRF_GRA },		  //  6 Tiles

	{ "epr11028.bin",       0x008000, 0xaf0b3972, BRF_GRA },		  //  7 Sprites
	{ "epr11027.bin",       0x008000, 0x277d8f1d, BRF_GRA },		  //  8 Sprites
	{ "epr11030.bin",       0x008000, 0xf05ffc76, BRF_GRA },		  //  9 Sprites
	{ "epr11029.bin",      0x008000, 0xcedc9c61, BRF_GRA },		  //  10 Sprites

	{ "pr11026.bin",       0x0100, 0x27057298, BRF_OPT },		  //  11 Palette
	{ "pr11025.bin",      0x0100, 0x41e4d86b, BRF_OPT },		  //  12 Palette
	{ "pr11024.bin",       0x0100, 0x08d71954, BRF_OPT },		  //  13 Palette

	{ "pr5317.bin",        0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Wbmljb)
STD_ROM_FN(Wbmljb)
*/

/*static*/ /*struct BurnRomInfo NprincesuRomDesc[] = {
	{ "epr6573.129",      0x002000, 0xd2919c7d, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr6574.130",      0x002000, 0x5a132833, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr6575.131",      0x002000, 0xa94b0bd4, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	{ "epr6576.132",      0x002000, 0x27d3bbdb, BRF_ESS | BRF_PRG }, //  3	Z80 #1 Program Code
	{ "epr6577.133",      0x002000, 0x73616e03, BRF_ESS | BRF_PRG }, //  4	Z80 #1 Program Code
	{ "epr6578.134",      0x002000, 0xab68499f, BRF_ESS | BRF_PRG }, //  5	Z80 #1 Program Code
	
	{ "epr6559.120",      0x002000, 0x5a1570ee, BRF_ESS | BRF_PRG }, //  6	Z80 #2 Program Code
	
	{ "epr6558.62",       0x002000, 0x2af9eaeb, BRF_GRA },		  //  7 Tiles
	{ "epr6557.61",       0x002000, 0x6eb131d0, BRF_GRA },		  //  8 Tiles
	{ "epr6556.64",       0x002000, 0x79fd26f7, BRF_GRA },		  //  9 Tiles
	{ "epr6555.63",       0x002000, 0x7f669aac, BRF_GRA },		  //  10 Tiles
	{ "epr6554.66",       0x002000, 0x5ac9d205, BRF_GRA },		  //  11 Tiles
	{ "epr6553.65",       0x002000, 0xeb82a8fe, BRF_GRA },		  //  12 Tiles
	
	{ "epr6546.117",      0x004000, 0xa4785692, BRF_GRA },		  //  13 Sprites
	{ "epr6548.04",       0x004000, 0xbdf278c1, BRF_GRA },		  //  14 Sprites
	{ "epr6547.110",      0x004000, 0x34451b08, BRF_GRA },		  //  15 Sprites
	{ "epr6549.05",       0x004000, 0xd2057668, BRF_GRA },		  //  16 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  17 Timing PROM
};

STD_ROM_PICK(Nprincesu)
STD_ROM_FN(Nprincesu)
*/

/*static*/ /*struct BurnRomInfo SeganinjuRomDesc[] = {
	{ "epr7149.116",      0x004000, 0xcd9fade7, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr7150.109",      0x004000, 0xc36351e2, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr6552.96",       0x004000, 0xf2eeb0d8, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr6559.120",      0x002000, 0x5a1570ee, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr6558.62",       0x002000, 0x2af9eaeb, BRF_GRA },		  //  4 Tiles
	{ "epr6592.61",       0x002000, 0x7804db86, BRF_GRA },		  //  5 Tiles
	{ "epr6556.64",       0x002000, 0x79fd26f7, BRF_GRA },		  //  6 Tiles
	{ "epr6590.63",       0x002000, 0xbf858cad, BRF_GRA },		  //  7 Tiles
	{ "epr6554.66",       0x002000, 0x5ac9d205, BRF_GRA },		  //  8 Tiles
	{ "epr6588.65",       0x002000, 0xdc931dbb, BRF_GRA },		  //  9 Tiles
	
	{ "epr6546.117",      0x004000, 0xa4785692, BRF_GRA },		  //  10 Sprites
	{ "epr6548.04",       0x004000, 0xbdf278c1, BRF_GRA },		  //  11 Sprites
	{ "epr6547.110",      0x004000, 0x34451b08, BRF_GRA },		  //  12 Sprites
	{ "epr6549.05",       0x004000, 0xd2057668, BRF_GRA },		  //  13 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Seganinju)
STD_ROM_FN(Seganinju)
*/

/*static*/ struct BurnRomInfo Flickys2RomDesc[] = {
	{ "epr6621.bin",      0x004000, 0xb21ff546, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr6622.bin",      0x004000, 0x133a8bf1, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	
	{ "epr5869.120",      0x002000, 0x6d220d4e, BRF_ESS | BRF_PRG }, //  2	Z80 #2 Program Code
	
	{ "epr5868.62",       0x002000, 0x7402256b, BRF_GRA },		  //  3 Tiles
	{ "epr5867.61",       0x002000, 0x2f5ce930, BRF_GRA },		  //  4 Tiles
	{ "epr5866.64",       0x002000, 0x967f1d9a, BRF_GRA },		  //  5 Tiles
	{ "epr5865.63",       0x002000, 0x03d9a34c, BRF_GRA },		  //  6 Tiles
	{ "epr5864.66",       0x002000, 0xe659f358, BRF_GRA },		  //  7 Tiles
	{ "epr5863.65",       0x002000, 0xa496ca15, BRF_GRA },		  //  8 Tiles
	
	{ "epr5855.117",      0x004000, 0xb5f894a1, BRF_GRA },		  //  9 Sprites
	{ "epr5856.110",      0x004000, 0x266af78f, BRF_GRA },		  //  10 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  11 Timing PROM
};

STD_ROM_PICK(Flickys2)
STD_ROM_FN(Flickys2)

/*static*/ struct BurnRomInfo WbdeluxeRomDesc[] = {
	{ "wbd1.bin",          0x002000, 0xa1bedbd7, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "ic13003.bin",      0x002000, 0x56463ede, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "wbd3.bin",          0x002000, 0x6fcdbd4c, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	{ "ic13205.bin",      0x002000, 0x7b922708, BRF_ESS | BRF_PRG }, //  3	Z80 #1 Program Code
	{ "wbd5.bin",          0x002000, 0xf6b02902, BRF_ESS | BRF_PRG }, //  4	Z80 #1 Program Code
	{ "wbd6.bin",          0x002000, 0x43df21fe, BRF_ESS | BRF_PRG }, //  5	Z80 #1 Program Code
	
	{ "epr7498a.3",        0x002000, 0xc198205c, BRF_ESS | BRF_PRG }, //  6	Z80 #2 Program Code
	
	{ "epr7497.62",       0x002000, 0x08d609ca, BRF_GRA },		  //  7 Tiles
	{ "epr7496.61",       0x002000, 0x6f61fdf1, BRF_GRA },		  //  8 Tiles
	{ "epr7495.64",       0x002000, 0x6a0d2c2d, BRF_GRA },		  //  9 Tiles
	{ "epr7494.63",       0x002000, 0xa8e281c7, BRF_GRA },		  //  10 Tiles
	{ "epr7493.66",       0x002000, 0x89305df4, BRF_GRA },		  //  11 Tiles
	{ "epr7492.65",       0x002000, 0x60f806b1, BRF_GRA },		  //  12 Tiles
	
	{ "epr7485.117",      0x004000, 0xc2891722, BRF_GRA },		  //  13 Sprites
	{ "epr7487.04",       0x004000, 0x2d3a421b, BRF_GRA },		  //  14 Sprites
	{ "epr7486.110",      0x004000, 0x8d622c50, BRF_GRA },		  //  15 Sprites
	{ "epr7488.05",       0x004000, 0x007c2f1b, BRF_GRA },		  //  16 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  17 Timing PROM
};

STD_ROM_PICK(Wbdeluxe)
STD_ROM_FN(Wbdeluxe)

/*static*/ /*struct BurnRomInfo FourdwarrioRomDesc[] = {
	{ "4d.116",            0x004000, 0x546d1bc7, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "4d.109",            0x004000, 0xf1074ec3, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "4d.96",             0x004000, 0x387c1e8f, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "4d.120",            0x002000, 0x5241c009, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "4d.62",             0x002000, 0xf31b2e09, BRF_GRA },		  //  4 Tiles
	{ "4d.61",             0x002000, 0x5430e925, BRF_GRA },		  //  5 Tiles
	{ "4d.64",             0x002000, 0x9f442351, BRF_GRA },		  //  6 Tiles
	{ "4d.63",             0x002000, 0x633232bd, BRF_GRA },		  //  7 Tiles
	{ "4d.66",             0x002000, 0x52bfa2ed, BRF_GRA },		  //  8 Tiles
	{ "4d.65",             0x002000, 0xe9ba4658, BRF_GRA },		  //  9 Tiles
	
	{ "4d.117",            0x004000, 0x436e4141, BRF_GRA },		  //  10 Sprites
	{ "4d.04",             0x004000, 0x8b7cecef, BRF_GRA },		  //  11 Sprites
	{ "4d.110",            0x004000, 0x6ec5990a, BRF_GRA },		  //  12 Sprites
	{ "4d.05",             0x004000, 0xf31a1e6a, BRF_GRA },		  //  13 Sprites

	{ "pr5317.76",         0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Fourdwarrio)
STD_ROM_FN(Fourdwarrio)
*/
/*static*/ /*struct BurnRomInfo WbmljbRomDesc[] = {
	{ "wbml.01",          0x010000, 0x66482638, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "m6.bin",             0x010000, 0x8c08cd11, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "m7.bin",             0x010000, 0x11881703, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr11037.126",  0x008000, 0x7a4ee585, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr11034.4",      0x008000, 0x37a2077d, BRF_GRA },		  //  4 Tiles
	{ "epr11035.5",      0x008000, 0xcdf2a21b, BRF_GRA },		  //  5 Tiles
	{ "epr11036.6",      0x008000, 0x644687fa, BRF_GRA },		  //  6 Tiles

	{ "epr11028.87",    0x008000, 0xaf0b3972, BRF_GRA },		  //  4 Sprites
	{ "epr11027.86",    0x008000, 0x277d8f1d, BRF_GRA },		  //  5 Sprites
	{ "epr11030.89",    0x008000, 0xf05ffc76, BRF_GRA },		  //  6 Sprites
	{ "epr11029.88",    0x008000, 0xcedc9c61, BRF_GRA },		  //  7 Sprites

	{ "pr11026.20",     0x000100, 0x27057298, BRF_OPT },		  //  8 Red PROM
	{ "pr11025.14",     0x000100, 0x41e4d86b, BRF_OPT },		  //  9 Green PROM
	{ "pr11024.8",       0x000100, 0x08d71954, BRF_OPT },		  //  10 Blue PROM


	{ "pr5317.37",    0x000100, 0x648350b8, BRF_OPT },		  //  11 Timing PROM
};

STD_ROM_PICK(Wbmljb)
STD_ROM_FN(Wbmljb)
*/


/*==============================================================================================
Driver defs
===============================================================================================*/
/*
struct BurnDriver BurnDrvWboy = {
	"wboy", NULL, NULL, "1986",
	"Wonder Boy (set 1, 315-5177)\0", NULL, "Sega (Escape License)", "System 1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEM1, GBF_PLATFORM, 0,
	NULL, WboyRomInfo, WboyRomName, WboyInputInfo, WboyDIPInfo,
	WboyInit, System1Exit, System1Frame, NULL, NULL,
	0, NULL, NULL, NULL, NULL, 0x600, 256, 224, 4, 3
};
*/

/*
struct BurnDriver BurnDrvWbmljb = {
	"wbmljb", NULL, NULL, "1987",
	"Wonder Boy in Monster Land (Japan not encrypted)\0", NULL, "Sega", "System 2",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEM1, GBF_PLATFORM, 0,
	NULL, WbmljbRomInfo, WbmljbRomName, WbmljbInputInfo, WbmljbDIPInfo,
	WbmljbInit, System1Exit, System1Frame, NULL//, NULL//,
//	0, NULL, NULL, NULL, NULL, 256, 224, 4, 3
};
*/
/*
struct BurnDriver BurnDrvNprincesu = {
	"nprincsu", "seganinj", NULL, "1985",
	"Ninja Princess (64k Ver.not encrypted)\0", NULL, "Sega", "System 1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM1, GBF_VERSHOOT, 0,
	NULL, NprincesuRomInfo, NprincesuRomName, SeganinjInputInfo, SeganinjDIPInfo,
	NprincsuInit, System1Exit, System1Frame, NULL//, NULL
//		,0, NULL, NULL, NULL, NULL, 256, 224, 4, 3
};
*/
/*
struct BurnDriver BurnDrvSeganinju = {
	"seganinu", "seganinj", NULL, "1985",
	"Sega Ninja (not encrypted)\0", NULL, "Sega", "System 1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM1, GBF_VERSHOOT, 0,
	NULL, SeganinjuRomInfo, SeganinjuRomName, SeganinjInputInfo, SeganinjDIPInfo,
	SeganinuInit, System1Exit, System1Frame, NULL//, NULL//,
//	0, NULL, NULL, NULL, NULL, 256, 224, 4, 3
};
*/


#endif