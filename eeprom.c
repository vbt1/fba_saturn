
#include "burnint.h"
#include "eeprom.h"

#define SERIAL_BUFFER_LENGTH 40
#define MEMORY_SIZE 1024

static  const eeprom_interface *intf = NULL;

static  int serial_count = 0;
static  unsigned char serial_buffer[SERIAL_BUFFER_LENGTH] = {0};
static  unsigned char eeprom_data[MEMORY_SIZE] = {0};
static  int eeprom_data_bits = 0;
static  int eeprom_read_address = 0;
static  int eeprom_clock_count = 0;
static  int latch = 0, reset_line = 0, clock_line = 0, sending = 0;
static  int locked = 0;
static  int reset_delay = 0;

static  int neeprom_available = 0;

static  int eeprom_command_match(const char *buf, const char *cmd, int len)
{
	if ( cmd == 0 )	return 0;
	if ( len == 0 )	return 0;

	for (;len>0;)
	{
		char b = *buf;
		char c = *cmd;

		if ((b==0) || (c==0))
			return (b==c);

		switch ( c )
		{
			case '0':
			case '1':
				if (b != c)	return 0;
			case 'X':
			case 'x':
				buf++;
				len--;
				cmd++;
				break;

			case '*':
				c = cmd[1];
				switch( c )
				{
					case '0':
					case '1':
					  	if (b == c)	{	cmd++;			}
						else		{	buf++;	len--;	}
						break;
					default:	return 0;
				}
		}
	}
	return (*cmd==0);
}

int EEPROMAvailable()
{
	return neeprom_available;
}

void EEPROMInit(const eeprom_interface *interface)
{
	intf = interface;

	if ((1 << intf->address_bits) * intf->data_bits / 8 > MEMORY_SIZE)
	{
//		bprintf(0, _T("EEPROM larger than eeprom allows"));
	}

	memset(eeprom_data,0xff,(1 << intf->address_bits) * intf->data_bits / 8);
	serial_count = 0;
	latch = 0;
	reset_line = EEPROM_ASSERT_LINE;
	clock_line = EEPROM_ASSERT_LINE;
	eeprom_read_address = 0;
	sending = 0;
	if (intf->cmd_unlock) locked = 1;
	else locked = 0;

//	char output[128];
//	sprintf (output, "config\\games\\%s.nv", BurnDrvGetTextA(DRV_NAME));

	neeprom_available = 0;

/*	int len = ((1 << intf->address_bits) * (intf->data_bits >> 3)) & (MEMORY_SIZE-1);

	FILE *fz = fopen(output, "rb");
	if (fz != NULL) {
		neeprom_available = 1;
		fread (eeprom_data, len, 1, fz);
		fclose (fz);
	}*/
}

void EEPROMExit()
{
//	char output[128];
//	sprintf (output, "config\\games\\%s.nv", BurnDrvGetTextA(DRV_NAME));

	neeprom_available = 0;

/*	int len = ((1 << intf->address_bits) * (intf->data_bits >> 3)) & (MEMORY_SIZE-1);

	FILE *fz = fopen(output, "wb");
	fwrite (eeprom_data, len, 1, fz);
	fclose (fz);
*/
}

