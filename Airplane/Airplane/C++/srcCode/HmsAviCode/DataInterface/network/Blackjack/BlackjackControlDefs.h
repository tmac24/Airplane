//
//  BlackjackControlDefs.h
//  XGPS170 Developers Kit.
//
//  Version 2.5.1
//  Licensed under the terms of the BSD License, as specified below.

/*
 Copyright (c) 2015 Dual Electronics Corp.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 Redistributions of source code must strong the above copyright notice, this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 Neither the name of the Dual Electronics Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __set170_h__
#define __set170_h__

#define	GDL90MSG_TO_XGPS170			123	//0x7B
#define	GDL90MSG_FROM_XGPS170		124	//0x7C

#define	cmd170_query				1
#define	cmd170_set					2
#define	cmd170_response				3
#define	cmd170_ack					80
#define cmd170_nack					88

                                    // Operations	Payload		Description
                                    // ===========	========	==============
#define	item_MessageFilter		7	// query/set	1 bytes		bit 0: Enable GPS Sat Info in GDL90 stream output  (Msg Id 121)
                                                //				bit 1: Enable ADS-B message Bypass (Msg Id 30, 31)
                                                //				bit 2: Enable Traffic messages (Msg Id 20)
#define	item_LedBrightness		14	// query/set	2 bytes		Brightness Level in %. Second byte for ADS-B Act indication


#endif

