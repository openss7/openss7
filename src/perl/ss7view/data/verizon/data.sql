-- "Vz_Fairpoint_Tandem_Mar2008.pdf" March 24, 2008
-- npanxxsource still shows some 41 switches homed off of the decommissinoed 4ESS tandem.
update switchdata SET tdm='NYCKNYWM25T' WHERE tdm=='NYCKNYWM12T'; -- retired 4ESS
-- npanxxsource still shows some 29 switches homed off of the DMS-200 as local tandem
update switchdata SET tdmlcl='NYCKNYWM25T' WHERE tdmlcl=='NYCKNYWM12T'; -- retired 4ESS
update switchdata SET tdmilt='NYCKNYWM25T' WHERE tdmilt=='NYCKNYWM12T'; -- retired 4ESS
update switchdata SET tdmfgb='NYCKNYWM25T' WHERE tdmfgb=='NYCKNYWM12T'; -- retired 4ESS
update switchdata SET tdmfgd='NYCKNYWM25T' WHERE tdmfgd=='NYCKNYWM12T'; -- retired 4ESS
UPDATE switchdata SET tdmlcl='NYCKNYWM24T' WHERE tdmlcl=='NYCKNYWM25T'; -- move local tandem function
-- "ND10-0021_LIDB.pdf" October 28, 2010
-- npanxxsource still shows some 41 switches homed off of the decommissioned 4ESS tandem.
UPDATE switchdata SET tdm='NYCMNY1306T' WHERE tdm=='NYCMNYBW21T';
UPDATE switchdata SET tdmilt='NYCMNY1306T' WHERE tdmilt=='NYCMNYBW21T';
UPDATE switchdata SET tdmfgb='NYCMNY1306T' WHERE tdmfgb=='NYCMNYBW21T';
UPDATE switchdata SET tdmfgd='NYCMNY1306T' WHERE tdmfgd=='NYCMNYBW21T';
-- "NetwrkDisclWVMD.pdf" November 24, 2009
-- npanxxsource does not have this rehoming of STP on these two switches
UPDATE switchdata SET stp1='CLBGWVMA77W',stp2='MGTWWVFY77W' WHERE switch='KYSRWVMRDS0'; -- and stp1=='BLTMMDCH77W' and stp2=='PIVLMDPK77W'
UPDATE switchdata SET stp1='CLBGWVMA77W',stp2='MGTWWVFY77W' WHERE switch='MRBGWVBUDS0'; -- and stp1=='BLTMMDCH77W' and stp2=='PIVLMDPK77W'
-- "ND11-0001_Bartonville.pdf" January 14, 2011
-- npanxxsource does not have the decomissioning of this remote a show it still assigned to the
-- denton host as well as hosting 3 10'000's blocks (940-241,940-455,940-584).
-- localcallingguide, NECA4, pinglo telcodata do not have this either
UPDATE prefixdata SET switch='DNTNTXXADS1' WHERE switch=='BNVLTXXARS1';
-- "ND10-0001.pdf" March 11, 2010
-- npanxxsource has this, localcallingguide does not
-- neither does pinglo nor telcodata NOR NECA4!!!!
UPDATE prefixdata SET switch=NULL WHERE switch=='PNTGVADFDS0';
UPDATE prefixdata SET switch='ALXNVABADS0' WHERE switch=='PNTGVADFDS0' AND npa=='703' AND (nxx=='545' OR nxx=='695');
UPDATE prefixdata SET switch='ALXNVACNDS0' WHERE switch=='PNTGVADFDS0' AND npa=='703' AND (nxx=='571' OR nxx=='693');
UPDATE prefixdata SET switch='ALXNVABRDS0' WHERE switch=='PNTGVADFDS0' AND npa=='703' AND nxx=='614';
UPDATE prefixdata SET switch='ARTNVACYDS0' WHERE switch=='PNTGVADFDS0' AND npa=='703' AND nxx=='692';
UPDATE prefixdata SET switch='ARTNVAARDS0' WHERE switch=='PNTGVADFDS0' AND npa=='703' AND nxx=='697';
-- "Disclosure-Woodport-4-27.pdf" May 2, 2011
-- this is a packet switch and nobody shows it anyway
UPDATE switchdata SET tdm='NBWJNJNBPS1' WHERE tdm=='WDPTNJWPDS5';
UPDATE switchdata SET tdmlcl='NBWJNJNBPS1' WHERE tdmlcl=='WDPTNJWPDS5';
UPDATE switchdata SET tdmilt='NBWJNJNBPS1' WHERE tdmilt=='WDPTNJWPDS5';
UPDATE switchdata SET tdmfgb='NBWJNJNBPS1' WHERE tdmfgb=='WDPTNJWPDS5';
UPDATE switchdata SET tdmfgd='NBWJNJNBPS1' WHERE tdmfgd=='WDPTNJWPDS5';
-- "NDLawrenceMA.pdf" May 10, 2011
-- npanxxsource still shows some 77 switches homed off of the decommissioned TOPS switch (and only 28 on the new)
UPDATE switchdata SET tdmops='FRMNMAUN2GT' WHERE tdmops=='LWRNMACA2GT';
