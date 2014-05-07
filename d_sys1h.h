#ifndef _D_SYS1_H_
#define _D_SYS1_H_

#include "d_sys1_common.h"

static INT32 GardiaInit();
static INT32 StarjackInit();
static INT32 RaflesiaInit();
static INT32 BlockgalInit();

/*==============================================================================================
Input Definitions
===============================================================================================*/

static struct BurnInputInfo BlockgalInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System1InputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System1InputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System1InputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System1InputPort2 + 5, "p2 start"  },

	{"P1 Left"           , BIT_DIGITAL  , System1InputPort0 + 0, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System1InputPort0 + 1, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System1InputPort2 + 6, "p1 fire 1" },

	{"P2 Left"           , BIT_DIGITAL  , System1InputPort0 + 2, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System1InputPort0 + 3, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System1InputPort2 + 7, "p2 fire 1" },

	{"Reset"             , BIT_DIGITAL  , &System1Reset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , System1InputPort2 + 3, "service"   },
	{"Dip 1"             , BIT_DIPSWITCH, System1Dip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, System1Dip + 1       , "dip"       },
};

STDINPUTINFO(Blockgal)

static struct BurnInputInfo MyheroInputList[] = {
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

STDINPUTINFO(Myhero)

#define SYSTEM1_COINAGE(dipval)								\
	{0   , 0xfe, 0   , 16   , "Coin A"                },				\
	{dipval, 0x01, 0x0f, 0x07, "4 Coins 1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x08, "3 Coins 1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x09, "2 Coins 1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x05, "2 Coins 1 Credit 4/2 5/3 6/4"},			\
	{dipval, 0x01, 0x0f, 0x04, "2 Coins 1 Credit 4/3"   },				\
	{dipval, 0x01, 0x0f, 0x0f, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x00, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x03, "1 Coin  1 Credit 5/6"   },				\
	{dipval, 0x01, 0x0f, 0x02, "1 Coin  1 Credit 4/5"   },				\
	{dipval, 0x01, 0x0f, 0x01, "1 Coin  1 Credit 2/3"   },				\
	{dipval, 0x01, 0x0f, 0x06, "2 Coins 3 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0e, "1 Coin  2 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0d, "1 Coin  3 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0c, "1 Coin  4 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0b, "1 Coin  5 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0a, "1 Coin  6 Credits"      },				\
											\
	{0   , 0xfe, 0   , 16   , "Coin B"                },				\
	{dipval, 0x01, 0xf0, 0x70, "4 Coins 1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x80, "3 Coins 1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x90, "2 Coins 1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x50, "2 Coins 1 Credit 4/2 5/3 6/4"},			\
	{dipval, 0x01, 0xf0, 0x40, "2 Coins 1 Credit 4/3"   },				\
	{dipval, 0x01, 0xf0, 0xf0, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x00, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x30, "1 Coin  1 Credit 5/6"   },				\
	{dipval, 0x01, 0xf0, 0x20, "1 Coin  1 Credit 4/5"   },				\
	{dipval, 0x01, 0xf0, 0x10, "1 Coin  1 Credit 2/3"   },				\
	{dipval, 0x01, 0xf0, 0x60, "2 Coins 3 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xe0, "1 Coin  2 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xd0, "1 Coin  3 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xc0, "1 Coin  4 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xb0, "1 Coin  5 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xa0, "1 Coin  6 Credits"      },

static struct BurnDIPInfo BlockgalDIPList[]=
{
	// Default Values
	{0x0c, 0xff, 0xff, 0xd7, NULL                     },
	{0x0d, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x0c, 0x01, 0x01, 0x00, "Upright"                },
	{0x0c, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0c, 0x01, 0x02, 0x00, "Off"                    },
	{0x0c, 0x01, 0x02, 0x02, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Lives"                  },
	{0x0c, 0x01, 0x08, 0x08, "2"                      },
	{0x0c, 0x01, 0x08, 0x00, "3"                      },
	
	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0c, 0x01, 0x10, 0x10, "10k 30k 60k 100k 150k"  },
	{0x0c, 0x01, 0x10, 0x00, "30k 50k 100k 200k 300k" },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x0c, 0x01, 0x20, 0x20, "Off"                    },
	{0x0c, 0x01, 0x20, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x0c, 0x01, 0x80, 0x80, "Off"                    },
	{0x0c, 0x01, 0x80, 0x00, "On"                     },
	
	// Dip 2
	SYSTEM1_COINAGE(0x0d)
};

