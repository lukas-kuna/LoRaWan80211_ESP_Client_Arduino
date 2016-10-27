/* LoRaWan80211.cpp -- LoRaWan over 802.11 implementation
 *
 * Copyright (C) 2016 Lukáš Kuna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

extern "C"
{
#include "LoRaMacCrypto.h"
#include "LoRaWan80211.h"
#include "user_interface.h"
}

#include <inttypes.h>
#include <string.h>
#include <Arduino.h>


LoRaWan80211::LoRaWan80211(uint32_t dev_addr)
{
	this->dev_addr = dev_addr;
}

void LoRaWan80211::begin()
{
	wifi_set_opmode(STATION_MODE);
	system_phy_set_max_tpw(this->power);
	wifi_set_user_limit_rate_mask(LIMIT_RATE_MASK_ALL);
	wifi_set_user_rate_limit(RC_LIMIT_11B, 0, RATE_11B_B1M, RATE_11B_B1M);
  
  this->started = 1;
}

void LoRaWan80211::set_tx_power(uint8_t power)
{
  this->power = power;
  
  if (this->started)
    system_phy_set_max_tpw(this->power);
}

void LoRaWan80211::set_counter(uint32_t counter)
{
  this->counter = counter;
}

void LoRaWan80211::enable_channel(uint8_t channel)
{
  if (channel < 1 || channel > 14) return;
  
  this->channels |= 1 << channel;
}

void LoRaWan80211::disable_channel(uint8_t channel)
{
  if (channel < 1 || channel > 14) return;
  
  this->channels &= ~(1 << channel);
}

uint8_t LoRaWan80211::send_raw(uint8_t *data, int len)
{
	int ch;

	for (ch = 1; ch <= 14; ch++)
	{
    if (! (channels & (1 << ch))) continue;
		
    wifi_set_channel(ch);
		wifi_send_pkt_freedom(data, len, 0);
		delay(2);
	}

  if (this->debug)
  {
    int i;
    Serial.printf("Sent:");
    for (i = 0; i < len; i++)
      Serial.printf(" %02X", data[i]);
    Serial.printf("\n");
  }
	
  return(1);
}


  // function is highly inspired by PrepareFrame (Lora-net / LoRaMac-node)
  // https://github.com/Lora-net/LoRaMac-node/blob/master/src/mac/LoRaMac.c
  // (C)2013 Semtech
  // License: Revised BSD License
uint8_t LoRaWan80211::send_abp(uint8_t *payload, int payload_size, uint8_t *appskey, uint8_t *nwkskey)
{
  uint8_t LoRaMacPayload[payload_size];
  uint32_t mic;
  uint8_t pktLen = 0;
  uint8_t lorawan_h[] = { 0x08, 0x01, 0x00, 0x00, 'L', 'O', 'R', 'A' };
  uint8_t LoRaMacBuffer[13 + payload_size + sizeof(lorawan_h)];

  if (payload_size < 3) return(0);

  memcpy(LoRaMacBuffer + pktLen, lorawan_h, sizeof(lorawan_h));
  pktLen += sizeof(lorawan_h); 

  LoRaMacBuffer[pktLen++] = 0x40;                                   // UU

  LoRaMacBuffer[pktLen++] = (this->dev_addr) & 0xFF;                // DevAddr
  LoRaMacBuffer[pktLen++] = (this->dev_addr >> 8) & 0xFF;
  LoRaMacBuffer[pktLen++] = (this->dev_addr >> 16) & 0xFF;
  LoRaMacBuffer[pktLen++] = (this->dev_addr >> 24) & 0xFF;

  LoRaMacBuffer[pktLen++] = 0;                                      // FCtrl

  LoRaMacBuffer[pktLen++] = this->counter & 0xFF;                   // FCounter
  LoRaMacBuffer[pktLen++] = (this->counter >> 8) & 0xFF;

  LoRaMacBuffer[pktLen++] = 3;                                      // FPort

  LoRaMacPayloadEncrypt((uint8_t *)payload,                         // payload
                        payload_size,
                        appskey,
                        this->dev_addr,
                        0,
                        this->counter,
                        LoRaMacPayload);
  memcpy(LoRaMacBuffer + pktLen, LoRaMacPayload, payload_size);
  pktLen += payload_size; 

  LoRaMacComputeMic(LoRaMacBuffer + sizeof(lorawan_h),
                    pktLen - sizeof(lorawan_h),
                    nwkskey,
                    dev_addr,
                    0,
                    this->counter,
                    &mic);

  LoRaMacBuffer[pktLen++] = mic & 0xFF;                             // FMic
  LoRaMacBuffer[pktLen++] = (mic >> 8) & 0xFF;
  LoRaMacBuffer[pktLen++] = (mic >> 16) & 0xFF;
  LoRaMacBuffer[pktLen++] = (mic >> 24) & 0xFF;

  this->counter++;

  return(this->send_raw(LoRaMacBuffer, pktLen));
}
