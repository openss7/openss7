/*****************************************************************************

 @(#) $Id: dcc.h,v 0.9.2.1 2008-05-07 16:01:41 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2008-05-07 16:01:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dcc.h,v $
 Revision 0.9.2.1  2008-05-07 16:01:41  brian
 - added NLI X.25-PLP CONS and XX25 implemetnation

 *****************************************************************************/

#ifndef __LOCAL_DCC_H__
#define __LOCAL_DCC_H__

#ident "@(#) $RCSfile: dcc.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* DCC (Data Country Codes) listed in Annex J of ITU Recommendation X.121 */

struct dccformat {
	ushort code;
	char *country;
} dcc[] = {
	[2] = {202, "Greece"},
	[4] = {204, "Netherlands (Kingdom of the)"},
	[5] = {205, "Netherlands (Kingdom of the)"},
	[6] = {206, "Belgium"},
	[8] = {208, "France"},
	[9] = {209, "France"},
	[10] = {210, "France"},
	[11] = {211, "France"},
	[12] = {212, "Monaco (Principality of)"},
	[13] = {213, "Andorra (Principality of)"},
	[14] = {214, "Spain"},
	[15] = {215, "Spain"},
	[16] = {216, "Hungary (Republic of)"},
	[18] = {218, "Bosnia and Herzegovina"},
	[19] = {219, "Croatia (Republic of)"},
	[20] = {220, "Yugoslavia (Federal Republic of)"},
	[22] = {222, "Italy"},
	[23] = {223, "Italy"},
	[24] = {224, "Italy"},
	[25] = {225, "Vatican City State"},
	[26] = {226, "Romania"},
	[28] = {228, "Switzerland (Confedration of)"},
	[29] = {229, "Switzerland (Confedration of)"},
	[30] = {230, "Czech Republic"},
	[31] = {231, "Slovak Republic"},
	[32] = {232, "Austria"},
	[33] = {233, "Austria"},
	[34] = {234, "United Kingdom of Great Britain and Northern Ireland"},
	[35] = {235, "United Kingdom of Great Britain and Northern Ireland"},
	[36] = {236, "United Kingdom of Great Britain and Northern Ireland"},
	[37] = {237, "United Kingdom of Great Britain and Northern Ireland"},
	[38] = {238, "Denmark"},
	[39] = {239, "Denmark"},
	[40] = {240, "Sweden"},
	[42] = {242, "Norway"},
	[43] = {243, "Norway"},
	[44] = {244, "Finland"},
	[46] = {246, "Lithuania (Republic of)"},
	[47] = {247, "Latvia (Republic of)"},
	[48] = {248, "Estonia (Republic of)"},
	[50] = {250, "Russian Federation"},
	[51] = {251, "Russian Federation"},
	[55] = {255, "Ukraine"},
	[57] = {257, "Belarus (Republic of)"},
	[59] = {259, "Moldova (Republic of)"},
	[60] = {260, "Poland (Republic of)"},
	[61] = {261, "Poland (Republic of)"},
	[62] = {262, "Germany (Federal Republic of)"},
	[63] = {263, "Germany (Federal Republic of)"},
	[64] = {264, "Gernany (Federal Republic of)"},
	[65] = {265, "Germany (Federal Republic of)"},
	[66] = {266, "Gibraltar"},
	[68] = {268, "Portugal"},
	[69] = {269, "Portugal"},
	[70] = {270, "Luxembourg"},
	[72] = {272, "Ireland"},
	[74] = {274, "Iceland"},
	[76] = {276, "Albania (Republic of)"},
	[78] = {278, "Malta"},
	[80] = {280, "Cyprus (Republic of)"},
	[82] = {282, "Georgia"},
	[83] = {283, "Armenia (Republic of)"},
	[84] = {284, "Bulgaria (Republic of)"},
	[86] = {286, "Turkey"},
	[88] = {288, "Faroe Islands"},
	[90] = {290, "Greenland"},
	[92] = {292, "San Marino (Republic of)"},
	[93] = {293, "Slovenia (Republic of)"},
	[94] = {294, "The Fomer Yugoslav Republic of Macedonia"},
	[95] = {295, "Liechtenstein (Principality of)"},
	[102] = {302, "Canada"},
	[103] = {303, "Canada"},
	[108] = {308, "Saint Pierre and Miquelon (Collectivité terrioriale de la République française)"},
	[110] = {310, "United States of America"},
	[111] = {311, "United States of America"},
	[112] = {312, "United States of America"},
	[113] = {313, "United States of America"},
	[114] = {314, "United States of America"},
	[115] = {315, "United States of America"},
	[116] = {316, "United States of America"},
	[130] = {330, "Puerto Rico"},
	[132] = {332, "United States Virgin Islands"},
	[134] = {334, "Mexico"},
	[135] = {335, "Mexico"},
	[138] = {338, "Jamaica"},
	[140] = {340, "Guadeloupe (French Department of) and Martinique (French Department of)"},
	[142] = {342, "Barbados"},
	[144] = {344, "Antigua and Barbuda"},
	[146] = {346, "Cayman Islands"},
	[148] = {348, "British Virgin Islands"},
	[150] = {350, "Bermuda"},
	[152] = {352, "Grenada"},
	[154] = {354, "Monserrat"},
	[156] = {356, "Saint Kitts and Nevis"},
	[158] = {358, "Saint Lucia"},
	[160] = {360, "Saint Vincent and the Grenadines"},
	[162] = {362, "Netherlands Antilles"},
	[163] = {363, "Aruba"},
	[164] = {364, "Bahamas (Commonwealth of the)"},
	[165] = {365, "Anguilla"},
	[166] = {366, "Dominica (Commonwealth of)"},
	[168] = {368, "Cuba"},
	[170] = {370, "Dominican Republic"},
	[172] = {372, "Haiti (Republic of)"},
	[174] = {374, "Trinidad and Tobago"},
	[176] = {376, "Turks and Caicos Islands"},
	[200] = {400, "Azerbaijani Republic"},
	[201] = {401, "Kazakstan (Republic of)"},
	[204] = {404, "India (Republic of)"},
	[210] = {410, "Pakistan (Islamic Republic of)"},
	[211] = {411, "Pakistan (Islamic Republic of)"},
	[212] = {412, "Afghanistan (Islamic State of)"},
	[213] = {413, "Sri Lanka (Democratic Socialist Republic of)"},
	[214] = {414, "Myanmar (Union of)"},
	[215] = {415, "Lebanon"},
	[216] = {416, "Syrian Arab Republic"},
	[218] = {418, "Iraq (Republic of)"},
	[219] = {419, "Kuwait (State of)"},
	[220] = {420, "Saudi Arabia (Kingdom of)"},
	[221] = {421, "Yemen (Republic of)"},
	[222] = {422, "Oman (Sultanate of)"},
	[224] = {424, "United Arab Emirates"},
	[225] = {425, "Israel (State of)"},
	[226] = {426, "Bahrain (State of)"},
	[227] = {427, "Qatar (State of)"},
	[228] = {428, "Mongolia"},
	[229] = {429, "Nepal"},
	[230] = {430, "United Arab Emirates (Abu Dhabi)"},
	[231] = {431, "United Arab Emirates (Duhai)"},
	[232] = {432, "Iran (Islamic Republic of)"},
	[234] = {434, "Uzbekistan (Republic of)"},
	[236] = {436, "Tajikistan (Republic of)"},
	[237] = {437, "Kyrgyz Republic"},
	[238] = {438, "Turkmenistan"},
	[240] = {440, "Japan"},
	[241] = {441, "Japan"},
	[242] = {442, "Japan"},
	[243] = {443, "Japan"},
	[250] = {450, "Korea (Replublic of)"},
	[252] = {452, "Viet Nam (Socialist Republic of)"},
	[253] = {453, "Hongkong"},
	[254] = {454, "Hongkong"},
	[255] = {455, "Macau"},
	[256] = {456, "Cambodia (Kingdom of)"},
	[257] = {457, "Lao People's Democratic Republic"},
	[260] = {460, "China (People's Republic of)"},
	[266] = {466, "Taiwan, China"},
	[267] = {467, "Democratic People's Republic of Korea"},
	[270] = {470, "Bangladesh (People's Republic of)"},
	[272] = {472, "Maldives (Republic of)"},
	[280] = {480, "Korea (Republic of)"},
	[281] = {481, "Korea (Republic of)"},
	[302] = {502, "Malaysia"},
	[305] = {505, "Australia"},
	[310] = {510, "Indonesia (Republic of)"},
	[315] = {515, "Philipines (Republic of the)"},
	[320] = {520, "Thailand"},
	[325] = {525, "Singapore (Republic of)"},
	[328] = {528, "Brunei Darussalam"},
	[330] = {530, "New Zeland"},
	[334] = {534, "Northern Mariana Islands (Commonwealth of the)"},
	[335] = {535, "Guam"},
	[336] = {536, "Nauru (Republic of)"},
	[337] = {537, "Paua New Guinea"},
	[339] = {539, "Tonga (Kingdom of)"},
	[340] = {540, "Solomon Islands"},
	[341] = {541, "Vanuatu (Republic of)"},
	[342] = {542, "Fiji (Republic of)"},
	[343] = {543, "Wallis and Futuna (French Overseas Territory)"},
	[344] = {544, "American Samoa"},
	[345] = {545, "Kiribati (Republic of)"},
	[346] = {546, "New Caledonia (French Overseas Territory)"},
	[347] = {547, "French Polynesia (French Overseas Territory)"},
	[348] = {548, "Cook Islands"},
	[349] = {549, "Samoa (Independent State of)"},
	[350] = {550, "Micronesia (Federated States of)"},
	[402] = {602, "Egypt (Arab Republic of)"},
	[403] = {603, "Algeria (People's Democratic Republic of)"},
	[404] = {604, "Morocco (Kingdom of)"},
	[405] = {605, "Tunisia"},
	[406] = {606, "Libya (Socialist People's Libyan Arab Jamahiriya)"},
	[407] = {607, "Gambia (Republic of the)"},
	[408] = {608, "Senegal (Republic of)"},
	[409] = {609, "Mauritania (Islamic Republic of)"},
	[410] = {610, "Mali (Republic of)"},
	[411] = {611, "Guinea (Republic of)"},
	[412] = {612, "Còte d'Ivorie (Republic of)"},
	[413] = {613, "Burkina Faso"},
	[414] = {614, "Niger (Republic of the)"},
	[415] = {615, "Togolese Republic"},
	[416] = {616, "Benin (Republic of)"},
	[417] = {617, "Mauritius (Republic of)"},
	[418] = {618, "Liberia (Republic of)"},
	[419] = {619, "Sierra Leone"},
	[420] = {620, "Ghana"},
	[421] = {621, "Nigeria (Federal Republic of)"},
	[422] = {622, "Chad (Republic of)"},
	[423] = {623, "Central African Republic"},
	[424] = {624, "Camaeroon (Republic of)"},
	[425] = {625, "Cape Verde (Republic of)"},
	[426] = {626, "Sao Tome and Principe (Democratic Repubic of)"},
	[427] = {627, "Equatorial Guinea (Republic of)"},
	[428] = {628, "Gabonese Republic"},
	[429] = {629, "Congo (Republic of the)"},
	[430] = {630, "Democratic Republic of the Congo"},
	[431] = {631, "Angola (Republic of)"},
	[432] = {632, "Guinea-Bissau (Republic of)"},
	[433] = {633, "Seychelles (Republic of)"},
	[434] = {634, "Sudan (Republic of the)"},
	[435] = {635, "Rwandese Republic"},
	[436] = {636, "Ethiopia (Federal Democratic Republic of)"},
	[437] = {637, "Somalia Democratic Republic"},
	[438] = {638, "Djibouti (Republic of)"},
	[439] = {639, "Kenya (Republic of)"},
	[440] = {640, "Tanzania (United Republic of)"},
	[441] = {641, "Uganda (Republic of)"},
	[442] = {642, "Burundi (Republic of)"},
	[443] = {643, "Mozmbique (Republic of)"},
	[445] = {645, "Zambia (Republic of)"},
	[446] = {646, "Madagascar (Republic of)"},
	[447] = {647, "Reunion (French Department of)"},
	[448] = {648, "Zimbabwe (Republic of)"},
	[449] = {649, "Namibia (Republic of)"},
	[450] = {650, "Malawi"},
	[451] = {651, "Lesotho (Kingdom of)"},
	[452] = {652, "Botswana (Republic of)"},
	[453] = {653, "Swaziland (Kingdom of)"},
	[454] = {654, "Comoros (Islamic Federal Republic of)"},
	[455] = {655, "South Africa (Republic of)"},
	[458] = {658, "Eritrea"},
	[502] = {702, "Belize"},
	[504] = {704, "Guatemala (Republic of)"},
	[506] = {706, "El Salvador (Republic of)"},
	[508] = {708, "Honduras (Republic of)"},
	[510] = {710, "Nicaragua"},
	[512] = {712, "Costa Rica"},
	[514] = {714, "Panama (Republic of)"},
	[516] = {716, "Peru"},
	[522] = {722, "Argentine Republic"},
	[524] = {724, "Brazil (Federative Republic of)"},
	[525] = {725, "Brazil (Federative Republic of)"},
	[530] = {730, "Chile"},
	[532] = {732, "Colombia (Republic of)"},
	[534] = {734, "Venezuela (Bolivarian Republic of)"},
	[536] = {736, "Bolivia (Republic of)"},
	[538] = {738, "Guyana"},
	[540] = {740, "Ecuador"},
	[542] = {742, "Guiana (French Department of)"},
	[544] = {744, "Paraguay (Republic of)"},
	[546] = {746, "Suriname (Republic of)"},
	[548] = {748, "Uruguay (Eastern Republic of)"},
	{1110, "Spare"},
	{1111, "Immarsat mobile satellite data transmission system - Atlantic Ocean - East"},
	{1112, "Immarsat mobile satellite data transmission system - Pacific Ocean"},
	{1113, "Immarsat mobile satellite data transmission system - Indian Ocean"},
	{1114, "Immarsat mobile satellite data transmission system - Atlantic Ocean - West"},
	{1115, "Spare"},
	{1116, "Spare"},
	{1117, "Spare"},
	{1118, "Spare"},
	{1119, "Spare"},
};

#endif				/* __LOCAL_DCC_H__ */

