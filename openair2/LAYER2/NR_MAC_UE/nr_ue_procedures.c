/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/* \file ue_procedures.c
 * \brief procedures related to UE
 * \author R. Knopp, K.H. HSU
 * \date 2018
 * \version 0.1
 * \company Eurecom / NTUST
 * \email: knopp@eurecom.fr, kai-hsiang.hsu@eurecom.fr
 * \note
 * \warning
 */

#include "mac_proto.h"
#include "RRC/NR_UE/rrc_proto.h"

#include <stdio.h>

int8_t nr_ue_decode_mib(
	module_id_t module_id,
	int 		cc_id,
	uint8_t 	gNB_index,
	uint8_t 	extra_bits,	//	8bits 38.212 c7.1.1
	uint32_t    l_ssb_equal_64,
	//uint32_t 	*ssb_index,	//	from decoded MIB
	//uint32_t 	*frameP,	//	10 bits = 6(in decoded MIB)+4(in extra bits from L1)
	void 		*pduP,		//	encoded MIB
	uint16_t 	pdu_len){

    printf("[L2][MAC] decode mib\n");

	NR_UE_MAC_INST_t *mac = get_mac_inst(module_id);

    nr_mac_rrc_data_ind_ue( module_id, cc_id, gNB_index,
		     NR_BCCH_BCH, (uint8_t *) pduP, pdu_len );
    

    if(mac->mib != NULL){
	    uint32_t frame = (mac->mib->systemFrameNumber.buf[0] >> mac->mib->systemFrameNumber.bits_unused);
	    uint32_t frame_number_4lsb = (uint32_t)(extra_bits & 0xf);                      //	extra bits[0:3]
	    uint32_t half_frame_bit = (uint32_t)(( extra_bits >> 4 ) & 0x1 );               //	extra bits[4]
	    uint32_t ssb_subcarrier_offset_msb = (uint32_t)(( extra_bits >> 5 ) & 0x1 );    //	extra bits[5]
	    
	    uint32_t ssb_subcarrier_offset = mac->mib->ssb_SubcarrierOffset;

	    uint32_t ssb_index = 0;

	    frame = frame << 4;
	    frame = frame | frame_number_4lsb;

	    if(l_ssb_equal_64){
	    	ssb_index = (( extra_bits >> 5 ) & 0x7 );                                   //	extra bits[5:7]
	    }else{
			if(ssb_subcarrier_offset_msb){
			    ssb_subcarrier_offset = ssb_subcarrier_offset | 0x10;
			}
	    }

		printf("system frame number(6 MSB bits): %d\n",  mac->mib->systemFrameNumber.buf[0]);
		printf("system frame number(with LSB): %d\n", (int)frame);
		printf("subcarrier spacing:            %d\n", (int)mac->mib->subCarrierSpacingCommon);
		printf("ssb carrier offset(with MSB):  %d\n", (int)ssb_subcarrier_offset);
		printf("dmrs type A position:          %d\n", (int)mac->mib->dmrs_TypeA_Position);
		printf("pdcch config sib1:             %d\n", (int)mac->mib->pdcch_ConfigSIB1);
		printf("cell barred:                   %d\n", (int)mac->mib->cellBarred);
		printf("intra frequcney reselection:   %d\n", (int)mac->mib->intraFreqReselection);
		printf("half frame bit(extra bits):    %d\n", (int)half_frame_bit);
		printf("ssb index(extra bits):         %d\n", (int)ssb_index);

	    // fill in the elements in config request inside P5 message
	    mac->phy_config.config_req.pbch_config.system_frame_number = frame;    //  after calculation
	    mac->phy_config.config_req.pbch_config.subcarrier_spacing_common = mac->mib->subCarrierSpacingCommon;
	    mac->phy_config.config_req.pbch_config.ssb_subcarrier_offset = ssb_subcarrier_offset;  //  after calculation
	    mac->phy_config.config_req.pbch_config.dmrs_type_a_position = mac->mib->dmrs_TypeA_Position;
	    mac->phy_config.config_req.pbch_config.pdcch_config_sib1 = mac->mib->pdcch_ConfigSIB1;
	    mac->phy_config.config_req.pbch_config.cell_barred = mac->mib->cellBarred;
	    mac->phy_config.config_req.pbch_config.intra_frequency_reselection = mac->mib->intraFreqReselection;
	    mac->phy_config.config_req.pbch_config.half_frame_bit = half_frame_bit;
	    mac->phy_config.config_req.pbch_config.ssb_index = ssb_index;
	    mac->phy_config.config_req.config_mask |= FAPI_NR_CONFIG_REQUEST_MASK_PBCH;

	    if(mac->if_module != NULL && mac->if_module->phy_config_request != NULL){
		mac->if_module->phy_config_request(&mac->phy_config);
	    }
    }
    return 0;
}

// Performs :
// 1. TODO: Call RRC for link status return to PHY
// 2. TODO: Perform SR/BSR procedures for scheduling feedback
// 3. TODO: Perform PHR procedures

NR_UE_L2_STATE_t nr_ue_scheduler(
    const module_id_t module_id,
    const uint8_t gNB_index,
    const int cc_id,
    const frame_t rx_frame,
    const slot_t rx_slot,
    const frame_t tx_frame,
    const slot_t tx_slot){


	return CONNECTION_OK;
}