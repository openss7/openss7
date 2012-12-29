--===================================
UPDATE ocndata    SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE pcdata     SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE rcdata     SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE rndata     SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE exdata     SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE lcadata    SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE wcdata     SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE switchdata SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE npadata    SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE prefixdata SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE pooldata   SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
UPDATE linedata   SET sect='nnxs' WHERE sect NOT NULL and sect!='nnxs';
--===================================
-- UPDATE prefixdata SET nxxtype='EOC' WHERE nxxtype=='Plain Old Telephone Service (POTS)';
-- UPDATE prefixdata SET nxxtype='PLN' WHERE nxxtype=='Planned Code (non-routable)';
-- UPDATE prefixdata SET nxxtype='TST' WHERE nxxtype=='Standard Plant Test Code';
-- UPDATE prefixdata SET nxxtype='SIC' WHERE nxxtype=='Special 800 Service Code';
-- UPDATE prefixdata SET nxxtype='VOI' WHERE nxxtype=='Voice over Internet Protocol (VoIP)';
-- UPDATE prefixdata SET nxxtype='AIN' WHERE nxxtype=='Advanced Intelligent Network';
-- UPDATE prefixdata SET nxxtype='BLG' WHERE nxxtype=='Billing Only';
-- UPDATE prefixdata SET nxxtype='BRD' WHERE nxxtype=='Broadband';
-- UPDATE prefixdata SET nxxtype='CDA' WHERE nxxtype=='Centralized Directory Assistance';
-- UPDATE prefixdata SET nxxtype='CTV' WHERE nxxtype=='Cable Television';
-- UPDATE prefixdata SET nxxtype='ENP' WHERE nxxtype=='Emergency Preparedness';
-- UPDATE prefixdata SET nxxtype='FGB' WHERE nxxtype=='Feature Group B Access';
-- UPDATE prefixdata SET nxxtype='HVL' WHERE nxxtype=='High Volume';
-- UPDATE prefixdata SET nxxtype='INP' WHERE nxxtype=='Information Provider Services';
-- UPDATE prefixdata SET nxxtype='N11' WHERE nxxtype=='N11 Code';
-- UPDATE prefixdata SET nxxtype='ONA' WHERE nxxtype=='Open Network Architecture';
-- UPDATE prefixdata SET nxxtype='RTG' WHERE nxxtype=='Routing Only';
-- UPDATE prefixdata SET nxxtype='UFA' WHERE nxxtype=='Unavailable for Assignment';
-- UPDATE prefixdata SET nxxtype='700' WHERE nxxtype=='700 IntraLATA Presubscription';
-- UPDATE prefixdata SET nxxtype='PMC' WHERE nxxtype=='Public Mobile Carrier';
-- UPDATE prefixdata SET nxxtype='SP2S' WHERE nxxtype=='Personal Communications Services (not NPA 500)';
-- UPDATE prefixdata SET nxxtype='SP2SX' WHERE nxxtype=='Personal Communications Services (not NPA 500) - Service Provider requests Local Exchange Carrier intraLATA Special Billing Option';
-- UPDATE prefixdata SET nxxtype='SP2SZ' WHERE nxxtype=='Personal Communications Services (not NPA 500) - Service Provider requests Local Exchange Carrier intraLATA Special Billing Option';
-- UPDATE prefixdata SET nxxtype='EOCC' WHERE nxxtype=='Shared between POTS and Cellular';
-- UPDATE prefixdata SET nxxtype='EOCR' WHERE nxxtype=='Shared between POTS and Mobile';
-- UPDATE prefixdata SET nxxtype='EOCS' WHERE nxxtype=='Shared between POTS and PCS';
-- UPDATE prefixdata SET nxxtype='EOCV' WHERE nxxtype=='Shared between POTS and VoIP';
-- UPDATE prefixdata SET nxxtype='EOCB' WHERE nxxtype=='Shared between POTS and Paging';
-- UPDATE prefixdata SET nxxtype='PMCN' WHERE nxxtype=='Dedicated to Mobile';
-- UPDATE prefixdata SET nxxtype='SP1B' WHERE nxxtype=='Paging';
-- UPDATE prefixdata SET nxxtype='SP2B' WHERE nxxtype=='Dedicated to Paging';
-- UPDATE prefixdata SET nxxtype='SP2C' WHERE nxxtype=='Dedicated to Cellular';
--=======================================================
UPDATE switchdata SET wcv='6641' WHERE switch=='LWTWMO01RL2';
UPDATE wcdata SET wcv='6641' WHERE wc=='LWTWMO01';
---------------------
UPDATE switchdata SET wcv='3964',wch='5976',wclat='55.3172',wclon='-97.69028' WHERE switch='TTPTMB01H01';
UPDATE wcdata SET wcv='3964',wch='5976',wclat='55.3172',wclon='-97.69028' WHERE wc='TTPTMB01';
---------------------
UPDATE switchdata SET wcv='3882',wch='5512',wclat='54.83816',wclon='-94.05645' WHERE switch='GDRVMBRRRS1';
UPDATE wcdata SET wcv='3882',wch='5512',wclat='54.83816',wclon='-94.05645' WHERE wc='GDRVMBRR';
---------------------
UPDATE switchdata SET wcv='7972',wch='3051',wclat='32.90984',wclon='-90.87719' WHERE switch='RLFKMSAEAMD';
UPDATE wcdata SET wcv='7972',wch='3051',wclat='32.90984',wclon='-90.87719' WHERE wc='RLFKMSAE';
---------------------
UPDATE switchdata SET wch='1084',wclat='45.365576',wclon='-65.948508' WHERE switch='STJHNBBHAMD';
UPDATE wcdata SET wch='1084',wclat='45.365576',wclon='-65.948508' WHERE wc='STJHNBBH';
---------------------
UPDATE switchdata SET wch='1282',wclat='46.735091',wclon='-65.829258' WHERE switch=='BAVLNBSCRS0';
UPDATE wcdata SET wch='1282',wclat='46.735091',wclon='-65.829258' WHERE wc=='BAVLNBSC';
---------------------
UPDATE switchdata SET wcv='3405',wclat=NULL,wclon=NULL WHERE switch='HRLDNBSCRS0';
UPDATE wcdata SET wcv='3405',wclat=NULL,wclon=NULL WHERE wc='HRLDNBSC';
---------------------
UPDATE switchdata SET wcv='6749',wch='1997',wclat='35.583183',wclon='-82.545027' WHERE switch LIKE 'AHVLNCWF___';
UPDATE wcdata SET wcv='6749',wch='1997',wclat='35.583183',wclon='-82.545027' WHERE wc=='AHVLNCWF';
---------------------
UPDATE switchdata SET wch='1118' WHERE switch=='WLMGNCWT0MD';
UPDATE wcdata SET wch='1118' WHERE wc=='WLMGNCWT';
---------------------
UPDATE switchdata SET wcv='6936',wch='4636',wclat='40.26691',wclon='96.7461' WHERE switch LIKE 'BTRCNEXC___';
UPDATE wcdata SET wcv='6936',wch='4636',wclat='40.26691',wclon='96.7461' WHERE wc=='BTRCNEXC';
---------------------
UPDATE switchdata SET wcv='3256',wch='714',wclat='44.65829',wclon='-63.60333' WHERE switch LIKE 'HLFXNSAA___';
UPDATE wcdata SET wcv='3256',wch='714',wclat='44.65829',wclon='-63.60333' WHERE wc=='HLFXNSAA';
UPDATE switchdata SET wcv='3256',wch='714',wclat='44.65829',wclon='-63.60333' WHERE switch LIKE 'HLFXNSYT___';
UPDATE wcdata SET wcv='3256',wch='714',wclat='44.65829',wclon='-63.60333' WHERE wc=='HLFXNSYT';
---------------------
UPDATE switchdata SET wcv='03456',wch='00828',wclat='44.3754',wclon='-65.02921' WHERE switch LIKE 'CAQNNS01___';
UPDATE wcdata SET wcv='03456',wch='00828',wclat='44.3754',wclon='-65.02921' WHERE wc=='CAQNNS01';
---------------------
UPDATE switchdata SET wcv='02721',wch='00717',wclat='46.33368',wclon='-61.09661' WHERE switch=='MGFKNS01RS1';
UPDATE wcdata SET wcv='02721',wch='00717',wclat='46.33368',wclon='-61.09661' WHERE wc=='MGFKNS01';
---------------------
UPDATE switchdata SET wcv='03499',wch='08899',wclat='60.44134',wclon='-121.24026' WHERE switch=='TRLKTN01DS0';
UPDATE wcdata SET wcv='03499',wch='08899',wclat='60.44134',wclon='-121.24026' WHERE wc=='TRLKTN01';
UPDATE exdata SET rcv='03499',rch='08899',rclat='60.44134',rclon='-121.24026' WHERE exch='130580';
UPDATE rcdata SET rcv='03499',rch='08899',rclat='60.44134',rclon='-121.24026' WHERE region='NT' and rcshort='TROUT LAKE';
---------------------
UPDATE exdata SET rcv='951',rch='9209',rclat='71.98655836',rclon='-125.25383206' WHERE exch='130550';
UPDATE rcdata SET rcv='951',rch='9209',rclat='71.98655836',rclon='-125.25383206' WHERE region='NT' and rcshort='SACHSHRBOR';
---------------------
-- Note: one must map lat and lon when v==00001
UPDATE switchdata SET wcv='00001',wch='05310',wclat='70.473889',wclon='-68.5925' WHERE switch='CLRVVU01DS0';
UPDATE wcdata SET wcv='00001',wch='05310',wclat='70.473889',wclon='-68.5925' WHERE wc='CLRVVU01';
UPDATE exdata SET rcv='00001',rch='05310',rclat='70.473889',rclon='-68.5925' WHERE exch='130140';
UPDATE rcdata SET rcv='00001',rch='05310',rclat='70.473889',rclon='-68.5925' WHERE region='NU' and rcshort='CLYDERIVER';
---------------------
UPDATE switchdata SET wcv='00001',wch='06574',wclat='73.03405',wclon='-84.51281' WHERE switch='NSVKVU01DS0';
UPDATE wcdata SET wcv='00001',wch='06574',wclat='73.03405',wclon='-84.51281' WHERE wc='NSVKVU0';
UPDATE exdata SET rcv='00001',rch='06574',rclat='73.03405',rclon='-84.51281' WHERE exch='130440';
UPDATE rcdata SET rcv='00001',rch='06574',rclat='73.03405',rclon='-84.51281' WHERE revion='VU' and rcshort='NANISIVIK';
---------------------
UPDATE switchdata SET wcv='00001',wch='06929',wclat='76.4175',wclon='-82.8938889' WHERE switch='GSFDVU01DS0';
UPDATE wcdata SET wcv='00001',wch='06929',wclat='76.4175',wclon='-82.8938889' WHERE wc='GSFDVU01';
UPDATE exdata SET rcv='00001',rch='06929',rclat='76.4175',rclon='-82.8938889' WHERE exch='130280';
UPDATE rcdata SET rcv='00001',rch='06929',rclat='76.4175',rclon='-82.8938889' WHERE region='VU' and rcshort='GRISEFIORD';
---------------------
UPDATE switchdata SET wcv='00001',wch='07352',wclat='74.68553',wclon='-94.89981' WHERE switch='RSLTVU01DS0';
UPDATE wcdata SET wcv='00001',wch='07352',wclat='74.68553',wclon='-94.89981' WHERE wc='RSLTVU01';
UPDATE exdata SET rcv='00001',rch='07352',rclat='74.68553',rclon='-94.89981' WHERE exch='130540';
UPDATE rcdata SET rcv='00001',rch='07352',rclat='74.68553',rclon='-94.89981' WHERE region='VU' and rcshort='RESOLUTE';
---------------------
UPDATE switchdata SET wcv='00956',wch='04170',wclat='62.849391',wclon='-69.878394' WHERE switch='KMRTVU01DS0';
UPDATE wcdata SET wcv='00956',wch='04170',wclat='62.849391',wclon='-69.878394' WHERE wc='KMRTVU01';
UPDATE exdata SET rcv='00957',rch='04169',rclat='62.849391',rclon='-69.878394' WHERE exch='130370';
UPDATE rcdata SET rcv='00957',rch='04169',rclat='62.849391',rclon='-69.878394' WHERE region='VU' and rcshort='KIMMIRUT';
---------------------
UPDATE switchdata SET wcv='00617',wch='05842',wclat='68.77496',wclon='-81.22983' WHERE switch='HLBHVU01DS0';
UPDATE wcdata SET wcv='00617',wch='05842',wclat='68.77496',wclon='-81.22983' WHERE wc='HLBHVU01';
UPDATE exdata SET rcv='00617',rch='05842',rclat='68.77496',rclon='-81.22983' WHERE exch='130290';
UPDATE rcdata SET rcv='00617',rch='05842',rclat='68.77496',rclon='-81.22983' WHERE region='VU' and rcshort='HALL BEACH';
---------------------
UPDATE switchdata SET wcv='08064',wch='08327',wclat='39.530361',wclon='-119.831254' WHERE switch LIKE 'RENONVXF___';
UPDATE wcdata SET wcv='08064',wch='08327',wclat='39.530361',wclon='-119.831254' WHERE wc='RENONVXF';
---------------------
UPDATE switchdata SET wcv='04371',wch='02259' WHERE switch='KNTAONDR0MD';
UPDATE wcdata SET wcv='04371',wch='02259' WHERE wc='KNTAONDR';
---------------------
UPDATE switchdata SET wcv='04455',wch='02215' WHERE switch LIKE 'SMFLONAT___';
UPDATE wcdata SET wcv='04455',wch='02215' WHERE wc=='SMFLONAT';
---------------------
UPDATE switchdata SET wcv='04803',wch='03237',wclat='46.184951',wclon='-82.816947' WHERE switch LIKE 'AGMLONAG___';
UPDATE wcdata SET wcv='04803',wch='03237',wclat='46.184951',wclon='-82.816947' WHERE wc=='AGMLONAG';
---------------------
UPDATE switchdata SET wcv='04946',wch='02800',wclat='44.5680432',wclon='-80.9184884' WHERE switch LIKE 'OWSDONAN___';
UPDATE wcdata SET wcv='04946',wch='02800',wclat='44.5680432',wclon='-80.9184884' WHERE wc='OWSDONAN';
---------------------
UPDATE switchdata SET wch='2813' WHERE switch LIKE 'SARNONST___';
UPDATE wcdata SET wch='2813' WHERE wc='SARNONST';
---------------------
UPDATE switchdata SET wcv='4392' WHERE switch LIKE 'PMBRONDA___';
UPDATE wcdata SET wcv='4392' WHERE wc='PMBRONDA';
---------------------
UPDATE switchdata SET wcv='4374' WHERE switch LIKE 'WMTHONXR___';
UPDATE wcdata SET wcv='4374' WHERE wc='WMTHONXR';
---------------------
UPDATE switchdata SET wcv='04036',wch='01800',wclat='45.2893842',wclon='-72.2184268' WHERE switch LIKE 'MAGOPQBU___';
UPDATE wcdata SET wcv='04036',wch='01800',wclat='45.2893842',wclon='-72.2184268' WHERE wc='MAGOPQBU';
---------------------
UPDATE switchdata SET wcv='03772',wch='01859',wclat='46.3858171',wclon='-71.412062' WHERE switch LIKE 'SAGLPQXQ___';
UPDATE wcdata SET wcv='03772',wch='01859',wclat='46.3858171',wclon='-71.412062' WHERE wc='SAGLPQXQ';
---------------------
UPDATE switchdata SET wcv='04188',wch='02031',wclat='45.3887397',wclon='-74.014703' WHERE switch LIKE 'VDRLPQ55___';
UPDATE wcdata SET wcv='04188',wch='02031',wclat='45.3887397',wclon='-74.014703' WHERE wc='VDRLPQ55';
---------------------
UPDATE switchdata SET wcv='03350',wch='01854',wclat='47.8318793',wclon='-69.5310543' WHERE switch LIKE 'RDLPPQAN___';
UPDATE wcdata SET wcv='03350',wch='01854',wclat='47.8318793',wclon='-69.5310543' WHERE wc='RDLPPQAN';
---------------------
UPDATE switchdata SET wcv='04082',wch='01858',wclat='45.2908917',wclon='-72.7031396' WHERE switch LIKE 'BRMTPQAJ___';
UPDATE wcdata SET wcv='04082',wch='01858',wclat='45.2908917',wclon='-72.7031396' WHERE wc='BRMTPQAJ';
---------------------
UPDATE switchdata SET wcv='01443',wch='04526',wclat='62.2',wclon='-75.6333333' WHERE switch LIKE 'SLLTPQ27___';
UPDATE wcdata SET wcv='01443',wch='04526',wclat='62.2',wclon='-75.6333333' WHERE wc='SLLTPQ27';
---------------------
UPDATE switchdata SET wcv='03042' WHERE switch LIKE 'PNCXPQXQ___';
UPDATE wcdata SET wcv='03042' WHERE wc='PNCXPQXQ';
---------------------
UPDATE switchdata SET wcv='03720',wch='01814',wclat='46.4390',wclon='-70.961' WHERE switch LIKE 'STMRPQ02___';
UPDATE wcdata SET wcv='03720',wch='01814',wclat='46.4390',wclon='-70.961' WHERE wc='STMRPQ02';
---------------------
UPDATE switchdata SET wcv='01976',wch='02648',wclat='54.820572',wclon='-66.825422' WHERE switch LIKE 'SCVLPQXU___';
UPDATE wcdata SET wcv='01976',wch='02648',wclat='54.820572',wclon='-66.825422' WHERE wc='SCVLPQXU';
---------------------
UPDATE switchdata SET wcv='01592',wch='03307',wclat='58.1066667',wclon='-68.3986111' WHERE switch LIKE 'PBLNPQ01___';
UPDATE wcdata SET wcv='01592',wch='03307',wclat='58.1066667',wclon='-68.3986111' WHERE wc='PBLNPQ01';
---------------------
UPDATE switchdata SET wcv='04121',wch='02367',wclat='46.553083',wclon='-75.497074' WHERE switch LIKE 'MTLRPQXH___';
UPDATE wcdata SET wcv='04121',wch='02367',wclat='46.553083',wclon='-75.497074' WHERE wc='MTLRPQXH';
---------------------
UPDATE switchdata SET wcv='04226',wch='02203',wclat='45.7248592',wclon='-75.0538201' WHERE switch LIKE 'SAAVPQXT___';
UPDATE wcdata SET wcv='04226',wch='02203',wclat='45.7248592',wclon='-75.0538201' WHERE wc='SAAVPQXT';
---------------------
UPDATE switchdata SET wcv='04096',wch='02140',wclat='46.0227462',wclon='-74.2067692' WHERE switch LIKE 'VLDVPQ59___';
UPDATE wcdata SET wcv='04096',wch='02140',wclat='46.0227462',wclon='-74.2067692' WHERE wc='VLDVPQ59';
---------------------
UPDATE switchdata SET wcv='03867',wch='02039',wclat='46.5645844',wclon='-72.7398247' WHERE switch LIKE 'SHGNPQ38___';
UPDATE wcdata SET wcv='03867',wch='02039',wclat='46.5645844',wclon='-72.7398247' WHERE wc='SHGNPQ38';
---------------------
UPDATE switchdata SET wcv='04123',wch='02367',wclat='46.5489464',wclon='-75.5069235' WHERE switch LIKE 'MTLRPQXT___';
UPDATE wcdata SET wcv='04123',wch='02367',wclat='46.5489464',wclon='-75.5069235' WHERE wc='MTLRPQXT';
---------------------
UPDATE switchdata SET wcv='06206',wch='05184',wclat='44.3375882',wclon='-98.2344731' WHERE switch LIKE 'HURNSDAX___';
UPDATE wcdata SET wcv='06206',wch='05184',wclat='44.3375882',wclon='-98.2344731' WHERE wc='HURNSDAX';
---------------------
UPDATE switchdata SET wcv='05112',wch='06181',wclat='50.6568523',wclon='-102.0757792' WHERE switch LIKE 'ESTRSKAA___';
UPDATE wcdata SET wcv='05112',wch='06181',wclat='50.6568523',wclon='-102.0757792' WHERE wc='ESTRSKAA';
---------------------
UPDATE switchdata SET wcv='09855',wch='03763',wclat='26.203783',wclon='-98.226748' WHERE switch LIKE 'MCALTXDE___';
UPDATE wcdata SET wcv='09855',wch='03763',wclat='26.203783',wclon='-98.226748' WHERE wc='MCALTXDE';
---------------------
UPDATE switchdata SET wcv='07653',wch='07413',wclat='40.7305866',wclon='-114.0395863' WHERE switch LIKE 'WNDVUTXC___';
UPDATE wcdata SET wcv='07653',wch='07413',wclat='40.7305866',wclon='-114.0395863' WHERE wc='WNDVUTXC';
---------------------
UPDATE exdata SET rcv='07683',rch='06989',rclat='40.2024564',rclon='-111.5754719' WHERE exch='200800';
UPDATE rcdata SET rcv='07683',rch='06989',rclat='40.2024564',rclon='-111.5754719' WHERE region='UT' and rcshort='KOLOB';
---------------------
UPDATE switchdata SET wcv='5638' WHERE switch LIKE 'VINNVACB___';
UPDATE wcdata SET wcv='5638' WHERE wc='VINNVACB';
---------------------
UPDATE switchdata SET wcv='04916',wch='08557',wclat='53.8901014',wclon='-119.1068173' WHERE switch LIKE 'GDCHAB01___';
UPDATE wcdata SET wcv='04916',wch='08557',wclat='53.8901014',wclon='-119.1068173' WHERE wc='GDCHAB01';
---------------------
UPDATE switchdata SET wcv='05438',wch='07811' WHERE switch LIKE 'CLGRABMT___';
UPDATE wcdata SET wcv='05438',wch='07811' WHERE wc='CLGRABMT';
--=======================================================
UPDATE prefixdata SET switch='MAY IDXCRS1' WHERE switch=='MAY_IDXCRS1'; -- FIXED: improper clli format character s/_/ / (208876) [208-876]
UPDATE OR IGNORE switchdata SET switch='MAY IDXCRS1' WHERE switch=='MAY_IDXCRS1'; -- FIXED: improper clli format character s/_/ / (208876) [MAY_IDXCRS1]
DELETE FROM switchdata WHERE switch=='MAY_IDXCRS1'; -- FIXED: improper clli format character s/_/ / (208876) [MAY_IDXCRS1]
UPDATE OR IGNORE wcdata SET wc='MAY IDXC' WHERE wc=='MAY_IDXC'; -- FIXED: improper clli format character s/_/ / (208876)
DELETE FROM wcdata WHERE wc=='MAY_IDXC'; -- FIXED: improper clli format character s/_/ / (208876)
---------------------
UPDATE prefixdata SET switch='ELY MNXEDS0' WHERE switch=='ELY_MNXEDS0'; -- FIXED: improper clli format character s/_/ / (218365) [218-365]
UPDATE OR IGNORE switchdata SET switch='ELY MNXEDS0' WHERE switch=='ELY_MNXEDS0'; -- FIXED: improper clli format character s/_/ / (218365) [ELY_MNXEDS0]
DELETE FROM switchdata WHERE switch=='ELY_MNXEDS0'; -- FIXED: improper clli format character s/_/ / (218365) [ELY_MNXEDS0]
UPDATE OR IGNORE wcdata SET wc='ELY MNXE' WHERE wc=='ELY_MNXE'; -- FIXED: improper clli format character s/_/ / (218365)
DELETE FROM wcdata WHERE wc=='ELY_MNXE'; -- FIXED: improper clli format character s/_/ / (218365)
---------------------
UPDATE pooldata   SET switch='ADA MNXADS0' WHERE switch=='ADA_MNXADS0'; -- FIXED: improper clli format character s/_/ / (2187840,2187841,2187842,2187843,2187844,2187845,2187846,2187847,2187849) [218-784-0,218-784-1,218-784-2,218-784-3,218-784-4,218-784-5,218-784-6,218-784-7,218-784-9]
UPDATE prefixdata SET switch='ADA MNXADS0' WHERE switch=='ADA_MNXADS0'; -- FIXED: improper clli format character s/_/ / (218494,218582,218784,2187840,2187841,2187842,2187843,2187844,2187845,2187846,...) [218-494,218-582,218-784,218-962]
UPDATE OR IGNORE switchdata SET switch='ADA MNXADS0' WHERE switch=='ADA_MNXADS0'; -- FIXED: improper clli format character s/_/ / (218494,218582,218784,2187840,2187841,2187842,2187843,2187844,2187845,2187846,...) [ADA_MNXADS0]
DELETE FROM switchdata WHERE switch=='ADA_MNXADS0'; -- FIXED: improper clli format character s/_/ / (218494,218582,218784,2187840,2187841,2187842,2187843,2187844,2187845,2187846,...) [ADA_MNXADS0]
UPDATE OR IGNORE wcdata SET wc='ADA MNXA' WHERE wc=='ADA_MNXA'; -- FIXED: improper clli format character s/_/ / (218494,218582,218784,2187840,2187841,2187842,2187843,2187844,2187845,2187846,...)
DELETE FROM wcdata WHERE wc=='ADA_MNXA'; -- FIXED: improper clli format character s/_/ / (218494,218582,218784,2187840,2187841,2187842,2187843,2187844,2187845,2187846,...)
---------------------
UPDATE prefixdata SET switch='ORR MNXADS0' WHERE switch=='ORR_MNXADS0'; -- FIXED: improper clli format character s/_/ / (218757) [218-757]
UPDATE OR IGNORE switchdata SET switch='ORR MNXADS0' WHERE switch=='ORR_MNXADS0'; -- FIXED: improper clli format character s/_/ / (218757) [ORR_MNXADS0]
DELETE FROM switchdata WHERE switch=='ORR_MNXADS0'; -- FIXED: improper clli format character s/_/ / (218757) [ORR_MNXADS0]
UPDATE OR IGNORE wcdata SET wc='ORR MNXA' WHERE wc=='ORR_MNXA'; -- FIXED: improper clli format character s/_/ / (218757)
DELETE FROM wcdata WHERE wc=='ORR_MNXA'; -- FIXED: improper clli format character s/_/ / (218757)
---------------------
UPDATE pooldata   SET switch='ROY WA01DS0' WHERE switch=='ROY_WA01DS0'; -- FIXED: improper clli format character s/_/ / (2538430,2538431,2538432,2538433,2538434,2538439) [253-843-0,253-843-1,253-843-2,253-843-3,253-843-4,253-843-9]
UPDATE prefixdata SET switch='ROY WA01DS0' WHERE switch=='ROY_WA01DS0'; -- FIXED: improper clli format character s/_/ / (253843,2538430,2538431,2538432,2538433,2538434,2538439) [253-843]
UPDATE pcdata SET switch='ROY WA01DS0' WHERE switch=='ROY_WA01DS0'; -- FIXED: improper clli format character s/_/ / (253843,2538430,2538431,2538432,2538433,2538434,2538439) [248-226-059]
UPDATE OR IGNORE switchdata SET switch='ROY WA01DS0' WHERE switch=='ROY_WA01DS0'; -- FIXED: improper clli format character s/_/ / (253843,2538430,2538431,2538432,2538433,2538434,2538439) [ROY_WA01DS0]
DELETE FROM switchdata WHERE switch=='ROY_WA01DS0'; -- FIXED: improper clli format character s/_/ / (253843,2538430,2538431,2538432,2538433,2538434,2538439) [ROY_WA01DS0]
UPDATE OR IGNORE wcdata SET wc='ROY WA01' WHERE wc=='ROY_WA01'; -- FIXED: improper clli format character s/_/ / (253843,2538430,2538431,2538432,2538433,2538434,2538439)
DELETE FROM wcdata WHERE wc=='ROY_WA01'; -- FIXED: improper clli format character s/_/ / (253843,2538430,2538431,2538432,2538433,2538434,2538439)
---------------------
UPDATE prefixdata SET switch='MAY TXXARS0' WHERE switch=='MAY_TXXARS0'; -- FIXED: improper clli format character s/_/ / (254259) [254-259]
UPDATE OR IGNORE switchdata SET switch='MAY TXXARS0' WHERE switch=='MAY_TXXARS0'; -- FIXED: improper clli format character s/_/ / (254259) [MAY_TXXARS0]
DELETE FROM switchdata WHERE switch=='MAY_TXXARS0'; -- FIXED: improper clli format character s/_/ / (254259) [MAY_TXXARS0]
UPDATE OR IGNORE wcdata SET wc='MAY TXXA' WHERE wc=='MAY_TXXA'; -- FIXED: improper clli format character s/_/ / (254259)
DELETE FROM wcdata WHERE wc=='MAY_TXXA'; -- FIXED: improper clli format character s/_/ / (254259)
---------------------
UPDATE pooldata   SET switch='LEO INXARS3' WHERE switch=='LEO_INXARS3'; -- FIXED: improper clli format character s/_/ / (2606270,2606272,2606273,2606275,2606276,2606277,2606278) [260-627-0,260-627-2,260-627-3,260-627-5,260-627-6,260-627-7,260-627-8]
UPDATE prefixdata SET switch='LEO INXARS3' WHERE switch=='LEO_INXARS3'; -- FIXED: improper clli format character s/_/ / (260627,2606270,2606272,2606273,2606275,2606276,2606277,2606278) [260-627]
UPDATE OR IGNORE switchdata SET switch='LEO INXARS3' WHERE switch=='LEO_INXARS3'; -- FIXED: improper clli format character s/_/ / (260627,2606270,2606272,2606273,2606275,2606276,2606277,2606278) [LEO_INXARS3]
DELETE FROM switchdata WHERE switch=='LEO_INXARS3'; -- FIXED: improper clli format character s/_/ / (260627,2606270,2606272,2606273,2606275,2606276,2606277,2606278) [LEO_INXARS3]
UPDATE OR IGNORE wcdata SET wc='LEO INXA' WHERE wc=='LEO_INXA'; -- FIXED: improper clli format character s/_/ / (260627,2606270,2606272,2606273,2606275,2606276,2606277,2606278)
DELETE FROM wcdata WHERE wc=='LEO_INXA'; -- FIXED: improper clli format character s/_/ / (260627,2606270,2606272,2606273,2606275,2606276,2606277,2606278)
---------------------
UPDATE pooldata   SET switch='POE INXARS0' WHERE switch=='POE_INXARS0'; -- FIXED: improper clli format character s/_/ / (2606390,2606393,2606396,2606397) [260-639-0,260-639-3,260-639-6,260-639-7]
UPDATE prefixdata SET switch='POE INXARS0' WHERE switch=='POE_INXARS0'; -- FIXED: improper clli format character s/_/ / (260639,2606390,2606393,2606396,2606397) [260-639]
UPDATE OR IGNORE switchdata SET switch='POE INXARS0' WHERE switch=='POE_INXARS0'; -- FIXED: improper clli format character s/_/ / (260639,2606390,2606393,2606396,2606397) [POE_INXARS0]
DELETE FROM switchdata WHERE switch=='POE_INXARS0'; -- FIXED: improper clli format character s/_/ / (260639,2606390,2606393,2606396,2606397) [POE_INXARS0]
UPDATE OR IGNORE wcdata SET wc='POE INXA' WHERE wc=='POE_INXA'; -- FIXED: improper clli format character s/_/ / (260639,2606390,2606393,2606396,2606397)
DELETE FROM wcdata WHERE wc=='POE_INXA'; -- FIXED: improper clli format character s/_/ / (260639,2606390,2606393,2606396,2606397)
---------------------
UPDATE pooldata   SET switch='MAN WVJURS1' WHERE switch=='MAN_WVJURS1'; -- FIXED: improper clli format character s/_/ / (3045830,3045832,3045836,3045837,3045838,3045839) [304-583-0,304-583-2,304-583-6,304-583-7,304-583-8,304-583-9]
UPDATE prefixdata SET switch='MAN WVJURS1' WHERE switch=='MAN_WVJURS1'; -- FIXED: improper clli format character s/_/ / (304583,3045830,3045832,3045834,3045836,3045837,3045838,3045839) [304-583]
UPDATE OR IGNORE switchdata SET switch='MAN WVJURS1' WHERE switch=='MAN_WVJURS1'; -- FIXED: improper clli format character s/_/ / (304583,3045830,3045832,3045836,3045837,3045838,3045839) [MAN_WVJURS1]
DELETE FROM switchdata WHERE switch=='MAN_WVJURS1'; -- FIXED: improper clli format character s/_/ / (304583,3045830,3045832,3045836,3045837,3045838,3045839) [MAN_WVJURS1]
UPDATE OR IGNORE wcdata SET wc='MAN WVJU' WHERE wc=='MAN_WVJU'; -- FIXED: improper clli format character s/_/ / (304583,3045830,3045832,3045836,3045837,3045838,3045839)
DELETE FROM wcdata WHERE wc=='MAN_WVJU'; -- FIXED: improper clli format character s/_/ / (304583,3045830,3045832,3045836,3045837,3045838,3045839)
---------------------
UPDATE prefixdata SET switch='WAR WVXADS0' WHERE switch=='WAR_WVXADS0'; -- FIXED: improper clli format character s/_/ / (304875) [304-875]
UPDATE OR IGNORE switchdata SET switch='WAR WVXADS0' WHERE switch=='WAR_WVXADS0'; -- FIXED: improper clli format character s/_/ / (304875) [WAR_WVXADS0]
DELETE FROM switchdata WHERE switch=='WAR_WVXADS0'; -- FIXED: improper clli format character s/_/ / (304875) [WAR_WVXADS0]
UPDATE OR IGNORE wcdata SET wc='WAR WVXA' WHERE wc=='WAR_WVXA'; -- FIXED: improper clli format character s/_/ / (304875)
DELETE FROM wcdata WHERE wc=='WAR_WVXA'; -- FIXED: improper clli format character s/_/ / (304875)
---------------------
UPDATE prefixdata SET switch='DIX NEXSRS0' WHERE switch=='DIX_NEXSRS0'; -- FIXED: improper clli format character s/_/ / (308682) [308-682]
UPDATE OR IGNORE switchdata SET switch='DIX NEXSRS0' WHERE switch=='DIX_NEXSRS0'; -- FIXED: improper clli format character s/_/ / (308682) [DIX_NEXSRS0]
DELETE FROM switchdata WHERE switch=='DIX_NEXSRS0'; -- FIXED: improper clli format character s/_/ / (308682) [DIX_NEXSRS0]
UPDATE OR IGNORE wcdata SET wc='DIX NEXS' WHERE wc=='DIX_NEXS'; -- FIXED: improper clli format character s/_/ / (308682)
DELETE FROM wcdata WHERE wc=='DIX_NEXS'; -- FIXED: improper clli format character s/_/ / (308682)
---------------------
UPDATE prefixdata SET switch='ORD NEXHDS0' WHERE switch=='ORD_NEXHDS0'; -- FIXED: improper clli format character s/_/ / (308728) [308-728]
UPDATE OR IGNORE switchdata SET switch='ORD NEXHDS0' WHERE switch=='ORD_NEXHDS0'; -- FIXED: improper clli format character s/_/ / (308728) [ORD_NEXHDS0]
DELETE FROM switchdata WHERE switch=='ORD_NEXHDS0'; -- FIXED: improper clli format character s/_/ / (308728) [ORD_NEXHDS0]
UPDATE OR IGNORE wcdata SET wc='ORD NEXH' WHERE wc=='ORD_NEXH'; -- FIXED: improper clli format character s/_/ / (308728)
DELETE FROM wcdata WHERE wc=='ORD_NEXH'; -- FIXED: improper clli format character s/_/ / (308728)
---------------------
UPDATE prefixdata SET switch='JOY ILXDRS0' WHERE switch=='JOY_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (309584) [309-584]
UPDATE OR IGNORE switchdata SET switch='JOY ILXDRS0' WHERE switch=='JOY_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (309584) [JOY_ILXDRS0]
DELETE FROM switchdata WHERE switch=='JOY_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (309584) [JOY_ILXDRS0]
UPDATE OR IGNORE wcdata SET wc='JOY ILXD' WHERE wc=='JOY_ILXD'; -- FIXED: improper clli format character s/_/ / (309584)
DELETE FROM wcdata WHERE wc=='JOY_ILXD'; -- FIXED: improper clli format character s/_/ / (309584)
---------------------
UPDATE prefixdata SET switch='RIO ILXDRS0' WHERE switch=='RIO_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (309872) [309-872]
UPDATE OR IGNORE switchdata SET switch='RIO ILXDRS0' WHERE switch=='RIO_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (309872) [RIO_ILXDRS0]
DELETE FROM switchdata WHERE switch=='RIO_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (309872) [RIO_ILXDRS0]
UPDATE OR IGNORE wcdata SET wc='RIO ILXD' WHERE wc=='RIO_ILXD'; -- FIXED: improper clli format character s/_/ / (309872)
DELETE FROM wcdata WHERE wc=='RIO_ILXD'; -- FIXED: improper clli format character s/_/ / (309872)
---------------------
UPDATE prefixdata SET switch='RIO ILABRS0' WHERE switch=='RIO_ILABRS0'; -- FIXED: improper clli format character s/_/ / (309972) [309-972]
UPDATE OR IGNORE switchdata SET switch='RIO ILABRS0' WHERE switch=='RIO_ILABRS0'; -- FIXED: improper clli format character s/_/ / (309972) [RIO_ILABRS0]
DELETE FROM switchdata WHERE switch=='RIO_ILABRS0'; -- FIXED: improper clli format character s/_/ / (309972) [RIO_ILABRS0]
UPDATE OR IGNORE wcdata SET wc='RIO ILAB' WHERE wc=='RIO_ILAB'; -- FIXED: improper clli format character s/_/ / (309972)
DELETE FROM wcdata WHERE wc=='RIO_ILAB'; -- FIXED: improper clli format character s/_/ / (309972)
---------------------
UPDATE prefixdata SET switch='IDA LAXARS1' WHERE switch=='IDA_LAXARS1'; -- FIXED: improper clli format character s/_/ / (318284) [318-284]
UPDATE OR IGNORE switchdata SET switch='IDA LAXARS1' WHERE switch=='IDA_LAXARS1'; -- FIXED: improper clli format character s/_/ / (318284) [IDA_LAXARS1]
DELETE FROM switchdata WHERE switch=='IDA_LAXARS1'; -- FIXED: improper clli format character s/_/ / (318284) [IDA_LAXARS1]
UPDATE OR IGNORE wcdata SET wc='IDA LAXA' WHERE wc=='IDA_LAXA'; -- FIXED: improper clli format character s/_/ / (318284)
DELETE FROM wcdata WHERE wc=='IDA_LAXA'; -- FIXED: improper clli format character s/_/ / (318284)
---------------------
UPDATE pooldata   SET switch='TOW TXXARS0' WHERE switch=='TOW_TXXARS0'; -- FIXED: improper clli format character s/_/ / (3253791,3253792,3253793,3257150,3257159) [325-379-1,325-379-2,325-379-3,325-715-0,325-715-9]
UPDATE prefixdata SET switch='TOW TXXARS0' WHERE switch=='TOW_TXXARS0'; -- FIXED: improper clli format character s/_/ / (325379,3253791,3253792,3253793,325715,3257150,3257159) [325-379,325-715]
UPDATE OR IGNORE switchdata SET switch='TOW TXXARS0' WHERE switch=='TOW_TXXARS0'; -- FIXED: improper clli format character s/_/ / (325379,3253791,3253792,3253793,325715,3257150,3257159) [TOW_TXXARS0]
DELETE FROM switchdata WHERE switch=='TOW_TXXARS0'; -- FIXED: improper clli format character s/_/ / (325379,3253791,3253792,3253793,325715,3257150,3257159) [TOW_TXXARS0]
UPDATE OR IGNORE wcdata SET wc='TOW TXXA' WHERE wc=='TOW_TXXA'; -- FIXED: improper clli format character s/_/ / (325379,3253791,3253792,3253793,325715,3257150,3257159)
DELETE FROM wcdata WHERE wc=='TOW_TXXA'; -- FIXED: improper clli format character s/_/ / (325379,3253791,3253792,3253793,325715,3257150,3257159)
---------------------
UPDATE prefixdata SET switch='OPP ALXADS0' WHERE switch=='OPP_ALXADS0'; -- FIXED: improper clli format character s/_/ / (334493) [334-493]
UPDATE OR IGNORE switchdata SET switch='OPP ALXADS0' WHERE switch=='OPP_ALXADS0'; -- FIXED: improper clli format character s/_/ / (334493) [OPP_ALXADS0]
DELETE FROM switchdata WHERE switch=='OPP_ALXADS0'; -- FIXED: improper clli format character s/_/ / (334493) [OPP_ALXADS0]
UPDATE OR IGNORE wcdata SET wc='OPP ALXA' WHERE wc=='OPP_ALXA'; -- FIXED: improper clli format character s/_/ / (334493)
DELETE FROM wcdata WHERE wc=='OPP_ALXA'; -- FIXED: improper clli format character s/_/ / (334493)
---------------------
UPDATE prefixdata SET switch='OPP AL08GT0' WHERE switch=='OPP_AL08GT0'; -- FIXED: improper clli format character s/_/ / (334772) [334-772]
UPDATE OR IGNORE switchdata SET switch='OPP AL08GT0' WHERE switch=='OPP_AL08GT0'; -- FIXED: improper clli format character s/_/ / (334772) [OPP_AL08GT0]
DELETE FROM switchdata WHERE switch=='OPP_AL08GT0'; -- FIXED: improper clli format character s/_/ / (334772) [OPP_AL08GT0]
UPDATE OR IGNORE wcdata SET wc='OPP AL08' WHERE wc=='OPP_AL08'; -- FIXED: improper clli format character s/_/ / (334772)
DELETE FROM wcdata WHERE wc=='OPP_AL08'; -- FIXED: improper clli format character s/_/ / (334772)
---------------------
UPDATE prefixdata SET switch='ONG NEXLRS0' WHERE switch=='ONG_NEXLRS0'; -- FIXED: improper clli format character s/_/ / (402284) [402-284]
UPDATE OR IGNORE switchdata SET switch='ONG NEXLRS0' WHERE switch=='ONG_NEXLRS0'; -- FIXED: improper clli format character s/_/ / (402284) [ONG_NEXLRS0]
DELETE FROM switchdata WHERE switch=='ONG_NEXLRS0'; -- FIXED: improper clli format character s/_/ / (402284) [ONG_NEXLRS0]
UPDATE OR IGNORE wcdata SET wc='ONG NEXL' WHERE wc=='ONG_NEXL'; -- FIXED: improper clli format character s/_/ / (402284)
DELETE FROM wcdata WHERE wc=='ONG_NEXL'; -- FIXED: improper clli format character s/_/ / (402284)
---------------------
UPDATE prefixdata SET switch='ROY MTXCRS1' WHERE switch=='ROY_MTXCRS1'; -- FIXED: improper clli format character s/_/ / (406464) [406-464]
UPDATE OR IGNORE switchdata SET switch='ROY MTXCRS1' WHERE switch=='ROY_MTXCRS1'; -- FIXED: improper clli format character s/_/ / (406464) [ROY_MTXCRS1]
DELETE FROM switchdata WHERE switch=='ROY_MTXCRS1'; -- FIXED: improper clli format character s/_/ / (406464) [ROY_MTXCRS1]
UPDATE OR IGNORE wcdata SET wc='ROY MTXC' WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (406464)
DELETE FROM wcdata WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (406464)
---------------------
UPDATE pooldata   SET switch='ULM MTMARS1' WHERE switch=='ULM_MTMARS1'; -- FIXED: improper clli format character s/_/ / (4068662,4068663) [406-866-2,406-866-3]
UPDATE prefixdata SET switch='ULM MTMARS1' WHERE switch=='ULM_MTMARS1'; -- FIXED: improper clli format character s/_/ / (406866,4068662,4068663) [406-866]
UPDATE OR IGNORE switchdata SET switch='ULM MTMARS1' WHERE switch=='ULM_MTMARS1'; -- FIXED: improper clli format character s/_/ / (406866,4068662,4068663) [ULM_MTMARS1]
DELETE FROM switchdata WHERE switch=='ULM_MTMARS1'; -- FIXED: improper clli format character s/_/ / (406866,4068662,4068663) [ULM_MTMARS1]
UPDATE OR IGNORE wcdata SET wc='ULM MTMA' WHERE wc=='ULM_MTMA'; -- FIXED: improper clli format character s/_/ / (406866,4068662,4068663)
DELETE FROM wcdata WHERE wc=='ULM_MTMA'; -- FIXED: improper clli format character s/_/ / (406866,4068662,4068663)
---------------------
UPDATE prefixdata SET switch='ROY MTXCCM1' WHERE switch=='ROY_MTXCCM1'; -- FIXED: improper clli format character s/_/ / (406954) [406-954]
UPDATE OR IGNORE switchdata SET switch='ROY MTXCCM1' WHERE switch=='ROY_MTXCCM1'; -- FIXED: improper clli format character s/_/ / (406954) [ROY_MTXCCM1]
DELETE FROM switchdata WHERE switch=='ROY_MTXCCM1'; -- FIXED: improper clli format character s/_/ / (406954) [ROY_MTXCCM1]
UPDATE OR IGNORE wcdata SET wc='ROY MTXC' WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (406954)
DELETE FROM wcdata WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (406954)
---------------------
UPDATE pooldata   SET switch='LEE MAHIDS0' WHERE switch=='LEE_MAHIDS0'; -- FIXED: improper clli format character s/_/ / (4133949) [413-394-9]
UPDATE prefixdata SET switch='LEE MAHIDS0' WHERE switch=='LEE_MAHIDS0'; -- FIXED: improper clli format character s/_/ / (413243,413394,4133949) [413-243,413-394]
UPDATE OR IGNORE switchdata SET switch='LEE MAHIDS0' WHERE switch=='LEE_MAHIDS0'; -- FIXED: improper clli format character s/_/ / (413243,413394,4133949) [LEE_MAHIDS0]
DELETE FROM switchdata WHERE switch=='LEE_MAHIDS0'; -- FIXED: improper clli format character s/_/ / (413243,413394,4133949) [LEE_MAHIDS0]
UPDATE OR IGNORE wcdata SET wc='LEE MAHI' WHERE wc=='LEE_MAHI'; -- FIXED: improper clli format character s/_/ / (413243,413394,4133949)
DELETE FROM wcdata WHERE wc=='LEE_MAHI'; -- FIXED: improper clli format character s/_/ / (413243,413394,4133949)
---------------------
UPDATE prefixdata SET switch='AVA MOXADS0' WHERE switch=='AVA_MOXADS0'; -- FIXED: improper clli format character s/_/ / (417653,417683) [417-653,417-683]
UPDATE OR IGNORE switchdata SET switch='AVA MOXADS0' WHERE switch=='AVA_MOXADS0'; -- FIXED: improper clli format character s/_/ / (417653,417683) [AVA_MOXADS0]
DELETE FROM switchdata WHERE switch=='AVA_MOXADS0'; -- FIXED: improper clli format character s/_/ / (417653,417683) [AVA_MOXADS0]
UPDATE OR IGNORE wcdata SET wc='AVA MOXA' WHERE wc=='AVA_MOXA'; -- FIXED: improper clli format character s/_/ / (417653,417683)
DELETE FROM wcdata WHERE wc=='AVA_MOXA'; -- FIXED: improper clli format character s/_/ / (417653,417683)
---------------------
UPDATE prefixdata SET switch='ADA OHXARS1' WHERE switch=='ADA_OHXARS1'; -- FIXED: improper clli format character s/_/ / (419634,419772) [419-634,419-772]
UPDATE OR IGNORE switchdata SET switch='ADA OHXARS1' WHERE switch=='ADA_OHXARS1'; -- FIXED: improper clli format character s/_/ / (419634,419772) [ADA_OHXARS1]
DELETE FROM switchdata WHERE switch=='ADA_OHXARS1'; -- FIXED: improper clli format character s/_/ / (419634,419772) [ADA_OHXARS1]
UPDATE OR IGNORE wcdata SET wc='ADA OHXA' WHERE wc=='ADA_OHXA'; -- FIXED: improper clli format character s/_/ / (419634,419772)
DELETE FROM wcdata WHERE wc=='ADA_OHXA'; -- FIXED: improper clli format character s/_/ / (419634,419772)
---------------------
UPDATE pooldata   SET switch='NEY OHXARS0' WHERE switch=='NEY_OHXARS0'; -- FIXED: improper clli format character s/_/ / (4196582,4196584,4196588) [419-658-2,419-658-4,419-658-8]
UPDATE prefixdata SET switch='NEY OHXARS0' WHERE switch=='NEY_OHXARS0'; -- FIXED: improper clli format character s/_/ / (419658,4196582,4196584,4196588) [419-658]
UPDATE OR IGNORE switchdata SET switch='NEY OHXARS0' WHERE switch=='NEY_OHXARS0'; -- FIXED: improper clli format character s/_/ / (419658,4196582,4196584,4196588) [NEY_OHXARS0]
DELETE FROM switchdata WHERE switch=='NEY_OHXARS0'; -- FIXED: improper clli format character s/_/ / (419658,4196582,4196584,4196588) [NEY_OHXARS0]
UPDATE OR IGNORE wcdata SET wc='NEY OHXA' WHERE wc=='NEY_OHXA'; -- FIXED: improper clli format character s/_/ / (419658,4196582,4196584,4196588)
DELETE FROM wcdata WHERE wc=='NEY_OHXA'; -- FIXED: improper clli format character s/_/ / (419658,4196582,4196584,4196588)
---------------------
UPDATE prefixdata SET switch='LOA UTXCRS1' WHERE switch=='LOA_UTXCRS1'; -- FIXED: improper clli format character s/_/ / (435836) [435-836]
UPDATE OR IGNORE switchdata SET switch='LOA UTXCRS1' WHERE switch=='LOA_UTXCRS1'; -- FIXED: improper clli format character s/_/ / (435836) [LOA_UTXCRS1]
DELETE FROM switchdata WHERE switch=='LOA_UTXCRS1'; -- FIXED: improper clli format character s/_/ / (435836) [LOA_UTXCRS1]
UPDATE OR IGNORE wcdata SET wc='LOA UTXC' WHERE wc=='LOA_UTXC'; -- FIXED: improper clli format character s/_/ / (435836)
DELETE FROM wcdata WHERE wc=='LOA_UTXC'; -- FIXED: improper clli format character s/_/ / (435836)
---------------------
UPDATE prefixdata SET switch='OLA ARXADS0' WHERE switch=='OLA_ARXADS0'; -- FIXED: improper clli format character s/_/ / (479489) [479-489]
UPDATE OR IGNORE switchdata SET switch='OLA ARXADS0' WHERE switch=='OLA_ARXADS0'; -- FIXED: improper clli format character s/_/ / (479489) [OLA_ARXADS0]
DELETE FROM switchdata WHERE switch=='OLA_ARXADS0'; -- FIXED: improper clli format character s/_/ / (479489) [OLA_ARXADS0]
UPDATE OR IGNORE wcdata SET wc='OLA ARXA' WHERE wc=='OLA_ARXA'; -- FIXED: improper clli format character s/_/ / (479489)
DELETE FROM wcdata WHERE wc=='OLA_ARXA'; -- FIXED: improper clli format character s/_/ / (479489)
---------------------
UPDATE prefixdata SET switch='COY ARXARS0' WHERE switch=='COY_ARXARS0'; -- FIXED: improper clli format character s/_/ / (501275) [501-275]
UPDATE OR IGNORE switchdata SET switch='COY ARXARS0' WHERE switch=='COY_ARXARS0'; -- FIXED: improper clli format character s/_/ / (501275) [COY_ARXARS0]
DELETE FROM switchdata WHERE switch=='COY_ARXARS0'; -- FIXED: improper clli format character s/_/ / (501275) [COY_ARXARS0]
UPDATE OR IGNORE wcdata SET wc='COY ARXA' WHERE wc=='COY_ARXA'; -- FIXED: improper clli format character s/_/ / (501275)
DELETE FROM wcdata WHERE wc=='COY_ARXA'; -- FIXED: improper clli format character s/_/ / (501275)
---------------------
UPDATE pooldata   SET switch='ELK WA01DS0' WHERE switch=='ELK_WA01DS0'; -- FIXED: improper clli format character s/_/ / (5092920,5092921,5092922,5092925,5092927,5092928,5092929) [509-292-0,509-292-1,509-292-2,509-292-5,509-292-7,509-292-8,509-292-9]
UPDATE prefixdata SET switch='ELK WA01DS0' WHERE switch=='ELK_WA01DS0'; -- FIXED: improper clli format character s/_/ / (509292,5092920,5092921,5092922,5092923,5092924,5092925,5092926,5092927,5092928,...) [509-292]
UPDATE pcdata SET switch='ELK WA01DS0' WHERE switch=='ELK_WA01DS0'; -- FIXED: improper clli format character s/_/ / (509292,5092920,5092921,5092922,5092925,5092927,5092928,5092929) [248-227-024]
UPDATE OR IGNORE switchdata SET switch='ELK WA01DS0' WHERE switch=='ELK_WA01DS0'; -- FIXED: improper clli format character s/_/ / (509292,5092920,5092921,5092922,5092925,5092927,5092928,5092929) [ELK_WA01DS0]
DELETE FROM switchdata WHERE switch=='ELK_WA01DS0'; -- FIXED: improper clli format character s/_/ / (509292,5092920,5092921,5092922,5092925,5092927,5092928,5092929) [ELK_WA01DS0]
UPDATE OR IGNORE wcdata SET wc='ELK WA01' WHERE wc=='ELK_WA01'; -- FIXED: improper clli format character s/_/ / (509292,5092920,5092921,5092922,5092925,5092927,5092928,5092929)
DELETE FROM wcdata WHERE wc=='ELK_WA01'; -- FIXED: improper clli format character s/_/ / (509292,5092920,5092921,5092922,5092925,5092927,5092928,5092929)
---------------------
UPDATE prefixdata SET switch='AJO AZXCDS0' WHERE switch=='AJO_AZXCDS0'; -- FIXED: improper clli format character s/_/ / (520387) [520-387]
UPDATE OR IGNORE switchdata SET switch='AJO AZXCDS0' WHERE switch=='AJO_AZXCDS0'; -- FIXED: improper clli format character s/_/ / (520387) [AJO_AZXCDS0]
DELETE FROM switchdata WHERE switch=='AJO_AZXCDS0'; -- FIXED: improper clli format character s/_/ / (520387) [AJO_AZXCDS0]
UPDATE OR IGNORE wcdata SET wc='AJO AZXC' WHERE wc=='AJO_AZXC'; -- FIXED: improper clli format character s/_/ / (520387)
DELETE FROM wcdata WHERE wc=='AJO_AZXC'; -- FIXED: improper clli format character s/_/ / (520387)
---------------------
UPDATE prefixdata SET switch='BLY ORXARS0' WHERE switch=='BLY_ORXARS0'; -- FIXED: improper clli format character s/_/ / (541353) [541-353]
UPDATE OR IGNORE switchdata SET switch='BLY ORXARS0' WHERE switch=='BLY_ORXARS0'; -- FIXED: improper clli format character s/_/ / (541353) [BLY_ORXARS0]
DELETE FROM switchdata WHERE switch=='BLY_ORXARS0'; -- FIXED: improper clli format character s/_/ / (541353) [BLY_ORXARS0]
UPDATE OR IGNORE wcdata SET wc='BLY ORXA' WHERE wc=='BLY_ORXA'; -- FIXED: improper clli format character s/_/ / (541353)
DELETE FROM wcdata WHERE wc=='BLY_ORXA'; -- FIXED: improper clli format character s/_/ / (541353)
---------------------
UPDATE prefixdata SET switch='JAL NMXARS1' WHERE switch=='JAL_NMXARS1'; -- FIXED: improper clli format character s/_/ / (575395) [575-395]
UPDATE OR IGNORE switchdata SET switch='JAL NMXARS1' WHERE switch=='JAL_NMXARS1'; -- FIXED: improper clli format character s/_/ / (575395) [JAL_NMXARS1]
DELETE FROM switchdata WHERE switch=='JAL_NMXARS1'; -- FIXED: improper clli format character s/_/ / (575395) [JAL_NMXARS1]
UPDATE OR IGNORE wcdata SET wc='JAL NMXA' WHERE wc=='JAL_NMXA'; -- FIXED: improper clli format character s/_/ / (575395)
DELETE FROM wcdata WHERE wc=='JAL_NMXA'; -- FIXED: improper clli format character s/_/ / (575395)
---------------------
UPDATE prefixdata SET switch='ROY NMXCRS0' WHERE switch=='ROY_NMXCRS0'; -- FIXED: improper clli format character s/_/ / (575485) [575-485]
UPDATE OR IGNORE switchdata SET switch='ROY NMXCRS0' WHERE switch=='ROY_NMXCRS0'; -- FIXED: improper clli format character s/_/ / (575485) [ROY_NMXCRS0]
DELETE FROM switchdata WHERE switch=='ROY_NMXCRS0'; -- FIXED: improper clli format character s/_/ / (575485) [ROY_NMXCRS0]
UPDATE OR IGNORE wcdata SET wc='ROY NMXC' WHERE wc=='ROY_NMXC'; -- FIXED: improper clli format character s/_/ / (575485)
DELETE FROM wcdata WHERE wc=='ROY_NMXC'; -- FIXED: improper clli format character s/_/ / (575485)
---------------------
UPDATE pooldata   SET switch='ADA OKMADS1' WHERE switch=='ADA_OKMADS1'; -- FIXED: improper clli format character s/_/ / (5802350,5802720,5802721,5802722,5802725,5802726,5802729,5803100,5803101,5803102,...) [580-235-0,580-272-0,580-272-1,580-272-2,580-272-5,580-272-6,580-272-9,580-310-0,580-310-1,580-310-2,...]
UPDATE prefixdata SET switch='ADA OKMADS1' WHERE switch=='ADA_OKMADS1'; -- FIXED: improper clli format character s/_/ / (580235,5802350,580272,5802720,5802721,5802722,5802725,5802726,5802729,580310,...) [580-235,580-272,580-310,580-332,580-421,580-436,580-559]
UPDATE pcdata SET switch='ADA OKMADS1' WHERE switch=='ADA_OKMADS1'; -- FIXED: improper clli format character s/_/ / (580235,5802350,580272,5802720,5802721,5802722,5802725,5802726,5802729,580310,...) [249-130-006]
UPDATE OR IGNORE switchdata SET switch='ADA OKMADS1' WHERE switch=='ADA_OKMADS1'; -- FIXED: improper clli format character s/_/ / (580235,5802350,580272,5802720,5802721,5802722,5802725,5802726,5802729,580310,...) [ADA_OKMADS1]
DELETE FROM switchdata WHERE switch=='ADA_OKMADS1'; -- FIXED: improper clli format character s/_/ / (580235,5802350,580272,5802720,5802721,5802722,5802725,5802726,5802729,580310,...) [ADA_OKMADS1]
UPDATE OR IGNORE wcdata SET wc='ADA OKMA' WHERE wc=='ADA_OKMA'; -- FIXED: improper clli format character s/_/ / (580235,5802350,580272,5802720,5802721,5802722,5802725,5802726,5802729,580310,...)
DELETE FROM wcdata WHERE wc=='ADA_OKMA'; -- FIXED: improper clli format character s/_/ / (580235,5802350,580272,5802720,5802721,5802722,5802725,5802726,5802729,580310,...)
---------------------
UPDATE prefixdata SET switch='ADA OKBSDS0' WHERE switch=='ADA_OKBSDS0'; -- FIXED: improper clli format character s/_/ / (580427) [580-427]
UPDATE OR IGNORE switchdata SET switch='ADA OKBSDS0' WHERE switch=='ADA_OKBSDS0'; -- FIXED: improper clli format character s/_/ / (580427) [ADA_OKBSDS0]
DELETE FROM switchdata WHERE switch=='ADA_OKBSDS0'; -- FIXED: improper clli format character s/_/ / (580427) [ADA_OKBSDS0]
UPDATE OR IGNORE wcdata SET wc='ADA OKBS' WHERE wc=='ADA_OKBS'; -- FIXED: improper clli format character s/_/ / (580427)
DELETE FROM wcdata WHERE wc=='ADA_OKBS'; -- FIXED: improper clli format character s/_/ / (580427)
---------------------
UPDATE prefixdata SET switch='EVA OKXARS1' WHERE switch=='EVA_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580545) [580-545]
UPDATE OR IGNORE switchdata SET switch='EVA OKXARS1' WHERE switch=='EVA_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580545) [EVA_OKXARS1]
DELETE FROM switchdata WHERE switch=='EVA_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580545) [EVA_OKXARS1]
UPDATE OR IGNORE wcdata SET wc='EVA OKXA' WHERE wc=='EVA_OKXA'; -- FIXED: improper clli format character s/_/ / (580545)
DELETE FROM wcdata WHERE wc=='EVA_OKXA'; -- FIXED: improper clli format character s/_/ / (580545)
---------------------
UPDATE prefixdata SET switch='MAY OKXARS1' WHERE switch=='MAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580689) [580-689]
UPDATE OR IGNORE switchdata SET switch='MAY OKXARS1' WHERE switch=='MAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580689) [MAY_OKXARS1]
DELETE FROM switchdata WHERE switch=='MAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580689) [MAY_OKXARS1]
UPDATE OR IGNORE wcdata SET wc='MAY OKXA' WHERE wc=='MAY_OKXA'; -- FIXED: improper clli format character s/_/ / (580689)
DELETE FROM wcdata WHERE wc=='MAY_OKXA'; -- FIXED: improper clli format character s/_/ / (580689)
---------------------
UPDATE prefixdata SET switch='FAY OKXARS1' WHERE switch=='FAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580887) [580-887]
UPDATE OR IGNORE switchdata SET switch='FAY OKXARS1' WHERE switch=='FAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580887) [FAY_OKXARS1]
DELETE FROM switchdata WHERE switch=='FAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (580887) [FAY_OKXARS1]
UPDATE OR IGNORE wcdata SET wc='FAY OKXA' WHERE wc=='FAY_OKXA'; -- FIXED: improper clli format character s/_/ / (580887)
DELETE FROM wcdata WHERE wc=='FAY_OKXA'; -- FIXED: improper clli format character s/_/ / (580887)
---------------------
UPDATE pooldata   SET switch='TEA SDCORS1' WHERE switch=='TEA_SDCORS1'; -- FIXED: improper clli format character s/_/ / (6053682,6053685,6053689) [605-368-2,605-368-5,605-368-9]
UPDATE prefixdata SET switch='TEA SDCORS1' WHERE switch=='TEA_SDCORS1'; -- FIXED: improper clli format character s/_/ / (605368,6053682,6053685,6053689) [605-368]
UPDATE OR IGNORE switchdata SET switch='TEA SDCORS1' WHERE switch=='TEA_SDCORS1'; -- FIXED: improper clli format character s/_/ / (605368,6053682,6053685,6053689) [TEA_SDCORS1]
DELETE FROM switchdata WHERE switch=='TEA_SDCORS1'; -- FIXED: improper clli format character s/_/ / (605368,6053682,6053685,6053689) [TEA_SDCORS1]
UPDATE OR IGNORE wcdata SET wc='TEA SDCO' WHERE wc=='TEA_SDCO'; -- FIXED: improper clli format character s/_/ / (605368,6053682,6053685,6053689)
DELETE FROM wcdata WHERE wc=='TEA_SDCO'; -- FIXED: improper clli format character s/_/ / (605368,6053682,6053685,6053689)
---------------------
UPDATE pooldata   SET switch='ADA MIMNDS0' WHERE switch=='ADA_MIMNDS0'; -- FIXED: improper clli format character s/_/ / (6166760,6166761,6166762,6166763,6166764,6166765,6166768,6166769,6166820,6166821,...) [616-676-0,616-676-1,616-676-2,616-676-3,616-676-4,616-676-5,616-676-8,616-676-9,616-682-0,616-682-1,...]
UPDATE prefixdata SET switch='ADA MIMNDS0' WHERE switch=='ADA_MIMNDS0'; -- FIXED: improper clli format character s/_/ / (616676,6166760,6166761,6166762,6166763,6166764,6166765,6166768,6166769,616682,...) [616-676,616-682,616-787]
UPDATE pcdata SET switch='ADA MIMNDS0' WHERE switch=='ADA_MIMNDS0'; -- FIXED: improper clli format character s/_/ / (616676,6166760,6166761,6166762,6166763,6166764,6166765,6166768,6166769,616682,...) [250-052-023]
UPDATE OR IGNORE switchdata SET switch='ADA MIMNDS0' WHERE switch=='ADA_MIMNDS0'; -- FIXED: improper clli format character s/_/ / (616676,6166760,6166761,6166762,6166763,6166764,6166765,6166768,6166769,616682,...) [ADA_MIMNDS0]
DELETE FROM switchdata WHERE switch=='ADA_MIMNDS0'; -- FIXED: improper clli format character s/_/ / (616676,6166760,6166761,6166762,6166763,6166764,6166765,6166768,6166769,616682,...) [ADA_MIMNDS0]
UPDATE OR IGNORE wcdata SET wc='ADA MIMN' WHERE wc=='ADA_MIMN'; -- FIXED: improper clli format character s/_/ / (616676,6166760,6166761,6166762,6166763,6166764,6166765,6166768,6166769,616682,...)
DELETE FROM wcdata WHERE wc=='ADA_MIMN'; -- FIXED: improper clli format character s/_/ / (616676,6166760,6166761,6166762,6166763,6166764,6166765,6166768,6166769,616682,...)
---------------------
UPDATE pooldata   SET switch='AVA ILXERS0' WHERE switch=='AVA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (6184261,6184263,6184269) [618-426-1,618-426-3,618-426-9]
UPDATE prefixdata SET switch='AVA ILXERS0' WHERE switch=='AVA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618426,6184261,6184263,6184264,6184269) [618-426]
UPDATE OR IGNORE switchdata SET switch='AVA ILXERS0' WHERE switch=='AVA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618426,6184261,6184263,6184264,6184269) [AVA_ILXERS0]
DELETE FROM switchdata WHERE switch=='AVA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618426,6184261,6184263,6184264,6184269) [AVA_ILXERS0]
UPDATE OR IGNORE wcdata SET wc='AVA ILXE' WHERE wc=='AVA_ILXE'; -- FIXED: improper clli format character s/_/ / (618426,6184261,6184263,6184264,6184269)
DELETE FROM wcdata WHERE wc=='AVA_ILXE'; -- FIXED: improper clli format character s/_/ / (618426,6184261,6184263,6184264,6184269)
---------------------
UPDATE prefixdata SET switch='INA ILXERS0' WHERE switch=='INA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618437) [618-437]
UPDATE OR IGNORE switchdata SET switch='INA ILXERS0' WHERE switch=='INA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618437) [INA_ILXERS0]
DELETE FROM switchdata WHERE switch=='INA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618437) [INA_ILXERS0]
UPDATE OR IGNORE wcdata SET wc='INA ILXE' WHERE wc=='INA_ILXE'; -- FIXED: improper clli format character s/_/ / (618437)
DELETE FROM wcdata WHERE wc=='INA_ILXE'; -- FIXED: improper clli format character s/_/ / (618437)
---------------------
UPDATE pooldata   SET switch='DOW ILXERS0' WHERE switch=='DOW_ILXERS0'; -- FIXED: improper clli format character s/_/ / (6188851,6188852,6188855,6188859) [618-885-1,618-885-2,618-885-5,618-885-9]
UPDATE prefixdata SET switch='DOW ILXERS0' WHERE switch=='DOW_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618885,6188851,6188852,6188855,6188859) [618-885]
UPDATE OR IGNORE switchdata SET switch='DOW ILXERS0' WHERE switch=='DOW_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618885,6188851,6188852,6188855,6188859) [DOW_ILXERS0]
DELETE FROM switchdata WHERE switch=='DOW_ILXERS0'; -- FIXED: improper clli format character s/_/ / (618885,6188851,6188852,6188855,6188859) [DOW_ILXERS0]
UPDATE OR IGNORE wcdata SET wc='DOW ILXE' WHERE wc=='DOW_ILXE'; -- FIXED: improper clli format character s/_/ / (618885,6188851,6188852,6188855,6188859)
DELETE FROM wcdata WHERE wc=='DOW_ILXE'; -- FIXED: improper clli format character s/_/ / (618885,6188851,6188852,6188855,6188859)
---------------------
UPDATE prefixdata SET switch='RAY NDXADS2' WHERE switch=='RAY_NDXADS2'; -- FIXED: improper clli format character s/_/ / (701568) [701-568]
UPDATE OR IGNORE switchdata SET switch='RAY NDXADS2' WHERE switch=='RAY_NDXADS2'; -- FIXED: improper clli format character s/_/ / (701568) [RAY_NDXADS2]
DELETE FROM switchdata WHERE switch=='RAY_NDXADS2'; -- FIXED: improper clli format character s/_/ / (701568) [RAY_NDXADS2]
UPDATE OR IGNORE wcdata SET wc='RAY NDXA' WHERE wc=='RAY_NDXA'; -- FIXED: improper clli format character s/_/ / (701568)
DELETE FROM wcdata WHERE wc=='RAY_NDXA'; -- FIXED: improper clli format character s/_/ / (701568)
---------------------
UPDATE pooldata   SET switch='MAX NDXARS6' WHERE switch=='MAX_NDXARS6'; -- FIXED: improper clli format character s/_/ / (7016790,7016791,7016792,7016793,7016794,7016796,7016797,7016798,7016799) [701-679-0,701-679-1,701-679-2,701-679-3,701-679-4,701-679-6,701-679-7,701-679-8,701-679-9]
UPDATE prefixdata SET switch='MAX NDXARS6' WHERE switch=='MAX_NDXARS6'; -- FIXED: improper clli format character s/_/ / (701679,7016790,7016791,7016792,7016793,7016794,7016796,7016797,7016798,7016799) [701-679]
UPDATE OR IGNORE switchdata SET switch='MAX NDXARS6' WHERE switch=='MAX_NDXARS6'; -- FIXED: improper clli format character s/_/ / (701679,7016790,7016791,7016792,7016793,7016794,7016796,7016797,7016798,7016799) [MAX_NDXARS6]
DELETE FROM switchdata WHERE switch=='MAX_NDXARS6'; -- FIXED: improper clli format character s/_/ / (701679,7016790,7016791,7016792,7016793,7016794,7016796,7016797,7016798,7016799) [MAX_NDXARS6]
UPDATE OR IGNORE wcdata SET wc='MAX NDXA' WHERE wc=='MAX_NDXA'; -- FIXED: improper clli format character s/_/ / (701679,7016790,7016791,7016792,7016793,7016794,7016796,7016797,7016798,7016799)
DELETE FROM wcdata WHERE wc=='MAX_NDXA'; -- FIXED: improper clli format character s/_/ / (701679,7016790,7016791,7016792,7016793,7016794,7016796,7016797,7016798,7016799)
---------------------
UPDATE prefixdata SET switch='ZAP NDXARS9' WHERE switch=='ZAP_NDXARS9'; -- FIXED: improper clli format character s/_/ / (701948) [701-948]
UPDATE OR IGNORE switchdata SET switch='ZAP NDXARS9' WHERE switch=='ZAP_NDXARS9'; -- FIXED: improper clli format character s/_/ / (701948) [ZAP_NDXARS9]
DELETE FROM switchdata WHERE switch=='ZAP_NDXARS9'; -- FIXED: improper clli format character s/_/ / (701948) [ZAP_NDXARS9]
UPDATE OR IGNORE wcdata SET wc='ZAP NDXA' WHERE wc=='ZAP_NDXA'; -- FIXED: improper clli format character s/_/ / (701948)
DELETE FROM wcdata WHERE wc=='ZAP_NDXA'; -- FIXED: improper clli format character s/_/ / (701948)
---------------------
UPDATE pooldata   SET switch='GAY GAMA53A' WHERE switch=='GAY_GAMA53A'; -- FIXED: improper clli format character s/_/ / (7065380,7065381,7065386) [706-538-0,706-538-1,706-538-6]
UPDATE prefixdata SET switch='GAY GAMA53A' WHERE switch=='GAY_GAMA53A'; -- FIXED: improper clli format character s/_/ / (706538,7065380,7065381,7065386,7065389) [706-538]
UPDATE OR IGNORE switchdata SET switch='GAY GAMA53A' WHERE switch=='GAY_GAMA53A'; -- FIXED: improper clli format character s/_/ / (706538,7065380,7065381,7065386,7065389) [GAY_GAMA53A]
DELETE FROM switchdata WHERE switch=='GAY_GAMA53A'; -- FIXED: improper clli format character s/_/ / (706538,7065380,7065381,7065386,7065389) [GAY_GAMA53A]
UPDATE OR IGNORE wcdata SET wc='GAY GAMA' WHERE wc=='GAY_GAMA'; -- FIXED: improper clli format character s/_/ / (706538,7065380,7065381,7065386,7065389)
DELETE FROM wcdata WHERE wc=='GAY_GAMA'; -- FIXED: improper clli format character s/_/ / (706538,7065380,7065381,7065386,7065389)
---------------------
UPDATE prefixdata SET switch='ILA GAXARS1' WHERE switch=='ILA_GAXARS1'; -- FIXED: improper clli format character s/_/ / (706789) [706-789]
UPDATE OR IGNORE switchdata SET switch='ILA GAXARS1' WHERE switch=='ILA_GAXARS1'; -- FIXED: improper clli format character s/_/ / (706789) [ILA_GAXARS1]
DELETE FROM switchdata WHERE switch=='ILA_GAXARS1'; -- FIXED: improper clli format character s/_/ / (706789) [ILA_GAXARS1]
UPDATE OR IGNORE wcdata SET wc='ILA GAXA' WHERE wc=='ILA_GAXA'; -- FIXED: improper clli format character s/_/ / (706789)
DELETE FROM wcdata WHERE wc=='ILA_GAXA'; -- FIXED: improper clli format character s/_/ / (706789)
---------------------
UPDATE pooldata   SET switch='ELK CA11RS1' WHERE switch=='ELK_CA11RS1'; -- FIXED: improper clli format character s/_/ / (7078771,7078773) [707-877-1,707-877-3]
UPDATE prefixdata SET switch='ELK CA11RS1' WHERE switch=='ELK_CA11RS1'; -- FIXED: improper clli format character s/_/ / (707877,7078771,7078773) [707-877]
UPDATE OR IGNORE switchdata SET switch='ELK CA11RS1' WHERE switch=='ELK_CA11RS1'; -- FIXED: improper clli format character s/_/ / (707877,7078771,7078773) [ELK_CA11RS1]
DELETE FROM switchdata WHERE switch=='ELK_CA11RS1'; -- FIXED: improper clli format character s/_/ / (707877,7078771,7078773) [ELK_CA11RS1]
UPDATE OR IGNORE wcdata SET wc='ELK CA11' WHERE wc=='ELK_CA11'; -- FIXED: improper clli format character s/_/ / (707877,7078771,7078773)
DELETE FROM wcdata WHERE wc=='ELK_CA11'; -- FIXED: improper clli format character s/_/ / (707877,7078771,7078773)
---------------------
UPDATE pooldata   SET switch='OTO IAXODS0' WHERE switch=='OTO_IAXODS0'; -- FIXED: improper clli format character s/_/ / (7128271,7128272,7128273,7128274,7128275,7128276,7128278,7128279) [712-827-1,712-827-2,712-827-3,712-827-4,712-827-5,712-827-6,712-827-8,712-827-9]
UPDATE prefixdata SET switch='OTO IAXODS0' WHERE switch=='OTO_IAXODS0'; -- FIXED: improper clli format character s/_/ / (712827,7128270,7128271,7128272,7128273,7128274,7128275,7128276,7128278,7128279) [712-827]
UPDATE OR IGNORE switchdata SET switch='OTO IAXODS0' WHERE switch=='OTO_IAXODS0'; -- FIXED: improper clli format character s/_/ / (712827,7128270,7128271,7128272,7128273,7128274,7128275,7128276,7128278,7128279) [OTO_IAXODS0]
DELETE FROM switchdata WHERE switch=='OTO_IAXODS0'; -- FIXED: improper clli format character s/_/ / (712827,7128270,7128271,7128272,7128273,7128274,7128275,7128276,7128278,7128279) [OTO_IAXODS0]
UPDATE OR IGNORE wcdata SET wc='OTO IAXO' WHERE wc=='OTO_IAXO'; -- FIXED: improper clli format character s/_/ / (712827,7128270,7128271,7128272,7128273,7128274,7128275,7128276,7128278,7128279)
DELETE FROM wcdata WHERE wc=='OTO_IAXO'; -- FIXED: improper clli format character s/_/ / (712827,7128270,7128271,7128272,7128273,7128274,7128275,7128276,7128278,7128279)
---------------------
UPDATE pooldata   SET switch='UTE IAXORS0' WHERE switch=='UTE_IAXORS0'; -- FIXED: improper clli format character s/_/ / (7128850,7128851,7128852,7128853,7128858,7128859) [712-885-0,712-885-1,712-885-2,712-885-3,712-885-8,712-885-9]
UPDATE prefixdata SET switch='UTE IAXORS0' WHERE switch=='UTE_IAXORS0'; -- FIXED: improper clli format character s/_/ / (712885,7128850,7128851,7128852,7128853,7128858,7128859) [712-885]
UPDATE OR IGNORE switchdata SET switch='UTE IAXORS0' WHERE switch=='UTE_IAXORS0'; -- FIXED: improper clli format character s/_/ / (712885,7128850,7128851,7128852,7128853,7128858,7128859) [UTE_IAXORS0]
DELETE FROM switchdata WHERE switch=='UTE_IAXORS0'; -- FIXED: improper clli format character s/_/ / (712885,7128850,7128851,7128852,7128853,7128858,7128859) [UTE_IAXORS0]
UPDATE OR IGNORE wcdata SET wc='UTE IAXO' WHERE wc=='UTE_IAXO'; -- FIXED: improper clli format character s/_/ / (712885,7128850,7128851,7128852,7128853,7128858,7128859)
DELETE FROM wcdata WHERE wc=='UTE_IAXO'; -- FIXED: improper clli format character s/_/ / (712885,7128850,7128851,7128852,7128853,7128858,7128859)
---------------------
UPDATE prefixdata SET switch='GAP PAXGRS1' WHERE switch=='GAP_PAXGRS1'; -- FIXED: improper clli format character s/_/ / (717442) [717-442]
UPDATE OR IGNORE switchdata SET switch='GAP PAXGRS1' WHERE switch=='GAP_PAXGRS1'; -- FIXED: improper clli format character s/_/ / (717442) [GAP_PAXGRS1]
DELETE FROM switchdata WHERE switch=='GAP_PAXGRS1'; -- FIXED: improper clli format character s/_/ / (717442) [GAP_PAXGRS1]
UPDATE OR IGNORE wcdata SET wc='GAP PAXG' WHERE wc=='GAP_PAXG'; -- FIXED: improper clli format character s/_/ / (717442)
DELETE FROM wcdata WHERE wc=='GAP_PAXG'; -- FIXED: improper clli format character s/_/ / (717442)
---------------------
UPDATE prefixdata SET switch='RYE COXCRS0' WHERE switch=='RYE_COXCRS0'; -- FIXED: improper clli format character s/_/ / (719489) [719-489]
UPDATE OR IGNORE switchdata SET switch='RYE COXCRS0' WHERE switch=='RYE_COXCRS0'; -- FIXED: improper clli format character s/_/ / (719489) [RYE_COXCRS0]
DELETE FROM switchdata WHERE switch=='RYE_COXCRS0'; -- FIXED: improper clli format character s/_/ / (719489) [RYE_COXCRS0]
UPDATE OR IGNORE wcdata SET wc='RYE COXC' WHERE wc=='RYE_COXC'; -- FIXED: improper clli format character s/_/ / (719489)
DELETE FROM wcdata WHERE wc=='RYE_COXC'; -- FIXED: improper clli format character s/_/ / (719489)
---------------------
UPDATE prefixdata SET switch='KIM COXCDS1' WHERE switch=='KIM_COXCDS1'; -- FIXED: improper clli format character s/_/ / (719643) [719-643]
UPDATE OR IGNORE switchdata SET switch='KIM COXCDS1' WHERE switch=='KIM_COXCDS1'; -- FIXED: improper clli format character s/_/ / (719643) [KIM_COXCDS1]
DELETE FROM switchdata WHERE switch=='KIM_COXCDS1'; -- FIXED: improper clli format character s/_/ / (719643) [KIM_COXCDS1]
UPDATE OR IGNORE wcdata SET wc='KIM COXC' WHERE wc=='KIM_COXC'; -- FIXED: improper clli format character s/_/ / (719643)
DELETE FROM wcdata WHERE wc=='KIM_COXC'; -- FIXED: improper clli format character s/_/ / (719643)
---------------------
UPDATE pooldata   SET switch='IDA MIXHRS0' WHERE switch=='IDA_MIXHRS0'; -- FIXED: improper clli format character s/_/ / (7342692,7342693,7342696,7342699) [734-269-2,734-269-3,734-269-6,734-269-9]
UPDATE prefixdata SET switch='IDA MIXHRS0' WHERE switch=='IDA_MIXHRS0'; -- FIXED: improper clli format character s/_/ / (734269,7342692,7342693,7342696,7342699) [734-269]
UPDATE OR IGNORE switchdata SET switch='IDA MIXHRS0' WHERE switch=='IDA_MIXHRS0'; -- FIXED: improper clli format character s/_/ / (734269,7342692,7342693,7342696,7342699) [IDA_MIXHRS0]
DELETE FROM switchdata WHERE switch=='IDA_MIXHRS0'; -- FIXED: improper clli format character s/_/ / (734269,7342692,7342693,7342696,7342699) [IDA_MIXHRS0]
UPDATE OR IGNORE wcdata SET wc='IDA MIXH' WHERE wc=='IDA_MIXH'; -- FIXED: improper clli format character s/_/ / (734269,7342692,7342693,7342696,7342699)
DELETE FROM wcdata WHERE wc=='IDA_MIXH'; -- FIXED: improper clli format character s/_/ / (734269,7342692,7342693,7342696,7342699)
---------------------
UPDATE pooldata   SET switch='ELY NV01DS1' WHERE switch=='ELY_NV01DS1'; -- FIXED: improper clli format character s/_/ / (7755910) [775-591-0]
UPDATE prefixdata SET switch='ELY NV01DS1' WHERE switch=='ELY_NV01DS1'; -- FIXED: improper clli format character s/_/ / (775289,775591,7755910) [775-289,775-591]
UPDATE OR IGNORE switchdata SET switch='ELY NV01DS1' WHERE switch=='ELY_NV01DS1'; -- FIXED: improper clli format character s/_/ / (775289,775591,7755910) [ELY_NV01DS1]
DELETE FROM switchdata WHERE switch=='ELY_NV01DS1'; -- FIXED: improper clli format character s/_/ / (775289,775591,7755910) [ELY_NV01DS1]
UPDATE OR IGNORE wcdata SET wc='ELY NV01' WHERE wc=='ELY_NV01'; -- FIXED: improper clli format character s/_/ / (775289,775591,7755910)
DELETE FROM wcdata WHERE wc=='ELY_NV01'; -- FIXED: improper clli format character s/_/ / (775289,775591,7755910)
---------------------
UPDATE prefixdata SET switch='ROY UTMARS3' WHERE switch=='ROY_UTMARS3'; -- FIXED: improper clli format character s/_/ / (801985) [801-985]
UPDATE OR IGNORE switchdata SET switch='ROY UTMARS3' WHERE switch=='ROY_UTMARS3'; -- FIXED: improper clli format character s/_/ / (801985) [ROY_UTMARS3]
DELETE FROM switchdata WHERE switch=='ROY_UTMARS3'; -- FIXED: improper clli format character s/_/ / (801985) [ROY_UTMARS3]
UPDATE OR IGNORE wcdata SET wc='ROY UTMA' WHERE wc=='ROY_UTMA'; -- FIXED: improper clli format character s/_/ / (801985)
DELETE FROM wcdata WHERE wc=='ROY_UTMA'; -- FIXED: improper clli format character s/_/ / (801985)
---------------------
UPDATE prefixdata SET switch='HUB TXXARS0' WHERE switch=='HUB_TXXARS0'; -- FIXED: improper clli format character s/_/ / (806265) [806-265]
UPDATE OR IGNORE switchdata SET switch='HUB TXXARS0' WHERE switch=='HUB_TXXARS0'; -- FIXED: improper clli format character s/_/ / (806265) [HUB_TXXARS0]
DELETE FROM switchdata WHERE switch=='HUB_TXXARS0'; -- FIXED: improper clli format character s/_/ / (806265) [HUB_TXXARS0]
UPDATE OR IGNORE wcdata SET wc='HUB TXXA' WHERE wc=='HUB_TXXA'; -- FIXED: improper clli format character s/_/ / (806265)
DELETE FROM wcdata WHERE wc=='HUB_TXXA'; -- FIXED: improper clli format character s/_/ / (806265)
---------------------
UPDATE pooldata   SET switch='EWA HICORS1' WHERE switch=='EWA_HICORS1'; -- FIXED: improper clli format character s/_/ / (8086810,8086811,8086813,8086814,8086815,8086816,8086818,8086873,8086874,8086879,...) [808-681-0,808-681-1,808-681-3,808-681-4,808-681-5,808-681-6,808-681-8,808-687-3,808-687-4,808-687-9,...]
UPDATE prefixdata SET switch='EWA HICORS1' WHERE switch=='EWA_HICORS1'; -- FIXED: improper clli format character s/_/ / (808681,8086810,8086811,8086813,8086814,8086815,8086816,8086817,8086818,808687,...) [808-681,808-687,808-699]
UPDATE OR IGNORE switchdata SET switch='EWA HICORS1' WHERE switch=='EWA_HICORS1'; -- FIXED: improper clli format character s/_/ / (808681,8086810,8086811,8086813,8086814,8086815,8086816,8086818,808687,8086873,...) [EWA_HICORS1]
DELETE FROM switchdata WHERE switch=='EWA_HICORS1'; -- FIXED: improper clli format character s/_/ / (808681,8086810,8086811,8086813,8086814,8086815,8086816,8086818,808687,8086873,...) [EWA_HICORS1]
UPDATE OR IGNORE wcdata SET wc='EWA HICO' WHERE wc=='EWA_HICO'; -- FIXED: improper clli format character s/_/ / (808681,8086810,8086811,8086813,8086814,8086815,8086816,8086818,808687,8086873,...)
DELETE FROM wcdata WHERE wc=='EWA_HICO'; -- FIXED: improper clli format character s/_/ / (808681,8086810,8086811,8086813,8086814,8086815,8086816,8086818,808687,8086873,...)
---------------------
UPDATE pooldata   SET switch='EWA HIXARS0' WHERE switch=='EWA_HIXARS0'; -- FIXED: improper clli format character s/_/ / (8086830,8086831,8086833,8086834,8086850,8086851,8086852,8086853,8086854,8086855,...) [808-683-0,808-683-1,808-683-3,808-683-4,808-685-0,808-685-1,808-685-2,808-685-3,808-685-4,808-685-5,...]
UPDATE prefixdata SET switch='EWA HIXARS0' WHERE switch=='EWA_HIXARS0'; -- FIXED: improper clli format character s/_/ / (808683,8086830,8086831,8086833,8086834,8086837,808685,8086850,8086851,8086852,...) [808-683,808-685]
UPDATE OR IGNORE switchdata SET switch='EWA HIXARS0' WHERE switch=='EWA_HIXARS0'; -- FIXED: improper clli format character s/_/ / (808683,8086830,8086831,8086833,8086834,808685,8086850,8086851,8086852,8086853,...) [EWA_HIXARS0]
DELETE FROM switchdata WHERE switch=='EWA_HIXARS0'; -- FIXED: improper clli format character s/_/ / (808683,8086830,8086831,8086833,8086834,808685,8086850,8086851,8086852,8086853,...) [EWA_HIXARS0]
UPDATE OR IGNORE wcdata SET wc='EWA HIXA' WHERE wc=='EWA_HIXA'; -- FIXED: improper clli format character s/_/ / (808683,8086830,8086831,8086833,8086834,808685,8086850,8086851,8086852,8086853,...)
DELETE FROM wcdata WHERE wc=='EWA_HIXA'; -- FIXED: improper clli format character s/_/ / (808683,8086830,8086831,8086833,8086834,808685,8086850,8086851,8086852,8086853,...)
---------------------
UPDATE pooldata   SET switch='REW PARERS1' WHERE switch=='REW_PARERS1'; -- FIXED: improper clli format character s/_/ / (8144653,8144659) [814-465-3,814-465-9]
UPDATE prefixdata SET switch='REW PARERS1' WHERE switch=='REW_PARERS1'; -- FIXED: improper clli format character s/_/ / (814465,8144652,8144653,8144659) [814-465]
UPDATE OR IGNORE switchdata SET switch='REW PARERS1' WHERE switch=='REW_PARERS1'; -- FIXED: improper clli format character s/_/ / (814465,8144652,8144653,8144659) [REW_PARERS1]
DELETE FROM switchdata WHERE switch=='REW_PARERS1'; -- FIXED: improper clli format character s/_/ / (814465,8144652,8144653,8144659) [REW_PARERS1]
UPDATE OR IGNORE wcdata SET wc='REW PARE' WHERE wc=='REW_PARE'; -- FIXED: improper clli format character s/_/ / (814465,8144652,8144653,8144659)
DELETE FROM wcdata WHERE wc=='REW_PARE'; -- FIXED: improper clli format character s/_/ / (814465,8144652,8144653,8144659)
---------------------
UPDATE prefixdata SET switch='JAY FLMARS0' WHERE switch=='JAY_FLMARS0'; -- FIXED: improper clli format character s/_/ / (850675) [850-675]
UPDATE OR IGNORE switchdata SET switch='JAY FLMARS0' WHERE switch=='JAY_FLMARS0'; -- FIXED: improper clli format character s/_/ / (850675) [JAY_FLMARS0]
DELETE FROM switchdata WHERE switch=='JAY_FLMARS0'; -- FIXED: improper clli format character s/_/ / (850675) [JAY_FLMARS0]
UPDATE OR IGNORE wcdata SET wc='JAY FLMA' WHERE wc=='JAY_FLMA'; -- FIXED: improper clli format character s/_/ / (850675)
DELETE FROM wcdata WHERE wc=='JAY_FLMA'; -- FIXED: improper clli format character s/_/ / (850675)
---------------------
UPDATE pooldata   SET switch='LEE FLXARS0' WHERE switch=='LEE_FLXARS0'; -- FIXED: improper clli format character s/_/ / (8509710,8509711,8509712,8509713,8509714,8509715,8509717,8509719) [850-971-0,850-971-1,850-971-2,850-971-3,850-971-4,850-971-5,850-971-7,850-971-9]
UPDATE prefixdata SET switch='LEE FLXARS0' WHERE switch=='LEE_FLXARS0'; -- FIXED: improper clli format character s/_/ / (850971,8509710,8509711,8509712,8509713,8509714,8509715,8509717,8509719) [850-971]
UPDATE OR IGNORE switchdata SET switch='LEE FLXARS0' WHERE switch=='LEE_FLXARS0'; -- FIXED: improper clli format character s/_/ / (850971,8509710,8509711,8509712,8509713,8509714,8509715,8509717,8509719) [LEE_FLXARS0]
DELETE FROM switchdata WHERE switch=='LEE_FLXARS0'; -- FIXED: improper clli format character s/_/ / (850971,8509710,8509711,8509712,8509713,8509714,8509715,8509717,8509719) [LEE_FLXARS0]
UPDATE OR IGNORE wcdata SET wc='LEE FLXA' WHERE wc=='LEE_FLXA'; -- FIXED: improper clli format character s/_/ / (850971,8509710,8509711,8509712,8509713,8509714,8509715,8509717,8509719)
DELETE FROM wcdata WHERE wc=='LEE_FLXA'; -- FIXED: improper clli format character s/_/ / (850971,8509710,8509711,8509712,8509713,8509714,8509715,8509717,8509719)
---------------------
UPDATE prefixdata SET switch='ROE ARXARS0' WHERE switch=='ROE_ARXARS0'; -- FIXED: improper clli format character s/_/ / (870241) [870-241]
UPDATE OR IGNORE switchdata SET switch='ROE ARXARS0' WHERE switch=='ROE_ARXARS0'; -- FIXED: improper clli format character s/_/ / (870241) [ROE_ARXARS0]
DELETE FROM switchdata WHERE switch=='ROE_ARXARS0'; -- FIXED: improper clli format character s/_/ / (870241) [ROE_ARXARS0]
UPDATE OR IGNORE wcdata SET wc='ROE ARXA' WHERE wc=='ROE_ARXA'; -- FIXED: improper clli format character s/_/ / (870241)
DELETE FROM wcdata WHERE wc=='ROE_ARXA'; -- FIXED: improper clli format character s/_/ / (870241)
---------------------
UPDATE prefixdata SET switch='FOX ARXADS0' WHERE switch=='FOX_ARXADS0'; -- FIXED: improper clli format character s/_/ / (870363) [870-363]
UPDATE OR IGNORE switchdata SET switch='FOX ARXADS0' WHERE switch=='FOX_ARXADS0'; -- FIXED: improper clli format character s/_/ / (870363) [FOX_ARXADS0]
DELETE FROM switchdata WHERE switch=='FOX_ARXADS0'; -- FIXED: improper clli format character s/_/ / (870363) [FOX_ARXADS0]
UPDATE OR IGNORE wcdata SET wc='FOX ARXA' WHERE wc=='FOX_ARXA'; -- FIXED: improper clli format character s/_/ / (870363)
DELETE FROM wcdata WHERE wc=='FOX_ARXA'; -- FIXED: improper clli format character s/_/ / (870363)
---------------------
UPDATE pooldata   SET switch='ARP TXXADS0' WHERE switch=='ARP_TXXADS0'; -- FIXED: improper clli format character s/_/ / (9038590,9038591,9038592,9038593,9038594,9038599) [903-859-0,903-859-1,903-859-2,903-859-3,903-859-4,903-859-9]
UPDATE prefixdata SET switch='ARP TXXADS0' WHERE switch=='ARP_TXXADS0'; -- FIXED: improper clli format character s/_/ / (903859,9038590,9038591,9038592,9038593,9038594,9038599) [903-859]
UPDATE OR IGNORE switchdata SET switch='ARP TXXADS0' WHERE switch=='ARP_TXXADS0'; -- FIXED: improper clli format character s/_/ / (903859,9038590,9038591,9038592,9038593,9038594,9038599) [ARP_TXXADS0]
DELETE FROM switchdata WHERE switch=='ARP_TXXADS0'; -- FIXED: improper clli format character s/_/ / (903859,9038590,9038591,9038592,9038593,9038594,9038599) [ARP_TXXADS0]
UPDATE OR IGNORE wcdata SET wc='ARP TXXA' WHERE wc=='ARP_TXXA'; -- FIXED: improper clli format character s/_/ / (903859,9038590,9038591,9038592,9038593,9038594,9038599)
DELETE FROM wcdata WHERE wc=='ARP_TXXA'; -- FIXED: improper clli format character s/_/ / (903859,9038590,9038591,9038592,9038593,9038594,9038599)
---------------------
UPDATE pooldata   SET switch='VAN TXXADS0' WHERE switch=='VAN_TXXADS0'; -- FIXED: improper clli format character s/_/ / (9039631,9039633,9039634,9039635,9039637,9039638) [903-963-1,903-963-3,903-963-4,903-963-5,903-963-7,903-963-8]
UPDATE prefixdata SET switch='VAN TXXADS0' WHERE switch=='VAN_TXXADS0'; -- FIXED: improper clli format character s/_/ / (903963,9039631,9039633,9039634,9039635,9039637,9039638) [903-963]
UPDATE OR IGNORE switchdata SET switch='VAN TXXADS0' WHERE switch=='VAN_TXXADS0'; -- FIXED: improper clli format character s/_/ / (903963,9039631,9039633,9039634,9039635,9039637,9039638) [VAN_TXXADS0]
DELETE FROM switchdata WHERE switch=='VAN_TXXADS0'; -- FIXED: improper clli format character s/_/ / (903963,9039631,9039633,9039634,9039635,9039637,9039638) [VAN_TXXADS0]
UPDATE OR IGNORE wcdata SET wc='VAN TXXA' WHERE wc=='VAN_TXXA'; -- FIXED: improper clli format character s/_/ / (903963,9039631,9039633,9039634,9039635,9039637,9039638)
DELETE FROM wcdata WHERE wc=='VAN_TXXA'; -- FIXED: improper clli format character s/_/ / (903963,9039631,9039633,9039634,9039635,9039637,9039638)
---------------------
UPDATE prefixdata SET switch='EEK AKXA536' WHERE switch=='EEK_AKXA536'; -- FIXED: improper clli format character s/_/ / (907536) [907-536]
UPDATE OR IGNORE switchdata SET switch='EEK AKXA536' WHERE switch=='EEK_AKXA536'; -- FIXED: improper clli format character s/_/ / (907536) [EEK_AKXA536]
DELETE FROM switchdata WHERE switch=='EEK_AKXA536'; -- FIXED: improper clli format character s/_/ / (907536) [EEK_AKXA536]
UPDATE OR IGNORE wcdata SET wc='EEK AKXA' WHERE wc=='EEK_AKXA'; -- FIXED: improper clli format character s/_/ / (907536)
DELETE FROM wcdata WHERE wc=='EEK_AKXA'; -- FIXED: improper clli format character s/_/ / (907536)
---------------------
UPDATE prefixdata SET switch='TOK AKXADS2' WHERE switch=='TOK_AKXADS2'; -- FIXED: improper clli format character s/_/ / (907883) [907-883]
UPDATE OR IGNORE switchdata SET switch='TOK AKXADS2' WHERE switch=='TOK_AKXADS2'; -- FIXED: improper clli format character s/_/ / (907883) [TOK_AKXADS2]
DELETE FROM switchdata WHERE switch=='TOK_AKXADS2'; -- FIXED: improper clli format character s/_/ / (907883) [TOK_AKXADS2]
UPDATE OR IGNORE wcdata SET wc='TOK AKXA' WHERE wc=='TOK_AKXA'; -- FIXED: improper clli format character s/_/ / (907883)
DELETE FROM wcdata WHERE wc=='TOK_AKXA'; -- FIXED: improper clli format character s/_/ / (907883)
---------------------
UPDATE prefixdata SET switch='RYE NYRYDS0' WHERE switch=='RYE_NYRYDS0'; -- FIXED: improper clli format character s/_/ / (914921,914925,914967) [914-921,914-925,914-967]
UPDATE OR IGNORE switchdata SET switch='RYE NYRYDS0' WHERE switch=='RYE_NYRYDS0'; -- FIXED: improper clli format character s/_/ / (914921,914925,914967) [RYE_NYRYDS0]
DELETE FROM switchdata WHERE switch=='RYE_NYRYDS0'; -- FIXED: improper clli format character s/_/ / (914921,914925,914967) [RYE_NYRYDS0]
UPDATE OR IGNORE wcdata SET wc='RYE NYRY' WHERE wc=='RYE_NYRY'; -- FIXED: improper clli format character s/_/ / (914921,914925,914967)
DELETE FROM wcdata WHERE wc=='RYE_NYRY'; -- FIXED: improper clli format character s/_/ / (914921,914925,914967)
---------------------
UPDATE prefixdata SET switch='JAY OKXADS0' WHERE switch=='JAY_OKXADS0'; -- FIXED: improper clli format character s/_/ / (918253) [918-253]
UPDATE OR IGNORE switchdata SET switch='JAY OKXADS0' WHERE switch=='JAY_OKXADS0'; -- FIXED: improper clli format character s/_/ / (918253) [JAY_OKXADS0]
DELETE FROM switchdata WHERE switch=='JAY_OKXADS0'; -- FIXED: improper clli format character s/_/ / (918253) [JAY_OKXADS0]
UPDATE OR IGNORE wcdata SET wc='JAY OKXA' WHERE wc=='JAY_OKXA'; -- FIXED: improper clli format character s/_/ / (918253)
DELETE FROM wcdata WHERE wc=='JAY_OKXA'; -- FIXED: improper clli format character s/_/ / (918253)
---------------------
UPDATE OR IGNORE switchdata SET switch='JAY OKXA00T' WHERE switch=='JAY-OKXA00T'; -- FIXED: improper clli format character s/-/ / (918253) [JAY-OKXA00T]
DELETE FROM switchdata WHERE switch=='JAY-OKXA00T'; -- FIXED: improper clli format character s/-/ / (918253) [JAY-OKXA00T]
UPDATE switchdata SET cls45sw='JAY OKXA00T' WHERE cls45sw=='JAY-OKXA00T'; -- FIXED: improper clli format character s/-/ / (918253) [JAY_OKXADS0]
UPDATE OR IGNORE wcdata SET wc='JAY OKXA' WHERE wc=='JAY-OKXA'; -- FIXED: improper clli format character s/-/ / (918253)
DELETE FROM wcdata WHERE wc=='JAY-OKXA'; -- FIXED: improper clli format character s/-/ / (918253)
---------------------
UPDATE prefixdata SET switch='RIO WIXARS0' WHERE switch=='RIO_WIXARS0'; -- FIXED: improper clli format character s/_/ / (920992) [920-992]
UPDATE OR IGNORE switchdata SET switch='RIO WIXARS0' WHERE switch=='RIO_WIXARS0'; -- FIXED: improper clli format character s/_/ / (920992) [RIO_WIXARS0]
DELETE FROM switchdata WHERE switch=='RIO_WIXARS0'; -- FIXED: improper clli format character s/_/ / (920992) [RIO_WIXARS0]
UPDATE OR IGNORE wcdata SET wc='RIO WIXA' WHERE wc=='RIO_WIXA'; -- FIXED: improper clli format character s/_/ / (920992)
DELETE FROM wcdata WHERE wc=='RIO_WIXA'; -- FIXED: improper clli format character s/_/ / (920992)
---------------------
UPDATE pooldata   SET switch='MIO MIXGDS0' WHERE switch=='MIO_MIXGDS0'; -- FIXED: improper clli format character s/_/ / (9898261,9898262,9898263,9898265,9898266,9898268) [989-826-1,989-826-2,989-826-3,989-826-5,989-826-6,989-826-8]
UPDATE prefixdata SET switch='MIO MIXGDS0' WHERE switch=='MIO_MIXGDS0'; -- FIXED: improper clli format character s/_/ / (989826,9898261,9898262,9898263,9898264,9898265,9898266,9898268) [989-826]
UPDATE pcdata SET switch='MIO MIXGDS0' WHERE switch=='MIO_MIXGDS0'; -- FIXED: improper clli format character s/_/ / (989826,9898261,9898262,9898263,9898264,9898265,9898266,9898268) [202-120-018]
UPDATE OR IGNORE switchdata SET switch='MIO MIXGDS0' WHERE switch=='MIO_MIXGDS0'; -- FIXED: improper clli format character s/_/ / (989826,9898261,9898262,9898263,9898264,9898265,9898266,9898268) [MIO_MIXGDS0]
DELETE FROM switchdata WHERE switch=='MIO_MIXGDS0'; -- FIXED: improper clli format character s/_/ / (989826,9898261,9898262,9898263,9898264,9898265,9898266,9898268) [MIO_MIXGDS0]
UPDATE OR IGNORE wcdata SET wc='MIO MIXG' WHERE wc=='MIO_MIXG'; -- FIXED: improper clli format character s/_/ / (989826,9898261,9898262,9898263,9898264,9898265,9898266,9898268)
DELETE FROM wcdata WHERE wc=='MIO_MIXG'; -- FIXED: improper clli format character s/_/ / (989826,9898261,9898262,9898263,9898264,9898265,9898266,9898268)
--=======================================================
UPDATE pooldata   SET switch=NULL WHERE switch=='XXXXXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (5185402,5185407,9149762,9149763,9149765) [518-540-2,518-540-7,914-976-2,914-976-3,914-976-5]
UPDATE prefixdata SET switch=NULL WHERE switch=='XXXXXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (201201,201211,201260,201311,201411,201511,201550,201554,201555,201611,...) [201-201,201-211,201-260,201-311,201-411,201-511,201-550,201-554,201-555,201-611,...]
DELETE FROM switchdata WHERE switch=='XXXXXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (201201,201211,201260,201311,201411,201511,201550,201554,201555,201611,...) [XXXXXXXXXXX]
DELETE FROM wcdata WHERE wc=='XXXXXXXX'; -- FIXED: invalid clli 'XXXXXXXXXXX' (201201,201211,201260,201311,201411,201511,201550,201554,201555,201611,...)
---------------------
UPDATE prefixdata SET switch=NULL WHERE switch=='__VARIOUS__'; -- FIXED: invalid clli '__VARIOUS__' (203976,206976,234211,234311,234411,234511,234611,234711,234811,234911,...) [203-976,206-976,234-211,234-311,234-411,234-511,234-611,234-711,234-811,234-911,...]
DELETE FROM switchdata WHERE switch=='__VARIOUS__'; -- FIXED: invalid clli '__VARIOUS__' (203976,206976,234211,234311,234411,234511,234611,234711,234811,234911,...) [__VARIOUS__]
DELETE FROM wcdata WHERE wc=='__VARIOU'; -- FIXED: invalid clli '__VARIOUS__' (203976,206976,234211,234311,234411,234511,234611,234711,234811,234911,...)
---------------------
DELETE FROM switchdata WHERE switch=='NONE'; -- FIXED: invalid clli 'NONE' (4062780,8013120) [NONE]
UPDATE switchdata SET tgclli=NULL WHERE tgclli=='NONE'; -- FIXED: invalid clli 'NONE' (8013120,801613,8016390,801691,801709,8017698,801899) [PROVUTAVG00]
UPDATE switchdata SET tdmlcl=NULL WHERE tdmlcl=='NONE'; -- FIXED: invalid clli 'NONE' (406278,4062780,4062783,4062784,4062785,4062787,4062789) [CNRDMTMARS1]
DELETE FROM wcdata WHERE wc=='NONE'; -- FIXED: invalid clli 'NONE' (406278,4062780,4062783,4062784,4062785,4062787,4062789)
--=======================================================
UPDATE prefixdata SET switch='GNBRARXAPS0' WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [501-287]
UPDATE OR IGNORE switchdata SET switch='GNBRARXAPS0' WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
DELETE FROM switchdata WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
UPDATE OR IGNORE wcdata SET wc='GNBRARXA' WHERE wc=='LTRKARTL'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
DELETE FROM wcdata WHERE wc=='LTRKARTL'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
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
UPDATE prefixdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (601216) [601-216]
UPDATE OR IGNORE switchdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (601216) [MTOLMSABAND]
DELETE FROM switchdata WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (601216) [MTOLMSABAND]
---------------------
UPDATE prefixdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (601233) [601-233]
UPDATE OR IGNORE switchdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (601233) [MCCMMSBLAMB]
DELETE FROM switchdata WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (601233) [MCCMMSBLAMB]
---------------------
-- UPDATE prefixdata SET switch='NYCMNYZRDFS' WHERE switch=='NYCMNYZRDFS'; -- FIXME: poor equipment code 'DFS' (917268) [917-268]
-- UPDATE switchdata SET switch='NYCMNYZRDFS' WHERE switch=='NYCMNYZRDFS'; -- FIXME: poor equipment code 'DFS' (917268) [NYCMNYZRDFS]
---------------------
-- UPDATE prefixdata SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS'; -- FIXME: poor equipment code 'DDS' (213295) [213-295]
-- UPDATE switchdata SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS'; -- FIXME: poor equipment code 'DDS' (213295) [LSANCARCDDS]
---------------------
-- UPDATE prefixdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724914) [724-914]
-- UPDATE switchdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724914) [WASHPAWAHPT]
---------------------
-- UPDATE switchdata SET switch='HNLLHIMNLAD' WHERE switch=='HNLLHIMNLAD'; -- FIXME: poor equipment code 'LAD' (808777) [HNLLHIMNLAD]
-- UPDATE switchdata SET actual='HNLLHIMNLAD' WHERE actual=='HNLLHIMNLAD'; -- FIXME: poor equipment code 'LAD' (808777) [HNLLHIMNXMD]
--=======================================================
