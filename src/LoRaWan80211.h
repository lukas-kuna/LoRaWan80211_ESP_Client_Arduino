/* LoRaWan80211.h -- LoRaWan over 802.11 header file
 *
 * Copyright (C) 2016 Lukáš Kuna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef _LoRaWan80211_h
#define _LoRaWan80211_h

class LoRaWan80211
{
	public:
		LoRaWan80211(uint32_t dev_addr);
		void begin();
    void set_tx_power(uint8_t power);
    void set_counter(uint32_t counter);
    void enable_channel(uint8_t channel); 
    void disable_channel(uint8_t channel); 
		uint8_t send_abp(uint8_t *payload, int payload_size, uint8_t *appskey, uint8_t *nwkskey);

	private:
		uint32_t dev_addr;
		uint32_t counter = 0;
    uint8_t power = 82;
    uint8_t started = 0;
    uint16_t channels = 0xFFFF;
    uint8_t debug = 1;

		uint8_t send_raw(uint8_t *data, int len);
};

#endif