STDDIPINFO(Blockgal)
/*
static struct BurnDIPInfo BlckgalbDIPList[]=
{
	// Default Values
	{0x0c, 0xff, 0xff, 0xd7, NULL                     },
	{0x0d, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x0c, 0x01, 0x01, 0x00, "Upright"                },
	{0x0c, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0c, 0x01, 0x02, 0x00, "Off"                    },
	{0x0c, 0x01, 0x02, 0x02, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Lives"                  },
	{0x0c, 0x01, 0x08, 0x08, "2"                      },
	{0x0c, 0x01, 0x08, 0x00, "3"                      },
	
	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0c, 0x01, 0x10, 0x10, "10k 30k 60k 100k 150k"  },
	{0x0c, 0x01, 0x10, 0x00, "30k 50k 100k 200k 300k" },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x0c, 0x01, 0x20, 0x20, "Off"                    },
	{0x0c, 0x01, 0x20, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x0c, 0x01, 0x80, 0x80, "Off"                    },
	{0x0c, 0x01, 0x80, 0x00, "On"                     },
	
	// Dip 2
	SYSTEM1_COINAGE(0x0d)
};

STDDIPINFO(Blckgalb)
*/
/*
static struct BurnDIPInfo WboyuDIPList[]=
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
*/
/*
static struct BurnDIPInfo TeddybbDIPList[]=
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

static struct BurnDIPInfo PitfalluDIPList[]=
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

static struct BurnDIPInfo MyheroDIPList[]=
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
*/
/*
static struct BurnDIPInfo WbmljbDIPList[]=
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
/*
static struct BurnDIPInfo SeganinjDIPList[]=
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

static struct BurnDIPInfo FlickyDIPList[]=
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
*/
static struct BurnDIPInfo GardiaDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0x7c, NULL                     },

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
	{0x14, 0x01, 0x30, 0x30, " 5k 20k 30k"            },
	{0x14, 0x01, 0x30, 0x20, "10k 25k 50k"            },
	{0x14, 0x01, 0x30, 0x10, "15k 30k 60k"            },
	{0x14, 0x01, 0x30, 0x00, "None"                   },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x14, 0x01, 0x40, 0x40, "Easy"                   },
	{0x14, 0x01, 0x40, 0x00, "Hard"                   },
};
STDDIPINFO(Gardia)

static struct BurnDIPInfo StarjackDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0xf6, NULL                     },

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
	{0x14, 0x01, 0x38, 0x38, "Every 20k"              },
	{0x14, 0x01, 0x38, 0x28, "Every 30k"              },
	{0x14, 0x01, 0x38, 0x18, "Every 40k"              },
	{0x14, 0x01, 0x38, 0x08, "Every 50k"              },
	{0x14, 0x01, 0x38, 0x30, "20k, then every 30k"    },
	{0x14, 0x01, 0x38, 0x20, "30k, then every 40k"    },
	{0x14, 0x01, 0x38, 0x10, "40k, then every 50k"    },
	{0x14, 0x01, 0x38, 0x00, "50k, then every 60k"    },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x14, 0x01, 0xc0, 0xc0, "Easy"                   },
	{0x14, 0x01, 0xc0, 0x80, "Medium"                 },
	{0x14, 0x01, 0xc0, 0x40, "Hard"                   },
	{0x14, 0x01, 0xc0, 0x00, "Hardest"                },
};

STDDIPINFO(Starjack)

static struct BurnDIPInfo RaflesiaDIPList[]=
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
	{0x14, 0x01, 0x30, 0x30, "20k,  70k and 120k"     },
	{0x14, 0x01, 0x30, 0x20, "30k,  80k and 150k"     },
	{0x14, 0x01, 0x30, 0x10, "50k, 100k and 200k"     },
	{0x14, 0x01, 0x30, 0x00, "None"                   },
};

STDDIPINFO(Raflesia)

#undef SYSTEM1_COINAGE

/*==============================================================================================
ROM Descriptions
===============================================================================================*/
static struct BurnRomInfo BlockgalRomDesc[] = {
	{ "bg.116",            0x004000, 0xa99b231a, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bg.109",            0x004000, 0xa6b573d5, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	
	{ "bg.120",            0x002000, 0xd848faff, BRF_ESS | BRF_PRG }, //  2	Z80 #2 Program Code
	
	{ "bg.62",             0x002000, 0x7e3ea4eb, BRF_GRA },		  //  3 Tiles
	{ "bg.61",             0x002000, 0x4dd3d39d, BRF_GRA },		  //  4 Tiles
	{ "bg.64",             0x002000, 0x17368663, BRF_GRA },		  //  5 Tiles
	{ "bg.63",             0x002000, 0x0c8bc404, BRF_GRA },		  //  6 Tiles
	{ "bg.66",             0x002000, 0x2b7dc4fa, BRF_GRA },		  //  7 Tiles
	{ "bg.65",             0x002000, 0xed121306, BRF_GRA },		  //  8 Tiles
	
	{ "bg.117",            0x004000, 0xe99cc920, BRF_GRA },		  //  9 Sprites
	{ "bg.04",             0x004000, 0x213057f8, BRF_GRA },		  //  10 Sprites
	{ "bg.110",            0x004000, 0x064c812c, BRF_GRA },		  //  11 Sprites
	{ "bg.05",             0x004000, 0x02e0b040, BRF_GRA },		  //  12 Sprites

	{ "pr5317.76",         0x000100, 0x648350b8, BRF_OPT },		  //  13 Timing PROM
	
