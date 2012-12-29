--===================================
UPDATE ocndata    SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE pcdata     SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE rcdata     SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE rndata     SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE exdata     SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE lcadata    SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE wcdata     SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE switchdata SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE npadata    SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE prefixdata SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE pooldata   SET sect='telc' WHERE sect NOT NULL and sect!='telc';
UPDATE linedata   SET sect='telc' WHERE sect NOT NULL and sect!='telc';
--===================================
-- CATEGORY: A term used since 1984 in the LERG to identify the type of
-- company associated with a Company Code/OCN. (LERG1)
-- CAP		Competitive Access Provider
-- CLEC		Competitive Local Exchange Carrier
-- GENERAL	Various miscellaneous cases
-- INTL		International
-- L RESELLER	Local Reseller
-- IC		Interexchange Carrier
-- ILEC		Incumber Local Exchage Carrier
-- IPES		Internet Protocol Enabled Services
-- P RESELLER	Personal Communication Services Reseller
-- PCS		Personal Communications Service
-- RBOC		Regional Bell Operating Company
-- ULEC		Unbundled Local Exchange Carrier
-- WIRELESS	Wireless Provider (Non-PCS Cellular,Paging,Radio)
-- W RESELLER	Wireless Reseller
UPDATE prefixdata SET nxxtype='C' WHERE nxxtype=='CLEC'; -- simple translation
UPDATE prefixdata SET nxxtype='W' WHERE nxxtype=='WIRELESS'; -- simple translation
UPDATE prefixdata SET nxxtype='P' WHERE nxxtype=='PCS'; -- simple translation
UPDATE prefixdata SET nxxtype='R' WHERE nxxtype=='RBOC'; -- simple translation
UPDATE prefixdata SET nxxtype='I' WHERE nxxtype=='ICO'; -- simple translation
UPDATE prefixdata SET nxxtype='?' WHERE nxxtype=='UNKNOWN'; -- simple translation
UPDATE prefixdata SET nxxtype='A' WHERE nxxtype=='CAP'; -- simple translation
UPDATE prefixdata SET nxxtype='L' WHERE nxxtype=='L RESELLER'; -- simple translation
UPDATE prefixdata SET nxxtype='-' WHERE nxxtype=='N/A'; -- simple translation
UPDATE prefixdata SET nxxtype='U' WHERE nxxtype=='ULEC'; -- simple translation
UPDATE prefixdata SET nxxtype='E' WHERE nxxtype=='W RESELLER'; -- simple translation
UPDATE prefixdata SET nxxtype='G' WHERE nxxtype=='GENERAL'; -- simple translation
--===================================
UPDATE prefixdata SET rc=NULL WHERE rc=='XXXXXXXXXX'; -- #FIXED: no use for this
DELETE FROM rcdata WHERE rc=='XXXXXXXXXX'; -- #FIXED: no use for this
--========================================================================
DELETE FROM switchdata WHERE switch=='N/A'; -- FIXED: invalid clli 'N' (data) [N/A]
DELETE FROM switchdata WHERE switch=='N'; -- FIXED: invalid clli 'N' (data) [N/A]
DELETE FROM switchdata WHERE switch=='A'; -- FIXED: invalid clli 'N' (data) [N/A]
DELETE FROM wcdata WHERE wc=='N/A'; -- FIXED: invalid clli 'N' (data)
DELETE FROM wcdata WHERE wc=='N'; -- FIXED: invalid clli 'N' (data)
DELETE FROM wcdata WHERE wc=='A'; -- FIXED: invalid clli 'N' (data)
UPDATE switchdata SET tdm=NULL WHERE tdm=='N/A'; -- FIXED: invalid clli 'A' (data) [AAPKOKXADS0,ABBGNCXA648,ABBYSK01RS1,ABCYONXTSG1,ABDLWIXARS0,ABFDBC010KD,ABFDBC011MD,ABFDBC01DS2,ABFDBC01DS3,ABFDBCAI0MD,...]
---------------
UPDATE pooldata   SET switch=NULL WHERE switch=='NOCLLIKNOWN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data) [201-204-0,201-204-1,201-204-3,201-204-9,201-205-1,201-205-2,201-210-0,201-210-2,201-210-4,201-210-5,...]
UPDATE prefixdata SET switch=NULL WHERE switch=='NOCLLIKNOWN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data) [201-201,201-211,201-260,201-311,201-351,201-411,201-479,201-509,201-511,201-550,...]
DELETE FROM switchdata WHERE switch=='NOCLLIKNOWN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data) [NOCLLIKNOWN]
DELETE FROM wcdata WHERE wc=='NOCLLIKN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data)
---------------
UPDATE prefixdata SET switch=NULL WHERE switch=='XXXXXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (data) [203-471,203-472,203-473,203-474,203-476,203-478,210-770,212-970,213-976,214-936,...]
DELETE FROM switchdata WHERE switch=='XXXXXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (data) [XXXXXXXXXXX]
DELETE FROM wcdata WHERE wc=='XXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (data)
---------------
UPDATE pooldata   SET switch='SMFRCT0164Z' WHERE switch=='SMFRC0164Z'; -- FIXED: invalid clli 'SMFRC0164Z' (data) [203-297-0,203-297-7]
UPDATE prefixdata SET switch='SMFRCT0164Z' WHERE switch=='SMFRC0164Z'; -- FIXED: invalid clli 'SMFRC0164Z' (data) [203-300]
UPDATE OR IGNORE switchdata SET switch='SMFRCT0164Z' WHERE switch=='SMFRC0164Z'; -- FIXED: invalid clli 'SMFRC0164Z' (data) [SMFRC0164Z]
DELETE FROM switchdata WHERE switch=='SMFRC0164Z'; -- FIXED: invalid clli 'SMFRC0164Z' (data) [SMFRC0164Z]
UPDATE OR IGNORE wcdata SET wc='SMFRCT01' WHERE wc=='SMFRC016'; -- FIXED: invalid clli 'SMFRC0164Z' (data)
DELETE FROM wcdata WHERE wc=='SMFRC016'; -- FIXED: invalid clli 'SMFRC0164Z' (data)
---------------
UPDATE prefixdata SET switch='MSCWIDDL0MD' WHERE switch=='MSCW1DDL0MD'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data) [208-669]
UPDATE OR IGNORE switchdata SET switch='MSCWIDDL0MD' WHERE switch=='MSCW1DDL0MD'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data) [MSCW1DDL0MD]
DELETE FROM switchdata WHERE switch=='MSCW1DDL0MD'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data) [MSCW1DDL0MD]
UPDATE OR IGNORE wcdata SET wc='MSCWIDDL' WHERE wc=='MSCW1DDL'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data)
DELETE FROM wcdata WHERE wc=='MSCW1DDL'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data)
---------------
UPDATE prefixdata SET switch='SNVACA13DS0' WHERE switch=='SVACA13DS0'; -- FIXED: invalid clli 'SVACA13DS0' (data) [408-416]
UPDATE OR IGNORE switchdata SET switch='SNVACA13DS0' WHERE switch=='SVACA13DS0'; -- FIXED: invalid clli 'SVACA13DS0' (data) [SVACA13DS0]
DELETE FROM switchdata WHERE switch=='SVACA13DS0'; -- FIXED: invalid clli 'SVACA13DS0' (data) [SVACA13DS0]
UPDATE OR IGNORE wcdata SET wc='SNVACA13' WHERE wc=='SVACA13D'; -- FIXED: invalid clli 'SVACA13DS0' (data)
DELETE FROM wcdata WHERE wc=='SVACA13D'; -- FIXED: invalid clli 'SVACA13DS0' (data)
---------------
UPDATE prefixdata SET switch='FORSMS02AMD' WHERE switch=='FIRNS92AMD'; -- FIXED: invalid clli 'FIRNS92AMD' (data) [601-257]
UPDATE OR IGNORE switchdata SET switch='FORSMS02AMD' WHERE switch=='FIRNS92AMD'; -- FIXED: invalid clli 'FIRNS92AMD' (data) [FIRNS92AMD]
DELETE FROM switchdata WHERE switch=='FIRNS92AMD'; -- FIXED: invalid clli 'FIRNS92AMD' (data) [FIRNS92AMD]
UPDATE OR IGNORE wcdata SET wc='FORSMS02' WHERE wc=='FIRNS92A'; -- FIXED: invalid clli 'FIRNS92AMD' (data)
DELETE FROM wcdata WHERE wc=='FIRNS92A'; -- FIXED: invalid clli 'FIRNS92AMD' (data)
---------------
UPDATE prefixdata SET switch='PRTGMI01DS0' WHERE switch=='PRTGM101DS0'; -- FIXED: invalid clli 'PRTGM101DS0' (data) [616-855]
UPDATE OR IGNORE switchdata SET switch='PRTGMI01DS0' WHERE switch=='PRTGM101DS0'; -- FIXED: invalid clli 'PRTGM101DS0' (data) [PRTGM101DS0]
DELETE FROM switchdata WHERE switch=='PRTGM101DS0'; -- FIXED: invalid clli 'PRTGM101DS0' (data) [PRTGM101DS0]
UPDATE OR IGNORE wcdata SET wc='PRTGMI01' WHERE wc=='PRTGM101'; -- FIXED: invalid clli 'PRTGM101DS0' (data)
DELETE FROM wcdata WHERE wc=='PRTGM101'; -- FIXED: invalid clli 'PRTGM101DS0' (data)
---------------
UPDATE prefixdata SET switch='ORNGCA14DS0' WHERE switch=='ORNCA14DS0'; -- FIXED: invalid clli 'ORNCA14DS0' (data) [714-621,714-750]
UPDATE OR IGNORE switchdata SET switch='ORNGCA14DS0' WHERE switch=='ORNCA14DS0'; -- FIXED: invalid clli 'ORNCA14DS0' (data) [ORNCA14DS0]
DELETE FROM switchdata WHERE switch=='ORNCA14DS0'; -- FIXED: invalid clli 'ORNCA14DS0' (data) [ORNCA14DS0]
UPDATE OR IGNORE wcdata SET wc='ORNGCA14' WHERE wc=='ORNCA14D'; -- FIXED: invalid clli 'ORNCA14DS0' (data)
DELETE FROM wcdata WHERE wc=='ORNCA14D'; -- FIXED: invalid clli 'ORNCA14DS0' (data)
---------------
UPDATE prefixdata SET switch='LJNTCO26DS0' WHERE switch=='LJNTC026DS0'; -- FIXED: invalid clli 'LJNTC026DS0' (data) [719-363]
UPDATE OR IGNORE switchdata SET switch='LJNTCO26DS0' WHERE switch=='LJNTC026DS0'; -- FIXED: invalid clli 'LJNTC026DS0' (data) [LJNTC026DS0]
DELETE FROM switchdata WHERE switch=='LJNTC026DS0'; -- FIXED: invalid clli 'LJNTC026DS0' (data) [LJNTC026DS0]
UPDATE OR IGNORE wcdata SET wc='LJNTCO26' WHERE wc=='LJNTC026'; -- FIXED: invalid clli 'LJNTC026DS0' (data)
DELETE FROM wcdata WHERE wc=='LJNTC026'; -- FIXED: invalid clli 'LJNTC026DS0' (data)
---------------
UPDATE prefixdata SET switch='CLSQCO01DS0' WHERE switch=='CLSQC001DS0'; -- FIXED: invalid clli 'CLSQC001DS0' (data) [719-559]
UPDATE OR IGNORE switchdata SET switch='CLSQCO01DS0' WHERE switch=='CLSQC001DS0'; -- FIXED: invalid clli 'CLSQC001DS0' (data) [CLSQC001DS0]
DELETE FROM switchdata WHERE switch=='CLSQC001DS0'; -- FIXED: invalid clli 'CLSQC001DS0' (data) [CLSQC001DS0]
UPDATE OR IGNORE wcdata SET wc='CLSQCO01' WHERE wc=='CLSQC001'; -- FIXED: invalid clli 'CLSQC001DS0' (data)
DELETE FROM wcdata WHERE wc=='CLSQC001'; -- FIXED: invalid clli 'CLSQC001DS0' (data)
---------------
UPDATE prefixdata SET switch=NULL WHERE switch=='APRIL 2011'; -- FIXED: invalid clli 'APRIL 2011' (data) [917-344]
DELETE FROM switchdata WHERE switch=='APRIL 2011'; -- FIXED: invalid clli 'APRIL 2011' (data) [APRIL 2011]
DELETE FROM wcdata WHERE wc=='APRIL 20'; -- FIXED: invalid clli 'APRIL 2011' (data)
--========================================================================
UPDATE pooldata   SET switch=NULL WHERE switch=='BRPTCTDS0'; -- FIXED: bad clli length 9 (data) [203-275-5,203-275-6,203-360-2,203-383-0,203-383-9,203-455-4,203-610-0,203-610-1,203-610-2,203-610-3,...]
UPDATE prefixdata SET switch=NULL WHERE switch=='BRPTCTDS0'; -- FIXED: bad clli length 9 (data) [203-209]
DELETE FROM switchdata WHERE switch=='BRPTCTDS0'; -- FIXED: bad clli length 9 (data) [BRPTCTDS0]
DELETE FROM wcdata WHERE wc=='BRPTCTDS'; -- FIXED: bad clli length 9 (data)
---------------
UPDATE prefixdata SET switch='SCHZTXCECM2' WHERE switch=='SCHTXCECM2'; -- FIXED: bad clli length 10 (data) [210-380]
UPDATE OR IGNORE switchdata SET switch='SCHZTXCECM2' WHERE switch=='SCHTXCECM2'; -- FIXED: bad clli length 10 (data) [SCHTXCECM2]
DELETE FROM switchdata WHERE switch=='SCHTXCECM2'; -- FIXED: bad clli length 10 (data) [SCHTXCECM2]
UPDATE OR IGNORE wcdata SET wc='SCHZTXCE' WHERE wc=='SCHTXCEC'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='SCHTXCEC'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='SPFDILSDPMD' WHERE switch=='SPFDLSDPMD'; -- FIXED: bad clli length 10 (data) [217-306]
UPDATE OR IGNORE switchdata SET switch='SPFDILSDPMD' WHERE switch=='SPFDLSDPMD'; -- FIXED: bad clli length 10 (data) [SPFDLSDPMD]
DELETE FROM switchdata WHERE switch=='SPFDLSDPMD'; -- FIXED: bad clli length 10 (data) [SPFDLSDPMD]
UPDATE OR IGNORE wcdata SET wc='SPFDILSD' WHERE wc=='SPFDLSDP'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='SPFDLSDP'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='HNVIALQUCM2' WHERE switch=='HNVIALQUCM'; -- FIXED: bad clli length 10 (data) [256-678]
UPDATE OR IGNORE switchdata SET switch='HNVIALQUCM2' WHERE switch=='HNVIALQUCM'; -- FIXED: bad clli length 10 (data) [HNVIALQUCM]
DELETE FROM switchdata WHERE switch=='HNVIALQUCM'; -- FIXED: bad clli length 10 (data) [HNVIALQUCM]
---------------
UPDATE prefixdata SET switch='OWBOKYMAUMD' WHERE switch=='OWBOKYMAMD'; -- FIXED: bad clli length 10 (data) [270-513]
UPDATE OR IGNORE switchdata SET switch='OWBOKYMAUMD' WHERE switch=='OWBOKYMAMD'; -- FIXED: bad clli length 10 (data) [OWBOKYMAMD]
DELETE FROM switchdata WHERE switch=='OWBOKYMAMD'; -- FIXED: bad clli length 10 (data) [OWBOKYMAMD]
---------------
UPDATE prefixdata SET switch='BWLGKYMAUMD' WHERE switch=='BWGKYMAUMD'; -- FIXED: bad clli length 10 (data) [270-518]
UPDATE OR IGNORE switchdata SET switch='BWLGKYMAUMD' WHERE switch=='BWGKYMAUMD'; -- FIXED: bad clli length 10 (data) [BWGKYMAUMD]
DELETE FROM switchdata WHERE switch=='BWGKYMAUMD'; -- FIXED: bad clli length 10 (data) [BWGKYMAUMD]
UPDATE OR IGNORE wcdata SET wc='BWLGKYMA' WHERE wc=='BWGKYMAU'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='BWGKYMAU'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch=NULL WHERE switch=='ODD BALL'; -- FIXED: bad clli length 8 (data) [315-910,518-780,518-910,518-920,518-927,518-979,518-985,518-997,518-998,518-999,...]
DELETE FROM switchdata WHERE switch=='ODD BALL'; -- FIXED: bad clli length 8 (data) [ODD BALL]
DELETE FROM wcdata WHERE wc=='ODD BALL'; -- FIXED: bad clli length 8 (data)
---------------
UPDATE prefixdata SET switch='IRVECACM2MD' WHERE switch=='IRVCACM2MD'; -- FIXED: bad clli length 10 (data) [323-353]
UPDATE OR IGNORE switchdata SET switch='IRVECACM2MD' WHERE switch=='IRVCACM2MD'; -- FIXED: bad clli length 10 (data) [IRVCACM2MD]
DELETE FROM switchdata WHERE switch=='IRVCACM2MD'; -- FIXED: bad clli length 10 (data) [IRVCACM2MD]
UPDATE OR IGNORE wcdata SET wc='IRVECACM' WHERE wc=='IRVCACM2'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='IRVCACM2'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE pooldata   SET switch='YNTWOHIGCMD' WHERE switch=='YNTOHIGCMD'; -- FIXED: bad clli length 10 (data) [330-531-2,330-623-1,330-623-2,330-623-9,330-707-5,330-707-6,330-707-7,330-781-1,330-781-5,330-781-9,...]
UPDATE prefixdata SET switch='YNTWOHIGCMD' WHERE switch=='YNTOHIGCMD'; -- FIXED: bad clli length 10 (data) [330-518]
UPDATE OR IGNORE switchdata SET switch='YNTWOHIGCMD' WHERE switch=='YNTOHIGCMD'; -- FIXED: bad clli length 10 (data) [YNTOHIGCMD]
DELETE FROM switchdata WHERE switch=='YNTOHIGCMD'; -- FIXED: bad clli length 10 (data) [YNTOHIGCMD]
UPDATE OR IGNORE wcdata SET wc='YNTWOHIG' WHERE wc=='YNTOHIGC'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='YNTOHIGC'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='MILWWIAUCM2' WHERE switch=='MILWAUKZN1'; -- FIXED: bad clli length 10 (data) [414-788]
UPDATE OR IGNORE switchdata SET switch='MILWWIAUCM2' WHERE switch=='MILWAUKZN1'; -- FIXED: bad clli length 10 (data) [MILWAUKZN1]
DELETE FROM switchdata WHERE switch=='MILWAUKZN1'; -- FIXED: bad clli length 10 (data) [MILWAUKZN1]
UPDATE OR IGNORE wcdata SET wc='MILWWIAU' WHERE wc=='MILWAUKZ'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='MILWAUKZ'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='PHLAPAFGDSE' WHERE switch=='PHLAPFGDSE'; -- FIXED: bad clli length 10 (data) [484-220]
UPDATE OR IGNORE switchdata SET switch='PHLAPAFGDSE' WHERE switch=='PHLAPFGDSE'; -- FIXED: bad clli length 10 (data) [PHLAPFGDSE]
DELETE FROM switchdata WHERE switch=='PHLAPFGDSE'; -- FIXED: bad clli length 10 (data) [PHLAPFGDSE]
UPDATE OR IGNORE wcdata SET wc='PHLAPAFG' WHERE wc=='PHLAPFGD'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='PHLAPFGD'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='WINOMNAFCM4' WHERE switch=='WINOMNFCM4'; -- FIXED: bad clli length 10 (data) [507-429]
UPDATE OR IGNORE switchdata SET switch='WINOMNAFCM4' WHERE switch=='WINOMNFCM4'; -- FIXED: bad clli length 10 (data) [WINOMNFCM4]
DELETE FROM switchdata WHERE switch=='WINOMNFCM4'; -- FIXED: bad clli length 10 (data) [WINOMNFCM4]
UPDATE OR IGNORE wcdata SET wc='WINOMNAF' WHERE wc=='WINOMNFC'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='WINOMNFC'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='FKLNMACNDS0' WHERE switch=='FKLNMLNDS0'; -- FIXED: bad clli length 10 (data) [508-741]
UPDATE OR IGNORE switchdata SET switch='FKLNMACNDS0' WHERE switch=='FKLNMLNDS0'; -- FIXED: bad clli length 10 (data) [FKLNMLNDS0]
DELETE FROM switchdata WHERE switch=='FKLNMLNDS0'; -- FIXED: bad clli length 10 (data) [FKLNMLNDS0]
UPDATE OR IGNORE wcdata SET wc='FKLNMACN' WHERE wc=='FKLNMLND'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='FKLNMLND'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE pooldata   SET switch='DBLNCAEWAMD' WHERE switch=='DBLNCAEMD'; -- FIXED: bad clli length 9 (data) [510-338-5]
UPDATE prefixdata SET switch='DBLNCAEWAMD' WHERE switch=='DBLNCAEMD'; -- FIXED: bad clli length 9 (data) [510-385]
UPDATE OR IGNORE switchdata SET switch='DBLNCAEWAMD' WHERE switch=='DBLNCAEMD'; -- FIXED: bad clli length 9 (data) [DBLNCAEMD]
DELETE FROM switchdata WHERE switch=='DBLNCAEMD'; -- FIXED: bad clli length 9 (data) [DBLNCAEMD]
UPDATE OR IGNORE wcdata SET wc='DBLNCAEW' WHERE wc=='DBLNCAEM'; -- FIXED: bad clli length 9 (data)
DELETE FROM wcdata WHERE wc=='DBLNCAEM'; -- FIXED: bad clli length 9 (data)
---------------
UPDATE prefixdata SET switch='AUSVTXLU1MD' WHERE switch=='AUSTXLU1MD'; -- FIXED: bad clli length 10 (data) [512-844]
UPDATE OR IGNORE switchdata SET switch='AUSVTXLU1MD' WHERE switch=='AUSTXLU1MD'; -- FIXED: bad clli length 10 (data) [AUSTXLU1MD]
DELETE FROM switchdata WHERE switch=='AUSTXLU1MD'; -- FIXED: bad clli length 10 (data) [AUSTXLU1MD]
UPDATE OR IGNORE wcdata SET wc='AUSVTXLU' WHERE wc=='AUSTXLU1'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='AUSTXLU1'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='PTMONHBN3MD' WHERE switch=='PTMONBN3MD'; -- FIXED: bad clli length 10 (data) [603-557]
UPDATE OR IGNORE switchdata SET switch='PTMONHBN3MD' WHERE switch=='PTMONBN3MD'; -- FIXED: bad clli length 10 (data) [PTMONBN3MD]
DELETE FROM switchdata WHERE switch=='PTMONBN3MD'; -- FIXED: bad clli length 10 (data) [PTMONBN3MD]
UPDATE OR IGNORE wcdata SET wc='PTMONHBN' WHERE wc=='PTMONBN3'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='PTMONBN3'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='GDRQMIIYYMD' WHERE switch=='GDRQMIIYMD'; -- FIXED: bad clli length 10 (data) [616-836]
UPDATE OR IGNORE switchdata SET switch='GDRQMIIYYMD' WHERE switch=='GDRQMIIYMD'; -- FIXED: bad clli length 10 (data) [GDRQMIIYMD]
DELETE FROM switchdata WHERE switch=='GDRQMIIYMD'; -- FIXED: bad clli length 10 (data) [GDRQMIIYMD]
---------------
UPDATE prefixdata SET switch='STLSMOYQ3MD' WHERE switch=='STLSMOYQMD'; -- FIXED: bad clli length 10 (data) [618-977]
UPDATE OR IGNORE switchdata SET switch='STLSMOYQ3MD' WHERE switch=='STLSMOYQMD'; -- FIXED: bad clli length 10 (data) [STLSMOYQMD]
DELETE FROM switchdata WHERE switch=='STLSMOYQMD'; -- FIXED: bad clli length 10 (data) [STLSMOYQMD]
---------------
UPDATE prefixdata SET switch='NYCQNYPLDS0' WHERE switch=='NYCQNYLDS0'; -- FIXED: bad clli length 10 (data) [646-514]
UPDATE OR IGNORE switchdata SET switch='NYCQNYPLDS0' WHERE switch=='NYCQNYLDS0'; -- FIXED: bad clli length 10 (data) [NYCQNYLDS0]
DELETE FROM switchdata WHERE switch=='NYCQNYLDS0'; -- FIXED: bad clli length 10 (data) [NYCQNYLDS0]
UPDATE OR IGNORE wcdata SET wc='NYCQNYPL' WHERE wc=='NYCQNYLD'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='NYCQNYLD'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='DLTNGAXC2MD' WHERE switch=='SNPGACPCM3'; -- FIXED: bad clli length 10 (data) [706-218]
UPDATE OR IGNORE switchdata SET switch='DLTNGAXC2MD' WHERE switch=='SNPGACPCM3'; -- FIXED: bad clli length 10 (data) [SNPGACPCM3]
DELETE FROM switchdata WHERE switch=='SNPGACPCM3'; -- FIXED: bad clli length 10 (data) [SNPGACPCM3]
UPDATE OR IGNORE wcdata SET wc='DLTNGAXC' WHERE wc=='SNPGACPC'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='SNPGACPC'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE pooldata   SET switch='EUCLWIAYXDY' WHERE switch=='EUCLWIAXDY'; -- FIXED: bad clli length 10 (data) [715-239-1,715-239-5]
UPDATE prefixdata SET switch='EUCLWIAYXDY' WHERE switch=='EUCLWIAXDY'; -- FIXED: bad clli length 10 (data) [715-202]
UPDATE OR IGNORE switchdata SET switch='EUCLWIAYXDY' WHERE switch=='EUCLWIAXDY'; -- FIXED: bad clli length 10 (data) [EUCLWIAXDY]
DELETE FROM switchdata WHERE switch=='EUCLWIAXDY'; -- FIXED: bad clli length 10 (data) [EUCLWIAXDY]
UPDATE OR IGNORE wcdata SET wc='EUCLWIAY' WHERE wc=='EUCLWIAX'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='EUCLWIAX'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='HVTSPAAACM0' WHERE switch=='HVTSPAACM0'; -- FIXED: bad clli length 10 (data) [724-955]
UPDATE OR IGNORE switchdata SET switch='HVTSPAAACM0' WHERE switch=='HVTSPAACM0'; -- FIXED: bad clli length 10 (data) [HVTSPAACM0]
DELETE FROM switchdata WHERE switch=='HVTSPAACM0'; -- FIXED: bad clli length 10 (data) [HVTSPAACM0]
UPDATE OR IGNORE wcdata SET wc='HVTSPAAA' WHERE wc=='HVTSPAAC'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='HVTSPAAC'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='CHCHILLEDS0' WHERE switch=='CHGILLEDS0'; -- FIXED: bad clli length 10 (data) [773-337]
UPDATE OR IGNORE switchdata SET switch='CHCHILLEDS0' WHERE switch=='CHGILLEDS0'; -- FIXED: bad clli length 10 (data) [CHGILLEDS0]
DELETE FROM switchdata WHERE switch=='CHGILLEDS0'; -- FIXED: bad clli length 10 (data) [CHGILLEDS0]
UPDATE OR IGNORE wcdata SET wc='CHCHILLE' WHERE wc=='CHGILLED'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='CHGILLED'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='DKLBILAOCM1' WHERE switch=='OCN CHANGE'; -- FIXED: bad clli length 10 (data) [815-755]
UPDATE OR IGNORE switchdata SET switch='DKLBILAOCM1' WHERE switch=='OCN CHANGE'; -- FIXED: bad clli length 10 (data) [OCN CHANGE]
DELETE FROM switchdata WHERE switch=='OCN CHANGE'; -- FIXED: bad clli length 10 (data) [OCN CHANGE]
UPDATE OR IGNORE wcdata SET wc='DKLBILAO' WHERE wc=='OCN CHAN'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='OCN CHAN'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='NCHRSCPLCM1' WHERE switch=='NCHRSCPLCM'; -- FIXED: bad clli length 10 (data) [843-693,843-696,843-697,843-810,843-860,843-906,843-991]
UPDATE OR IGNORE switchdata SET switch='NCHRSCPLCM1' WHERE switch=='NCHRSCPLCM'; -- FIXED: bad clli length 10 (data) [NCHRSCPLCM]
DELETE FROM switchdata WHERE switch=='NCHRSCPLCM'; -- FIXED: bad clli length 10 (data) [NCHRSCPLCM]
---------------
UPDATE prefixdata SET switch='NYCKNYWM12N' WHERE switch=='NCKNYWM12N'; -- FIXED: bad clli length 10 (data) [917-221]
UPDATE OR IGNORE switchdata SET switch='NYCKNYWM12N' WHERE switch=='NCKNYWM12N'; -- FIXED: bad clli length 10 (data) [NCKNYWM12N]
DELETE FROM switchdata WHERE switch=='NCKNYWM12N'; -- FIXED: bad clli length 10 (data) [NCKNYWM12N]
UPDATE OR IGNORE wcdata SET wc='NYCKNYWM' WHERE wc=='NCKNYWM1'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='NCKNYWM1'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='NYCQNYWSCM1' WHERE switch=='NYCQNWSCM1'; -- FIXED: bad clli length 10 (data) [917-923]
UPDATE OR IGNORE switchdata SET switch='NYCQNYWSCM1' WHERE switch=='NYCQNWSCM1'; -- FIXED: bad clli length 10 (data) [NYCQNWSCM1]
DELETE FROM switchdata WHERE switch=='NYCQNWSCM1'; -- FIXED: bad clli length 10 (data) [NYCQNWSCM1]
UPDATE OR IGNORE wcdata SET wc='NYCQNYWS' WHERE wc=='NYCQNWSC'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='NYCQNWSC'; -- FIXED: bad clli length 10 (data)
---------------
UPDATE prefixdata SET switch='NWBLWIDPCM0' WHERE switch=='MILXWLB0MD'; -- FIXED: bad clli length 10 (data) [920-318]
UPDATE OR IGNORE switchdata SET switch='NWBLWIDPCM0' WHERE switch=='MILXWLB0MD'; -- FIXED: bad clli length 10 (data) [MILXWLB0MD]
DELETE FROM switchdata WHERE switch=='MILXWLB0MD'; -- FIXED: bad clli length 10 (data) [MILXWLB0MD]
UPDATE OR IGNORE wcdata SET wc='NWBLWIDP' WHERE wc=='MILXWLB0'; -- FIXED: bad clli length 10 (data)
DELETE FROM wcdata WHERE wc=='MILXWLB0'; -- FIXED: bad clli length 10 (data)
--========================================================================
UPDATE prefixdata SET switch=NULL WHERE switch=='ALL SWITCHE'; -- FIXED: invalid state/province/territory 'SW' (data) [205-203,205-204]
DELETE FROM switchdata WHERE switch=='ALL SWITCHE'; -- FIXED: invalid state/province/territory 'SW' (data) [ALL SWITCHE]
DELETE FROM wcdata WHERE wc=='ALL SWIT'; -- FIXED: invalid state/province/territory 'SW' (data)
---------------
UPDATE prefixdata SET switch='GAVLIDAJCM0' WHERE switch=='GAVILDAJCM0'; -- FIXED: invalid state/province/territory 'LD' (data) [208-507]
UPDATE OR IGNORE switchdata SET switch='GAVLIDAJCM0' WHERE switch=='GAVILDAJCM0'; -- FIXED: invalid state/province/territory 'LD' (data) [GAVILDAJCM0]
DELETE FROM switchdata WHERE switch=='GAVILDAJCM0'; -- FIXED: invalid state/province/territory 'LD' (data) [GAVILDAJCM0]
UPDATE OR IGNORE wcdata SET wc='GAVLIDAJ' WHERE wc=='GAVILDAJ'; -- FIXED: invalid state/province/territory 'LD' (data)
DELETE FROM wcdata WHERE wc=='GAVILDAJ'; -- FIXED: invalid state/province/territory 'LD' (data)
---------------
UPDATE prefixdata SET switch='DCTRILDCGMD' WHERE switch=='DCTRUKBXCM8'; -- FIXED: invalid state/province/territory 'UK' (data) [217-412]
UPDATE OR IGNORE switchdata SET switch='DCTRILDCGMD' WHERE switch=='DCTRUKBXCM8'; -- FIXED: invalid state/province/territory 'UK' (data) [DCTRUKBXCM8]
DELETE FROM switchdata WHERE switch=='DCTRUKBXCM8'; -- FIXED: invalid state/province/territory 'UK' (data) [DCTRUKBXCM8]
UPDATE OR IGNORE wcdata SET wc='DCTRILDC' WHERE wc=='DCTRUKBX'; -- FIXED: invalid state/province/territory 'UK' (data)
DELETE FROM wcdata WHERE wc=='DCTRUKBX'; -- FIXED: invalid state/province/territory 'UK' (data)
---------------
UPDATE prefixdata SET switch='MOBLAL02XFX' WHERE switch=='MOBLAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (data) [251-583]
UPDATE OR IGNORE switchdata SET switch='MOBLAL02XFX' WHERE switch=='MOBLAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (data) [MOBLAAL02XF]
DELETE FROM switchdata WHERE switch=='MOBLAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (data) [MOBLAAL02XF]
UPDATE OR IGNORE wcdata SET wc='MOBLAL02' WHERE wc=='MOBLAAL0'; -- FIXED: invalid state/province/territory 'AA' (data)
DELETE FROM wcdata WHERE wc=='MOBLAAL0'; -- FIXED: invalid state/province/territory 'AA' (data)
---------------
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [314-737-0,314-737-1,314-737-2,314-737-3,314-737-6,314-737-7,314-737-8,314-737-9]
UPDATE prefixdata SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [314-737]
UPDATE OR IGNORE switchdata SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [NWTPMPABCM8]
DELETE FROM switchdata WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [NWTPMPABCM8]
UPDATE OR IGNORE wcdata SET wc='NWTPMOAB' WHERE wc=='NWTPMPAB'; -- FIXED: invalid state/province/territory 'MP' (data)
DELETE FROM wcdata WHERE wc=='NWTPMPAB'; -- FIXED: invalid state/province/territory 'MP' (data)
---------------
UPDATE pooldata   SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [323-366-3,323-366-4]
UPDATE prefixdata SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [323-366]
UPDATE OR IGNORE switchdata SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [LSANDAJWGT0]
DELETE FROM switchdata WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [LSANDAJWGT0]
UPDATE OR IGNORE wcdata SET wc='LSANCAJW' WHERE wc=='LSANDAJW'; -- FIXED: invalid state/province/territory 'DA' (data)
DELETE FROM wcdata WHERE wc=='LSANDAJW'; -- FIXED: invalid state/province/territory 'DA' (data)
---------------
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [405-881-0,405-881-1,405-881-2,405-881-3,405-881-4,405-881-5,405-881-6,405-881-7,405-881-8,405-881-9]
UPDATE prefixdata SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [405-881]
UPDATE OR IGNORE switchdata SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [OKCYPKCEX8X]
DELETE FROM switchdata WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [OKCYPKCEX8X]
UPDATE OR IGNORE wcdata SET wc='OKCYOKCE' WHERE wc=='OKCYPKCE'; -- FIXED: invalid state/province/territory 'PK' (data)
DELETE FROM wcdata WHERE wc=='OKCYPKCE'; -- FIXED: invalid state/province/territory 'PK' (data)
---------------
UPDATE pooldata   SET switch='BUMTTXTEWMD' WHERE switch=='BUMMTTXEWMD'; -- FIXED: invalid state/province/territory 'TT' (data) [409-223-1]
UPDATE prefixdata SET switch='BUMTTXTEWMD' WHERE switch=='BUMMTTXEWMD'; -- FIXED: invalid state/province/territory 'TT' (data) [409-223]
UPDATE OR IGNORE switchdata SET switch='BUMTTXTEWMD' WHERE switch=='BUMMTTXEWMD'; -- FIXED: invalid state/province/territory 'TT' (data) [BUMMTTXEWMD]
DELETE FROM switchdata WHERE switch=='BUMMTTXEWMD'; -- FIXED: invalid state/province/territory 'TT' (data) [BUMMTTXEWMD]
UPDATE OR IGNORE wcdata SET wc='BUMTTXTE' WHERE wc=='BUMMTTXE'; -- FIXED: invalid state/province/territory 'TT' (data)
DELETE FROM wcdata WHERE wc=='BUMMTTXE'; -- FIXED: invalid state/province/territory 'TT' (data)
---------------
UPDATE prefixdata SET switch='CJTGTN100MD' WHERE switch=='CJTGTM100MD'; -- FIXED: invalid state/province/territory 'TM' (data) [423-503]
UPDATE OR IGNORE switchdata SET switch='CJTGTN100MD' WHERE switch=='CJTGTM100MD'; -- FIXED: invalid state/province/territory 'TM' (data) [CJTGTM100MD]
DELETE FROM switchdata WHERE switch=='CJTGTM100MD'; -- FIXED: invalid state/province/territory 'TM' (data) [CJTGTM100MD]
UPDATE OR IGNORE wcdata SET wc='CJTGTN10' WHERE wc=='CJTGTM10'; -- FIXED: invalid state/province/territory 'TM' (data)
DELETE FROM wcdata WHERE wc=='CJTGTM10'; -- FIXED: invalid state/province/territory 'TM' (data)
---------------
UPDATE prefixdata SET switch='ALBYNY80DS0' WHERE switch=='ALBYBYBODS0'; -- FIXED: invalid state/province/territory 'BY' (data) [518-874]
UPDATE OR IGNORE switchdata SET switch='ALBYNY80DS0' WHERE switch=='ALBYBYBODS0'; -- FIXED: invalid state/province/territory 'BY' (data) [ALBYBYBODS0]
DELETE FROM switchdata WHERE switch=='ALBYBYBODS0'; -- FIXED: invalid state/province/territory 'BY' (data) [ALBYBYBODS0]
UPDATE OR IGNORE wcdata SET wc='ALBYNY80' WHERE wc=='ALBYBYBO'; -- FIXED: invalid state/province/territory 'BY' (data)
DELETE FROM wcdata WHERE wc=='ALBYBYBO'; -- FIXED: invalid state/province/territory 'BY' (data)
---------------
UPDATE prefixdata SET switch='ULYSKS03CM0' WHERE switch=='ULYSHS03CM0'; -- FIXED: invalid state/province/territory 'HS' (data) [620-934]
UPDATE OR IGNORE switchdata SET switch='ULYSKS03CM0' WHERE switch=='ULYSHS03CM0'; -- FIXED: invalid state/province/territory 'HS' (data) [ULYSHS03CM0]
DELETE FROM switchdata WHERE switch=='ULYSHS03CM0'; -- FIXED: invalid state/province/territory 'HS' (data) [ULYSHS03CM0]
UPDATE OR IGNORE wcdata SET wc='ULYSKS03' WHERE wc=='ULYSHS03'; -- FIXED: invalid state/province/territory 'HS' (data)
DELETE FROM wcdata WHERE wc=='ULYSHS03'; -- FIXED: invalid state/province/territory 'HS' (data)
---------------
UPDATE pooldata   SET switch='NYCSNYOODS0' WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [646-503-1,646-570-1]
UPDATE prefixdata SET switch='NYCSNYOODS0' WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [646-503,646-570]
UPDATE OR IGNORE switchdata SET switch='NYCSNYOODS0' WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [NYCMMYYJOMD]
DELETE FROM switchdata WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [NYCMMYYJOMD]
UPDATE OR IGNORE wcdata SET wc='NYCSNYOO' WHERE wc=='NYCMMYYJ'; -- FIXED: invalid state/province/territory 'MY' (data)
DELETE FROM wcdata WHERE wc=='NYCMMYYJ'; -- FIXED: invalid state/province/territory 'MY' (data)
---------------
UPDATE prefixdata SET switch='CLSPCO63DS0' WHERE switch=='CLSPCP63DS0'; -- FIXED: invalid state/province/territory 'CP' (data) [719-234]
UPDATE OR IGNORE switchdata SET switch='CLSPCO63DS0' WHERE switch=='CLSPCP63DS0'; -- FIXED: invalid state/province/territory 'CP' (data) [CLSPCP63DS0]
DELETE FROM switchdata WHERE switch=='CLSPCP63DS0'; -- FIXED: invalid state/province/territory 'CP' (data) [CLSPCP63DS0]
UPDATE OR IGNORE wcdata SET wc='CLSPCO63' WHERE wc=='CLSPCP63'; -- FIXED: invalid state/province/territory 'CP' (data)
DELETE FROM wcdata WHERE wc=='CLSPCP63'; -- FIXED: invalid state/province/territory 'CP' (data)
---------------
UPDATE pooldata   SET switch='ELGNILOT0MD' WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [773-816-0,773-816-1,773-816-2,773-816-3,773-816-4,773-816-6,773-816-7,773-816-8,773-816-9]
UPDATE prefixdata SET switch='ELGNILOT0MD' WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [773-816]
UPDATE OR IGNORE switchdata SET switch='ELGNILOT0MD' WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [CHICTGOZN10]
DELETE FROM switchdata WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [CHICTGOZN10]
UPDATE OR IGNORE wcdata SET wc='ELGNILOT' WHERE wc=='CHICTGOZ'; -- FIXED: invalid state/province/territory 'TG' (data)
DELETE FROM wcdata WHERE wc=='CHICTGOZ'; -- FIXED: invalid state/province/territory 'TG' (data)
---------------
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [828-217-8,828-217-9,828-270-4,828-270-5,828-270-6,828-270-8,828-404-4,828-404-7,828-404-9,828-409-5,...]
UPDATE prefixdata SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [828-238]
UPDATE OR IGNORE switchdata SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [HCLRMCXA2MD]
DELETE FROM switchdata WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [HCLRMCXA2MD]
UPDATE OR IGNORE wcdata SET wc='HCLRNCXA' WHERE wc=='HCLRMCXA'; -- FIXED: invalid state/province/territory 'MC' (data)
DELETE FROM wcdata WHERE wc=='HCLRMCXA'; -- FIXED: invalid state/province/territory 'MC' (data)
---------------
UPDATE pooldata   SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [843-606-5,843-606-6]
UPDATE prefixdata SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [843-606]
UPDATE OR IGNORE switchdata SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [CHTNXDCTX4Y]
DELETE FROM switchdata WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [CHTNXDCTX4Y]
UPDATE OR IGNORE wcdata SET wc='CHTNSCDT' WHERE wc=='CHTNXDCT'; -- FIXED: invalid state/province/territory 'XD' (data)
DELETE FROM wcdata WHERE wc=='CHTNXDCT'; -- FIXED: invalid state/province/territory 'XD' (data)
---------------
UPDATE prefixdata SET switch='TXRKTXAD1MD' WHERE switch=='TXRKRXAD1MD'; -- FIXED: invalid state/province/territory 'RX' (data) [903-219]
UPDATE OR IGNORE switchdata SET switch='TXRKTXAD1MD' WHERE switch=='TXRKRXAD1MD'; -- FIXED: invalid state/province/territory 'RX' (data) [TXRKRXAD1MD]
DELETE FROM switchdata WHERE switch=='TXRKRXAD1MD'; -- FIXED: invalid state/province/territory 'RX' (data) [TXRKRXAD1MD]
UPDATE OR IGNORE wcdata SET wc='TXRKTXAD' WHERE wc=='TXRKRXAD'; -- FIXED: invalid state/province/territory 'RX' (data)
DELETE FROM wcdata WHERE wc=='TXRKRXAD'; -- FIXED: invalid state/province/territory 'RX' (data)
---------------
UPDATE prefixdata SET switch='SVNHGABSIMD' WHERE switch=='SVNGHABSIMD'; -- FIXED: invalid state/province/territory 'HA' (data) [912-596]
UPDATE OR IGNORE switchdata SET switch='SVNHGABSIMD' WHERE switch=='SVNGHABSIMD'; -- FIXED: invalid state/province/territory 'HA' (data) [SVNGHABSIMD]
DELETE FROM switchdata WHERE switch=='SVNGHABSIMD'; -- FIXED: invalid state/province/territory 'HA' (data) [SVNGHABSIMD]
UPDATE OR IGNORE wcdata SET wc='SVNHGABS' WHERE wc=='SVNGHABS'; -- FIXED: invalid state/province/territory 'HA' (data)
DELETE FROM wcdata WHERE wc=='SVNGHABS'; -- FIXED: invalid state/province/territory 'HA' (data)
---------------
UPDATE prefixdata SET switch='LENYKSCJCM2' WHERE switch=='LENYHSCJCM2'; -- FIXED: invalid state/province/territory 'HS' (data) [913-302]
UPDATE OR IGNORE switchdata SET switch='LENYKSCJCM2' WHERE switch=='LENYHSCJCM2'; -- FIXED: invalid state/province/territory 'HS' (data) [LENYHSCJCM2]
DELETE FROM switchdata WHERE switch=='LENYHSCJCM2'; -- FIXED: invalid state/province/territory 'HS' (data) [LENYHSCJCM2]
UPDATE OR IGNORE wcdata SET wc='LENYKSCJ' WHERE wc=='LENYHSCJ'; -- FIXED: invalid state/province/territory 'HS' (data)
DELETE FROM wcdata WHERE wc=='LENYHSCJ'; -- FIXED: invalid state/province/territory 'HS' (data)
--======================================================================
UPDATE prefixdata SET switch='BRHMALXBCM2' WHERE switch=='TSC LALBX0M'; -- FIXED: poor equipment code 'X0M' (data) [205-246]
UPDATE OR IGNORE switchdata SET switch='BRHMALXBCM2' WHERE switch=='TSC LALBX0M'; -- FIXED: poor equipment code 'X0M' (data) [TSC LALBX0M]
---------------
UPDATE prefixdata SET switch='SKTNCA01XTY' WHERE switch=='SSKTNCA01XT'; -- FIXED: poor equipment code '1XT' (data) [209-553]
UPDATE OR IGNORE switchdata SET switch='SKTNCA01XTY' WHERE switch=='SSKTNCA01XT'; -- FIXED: poor equipment code '1XT' (data) [SSKTNCA01XT]
DELETE FROM switchdata WHERE switch=='SSKTNCA01XT'; -- FIXED: poor equipment code '1XT' (data) [SSKTNCA01XT]
UPDATE OR IGNORE wcdata SET wc='SKTNCA01' WHERE wc=='SSKTNCA0'; -- FIXED: poor equipment code '1XT' (data) [SSKTNCA0]
DELETE FROM wcdata WHERE wc=='SSKTNCA0'; -- FIXED: poor equipment code '1XT' (data) [SSKTNCA0]
---------------
UPDATE prefixdata SET switch='PLANTXYLCM0' WHERE switch=='PLANTXYCLM0'; -- FIXED: poor equipment code 'LM0' (data) [214-898]
UPDATE OR IGNORE switchdata SET switch='PLANTXYLCM0' WHERE switch=='PLANTXYCLM0'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
DELETE FROM switchdata WHERE switch=='PLANTXYCLM0'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
UPDATE OR IGNORE wcdata SET wc='PLANTXYL' WHERE wc=='PLANTXYC'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
DELETE FROM wcdata WHERE wc=='PLANTXYC'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
---------------
UPDATE prefixdata SET switch='MRSHIL020MD' WHERE switch=='DANVILT1AXA'; -- FIXED: poor equipment code 'AXA' (data) [217-822]
UPDATE OR IGNORE switchdata SET switch='MRSHIL020MD' WHERE switch=='DANVILT1AXA'; -- FIXED: poor equipment code 'AXA' (data) [DANVILT1AXA]
DELETE FROM switchdata WHERE switch=='DANVILT1AXA'; -- FIXED: poor equipment code 'AXA' (data) [DANVILT1AXA]
UPDATE OR IGNORE wcdata SET wc='MRSHIL02' WHERE wc=='DANVILT1'; -- FIXED: poor equipment code 'AXA' (data) [DANVILT1AXA]
DELETE FROM wcdata WHERE wc=='DANVILT1'; -- FIXED: poor equipment code 'AXA' (data) [DANVILT1AXA]
---------------
UPDATE prefixdata SET switch=NULL WHERE switch=='ALLSWITCHES'; -- FIXED: poor equipment code 'HES' (data) [228-203,228-204,256-203,256-204,334-203,334-204,404-204,404-440,404-930,502-203,...]
DELETE FROM switchdata WHERE switch=='ALLSWITCHES'; -- FIXED: poor equipment code 'HES' (data) [ALLSWITCHES]
---------------
UPDATE pooldata   SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (data) [234-284-0,234-284-6]
UPDATE prefixdata SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (data) [234-284]
UPDATE OR IGNORE switchdata SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (data) [HDSNOHXAPSO]
DELETE FROM switchdata WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (data) [HDSNOHXAPSO]
---------------
UPDATE prefixdata SET switch='TACMWA43CM2' WHERE switch=='TACMWA43XM2'; -- FIXED: poor equipment code 'XM2' (data) [253-677]
UPDATE OR IGNORE switchdata SET switch='TACMWA43CM2' WHERE switch=='TACMWA43XM2'; -- FIXED: poor equipment code 'XM2' (data) [TACMWA43XM2]
DELETE FROM switchdata WHERE switch=='TACMWA43XM2'; -- FIXED: poor equipment code 'XM2' (data) [TACMWA43XM2]
---------------
UPDATE prefixdata SET switch='HNVIALUN0GT' WHERE switch=='HNVIALUNOGT'; -- FIXED: poor equipment code 'OGT' (data) [256-631]
UPDATE OR IGNORE switchdata SET switch='HNVIALUN0GT' WHERE switch=='HNVIALUNOGT'; -- FIXED: poor equipment code 'OGT' (data) [HNVIALUNOGT]
DELETE FROM switchdata WHERE switch=='HNVIALUNOGT'; -- FIXED: poor equipment code 'OGT' (data) [HNVIALUNOGT]
---------------
UPDATE pooldata   SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [256-853-6]
UPDATE prefixdata SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [256-853]
UPDATE OR IGNORE switchdata SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [ALCYALAYBMX]
DELETE FROM switchdata WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [ALCYALAYBMX]
---------------
UPDATE prefixdata SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA'; -- FIXED: poor equipment code 'HMA' (data) [267-475,267-992]
UPDATE OR IGNORE switchdata SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
DELETE FROM switchdata WHERE switch=='WNRTPAACHMA'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
UPDATE OR IGNORE wcdata SET wc='WNRTPAAH' WHERE wc=='WNRTPAAC'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
DELETE FROM wcdata WHERE wc=='WNRTPAAC'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
---------------
UPDATE prefixdata SET switch='FDULSK04DS0' WHERE switch=='FDULSK04ET1'; -- FIXED: poor equipment code 'ET1' (data) [306-686]
UPDATE OR IGNORE switchdata SET switch='FDULSK04DS0' WHERE switch=='FDULSK04ET1'; -- FIXED: poor equipment code 'ET1' (data) [FDULSK04ET1]
DELETE FROM switchdata WHERE switch=='FDULSK04ET1'; -- FIXED: poor equipment code 'ET1' (data) [FDULSK04ET1]
---------------
UPDATE prefixdata SET switch='DVNPIAEASMD' WHERE switch=='DVNPIAEATDM'; -- FIXED: poor equipment code 'TDM' (data) [309-771]
UPDATE OR IGNORE switchdata SET switch='DVNPIAEASMD' WHERE switch=='DVNPIAEATDM'; -- FIXED: poor equipment code 'TDM' (data) [DVNPIAEATDM]
DELETE FROM switchdata WHERE switch=='DVNPIAEATDM'; -- FIXED: poor equipment code 'TDM' (data) [DVNPIAEATDM]
---------------
UPDATE pooldata   SET switch='STCDMNTOXRX' WHERE switch=='STCDMNOTOXR'; -- FIXED: poor equipment code 'OXR' (data) [320-428-0]
UPDATE prefixdata SET switch='STCDMNTOXRX' WHERE switch=='STCDMNOTOXR'; -- FIXED: poor equipment code 'OXR' (data) [320-428]
UPDATE OR IGNORE switchdata SET switch='STCDMNTOXRX' WHERE switch=='STCDMNOTOXR'; -- FIXED: poor equipment code 'OXR' (data) [STCDMNOTOXR]
DELETE FROM switchdata WHERE switch=='STCDMNOTOXR'; -- FIXED: poor equipment code 'OXR' (data) [STCDMNOTOXR]
UPDATE OR IGNORE wcdata SET wc='STCDMNTO' WHERE wc=='STCDMNOT'; -- FIXED: poor equipment code 'OXR' (data) [STCDMNOTOXR]
DELETE FROM wcdata WHERE wc=='STCDMNOT'; -- FIXED: poor equipment code 'OXR' (data) [STCDMNOTOXR]
---------------
UPDATE prefixdata SET switch='BDTMOHIDCM0' WHERE switch=='BDTMMOHIDCM'; -- FIXED: poor equipment code 'DCM' (data) [330-531]
UPDATE OR IGNORE switchdata SET switch='BDTMOHIDCM0' WHERE switch=='BDTMMOHIDCM'; -- FIXED: poor equipment code 'DCM' (data) [BDTMMOHIDCM]
DELETE FROM switchdata WHERE switch=='BDTMMOHIDCM'; -- FIXED: poor equipment code 'DCM' (data) [BDTMMOHIDCM]
UPDATE OR IGNORE wcdata SET wc='BDTMOHID' WHERE wc=='BDTMMOHI'; -- FIXED: poor equipment code 'DCM' (data) [BDTMMOHIDCM]
DELETE FROM wcdata WHERE wc=='BDTMMOHI'; -- FIXED: poor equipment code 'DCM' (data) [BDTMMOHIDCM]
---------------
UPDATE prefixdata SET switch='GSVLFLBSCM1' WHERE switch=='GSVLFLBASCM'; -- FIXED: poor equipment code 'SCM' (data) [352-517]
UPDATE OR IGNORE switchdata SET switch='GSVLFLBSCM1' WHERE switch=='GSVLFLBASCM'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
DELETE FROM switchdata WHERE switch=='GSVLFLBASCM'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
UPDATE OR IGNORE wcdata SET wc='GSVLFLBS' WHERE wc=='GSVLFLBA'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
DELETE FROM wcdata WHERE wc=='GSVLFLBA'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
---------------
UPDATE prefixdata SET switch='BLTMMDCH13Z' WHERE switch=='BLTMMDCHI3Z'; -- FIXED: poor equipment code 'I3Z' (data) [410-800]
UPDATE OR IGNORE switchdata SET switch='BLTMMDCH13Z' WHERE switch=='BLTMMDCHI3Z'; -- FIXED: poor equipment code 'I3Z' (data) [BLTMMDCHI3Z]
DELETE FROM switchdata WHERE switch=='BLTMMDCHI3Z'; -- FIXED: poor equipment code 'I3Z' (data) [BLTMMDCHI3Z]
---------------
UPDATE prefixdata SET switch='INDIPAASCM8' WHERE switch=='PITBPADTHVE'; -- FIXED: poor equipment code 'HVE' (data) [412-340]
UPDATE OR IGNORE switchdata SET switch='INDIPAASCM8' WHERE switch=='PITBPADTHVE'; -- FIXED: poor equipment code 'HVE' (data) [PITBPADTHVE]
DELETE FROM switchdata WHERE switch=='PITBPADTHVE'; -- FIXED: poor equipment code 'HVE' (data) [PITBPADTHVE]
UPDATE OR IGNORE wcdata SET wc='INDIPAAS' WHERE wc=='PITBPADT'; -- FIXED: poor equipment code 'HVE' (data) [PITBPADTHVE]
DELETE FROM wcdata WHERE wc=='PITBPADT'; -- FIXED: poor equipment code 'HVE' (data) [PITBPADTHVE]
---------------
UPDATE prefixdata SET switch='RSVTUTANCM0' WHERE switch=='RSVTUTTANCM'; -- FIXED: poor equipment code 'NCM' (data) [435-364]
UPDATE OR IGNORE switchdata SET switch='RSVTUTANCM0' WHERE switch=='RSVTUTTANCM'; -- FIXED: poor equipment code 'NCM' (data) [RSVTUTTANCM]
DELETE FROM switchdata WHERE switch=='RSVTUTTANCM'; -- FIXED: poor equipment code 'NCM' (data) [RSVTUTTANCM]
UPDATE OR IGNORE wcdata SET wc='RSVTUTAN' WHERE wc=='RSVTUTTA'; -- FIXED: poor equipment code 'NCM' (data) [RSVTUTTANCM]
DELETE FROM wcdata WHERE wc=='RSVTUTTA'; -- FIXED: poor equipment code 'NCM' (data) [RSVTUTTANCM]
---------------
UPDATE pooldata   SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [478-220-4]
UPDATE prefixdata SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [478-220]
UPDATE OR IGNORE switchdata SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [MACNGAMTXUS]
DELETE FROM switchdata WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [MACNGAMTXUS]
---------------
UPDATE prefixdata SET switch='PHLAPAFGW29' WHERE switch=='PHLAPAFGSW2'; -- FIXED: poor equipment code 'SW2' (data) [484-261]
UPDATE OR IGNORE switchdata SET switch='PHLAPAFGW29' WHERE switch=='PHLAPAFGSW2'; -- FIXED: poor equipment code 'SW2' (data) [PHLAPAFGSW2]
DELETE FROM switchdata WHERE switch=='PHLAPAFGSW2'; -- FIXED: poor equipment code 'SW2' (data) [PHLAPAFGSW2]
---------------
UPDATE prefixdata SET switch='PHLAPAFGW29' WHERE switch=='PHLAPAFGWS2'; -- FIXED: poor equipment code 'WS2' (data) [484-715]
UPDATE OR IGNORE switchdata SET switch='PHLAPAFGW29' WHERE switch=='PHLAPAFGWS2'; -- FIXED: poor equipment code 'WS2' (data) [PHLAPAFGWS2]
DELETE FROM switchdata WHERE switch=='PHLAPAFGWS2'; -- FIXED: poor equipment code 'WS2' (data) [PHLAPAFGWS2]
---------------
UPDATE prefixdata SET switch='PTTWPAPT3MD' WHERE switch=='PTTWPAPTHPF'; -- FIXED: poor equipment code 'HPF' (data) [484-941]
UPDATE OR IGNORE switchdata SET switch='PTTWPAPT3MD' WHERE switch=='PTTWPAPTHPF'; -- FIXED: poor equipment code 'HPF' (data) [PTTWPAPTHPF]
DELETE FROM switchdata WHERE switch=='PTTWPAPTHPF'; -- FIXED: poor equipment code 'HPF' (data) [PTTWPAPTHPF]
---------------
UPDATE prefixdata SET switch='LSVLKYAPX0X' WHERE switch=='LSVLKYKYAPX'; -- FIXED: poor equipment code 'APX' (data) [502-236,502-237,502-317,502-334,502-462]
UPDATE OR IGNORE switchdata SET switch='LSVLKYAPX0X' WHERE switch=='LSVLKYKYAPX'; -- FIXED: poor equipment code 'APX' (data) [LSVLKYKYAPX]
DELETE FROM switchdata WHERE switch=='LSVLKYKYAPX'; -- FIXED: poor equipment code 'APX' (data) [LSVLKYKYAPX]
UPDATE OR IGNORE wcdata SET wc='LSVLKYAP' WHERE wc=='LSVLKYKY'; -- FIXED: poor equipment code 'APX' (data) [LSVLKYKYAPX]
DELETE FROM wcdata WHERE wc=='LSVLKYKY'; -- FIXED: poor equipment code 'APX' (data) [LSVLKYKYAPX]
---------------
UPDATE pooldata   SET switch='ALBRNMJW0MD' WHERE switch=='ALBRNMJW0ND'; -- FIXED: poor equipment code '0ND' (data) [505-806-7,505-806-8]
UPDATE prefixdata SET switch='ALBRNMJW0MD' WHERE switch=='ALBRNMJW0ND'; -- FIXED: poor equipment code '0ND' (data) [505-806]
UPDATE OR IGNORE switchdata SET switch='ALBRNMJW0MD' WHERE switch=='ALBRNMJW0ND'; -- FIXED: poor equipment code '0ND' (data) [ALBRNMJW0ND]
DELETE FROM switchdata WHERE switch=='ALBRNMJW0ND'; -- FIXED: poor equipment code '0ND' (data) [ALBRNMJW0ND]
---------------
UPDATE pooldata   SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [509-385-0]
UPDATE prefixdata SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [509-385]
UPDATE OR IGNORE switchdata SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [SPKNWAOBWT1]
DELETE FROM switchdata WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [SPKNWAOBWT1]
---------------
UPDATE pooldata   SET switch='AUSUTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [512-961-2,512-961-9]
UPDATE prefixdata SET switch='AUSUTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [512-961]
UPDATE OR IGNORE switchdata SET switch='AUSUTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
DELETE FROM switchdata WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
UPDATE OR IGNORE wcdata SET wc='AUSUTXZM' WHERE wc=='AUSTUTXZ'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
DELETE FROM wcdata WHERE wc=='AUSTUTXZ'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
---------------
UPDATE prefixdata SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (data) [513-229]
UPDATE OR IGNORE switchdata SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (data) [RSMYOHROWV1]
DELETE FROM switchdata WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (data) [RSMYOHROWV1]
---------------
UPDATE prefixdata SET switch='SPNCIAMURL0' WHERE switch=='SPNCIAMURLO'; -- FIXED: poor equipment code 'RLO' (data) [515-395]
UPDATE OR IGNORE switchdata SET switch='SPNCIAMURL0' WHERE switch=='SPNCIAMURLO'; -- FIXED: poor equipment code 'RLO' (data) [SPNCIAMURLO]
DELETE FROM switchdata WHERE switch=='SPNCIAMURLO'; -- FIXED: poor equipment code 'RLO' (data) [SPNCIAMURLO]
---------------
UPDATE pooldata   SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [515-441-2,515-441-4,515-441-5,515-822-0,515-822-7,515-822-9]
UPDATE prefixdata SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [515-441,515-822]
UPDATE OR IGNORE switchdata SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [URDLIASDIKD]
DELETE FROM switchdata WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [URDLIASDIKD]
---------------
UPDATE pooldata   SET switch='INDNIABF1MD' WHERE switch=='INDNIABFELD'; -- FIXED: poor equipment code 'ELD' (data) [515-962-5]
UPDATE OR IGNORE switchdata SET switch='INDNIABF1MD' WHERE switch=='INDNIABFELD'; -- FIXED: poor equipment code 'ELD' (data) [INDNIABFELD]
DELETE FROM switchdata WHERE switch=='INDNIABFELD'; -- FIXED: poor equipment code 'ELD' (data) [INDNIABFELD]
---------------
UPDATE pooldata   SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK'; -- FIXED: poor equipment code 'XRK' (data) [540-466-0,540-466-2,540-466-6,540-466-9]
UPDATE prefixdata SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK'; -- FIXED: poor equipment code 'XRK' (data) [540-466]
UPDATE OR IGNORE switchdata SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK'; -- FIXED: poor equipment code 'XRK' (data) [RONKVALKXRK]
DELETE FROM switchdata WHERE switch=='RONKVALKXRK'; -- FIXED: poor equipment code 'XRK' (data) [RONKVALKXRK]
---------------
UPDATE pooldata   SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [570-233-4,570-233-8,570-233-9,570-579-5,570-751-2]
UPDATE prefixdata SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [570-233]
UPDATE OR IGNORE switchdata SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [SCTNPASCCOM]
DELETE FROM switchdata WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [SCTNPASCCOM]
---------------
UPDATE prefixdata SET switch='BMBGPABLX7X' WHERE switch=='BMBGPABLHVA'; -- FIXED: poor equipment code 'HVA' (data) [570-416]
UPDATE OR IGNORE switchdata SET switch='BMBGPABLX7X' WHERE switch=='BMBGPABLHVA'; -- FIXED: poor equipment code 'HVA' (data) [BMBGPABLHVA]
DELETE FROM switchdata WHERE switch=='BMBGPABLHVA'; -- FIXED: poor equipment code 'HVA' (data) [BMBGPABLHVA]
---------------
UPDATE prefixdata SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1'; -- FIXED: poor equipment code 'EC1' (data) [575-416,575-909]
UPDATE OR IGNORE switchdata SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1'; -- FIXED: poor equipment code 'EC1' (data) [HBBSNMCREC1]
DELETE FROM switchdata WHERE switch=='HBBSNMCREC1'; -- FIXED: poor equipment code 'EC1' (data) [HBBSNMCREC1]
---------------
UPDATE prefixdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (data) [601-216]
UPDATE OR IGNORE switchdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (data) [MTOLMSABAND]
DELETE FROM switchdata WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (data) [MTOLMSABAND]
---------------
UPDATE prefixdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (data) [601-233]
UPDATE OR IGNORE switchdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (data) [MCCMMSBLAMB]
DELETE FROM switchdata WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (data) [MCCMMSBLAMB]
---------------
UPDATE prefixdata SET switch='PRVSMSAMAMD' WHERE switch=='PRVSMSAMAND'; -- FIXED: poor equipment code 'AND' (data) [601-464]
UPDATE OR IGNORE switchdata SET switch='PRVSMSAMAMD' WHERE switch=='PRVSMSAMAND'; -- FIXED: poor equipment code 'AND' (data) [PRVSMSAMAND]
DELETE FROM switchdata WHERE switch=='PRVSMSAMAND'; -- FIXED: poor equipment code 'AND' (data) [PRVSMSAMAND]
---------------
UPDATE pooldata   SET switch='MNCHNHCOXEZ' WHERE switch=='MNCHNHCOXE7'; -- FIXED: poor equipment code 'XE7' (data) [603-757-5,603-903-0,603-903-1]
UPDATE prefixdata SET switch='MNCHNHCOXEZ' WHERE switch=='MNCHNHCOXE7'; -- FIXED: poor equipment code 'XE7' (data) [603-903]
UPDATE OR IGNORE switchdata SET switch='MNCHNHCOXEZ' WHERE switch=='MNCHNHCOXE7'; -- FIXED: poor equipment code 'XE7' (data) [MNCHNHCOXE7]
DELETE FROM switchdata WHERE switch=='MNCHNHCOXE7'; -- FIXED: poor equipment code 'XE7' (data) [MNCHNHCOXE7]
---------------
UPDATE prefixdata SET switch='LXTNKYXAXUX' WHERE switch=='LXTNKYAXAUX'; -- FIXED: poor equipment code 'AUX' (data) [606-655]
UPDATE OR IGNORE switchdata SET switch='LXTNKYXAXUX' WHERE switch=='LXTNKYAXAUX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
DELETE FROM switchdata WHERE switch=='LXTNKYAXAUX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
UPDATE OR IGNORE wcdata SET wc='LXTNKYXA' WHERE wc=='LXTNKYAX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
DELETE FROM wcdata WHERE wc=='LXTNKYAX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
---------------
UPDATE prefixdata SET switch='NWCSDEBCCM2' WHERE switch=='NWCSDEBCGM2'; -- FIXED: poor equipment code 'GM2' (data) [610-368]
UPDATE OR IGNORE switchdata SET switch='NWCSDEBCCM2' WHERE switch=='NWCSDEBCGM2'; -- FIXED: poor equipment code 'GM2' (data) [NWCSDEBCGM2]
DELETE FROM switchdata WHERE switch=='NWCSDEBCGM2'; -- FIXED: poor equipment code 'GM2' (data) [NWCSDEBCGM2]
---------------
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY'; -- FIXED: poor equipment code 'WPY' (data) [614-915-1,614-915-2,614-915-4,614-915-5,614-915-6,614-915-7,614-915-8,614-915-9]
UPDATE prefixdata SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY'; -- FIXED: poor equipment code 'WPY' (data) [614-915]
UPDATE OR IGNORE switchdata SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY'; -- FIXED: poor equipment code 'WPY' (data) [CLMBOHIAWPY]
DELETE FROM switchdata WHERE switch=='CLMBOHIAWPY'; -- FIXED: poor equipment code 'WPY' (data) [CLMBOHIAWPY]
---------------
UPDATE prefixdata SET switch='WCHTKSBRXFX' WHERE switch=='WCHTSKSBRXF'; -- FIXED: poor equipment code 'RXF' (data) [620-490]
UPDATE OR IGNORE switchdata SET switch='WCHTKSBRXFX' WHERE switch=='WCHTSKSBRXF'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
DELETE FROM switchdata WHERE switch=='WCHTSKSBRXF'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
UPDATE OR IGNORE wcdata SET wc='WCHTKSBR' WHERE wc=='WCHTSKSB'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
DELETE FROM wcdata WHERE wc=='WCHTSKSB'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
---------------
UPDATE prefixdata SET switch='LSANCARCDSZ' WHERE switch=='LSANCARCOSZ'; -- FIXED: poor equipment code 'OSZ' (data) [626-370]
UPDATE OR IGNORE switchdata SET switch='LSANCARCDSZ' WHERE switch=='LSANCARCOSZ'; -- FIXED: poor equipment code 'OSZ' (data) [LSANCARCOSZ]
DELETE FROM switchdata WHERE switch=='LSANCARCOSZ'; -- FIXED: poor equipment code 'OSZ' (data) [LSANCARCOSZ]
---------------
UPDATE prefixdata SET switch='FRLKMNFLDS0' WHERE switch=='FRLKMNFRLDS'; -- FIXED: poor equipment code 'LDS' (data) [651-466]
UPDATE OR IGNORE switchdata SET switch='FRLKMNFLDS0' WHERE switch=='FRLKMNFRLDS'; -- FIXED: poor equipment code 'LDS' (data) [FRLKMNFRLDS]
DELETE FROM switchdata WHERE switch=='FRLKMNFRLDS'; -- FIXED: poor equipment code 'LDS' (data) [FRLKMNFRLDS]
UPDATE OR IGNORE wcdata SET wc='FRLKMNFL' WHERE wc=='FRLKMNFR'; -- FIXED: poor equipment code 'LDS' (data) [FRLKMNFRLDS]
DELETE FROM wcdata WHERE wc=='FRLKMNFR'; -- FIXED: poor equipment code 'LDS' (data) [FRLKMNFRLDS]
---------------
UPDATE pooldata   SET switch='HSTNTXNAYLX' WHERE switch=='KATYTXXAJF1'; -- FIXED: poor equipment code 'JF1' (data) [713-969-0]
UPDATE prefixdata SET switch='HSTNTXNAYLX' WHERE switch=='KATYTXXAJF1'; -- FIXED: poor equipment code 'JF1' (data) [713-969]
UPDATE OR IGNORE switchdata SET switch='HSTNTXNAYLX' WHERE switch=='KATYTXXAJF1'; -- FIXED: poor equipment code 'JF1' (data) [KATYTXXAJF1]
DELETE FROM switchdata WHERE switch=='KATYTXXAJF1'; -- FIXED: poor equipment code 'JF1' (data) [KATYTXXAJF1]
UPDATE OR IGNORE wcdata SET wc='HSTNTXNA' WHERE wc=='KATYTXXA'; -- FIXED: poor equipment code 'JF1' (data) [KATYTXXAJF1]
DELETE FROM wcdata WHERE wc=='KATYTXXA'; -- FIXED: poor equipment code 'JF1' (data) [KATYTXXAJF1]
---------------
UPDATE prefixdata SET switch='WNDSCODCCM0' WHERE switch=='WNDSCOMAZ01'; -- FIXED: poor equipment code 'Z01' (data) [720-658]
UPDATE OR IGNORE switchdata SET switch='WNDSCODCCM0' WHERE switch=='WNDSCOMAZ01'; -- FIXED: poor equipment code 'Z01' (data) [WNDSCOMAZ01]
DELETE FROM switchdata WHERE switch=='WNDSCOMAZ01'; -- FIXED: poor equipment code 'Z01' (data) [WNDSCOMAZ01]
UPDATE OR IGNORE wcdata SET wc='WNDSCODC' WHERE wc=='WNDSCOMA'; -- FIXED: poor equipment code 'Z01' (data) [WNDSCOMAZ01]
DELETE FROM wcdata WHERE wc=='WNDSCOMA'; -- FIXED: poor equipment code 'Z01' (data) [WNDSCOMAZ01]
---------------
UPDATE prefixdata SET switch='SFLDMICTCM2' WHERE switch=='SFLDMIUCTCM'; -- FIXED: poor equipment code 'TCM' (data) [734-473]
UPDATE OR IGNORE switchdata SET switch='SFLDMICTCM2' WHERE switch=='SFLDMIUCTCM'; -- FIXED: poor equipment code 'TCM' (data) [SFLDMIUCTCM]
DELETE FROM switchdata WHERE switch=='SFLDMIUCTCM'; -- FIXED: poor equipment code 'TCM' (data) [SFLDMIUCTCM]
UPDATE OR IGNORE wcdata SET wc='SFLDMICT' WHERE wc=='SFLDMIUC'; -- FIXED: poor equipment code 'TCM' (data) [SFLDMIUCTCM]
DELETE FROM wcdata WHERE wc=='SFLDMIUC'; -- FIXED: poor equipment code 'TCM' (data) [SFLDMIUCTCM]
---------------
UPDATE prefixdata SET switch='SNDGCA023KD' WHERE switch=='SNDGCA023DK'; -- FIXED: poor equipment code '3DK' (data) [760-825]
UPDATE OR IGNORE switchdata SET switch='SNDGCA023KD' WHERE switch=='SNDGCA023DK'; -- FIXED: poor equipment code '3DK' (data) [SNDGCA023DK]
DELETE FROM switchdata WHERE switch=='SNDGCA023DK'; -- FIXED: poor equipment code '3DK' (data) [SNDGCA023DK]
---------------
UPDATE prefixdata SET switch='MPLSMNCD1KD' WHERE switch=='MPLSMNCD1KM'; -- FIXED: poor equipment code '1KM' (data) [763-286]
UPDATE OR IGNORE switchdata SET switch='MPLSMNCD1KD' WHERE switch=='MPLSMNCD1KM'; -- FIXED: poor equipment code '1KM' (data) [MPLSMNCD1KM]
DELETE FROM switchdata WHERE switch=='MPLSMNCD1KM'; -- FIXED: poor equipment code '1KM' (data) [MPLSMNCD1KM]
---------------
UPDATE prefixdata SET switch='WTWPIN01RL0' WHERE switch=='WTWPIN01RLO'; -- FIXED: poor equipment code 'RLO' (data) [765-445]
UPDATE OR IGNORE switchdata SET switch='WTWPIN01RL0' WHERE switch=='WTWPIN01RLO'; -- FIXED: poor equipment code 'RLO' (data) [WTWPIN01RLO]
DELETE FROM switchdata WHERE switch=='WTWPIN01RLO'; -- FIXED: poor equipment code 'RLO' (data) [WTWPIN01RLO]
---------------
UPDATE pooldata   SET switch='HAYSKSLD1MD' WHERE switch=='HAYSKASLD1M'; -- FIXED: poor equipment code 'D1M' (data) [785-513-0,785-626-2,785-626-7]
UPDATE prefixdata SET switch='HAYSKSLD1MD' WHERE switch=='HAYSKASLD1M'; -- FIXED: poor equipment code 'D1M' (data) [785-513]
UPDATE OR IGNORE switchdata SET switch='HAYSKSLD1MD' WHERE switch=='HAYSKASLD1M'; -- FIXED: poor equipment code 'D1M' (data) [HAYSKASLD1M]
DELETE FROM switchdata WHERE switch=='HAYSKASLD1M'; -- FIXED: poor equipment code 'D1M' (data) [HAYSKASLD1M]
UPDATE OR IGNORE wcdata SET wc='HAYSKSLD' WHERE wc=='HAYSKASL'; -- FIXED: poor equipment code 'D1M' (data) [HAYSKASLD1M]
DELETE FROM wcdata WHERE wc=='HAYSKASL'; -- FIXED: poor equipment code 'D1M' (data) [HAYSKASLD1M]
---------------
UPDATE prefixdata SET switch='WVCYUTCL1KD' WHERE switch=='WVCYUTCK1JD'; -- FIXED: poor equipment code '1JD' (data) [801-630]
UPDATE OR IGNORE switchdata SET switch='WVCYUTCL1KD' WHERE switch=='WVCYUTCK1JD'; -- FIXED: poor equipment code '1JD' (data) [WVCYUTCK1JD]
DELETE FROM switchdata WHERE switch=='WVCYUTCK1JD'; -- FIXED: poor equipment code '1JD' (data) [WVCYUTCK1JD]
UPDATE OR IGNORE wcdata SET wc='WVCYUTCL' WHERE wc=='WVCYUTCK'; -- FIXED: poor equipment code '1JD' (data) [WVCYUTCK1JD]
DELETE FROM wcdata WHERE wc=='WVCYUTCK'; -- FIXED: poor equipment code '1JD' (data) [WVCYUTCK1JD]
---------------
UPDATE pooldata   SET switch='CLMASCTSDC0' WHERE switch=='CLMASCTSDCO'; -- FIXED: poor equipment code 'DCO' (data) [803-298-2,803-821-6]
UPDATE OR IGNORE switchdata SET switch='CLMASCTSDC0' WHERE switch=='CLMASCTSDCO'; -- FIXED: poor equipment code 'DCO' (data) [CLMASCTSDCO]
DELETE FROM switchdata WHERE switch=='CLMASCTSDCO'; -- FIXED: poor equipment code 'DCO' (data) [CLMASCTSDCO]
---------------
UPDATE pooldata   SET switch='CLMASCSNXGY' WHERE switch=='CLMASCSNXG4'; -- FIXED: poor equipment code 'XG4' (data) [803-569-6]
UPDATE prefixdata SET switch='CLMASCSNXGY' WHERE switch=='CLMASCSNXG4'; -- FIXED: poor equipment code 'XG4' (data) [803-569]
UPDATE OR IGNORE switchdata SET switch='CLMASCSNXGY' WHERE switch=='CLMASCSNXG4'; -- FIXED: poor equipment code 'XG4' (data) [CLMASCSNXG4]
DELETE FROM switchdata WHERE switch=='CLMASCSNXG4'; -- FIXED: poor equipment code 'XG4' (data) [CLMASCSNXG4]
---------------
UPDATE prefixdata SET switch='AMRLTX02CG0' WHERE switch=='AMRLTX02CGO'; -- FIXED: poor equipment code 'CGO' (data) [806-640]
UPDATE OR IGNORE switchdata SET switch='AMRLTX02CG0' WHERE switch=='AMRLTX02CGO'; -- FIXED: poor equipment code 'CGO' (data) [AMRLTX02CGO]
DELETE FROM switchdata WHERE switch=='AMRLTX02CGO'; -- FIXED: poor equipment code 'CGO' (data) [AMRLTX02CGO]
---------------
UPDATE prefixdata SET switch='HCKRNCXAX6X' WHERE switch=='HCKRNCXAX6C'; -- FIXED: poor equipment code 'X6C' (data) [828-826]
UPDATE OR IGNORE switchdata SET switch='HCKRNCXAX6X' WHERE switch=='HCKRNCXAX6C'; -- FIXED: poor equipment code 'X6C' (data) [HCKRNCXAX6C]
DELETE FROM switchdata WHERE switch=='HCKRNCXAX6C'; -- FIXED: poor equipment code 'X6C' (data) [HCKRNCXAX6C]
---------------
UPDATE pooldata   SET switch='NWRKNJMDGMD' WHERE switch=='NWRKNJMDHC8'; -- FIXED: poor equipment code 'HC8' (data) [862-944-1]
UPDATE prefixdata SET switch='NWRKNJMDGMD' WHERE switch=='NWRKNJMDHC8'; -- FIXED: poor equipment code 'HC8' (data) [862-944]
UPDATE OR IGNORE switchdata SET switch='NWRKNJMDGMD' WHERE switch=='NWRKNJMDHC8'; -- FIXED: poor equipment code 'HC8' (data) [NWRKNJMDHC8]
DELETE FROM switchdata WHERE switch=='NWRKNJMDHC8'; -- FIXED: poor equipment code 'HC8' (data) [NWRKNJMDHC8]
---------------
UPDATE prefixdata SET switch='NWRKNJ41W10' WHERE switch=='NWRKNJ4IWI0'; -- FIXED: poor equipment code 'WI0' (data) [908-235]
UPDATE OR IGNORE switchdata SET switch='NWRKNJ41W10' WHERE switch=='NWRKNJ4IWI0'; -- FIXED: poor equipment code 'WI0' (data) [NWRKNJ4IWI0]
DELETE FROM switchdata WHERE switch=='NWRKNJ4IWI0'; -- FIXED: poor equipment code 'WI0' (data) [NWRKNJ4IWI0]
UPDATE OR IGNORE wcdata SET wc='NWRKNJ41' WHERE wc=='NWRKNJ4I'; -- FIXED: poor equipment code 'WI0' (data) [NWRKNJ4IWI0]
DELETE FROM wcdata WHERE wc=='NWRKNJ4I'; -- FIXED: poor equipment code 'WI0' (data) [NWRKNJ4IWI0]
---------------
UPDATE pooldata   SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [916-259-4]
UPDATE prefixdata SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [916-259]
UPDATE OR IGNORE switchdata SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [SCRMCA01XPT]
DELETE FROM switchdata WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [SCRMCA01XPT]
---------------
UPDATE prefixdata SET switch='SCRMCAAMDS0' WHERE switch=='SCRMCAAM0S0'; -- FIXED: poor equipment code '0S0' (data) [916-493]
UPDATE OR IGNORE switchdata SET switch='SCRMCAAMDS0' WHERE switch=='SCRMCAAM0S0'; -- FIXED: poor equipment code '0S0' (data) [SCRMCAAM0S0]
DELETE FROM switchdata WHERE switch=='SCRMCAAM0S0'; -- FIXED: poor equipment code '0S0' (data) [SCRMCAAM0S0]
---------------
UPDATE prefixdata SET switch='GRVEOKMARL0' WHERE switch=='GRVEOKMARLC'; -- FIXED: poor equipment code 'RLC' (data) [918-791]
UPDATE OR IGNORE switchdata SET switch='GRVEOKMARL0' WHERE switch=='GRVEOKMARLC'; -- FIXED: poor equipment code 'RLC' (data) [GRVEOKMARLC]
DELETE FROM switchdata WHERE switch=='GRVEOKMARLC'; -- FIXED: poor equipment code 'RLC' (data) [GRVEOKMARLC]
---------------
UPDATE pooldata   SET switch='PHNXAZMACM6' WHERE switch=='PHNXAZMACCM'; -- FIXED: poor equipment code 'CCM' (data) [928-363-0]
UPDATE prefixdata SET switch='PHNXAZMACM6' WHERE switch=='PHNXAZMACCM'; -- FIXED: poor equipment code 'CCM' (data) [928-363]
UPDATE OR IGNORE switchdata SET switch='PHNXAZMACM6' WHERE switch=='PHNXAZMACCM'; -- FIXED: poor equipment code 'CCM' (data) [PHNXAZMACCM]
DELETE FROM switchdata WHERE switch=='PHNXAZMACCM'; -- FIXED: poor equipment code 'CCM' (data) [PHNXAZMACCM]
---------------
UPDATE prefixdata SET switch='WNHNFLBUCM1' WHERE switch=='WNHNFLBUVM1'; -- FIXED: poor equipment code 'VM1' (data) [941-809]
UPDATE OR IGNORE switchdata SET switch='WNHNFLBUCM1' WHERE switch=='WNHNFLBUVM1'; -- FIXED: poor equipment code 'VM1' (data) [WNHNFLBUVM1]
DELETE FROM switchdata WHERE switch=='WNHNFLBUVM1'; -- FIXED: poor equipment code 'VM1' (data) [WNHNFLBUVM1]
---------------
UPDATE pooldata   SET switch='HRLNTXHGX4X' WHERE switch=='HRLNTXHGIX4'; -- FIXED: poor equipment code 'IX4' (data) [956-336-0,956-336-1,956-336-2]
UPDATE prefixdata SET switch='HRLNTXHGX4X' WHERE switch=='HRLNTXHGIX4'; -- FIXED: poor equipment code 'IX4' (data) [956-336]
UPDATE OR IGNORE switchdata SET switch='HRLNTXHGX4X' WHERE switch=='HRLNTXHGIX4'; -- FIXED: poor equipment code 'IX4' (data) [HRLNTXHGIX4]
DELETE FROM switchdata WHERE switch=='HRLNTXHGIX4'; -- FIXED: poor equipment code 'IX4' (data) [HRLNTXHGIX4]
---------------
UPDATE prefixdata SET switch='SANGTXXCCM0' WHERE switch=='SANGTXXCCCM'; -- FIXED: poor equipment code 'CCM' (data) [979-295,979-322]
UPDATE OR IGNORE switchdata SET switch='SANGTXXCCM0' WHERE switch=='SANGTXXCCCM'; -- FIXED: poor equipment code 'CCM' (data) [SANGTXXCCCM]
DELETE FROM switchdata WHERE switch=='SANGTXXCCCM'; -- FIXED: poor equipment code 'CCM' (data) [SANGTXXCCCM]
---------------
UPDATE prefixdata SET switch='AMRLTX9843Z' WHERE switch=='AMRLTX92WSM'; -- FIXED: poor equipment code 'WSM' (ping) [806-344]
UPDATE OR IGNORE switchdata SET switch='AMRLTX9843Z' WHERE switch=='AMRLTX92WSM'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
DELETE FROM switchdata WHERE switch=='AMRLTX92WSM'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
UPDATE OR IGNORE wcdata SET wc='AMRLTX98' WHERE wc=='AMRLTX92'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
DELETE FROM wcdata WHERE wc=='AMRLTX92'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
---------------------
UPDATE prefixdata SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP'; -- FIXED: poor equipment code 'WTP' (206) [206-420]
UPDATE OR IGNORE switchdata SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP'; -- FIXED: poor equipment code 'WTP' (206,STTLWAWBWTP) [STTLWAWBWTP]
DELETE FROM switchdata WHERE switch=='STTLWAWBWTP'; -- FIXED: poor equipment code 'WTP' (206,STTLWAWBWTP) [STTLWAWBWTP]
---------------
-- UPDATE prefixdata SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS'; -- FIXME: poor equipment code 'DDS' (213295) [213-295]
-- UPDATE switchdata SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS'; -- FIXME: poor equipment code 'DDS' (213295) [LSANCARCDDS]
---------------------
-- UPDATE pooldata   SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB'; -- FIXME: poor equipment code 'PSB' (2144079,6827388) [214-407-9,682-738-8]
-- UPDATE prefixdata SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB'; -- FIXME: poor equipment code 'PSB' (214785,2147852) [214-785]
-- UPDATE switchdata SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB'; -- FIXME: poor equipment code 'PSB' (2144079,214785,6827388) [IRNGTXXBPSB]
---------------------
-- UPDATE prefixdata SET switch='DNVRCO26WBA' WHERE switch=='DNVRCO26WBA'; -- FIXME: poor equipment code 'WBA' (303) [303-330]
-- UPDATE switchdata SET switch='DNVRCO26WBA' WHERE switch=='DNVRCO26WBA'; -- FIXME: poor equipment code 'WBA' (303,DNVRCO26WBA) [DNVRCO26WBA]
---------------------
-- UPDATE prefixdata SET switch='HLNQSK01CTA' WHERE switch=='HLNQSK01CTA'; -- FIXME: poor equipment code 'CTA' (306553) [306-553]
-- UPDATE switchdata SET switch='HLNQSK01CTA' WHERE switch=='HLNQSK01CTA'; -- FIXME: poor equipment code 'CTA' (306553) [HLNQSK01CTA]
---------------------
-- UPDATE prefixdata SET switch='WEBBSK02CTA' WHERE switch=='WEBBSK02CTA'; -- FIXME: poor equipment code 'CTA' (306674) [306-674]
-- UPDATE switchdata SET switch='WEBBSK02CTA' WHERE switch=='WEBBSK02CTA'; -- FIXME: poor equipment code 'CTA' (306674) [WEBBSK02CTA]
---------------------
UPDATE prefixdata SET switch='GNBRARXAPS0' WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [501-287]
UPDATE OR IGNORE switchdata SET switch='GNBRARXAPS0' WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
DELETE FROM switchdata WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
UPDATE OR IGNORE wcdata SET wc='GNBRARXA' WHERE wc=='LTRKARTL'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
DELETE FROM wcdata WHERE wc=='LTRKARTL'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
---------------------
-- UPDATE prefixdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724) [724-914]
-- UPDATE switchdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724,WASHPAWAHPT) [WASHPAWAHPT]
---------------------
UPDATE pooldata   SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [760-592-5]
UPDATE prefixdata SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [760-592]
UPDATE OR IGNORE switchdata SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [SNDGCA02W0P]
DELETE FROM switchdata WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [SNDGCA02W0P]
---------------------
-- UPDATE prefixdata SET switch='NYCMNYZRDFS' WHERE switch=='NYCMNYZRDFS'; -- FIXME: poor equipment code 'DFS' (917268) [917-268]
-- UPDATE switchdata SET switch='NYCMNYZRDFS' WHERE switch=='NYCMNYZRDFS'; -- FIXME: poor equipment code 'DFS' (917268) [NYCMNYZRDFS]
---------------------
