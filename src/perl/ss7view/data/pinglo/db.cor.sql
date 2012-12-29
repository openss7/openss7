--===================================
UPDATE ocndata    SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE pcdata     SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE rcdata     SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE rndata     SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE exdata     SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE lcadata    SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE wcdata     SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE switchdata SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE npadata    SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE prefixdata SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE pooldata   SET sect='ping' WHERE sect NOT NULL and sect!='ping';
UPDATE linedata   SET sect='ping' WHERE sect NOT NULL and sect!='ping';
--===================================
--===============================================
UPDATE prefixdata SET switch=NULL WHERE switch=='XXXXXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (201,202,203,204,205,206,207,208,209,210,...) [201-201,201-211,201-260,201-311,201-411,201-511,201-550,201-554,201-555,201-611,...]
DELETE FROM switchdata WHERE switch=='XXXXXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (201,202,203,204,205,206,207,208,209,210,...) [XXXXXXXXXXX]
DELETE FROM wcdata WHERE wc=='XXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (201,202,203,204,205,206,207,208,209,210,...)
---------------------
UPDATE prefixdata SET switch=NULL WHERE switch=='__VARIOUS__'; -- FIXED: invalid clli '__VARIOUS__' (203,206,303,304,310,323,360,364,385,408,...) [203-976,206-976,303-976,304-950,304-976,310-976,323-976,360-976,364-555,385-211,...]
DELETE FROM switchdata WHERE switch=='__VARIOUS__'; -- FIXED: invalid clli '__VARIOUS__' (203,206,303,304,310,323,360,364,385,408,...) [__VARIOUS__]
DELETE FROM wcdata WHERE wc=='__VARIOU'; -- FIXED: invalid clli '__VARIOUS__' (203,206,303,304,310,323,360,364,385,408,...)
---------------------
UPDATE prefixdata SET switch=NULL WHERE npa=='500' OR npa=='710' OR npa=='900'; -- FIXED: invalid clli
DELETE from switchdata WHERE switch LIKE 'ASGNPCS%'; -- FIXED: invalid clli
DELETE from switchdata WHERE switch=='RTGETSAGENT'; -- FIXED: invalid clli
DELETE from switchdata WHERE switch LIKE 'ASGNIC%'; -- FIXED: invalid clli
DELETE from wcdata WHERE wc LIKE 'ASGNPCS%'; -- FIXED: invalid clli
DELETE from wcdata WHERE wc=='RTGETSAG'; -- FIXED: invalid clli
DELETE from wcdata WHERE wc LIKE 'ASGNIC%'; -- FIXED: invalid clli
--===============================================
UPDATE prefixdata SET switch='NYCMNY50DS2' WHERE switch=='NYCMNYWSTL1'; -- FIXED: poor equipment code 'TL1' (212) [212-347]
UPDATE OR IGNORE switchdata SET switch='NYCMNY50DS2' WHERE switch=='NYCMNYWSTL1'; -- FIXED: poor equipment code 'TL1' (212) [NYCMNYWSTL1]
DELETE FROM switchdata WHERE switch=='NYCMNYWSTL1'; -- FIXED: poor equipment code 'TL1' (212) [NYCMNYWSTL1]
UPDATE OR IGNORE wcdata SET wc='NYCMNY50' WHERE wc=='NYCMNYWS'; -- FIXED: poor equipment code 'TL1' (212) [NYCMNYWSTL1]
DELETE FROM wcdata WHERE wc=='NYCMNYWS'; -- FIXED: poor equipment code 'TL1' (212) [NYCMNYWSTL1]
---------------------
UPDATE prefixdata SET switch='HLTPTXWICM0' WHERE switch=='SGLDTXAQWWM'; -- FIXED: poor equipment code 'WWM' (281,713) [281-665,713-310,713-519]
UPDATE OR IGNORE switchdata SET switch='HLTPTXWICM0' WHERE switch=='SGLDTXAQWWM'; -- FIXED: poor equipment code 'WWM' (281,713) [SGLDTXAQWWM]
DELETE FROM switchdata WHERE switch=='SGLDTXAQWWM'; -- FIXED: poor equipment code 'WWM' (281,713) [SGLDTXAQWWM]
UPDATE OR IGNORE wcdata SET wc='HLTPTXWI' WHERE wc=='SGLDTXAQ'; -- FIXED: poor equipment code 'WWM' (281,713) [SGLDTXAQWWM]
DELETE FROM wcdata WHERE wc=='SGLDTXAQ'; -- FIXED: poor equipment code 'WWM' (281,713) [SGLDTXAQWWM]
---------------------
UPDATE prefixdata SET switch='MTGMAL13X1X' WHERE switch=='MTGMALMTW6C'; -- FIXED: poor equipment code 'W6C' (334) [334-744,334-755,334-757,334-759]
UPDATE OR IGNORE switchdata SET switch='MTGMAL13X1X' WHERE switch=='MTGMALMTW6C'; -- FIXED: poor equipment code 'W6C' (334) [MTGMALMTW6C]
DELETE FROM switchdata WHERE switch=='MTGMALMTW6C'; -- FIXED: poor equipment code 'W6C' (334) [MTGMALMTW6C]
---------------------
UPDATE prefixdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (601) [601-216]
UPDATE OR IGNORE switchdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (601) [MTOLMSABAND]
DELETE FROM switchdata WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (601) [MTOLMSABAND]
---------------------
UPDATE prefixdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (601) [601-233]
UPDATE OR IGNORE switchdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (601) [MCCMMSBLAMB]
DELETE FROM switchdata WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (601) [MCCMMSBLAMB]
---------------------
UPDATE prefixdata SET switch='BKFDCAKT1KD' WHERE switch=='BKFDCAKTIKD'; -- FIXED: poor equipment code 'IKD' (805) [805-840]
UPDATE OR IGNORE switchdata SET switch='BKFDCAKT1KD' WHERE switch=='BKFDCAKTIKD'; -- FIXED: poor equipment code 'IKD' (805) [BKFDCAKTIKD]
DELETE FROM switchdata WHERE switch=='BKFDCAKTIKD'; -- FIXED: poor equipment code 'IKD' (805) [BKFDCAKTIKD]
---------------------
UPDATE prefixdata SET switch='EVVLIN030MD' WHERE switch=='CLMBINAKWM1'; -- FIXED: poor equipment code 'WM1' (812) [812-370]
UPDATE OR IGNORE switchdata SET switch='EVVLIN030MD' WHERE switch=='CLMBINAKWM1'; -- FIXED: poor equipment code 'WM1' (812) [CLMBINAKWM1]
DELETE FROM switchdata WHERE switch=='CLMBINAKWM1'; -- FIXED: poor equipment code 'WM1' (812) [CLMBINAKWM1]
---------------------
-- UPDATE prefixdata SET switch='NYCMNYWSTL2' WHERE switch=='NYCMNYWSTL2'; -- FIXME: poor equipment code 'TL2' (ping) [212-470,917-310]
-- UPDATE switchdata SET switch='NYCMNYWSTL2' WHERE switch=='NYCMNYWSTL2'; -- FIXME: poor equipment code 'TL2' (ping) [NYCMNYWSTL2]
---------------------
-- UPDATE pooldata   SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB'; -- FIXME: poor equipment code 'PSB' (2144079,6827388) [214-407-9,682-738-8]
-- UPDATE prefixdata SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB'; -- FIXME: poor equipment code 'PSB' (214785,2147852) [214-785]
-- UPDATE switchdata SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB'; -- FIXME: poor equipment code 'PSB' (2144079,214785,6827388) [IRNGTXXBPSB]
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
-- UPDATE prefixdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724914) [724-914]
-- UPDATE switchdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724914) [WASHPAWAHPT]
---------------------
UPDATE prefixdata SET switch='AMRLTX9843Z' WHERE switch=='AMRLTX92WSM'; -- FIXED: poor equipment code 'WSM' (ping) [806-344]
UPDATE OR IGNORE switchdata SET switch='AMRLTX9843Z' WHERE switch=='AMRLTX92WSM'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
DELETE FROM switchdata WHERE switch=='AMRLTX92WSM'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
UPDATE OR IGNORE wcdata SET wc='AMRLTX98' WHERE wc=='AMRLTX92'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
DELETE FROM wcdata WHERE wc=='AMRLTX92'; -- FIXED: poor equipment code 'WSM' (ping) [AMRLTX92WSM]
--===============================================
--===============================================