	{ "3170029.key",      0x002000, 0x350d7f93, BRF_ESS | BRF_PRG }, //  14 MC8123 Key
};
STD_ROM_PICK(Blockgal)
STD_ROM_FN(Blockgal)

static struct BurnRomInfo GardiaRomDesc[] = {
	{ "epr10255.1",        0x008000, 0x89282a6b, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr10254.2",        0x008000, 0x2826b6d8, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr10253.3",        0x008000, 0x7911260f, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr10243.120",      0x004000, 0x87220660, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr10249.61",       0x004000, 0x4e0ad0f2, BRF_GRA },		  //  4 Tiles
	{ "epr10248.64",       0x004000, 0x3515d124, BRF_GRA },		  //  5 Tiles
	{ "epr10247.66",       0x004000, 0x541e1555, BRF_GRA },		  //  6 Tiles
	
	{ "epr10234.117",      0x008000, 0x8a6aed33, BRF_GRA },		  //  7 Sprites
	{ "epr10233.110",      0x008000, 0xc52784d3, BRF_GRA },		  //  8 Sprites
	{ "epr10236.04",       0x008000, 0xb35ab227, BRF_GRA },		  //  9 Sprites
	{ "epr10235.5",        0x008000, 0x006a3151, BRF_GRA },		  //  10 Sprites

	{ "bprom.3",           0x000100, 0x8eee0f72, BRF_OPT },		  //  11 Red PROM
	{ "bprom.2",           0x000100, 0x3e7babd7, BRF_OPT },		  //  12 Green PROM
	{ "bprom.1",           0x000100, 0x371c44a6, BRF_OPT },		  //  13 Blue PROM
	{ "pr5317.4",          0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Gardia)
STD_ROM_FN(Gardia)

static struct BurnRomInfo StarjackRomDesc[] = {
	{ "epr5320b.129",      0x002000, 0x7ab72ecd, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr5321a.130",      0x002000, 0x38b99050, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr5322a.131",      0x002000, 0x103a595b, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	{ "epr5323.132",      0x002000, 0x46af0d58, BRF_ESS | BRF_PRG }, //  3	Z80 #1 Program Code
	{ "epr5324.133",      0x002000, 0x1e89efe2, BRF_ESS | BRF_PRG }, //  4	Z80 #1 Program Code
	{ "epr5325.134",      0x002000, 0xd6e379a1, BRF_ESS | BRF_PRG }, //  5	Z80 #1 Program Code
	
	{ "epr5332.3",        0x002000, 0x7a72ab3d, BRF_ESS | BRF_PRG }, //  6	Z80 #2 Program Code
	
	{ "epr5331.82",       0x002000, 0x251d898f, BRF_GRA },		  //  7 Tiles
	{ "epr5330.65",       0x002000, 0xeb048745, BRF_GRA },		  //  8 Tiles
	{ "epr5329.81",       0x002000, 0x3e8bcaed, BRF_GRA },		  //  9 Tiles
	{ "epr5328.64",       0x002000, 0x9ed7849f, BRF_GRA },		  //  10 Tiles
	{ "epr5327.80",       0x002000, 0x79e92cb1, BRF_GRA },		  //  11 Tiles
	{ "epr5326.63",       0x002000, 0xba7e2b47, BRF_GRA },		  //  12 Tiles
	
	{ "epr5318.86",       0x004000, 0x6f2e1fd3, BRF_GRA },		  //  13 Sprites
	{ "epr5319.93",       0x004000, 0xebee4999, BRF_GRA },		  //  14 Sprites

	{ "pr5317.106",       0x000100, 0x648350b8, BRF_OPT },		  //  15 Timing PROM
};

STD_ROM_PICK(Starjack)
STD_ROM_FN(Starjack)

static struct BurnRomInfo RaflesiaRomDesc[] = {
	{ "epr7411.116",      0x004000, 0x88a0c6c6, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr7412.109",      0x004000, 0xd3b8cddf, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr7413.96",       0x004000, 0xb7e688b3, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr7420.120",      0x002000, 0x14387666, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr7419.62",       0x002000, 0xbfd5f34c, BRF_GRA },		  //  4 Tiles
	{ "epr7418.61",       0x002000, 0xf8cbc9b6, BRF_GRA },		  //  5 Tiles
	{ "epr7417.64",       0x002000, 0xe63501bc, BRF_GRA },		  //  6 Tiles
	{ "epr7416.63",       0x002000, 0x093e5693, BRF_GRA },		  //  7 Tiles
	{ "epr7415.66",       0x002000, 0x1a8d6bd6, BRF_GRA },		  //  8 Tiles
	{ "epr7414.65",       0x002000, 0x5d20f218, BRF_GRA },		  //  9 Tiles
	
	{ "epr7407.117",      0x004000, 0xf09fc057, BRF_GRA },		  //  10 Sprites
	{ "epr7409.04",       0x004000, 0x819fedb8, BRF_GRA },		  //  11 Sprites
	{ "epr7408.110",      0x004000, 0x3189f33c, BRF_GRA },		  //  12 Sprites
	{ "epr7410.05",       0x004000, 0xced74789, BRF_GRA },		  //  13 Sprites

	{ "pr5317.76",        0x000100, 0x648350b8, BRF_OPT },		  //  14 Timing PROM
};

STD_ROM_PICK(Raflesia)
STD_ROM_FN(Raflesia)

#endif