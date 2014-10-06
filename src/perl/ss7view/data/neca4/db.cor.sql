--==========================================================================
UPDATE switchdata SET wcv='6641' WHERE switch=='LWTWMO01RL2';
UPDATE wcdata SET wcv='6641' WHERE wc=='LWTWMO01';
--==========================================================================
UPDATE pooldata   SET switch='SXCTIAXO1MD' WHERE switch=='SXCTIZXO1MD'; -- FIXED: invalid state/province/territory 'IZ' (27-100) [712-463-1]
UPDATE OR IGNORE switchdata SET switch='SXCTIAXO1MD' WHERE switch=='SXCTIZXO1MD'; -- FIXED: invalid state/province/territory 'IZ' (27-100) [SXCTIZXO1MD]
DELETE FROM switchdata WHERE switch=='SXCTIZXO1MD'; -- FIXED: invalid state/province/territory 'IZ' (27-100) [SXCTIZXO1MD]
UPDATE OR IGNORE wcdata SET wc='SXCTIAXO' WHERE wc=='SXCTIZXO'; -- FIXED: invalid state/province/territory 'IZ' (27-100)
DELETE FROM wcdata WHERE wc=='SXCTIZXO'; -- FIXED: invalid state/province/territory 'IZ' (27-100)
---------------------
-- UPDATE switchdata SET switch='H109NCXARS0' WHERE switch=='H109NCXARS0'; -- FIXME: invalid clli 'H109NCXARS0' (45-90) [H109NCXARS0]
-- UPDATE wcdata SET wc='H109NCXA' WHERE wc=='H109NCXA'; -- FIXME: invalid clli 'H109NCXARS0' (45-90)
---------------------
-- UPDATE switchdata SET switch='S162NDXARS2' WHERE switch=='S162NDXARS2'; -- FIXME: invalid clli 'S162NDXARS2' (46-49) [S162NDXARS2]
-- UPDATE wcdata SET wc='S162NDXA' WHERE wc=='S162NDXA'; -- FIXME: invalid clli 'S162NDXARS2' (46-49)
---------------------
-- UPDATE switchdata SET switch='H151SCXARS0' WHERE switch=='H151SCXARS0'; -- FIXME: invalid clli 'H151SCXARS0' (53-45) [H151SCXARS0]
-- UPDATE wcdata SET wc='H151SCXA' WHERE wc=='H151SCXA'; -- FIXME: invalid clli 'H151SCXARS0' (53-45)
---------------------
-- UPDATE switchdata SET switch='H9CHSCXARS0' WHERE switch=='H9CHSCXARS0'; -- FIXME: invalid clli 'H9CHSCXARS0' (53-45) [H9CHSCXARS0]
-- UPDATE wcdata SET wc='H9CHSCXA' WHERE wc=='H9CHSCXA'; -- FIXME: invalid clli 'H9CHSCXARS0' (53-45)
---------------------
-- UPDATE switchdata SET switch='H9PGSCXARS0' WHERE switch=='H9PGSCXARS0'; -- FIXME: invalid clli 'H9PGSCXARS0' (53-46) [H9PGSCXARS0]
-- UPDATE wcdata SET wc='H9PGSCXA' WHERE wc=='H9PGSCXA'; -- FIXME: invalid clli 'H9PGSCXARS0' (53-46)
--==========================================================================
UPDATE pooldata   SET switch='STCDMNTOXUX' WHERE switch=='STCDMITOXUX'; -- FIXED
UPDATE OR IGNORE switchdata SET switch='STCDMNTOXUX' WHERE switch=='STCDMITOXUX'; -- FIXED
DELETE FROM switchdata WHERE switch=='STCDMITOXUX'; -- FIXED
UPDATE OR IGNORE wcdata SET wc='STCDMNTO' WHERE wc=='STCDMITO'; -- FIXED
DELETE FROM wcdata WHERE wc=='STCDMITO'; -- FIXED
--========================================================================
UPDATE pooldata   SET switch='THTNGAXAPS0' WHERE switch=='THTNGAXAPSO'; -- FIXED: poor equipment code 'PSO' (22-96) [706-938-8]
UPDATE OR IGNORE switchdata SET switch='THTNGAXAPS0' WHERE switch=='THTNGAXAPSO'; -- FIXED: poor equipment code 'PSO' (22-96) [THTNGAXAPSO]
DELETE FROM switchdata WHERE switch=='THTNGAXAPSO'; -- FIXED: poor equipment code 'PSO' (22-96) [THTNGAXAPSO]
---------------------
UPDATE pooldata   SET switch='BLTNILXDPS0' WHERE switch=='BLTNILXDPSO'; -- FIXED: poor equipment code 'PSO' (25-13) [309-831-0]
UPDATE OR IGNORE switchdata SET switch='BLTNILXDPS0' WHERE switch=='BLTNILXDPSO'; -- FIXED: poor equipment code 'PSO' (25-13) [BLTNILXDPSO]
DELETE FROM switchdata WHERE switch=='BLTNILXDPSO'; -- FIXED: poor equipment code 'PSO' (25-13) [BLTNILXDPSO]
---------------------
UPDATE prefixdata SET switch='WTWPIN01RL0' WHERE switch=='WTWPIN01RLO'; -- FIXED: poor equipment code 'RLO' (26-54) [765-445]
UPDATE OR IGNORE switchdata SET switch='WTWPIN01RL0' WHERE switch=='WTWPIN01RLO'; -- FIXED: poor equipment code 'RLO' (26-54) [WTWPIN01RLO]
DELETE FROM switchdata WHERE switch=='WTWPIN01RLO'; -- FIXED: poor equipment code 'RLO' (26-54) [WTWPIN01RLO]
---------------------
UPDATE prefixdata SET switch='SPNCIAMURL0' WHERE switch=='SPNCIAMURLO'; -- FIXED: poor equipment code 'RLO' (27-3) [515-395]
UPDATE OR IGNORE switchdata SET switch='SPNCIAMURL0' WHERE switch=='SPNCIAMURLO'; -- FIXED: poor equipment code 'RLO' (27-3) [SPNCIAMURLO]
DELETE FROM switchdata WHERE switch=='SPNCIAMURLO'; -- FIXED: poor equipment code 'RLO' (27-3) [SPNCIAMURLO]
---------------------
UPDATE pooldata   SET switch='INDNIABF1MD' WHERE switch=='INDNIABFELD'; -- FIXED: poor equipment code 'ELD' (27-54) [515-962-5]
UPDATE OR IGNORE switchdata SET switch='INDNIABF1MD' WHERE switch=='INDNIABFELD'; -- FIXED: poor equipment code 'ELD' (27-54) [INDNIABFELD]
DELETE FROM switchdata WHERE switch=='INDNIABFELD'; -- FIXED: poor equipment code 'ELD' (27-54) [INDNIABFELD]
---------------------
UPDATE pooldata   SET switch='BEREKYXAPS0' WHERE switch=='BEREKYXAPSO'; -- FIXED: poor equipment code 'PSO' (29-4) [859-228-3]
UPDATE OR IGNORE switchdata SET switch='BEREKYXAPS0' WHERE switch=='BEREKYXAPSO'; -- FIXED: poor equipment code 'PSO' (29-4) [BEREKYXAPSO]
DELETE FROM switchdata WHERE switch=='BEREKYXAPSO'; -- FIXED: poor equipment code 'PSO' (29-4) [BEREKYXAPSO]
---------------------
UPDATE pooldata   SET switch='JMTWNYXAPS0' WHERE switch=='JMTWNYXAPSO'; -- FIXED: poor equipment code 'PSO' (44-71) [716-485-9]
UPDATE OR IGNORE switchdata SET switch='JMTWNYXAPS0' WHERE switch=='JMTWNYXAPSO'; -- FIXED: poor equipment code 'PSO' (44-71) [JMTWNYXAPSO]
DELETE FROM switchdata WHERE switch=='JMTWNYXAPSO'; -- FIXED: poor equipment code 'PSO' (44-71) [JMTWNYXAPSO]
---------------------
UPDATE pooldata   SET switch='WWFDNYXAPS0' WHERE switch=='WWFDNYXAPSO'; -- FIXED: poor equipment code 'PSO' (44-207) [315-972-0]
UPDATE OR IGNORE switchdata SET switch='WWFDNYXAPS0' WHERE switch=='WWFDNYXAPSO'; -- FIXED: poor equipment code 'PSO' (44-207) [WWFDNYXAPSO]
DELETE FROM switchdata WHERE switch=='WWFDNYXAPSO'; -- FIXED: poor equipment code 'PSO' (44-207) [WWFDNYXAPSO]
---------------------
UPDATE pooldata   SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (47-98) [234-284-0]
UPDATE OR IGNORE switchdata SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (47-98) [HDSNOHXAPSO]
DELETE FROM switchdata WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (47-98) [HDSNOHXAPSO]
---------------------
UPDATE pooldata   SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (47-155) [513-480-0]
UPDATE prefixdata SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (47-155) [513-229]
UPDATE OR IGNORE switchdata SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (47-155) [RSMYOHROWV1]
DELETE FROM switchdata WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (47-155) [RSMYOHROWV1]
---------------------
UPDATE pooldata   SET switch='ELDRPAXEPS0' WHERE switch=='ELDRPAXEPSO'; -- FIXED: poor equipment code 'PSO' (50-44) [724-354-7]
UPDATE OR IGNORE switchdata SET switch='ELDRPAXEPS0' WHERE switch=='ELDRPAXEPSO'; -- FIXED: poor equipment code 'PSO' (50-44) [ELDRPAXEPSO]
DELETE FROM switchdata WHERE switch=='ELDRPAXEPSO'; -- FIXED: poor equipment code 'PSO' (50-44) [ELDRPAXEPSO]
---------------------
UPDATE pooldata   SET switch='EPHRPAXLPS0' WHERE switch=='EPHRPAXLPSO'; -- FIXED: poor equipment code 'PSO' (50-46) [717-863-2]
UPDATE OR IGNORE switchdata SET switch='EPHRPAXLPS0' WHERE switch=='EPHRPAXLPSO'; -- FIXED: poor equipment code 'PSO' (50-46) [EPHRPAXLPSO]
DELETE FROM switchdata WHERE switch=='EPHRPAXLPSO'; -- FIXED: poor equipment code 'PSO' (50-46) [EPHRPAXLPSO]
---------------------
UPDATE pooldata   SET switch='LWBGPAXLPS0' WHERE switch=='LWBGPAXLPSO'; -- FIXED: poor equipment code 'PSO' (50-91) [570-551-2]
UPDATE OR IGNORE switchdata SET switch='LWBGPAXLPS0' WHERE switch=='LWBGPAXLPSO'; -- FIXED: poor equipment code 'PSO' (50-91) [LWBGPAXLPSO]
DELETE FROM switchdata WHERE switch=='LWBGPAXLPSO'; -- FIXED: poor equipment code 'PSO' (50-91) [LWBGPAXLPSO]
---------------------
UPDATE pooldata   SET switch='CLMASCTSDC0' WHERE switch=='CLMASCTSDCO'; -- FIXED: poor equipment code 'DCO' (53-23) [803-298-2,803-821-6,803-978-0,803-978-1,803-978-2,803-978-5,803-978-6,803-978-7,803-978-9]
UPDATE OR IGNORE switchdata SET switch='CLMASCTSDC0' WHERE switch=='CLMASCTSDCO'; -- FIXED: poor equipment code 'DCO' (53-23) [CLMASCTSDCO]
DELETE FROM switchdata WHERE switch=='CLMASCTSDCO'; -- FIXED: poor equipment code 'DCO' (53-23) [CLMASCTSDCO]
---------------------
UPDATE pooldata   SET switch='NWMDWIXAPOI' WHERE switch=='NWMDWIXAPOI'; -- FIXME: poor equipment code 'POI' (63-82) [715-248-5,715-248-6,715-907-4,715-907-5]
UPDATE switchdata SET switch='NWMDWIXAPOI' WHERE switch=='NWMDWIXAPOI'; -- FIXME: poor equipment code 'POI' (63-82) [NWMDWIXAPOI]
--==========================================================================
--========================================================================
UPDATE OR IGNORE switchdata SET switch='BRHMALHW0GT' WHERE switch=='BRHMALHWOGT'; -- FIXED: poor equipment code 'OGT' (12-12) [BRHMALHWOGT]
DELETE FROM switchdata WHERE switch=='BRHMALHWOGT'; -- FIXED: poor equipment code 'OGT' (12-12) [BRHMALHWOGT]
---------------------
UPDATE OR IGNORE switchdata SET switch='SNANCACZDS0' WHERE switch=='SNANCACZDZ0'; -- FIXED: poor equipment code 'DZ0' (16-339) [SNANCACZDZ0]
DELETE FROM switchdata WHERE switch=='SNANCACZDZ0'; -- FIXED: poor equipment code 'DZ0' (16-339) [SNANCACZDZ0]
---------------------
UPDATE OR IGNORE switchdata SET switch='WSCRCABYDS1' WHERE switch=='WSCRCABYESS'; -- FIXED: poor equipment code 'ESS' (16-389) [WSCRCABYESS]
DELETE FROM switchdata WHERE switch=='WSCRCABYESS'; -- FIXED: poor equipment code 'ESS' (16-389) [WSCRCABYESS]
---------------------
DELETE FROM switchdata WHERE switch=='PHLAPAFG5ES'; -- FIXME: poor equipment code '5ES' (19-3) [PHLAPAFG5ES]
DELETE FROM switchdata WHERE switch=='MIAMFLAEDMS'; -- FIXME: poor equipment code 'DMS' (21-1) [MIAMFLAEDMS]
DELETE FROM switchdata WHERE switch=='NDADFLACDMS'; -- FIXME: poor equipment code 'DMS' (21-3) [NDADFLACDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLARDMS'; -- FIXME: poor equipment code 'DMS' (21-4) [JCVLFLARDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLAPDMS'; -- FIXME: poor equipment code 'DMS' (21-5) [ORLDFLAPDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLBADMS'; -- FIXME: poor equipment code 'DMS' (21-6) [MIAMFLBADMS]
DELETE FROM switchdata WHERE switch=='MIAMFLBRDMS'; -- FIXME: poor equipment code 'DMS' (21-6) [MIAMFLBRDMS]
DELETE FROM switchdata WHERE switch=='NDADFLBRDMS'; -- FIXME: poor equipment code 'DMS' (21-12) [NDADFLBRDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLCADMS'; -- FIXME: poor equipment code 'DMS' (21-14) [MIAMFLCADMS]
DELETE FROM switchdata WHERE switch=='JCVLFLCLDMS'; -- FIXME: poor equipment code 'DMS' (21-18) [JCVLFLCLDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLCLDMS'; -- FIXME: poor equipment code 'DMS' (21-22) [ORLDFLCLDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLCRDMS'; -- FIXME: poor equipment code 'DMS' (21-22) [FTLDFLCRDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLCYDMS'; -- FIXME: poor equipment code 'DMS' (21-24) [FTLDFLCYDMS]
DELETE FROM switchdata WHERE switch=='DRBHFLMADMS'; -- FIXME: poor equipment code 'DMS' (21-29) [DRBHFLMADMS]
DELETE FROM switchdata WHERE switch=='JCVLFLFCDMS'; -- FIXME: poor equipment code 'DMS' (21-36) [JCVLFLFCDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLGRDMS'; -- FIXME: poor equipment code 'DMS' (21-56) [MIAMFLGRDMS]
DELETE FROM switchdata WHERE switch=='HLWDFLHADMS'; -- FIXME: poor equipment code 'DMS' (21-58) [HLWDFLHADMS]
DELETE FROM switchdata WHERE switch=='MIAMFLHLDMS'; -- FIXME: poor equipment code 'DMS' (21-59) [MIAMFLHLDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLICDMS'; -- FIXME: poor equipment code 'DMS' (21-64) [MIAMFLICDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLMSDMS'; -- FIXME: poor equipment code 'DMS' (21-73) [JCVLFLMSDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLLEDMS'; -- FIXME: poor equipment code 'DMS' (21-87) [WPBHFLLEDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLMRDMS'; -- FIXME: poor equipment code 'DMS' (21-91) [FTLDFLMRDMS]
DELETE FROM switchdata WHERE switch=='PMBHFLMADMS'; -- FIXME: poor equipment code 'DMS' (21-95) [PMBHFLMADMS]
DELETE FROM switchdata WHERE switch=='MIAMFLNMDMS'; -- FIXME: poor equipment code 'DMS' (21-130) [MIAMFLNMDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLOADMS'; -- FIXME: poor equipment code 'DMS' (21-131) [FTLDFLOADMS]
DELETE FROM switchdata WHERE switch=='ORLDFLMADMS'; -- FIXME: poor equipment code 'DMS' (21-140) [ORLDFLMADMS]
DELETE FROM switchdata WHERE switch=='MIAMFLPLDMS'; -- FIXME: poor equipment code 'DMS' (21-150) [MIAMFLPLDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLPCDMS'; -- FIXME: poor equipment code 'DMS' (21-157) [ORLDFLPCDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLPHDMS'; -- FIXME: poor equipment code 'DMS' (21-158) [ORLDFLPHDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLPLDMS'; -- FIXME: poor equipment code 'DMS' (21-158) [FTLDFLPLDMS]
DELETE FROM switchdata WHERE switch=='PMBHFLFEDMS'; -- FIXME: poor equipment code 'DMS' (21-162) [PMBHFLFEDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLRRDMS'; -- FIXME: poor equipment code 'DMS' (21-165) [MIAMFLRRDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLRVDMS'; -- FIXME: poor equipment code 'DMS' (21-165) [JCVLFLRVDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLRBDMS'; -- FIXME: poor equipment code 'DMS' (21-165) [WPBHFLRBDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLSJDMS'; -- FIXME: poor equipment code 'DMS' (21-171) [JCVLFLSJDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLSADMS'; -- FIXME: poor equipment code 'DMS' (21-171) [ORLDFLSADMS]
DELETE FROM switchdata WHERE switch=='MIAMFLSODMS'; -- FIXME: poor equipment code 'DMS' (21-176) [MIAMFLSODMS]
DELETE FROM switchdata WHERE switch=='HLWDFLWHDMS'; -- FIXME: poor equipment code 'DMS' (21-208) [HLWDFLWHDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLWMDMS'; -- FIXME: poor equipment code 'DMS' (21-208) [MIAMFLWMDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLANDMS'; -- FIXME: poor equipment code 'DMS' (21-208) [WPBHFLANDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLGRDMS'; -- FIXME: poor equipment code 'DMS' (21-214) [WPBHFLGRDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLGADMS'; -- FIXME: poor equipment code 'DMS' (21-214) [WPBHFLGADMS]
DELETE FROM switchdata WHERE switch=='WPBHFLHHDMS'; -- FIXME: poor equipment code 'DMS' (21-214) [WPBHFLHHDMS]
--==========================================================
DELETE FROM switchdata WHERE switch=='PHNXAZNO0W1'; -- FIXME: poor equipment code '0W1' (14-42) [PHNXAZNO0W1]
DELETE FROM switchdata WHERE switch=='LTRKARCAHB1'; -- FIXME: poor equipment code 'HB1' (15-33) [LTRKARCAHB1]
DELETE FROM switchdata WHERE switch=='LTRKARFRHD1'; -- FIXME: poor equipment code 'HD1' (15-34) [LTRKARFRHD1]
DELETE FROM switchdata WHERE switch=='LTRKARMOHB1'; -- FIXME: poor equipment code 'HB1' (15-36) [LTRKARMOHB1]
DELETE FROM switchdata WHERE switch=='FTLDFLPLG1W'; -- FIXME: poor equipment code 'G1W' (21-41) [FTLDFLPLG1W]
DELETE FROM switchdata WHERE switch=='ATLNGACSBC2'; -- FIXME: poor equipment code 'BC2' (22-13) [ATLNGACSBC2]
DELETE FROM switchdata WHERE switch=='BOISIDMAKXX'; -- FIXME: poor equipment code 'KXX' (24-4) [BOISIDMAKXX]
DELETE FROM switchdata WHERE switch=='IDFLIDMAKXX'; -- FIXME: poor equipment code 'KXX' (24-15) [IDFLIDMAKXX]
DELETE FROM switchdata WHERE switch=='PCTLIDMAKXX'; -- FIXME: poor equipment code 'KXX' (24-24) [PCTLIDMAKXX]
DELETE FROM switchdata WHERE switch=='TWFLIDMAKXX'; -- FIXME: poor equipment code 'KXX' (24-30) [TWFLIDMAKXX]
DELETE FROM switchdata WHERE switch=='CHCGILCLTDS'; -- FIXME: poor equipment code 'TDS' (25-24) [CHCGILCLTDS]
DELETE FROM switchdata WHERE switch=='EGVGILEGTDS'; -- FIXME: poor equipment code 'TDS' (25-74) [EGVGILEGTDS]
DELETE FROM switchdata WHERE switch=='HNDLILHNTDS'; -- FIXME: poor equipment code 'TDS' (25-95) [HNDLILHNTDS]
DELETE FROM switchdata WHERE switch=='NBRKILNTTDS'; -- FIXME: poor equipment code 'TDS' (25-127) [NBRKILNTTDS]
DELETE FROM switchdata WHERE switch=='PEORILPJTDS'; -- FIXME: poor equipment code 'TDS' (25-137) [PEORILPJTDS]
DELETE FROM switchdata WHERE switch=='RCISILRIKA1'; -- FIXME: poor equipment code 'KA1' (25-146) [RCISILRIKA1]
DELETE FROM switchdata WHERE switch=='RCFRILRTTDS'; -- FIXME: poor equipment code 'TDS' (25-147) [RCFRILRTTDS]
DELETE FROM switchdata WHERE switch=='SXCYIADTSB3'; -- FIXME: poor equipment code 'SB3' (27-102) [SXCYIADTSB3]
DELETE FROM switchdata WHERE switch=='JNCYKSXCHS0'; -- FIXME: poor equipment code 'HS0' (28-30) [JNCYKSXCHS0]
DELETE FROM switchdata WHERE switch=='PRLKKYAAJ00'; -- FIXME: poor equipment code 'J00' (29-55) [PRLKKYAAJ00]
DELETE FROM switchdata WHERE switch=='GDRPMIBLH1C'; -- FIXME: poor equipment code 'H1C' (34-55) [GDRPMIBLH1C]
DELETE FROM switchdata WHERE switch=='LNNGMIMNH0A'; -- FIXME: poor equipment code 'H0A' (34-83) [LNNGMIMNH0A]
DELETE FROM switchdata WHERE switch=='SFLDMIMNVS0'; -- FIXME: poor equipment code 'VS0' (34-142) [SFLDMIMNVS0]
DELETE FROM switchdata WHERE switch=='MPLSMNDTKDZ'; -- FIXME: poor equipment code 'KDZ' (35-93) [MPLSMNDTKDZ]
DELETE FROM switchdata WHERE switch=='STPLMNMKKDZ'; -- FIXME: poor equipment code 'KDZ' (35-139) [STPLMNMKKDZ]
DELETE FROM switchdata WHERE switch=='WOTNMN14WOO'; -- FIXME: poor equipment code 'WOO' (35-158) [WOTNMN14WOO]
DELETE FROM switchdata WHERE switch=='MEXCMOJUHC1'; -- FIXME: poor equipment code 'HC1' (37-68) [MEXCMOJUHC1]
DELETE FROM switchdata WHERE switch=='BGTMMTXCHS0'; -- FIXME: poor equipment code 'HS0' (38-4) [BGTMMTXCHS0]
DELETE FROM switchdata WHERE switch=='CHRLMT02WOO'; -- FIXME: poor equipment code 'WOO' (38-9) [CHRLMT02WOO]
DELETE FROM switchdata WHERE switch=='HAVRMTXCHS0'; -- FIXME: poor equipment code 'HS0' (38-23) [HAVRMTXCHS0]
DELETE FROM switchdata WHERE switch=='ANPKNV10HBC'; -- FIXME: poor equipment code 'HBC' (40-1) [ANPKNV10HBC]
DELETE FROM switchdata WHERE switch=='CLDLNV09HBC'; -- FIXME: poor equipment code 'HBC' (40-3) [CLDLNV09HBC]
DELETE FROM switchdata WHERE switch=='FLLNNVCTHBC'; -- FIXME: poor equipment code 'HBC' (40-5) [FLLNNVCTHBC]
DELETE FROM switchdata WHERE switch=='FLLNNVXDHBC'; -- FIXME: poor equipment code 'HBC' (40-5) [FLLNNVXDHBC]
DELETE FROM switchdata WHERE switch=='LSVGNV03HBC'; -- FIXME: poor equipment code 'HBC' (40-10) [LSVGNV03HBC]
DELETE FROM switchdata WHERE switch=='POMTNV10HBC'; -- FIXME: poor equipment code 'HBC' (40-17) [POMTNV10HBC]
DELETE FROM switchdata WHERE switch=='PSVLNJPL2GN'; -- FIXME: poor equipment code '2GN' (42-63) [PSVLNJPL2GN]
DELETE FROM switchdata WHERE switch=='MONRNYXAJUD'; -- FIXME: poor equipment code 'JUD' (44-89) [MONRNYXAJUD]
DELETE FROM switchdata WHERE switch=='BSMRNDBCFH5'; -- FIXME: poor equipment code 'FH5' (46-5) [BSMRNDBCFH5]
DELETE FROM switchdata WHERE switch=='FARGNDBCF2E'; -- FIXME: poor equipment code 'F2E' (46-17) [FARGNDBCF2E]
DELETE FROM switchdata WHERE switch=='KGFSOKXAO1W'; -- FIXME: poor equipment code 'O1W' (48-36) [KGFSOKXAO1W]
DELETE FROM switchdata WHERE switch=='PTLDORPBWID'; -- FIXME: poor equipment code 'WID' (49-45) [PTLDORPBWID]
DELETE FROM switchdata WHERE switch=='CLSMPAXCRU1'; -- FIXME: poor equipment code 'RU1' (50-27) [CLSMPAXCRU1]
DELETE FROM switchdata WHERE switch=='CLSMPAXCRU2'; -- FIXME: poor equipment code 'RU2' (50-27) [CLSMPAXCRU2]
DELETE FROM switchdata WHERE switch=='CLMASCCHKXX'; -- FIXME: poor equipment code 'KXX' (53-17) [CLMASCCHKXX]
DELETE FROM switchdata WHERE switch=='WCLMSCMAKXX'; -- FIXME: poor equipment code 'KXX' (53-24) [WCLMSCMAKXX]
DELETE FROM switchdata WHERE switch=='CLMSSCARKXX'; -- FIXME: poor equipment code 'KXX' (53-24) [CLMSSCARKXX]
DELETE FROM switchdata WHERE switch=='JHCYTNXAHS0'; -- FIXME: poor equipment code 'HS0' (55-48) [JHCYTNXAHS0]
DELETE FROM switchdata WHERE switch=='ABLNTXOR1ST'; -- FIXME: poor equipment code '1ST' (56-1) [ABLNTXOR1ST]
DELETE FROM switchdata WHERE switch=='ALTOTXXOHPI'; -- FIXME: poor equipment code 'HPI' (56-5) [ALTOTXXOHPI]
DELETE FROM switchdata WHERE switch=='APSPTXXPHPI'; -- FIXME: poor equipment code 'HPI' (56-9) [APSPTXXPHPI]
DELETE FROM switchdata WHERE switch=='BKSHTXXAHPI'; -- FIXME: poor equipment code 'HPI' (56-41) [BKSHTXXAHPI]
DELETE FROM switchdata WHERE switch=='CENTTXXCHPI'; -- FIXME: poor equipment code 'HPI' (56-53) [CENTTXXCHPI]
DELETE FROM switchdata WHERE switch=='CONRTXXAHPI'; -- FIXME: poor equipment code 'HPI' (56-60) [CONRTXXAHPI]
DELETE FROM switchdata WHERE switch=='CTSHTXXCHPI'; -- FIXME: poor equipment code 'HPI' (56-67) [CTSHTXXCHPI]
DELETE FROM switchdata WHERE switch=='DBLLTXXDHPI'; -- FIXME: poor equipment code 'HPI' (56-102) [DBLLTXXDHPI]
DELETE FROM switchdata WHERE switch=='ETOLTXXEHPI'; -- FIXME: poor equipment code 'HPI' (56-113) [ETOLTXXEHPI]
DELETE FROM switchdata WHERE switch=='FLSPTXXFHPI'; -- FIXME: poor equipment code 'HPI' (56-135) [FLSPTXXFHPI]
DELETE FROM switchdata WHERE switch=='GNLDTXXGHPI'; -- FIXME: poor equipment code 'HPI' (56-143) [GNLDTXXGHPI]
DELETE FROM switchdata WHERE switch=='HDSNTXXHHPI'; -- FIXME: poor equipment code 'HPI' (56-185) [HDSNTXXHHPI]
DELETE FROM switchdata WHERE switch=='KATYTXXAHPI'; -- FIXME: poor equipment code 'HPI' (56-197) [KATYTXXAHPI]
DELETE FROM switchdata WHERE switch=='KATYTXXCHPI'; -- FIXME: poor equipment code 'HPI' (56-197) [KATYTXXCHPI]
DELETE FROM switchdata WHERE switch=='KATYTXXBHPI'; -- FIXME: poor equipment code 'HPI' (56-198) [KATYTXXBHPI]
DELETE FROM switchdata WHERE switch=='LKCFTXXLHIP'; -- FIXME: poor equipment code 'HIP' (56-206) [LKCFTXXLHIP]
DELETE FROM switchdata WHERE switch=='LFKNTXXAHPI'; -- FIXME: poor equipment code 'HPI' (56-221) [LFKNTXXAHPI]
DELETE FROM switchdata WHERE switch=='MTGMTXXMHPI'; -- FIXME: poor equipment code 'HPI' (56-235) [MTGMTXXMHPI]
DELETE FROM switchdata WHERE switch=='RVBKTXXRHPI'; -- FIXME: poor equipment code 'HPI' (56-269) [RVBKTXXRHPI]
DELETE FROM switchdata WHERE switch=='SNANTXCAHC1'; -- FIXME: poor equipment code 'HC1' (56-278) [SNANTXCAHC1]
DELETE FROM switchdata WHERE switch=='WLDMTXXWHPI'; -- FIXME: poor equipment code 'HPI' (56-325) [WLDMTXXWHPI]
DELETE FROM switchdata WHERE switch=='WLLSTXXWHPI'; -- FIXME: poor equipment code 'HPI' (56-328) [WLLSTXXWHPI]
DELETE FROM switchdata WHERE switch=='LKPTUTBHOOT'; -- FIXME: poor equipment code 'OOT' (58-12) [LKPTUTBHOOT]
DELETE FROM switchdata WHERE switch=='TACMWAFAKXX'; -- FIXME: poor equipment code 'KXX' (61-90) [TACMWAFAKXX]
--==========================================================================