static inline void eeprom_write(int bit)
{
	if (serial_count >= SERIAL_BUFFER_LENGTH-1)
	{
//		bprintf(0, _T("error: EEPROM serial buffer overflow\n"));
		return;
	}

	serial_buffer[serial_count++] = (bit ? '1' : '0');
	serial_buffer[serial_count] = 0;

	if ( (serial_count > intf->address_bits) &&
	      eeprom_command_match((char*)serial_buffer,intf->cmd_read,strlen((char*)serial_buffer)-intf->address_bits) )
	{
		int i,address;

		address = 0;
		for (i = serial_count-intf->address_bits;i < serial_count;i++)
		{
			address <<= 1;
			if (serial_buffer[i] == '1') address |= 1;
		}
		if (intf->data_bits == 16)
			eeprom_data_bits = (eeprom_data[2*address+0] << 8) + eeprom_data[2*address+1];
//			eeprom_data_bits = (eeprom_data[2*address+1] << 8) + eeprom_data[2*address+0];
		else
			eeprom_data_bits = eeprom_data[address];
		eeprom_read_address = address;
		eeprom_clock_count = 0;
		sending = 1;
		serial_count = 0;
	}
	else if ( (serial_count > intf->address_bits) &&
	           eeprom_command_match((char*)serial_buffer,intf->cmd_erase,strlen((char*)serial_buffer)-intf->address_bits) )
	{
		int i,address;

		address = 0;
		for (i = serial_count-intf->address_bits;i < serial_count;i++)
		{
			address <<= 1;
			if (serial_buffer[i] == '1') address |= 1;
		}

		if (locked == 0)
		{
			if (intf->data_bits == 16)
			{
				eeprom_data[2*address+0] = 0xff;
				eeprom_data[2*address+1] = 0xff;
			}
			else
				eeprom_data[address] = 0xff;
		}
		else
			serial_count = 0;
	}
	else if ( (serial_count > (intf->address_bits + intf->data_bits)) &&
	           eeprom_command_match((char*)serial_buffer,intf->cmd_write,strlen((char*)serial_buffer)-(intf->address_bits + intf->data_bits)) )
	{
		int i,address,data;

		address = 0;
		for (i = serial_count-intf->data_bits-intf->address_bits;i < (serial_count-intf->data_bits);i++)
		{
			address <<= 1;
			if (serial_buffer[i] == '1') address |= 1;
		}
		data = 0;
		for (i = serial_count-intf->data_bits;i < serial_count;i++)
		{
			data <<= 1;
			if (serial_buffer[i] == '1') data |= 1;
		}

		if (locked == 0)
		{
			if (intf->data_bits == 16)
			{
				eeprom_data[2*address+0] = data >> 8;
				eeprom_data[2*address+1] = data & 0xff;
			}
			else
				eeprom_data[address] = data;
		}
		else
			serial_count = 0;
	}
	else if ( eeprom_command_match((char*)serial_buffer,intf->cmd_lock,strlen((char*)serial_buffer)) )
	{
		locked = 1;
		serial_count = 0;
	}
	else if ( eeprom_command_match((char*)serial_buffer,intf->cmd_unlock,strlen((char*)serial_buffer)) )
	{
		locked = 0;
		serial_count = 0;
	}
}

void EEPROMReset()
{
	serial_count = 0;
	sending = 0;
	reset_delay = intf->reset_delay;
}

void EEPROMWriteBit(int bit)
{
	latch = bit;
}

int EEPROMRead()
{
	int res;

	if (sending)
		res = (eeprom_data_bits >> intf->data_bits) & 1;
	else
	{
		if (reset_delay > 0)
		{
			/* this is needed by wbeachvl */
			reset_delay--;
			res = 0;
		}
		else
			res = 1;
	}

	return res;
}

void EEPROMSetCSLine(int state)
{
	reset_line = state;

	if (reset_line != EEPROM_CLEAR_LINE)
		EEPROMReset();
}

void EEPROMSetClockLine(int state)
{
	if (state == EEPROM_PULSE_LINE || (clock_line == EEPROM_CLEAR_LINE && state != EEPROM_CLEAR_LINE))
	{
		if (reset_line == EEPROM_CLEAR_LINE)
		{
			if (sending)
			{
				if (eeprom_clock_count == intf->data_bits && intf->enable_multi_read)
				{
					eeprom_read_address = (eeprom_read_address + 1) & ((1 << intf->address_bits) - 1);
					if (intf->data_bits == 16)
						eeprom_data_bits = (eeprom_data[2*eeprom_read_address+0] << 8) + eeprom_data[2*eeprom_read_address+1];
//						eeprom_data_bits = (eeprom_data[2*eeprom_read_address+1] << 8) + eeprom_data[2*eeprom_read_address+0];
					else
						eeprom_data_bits = eeprom_data[eeprom_read_address];
					eeprom_clock_count = 0;
				}
				eeprom_data_bits = (eeprom_data_bits << 1) | 1;
				eeprom_clock_count++;
			}
			else
				eeprom_write(latch);
		}
	}

	clock_line = state;
}

void EEPROMFill(const unsigned char *data, int offset, int length)
{
	memcpyl(eeprom_data + offset, data, length);
//	neeprom_available=1;// vbt ajout
}
