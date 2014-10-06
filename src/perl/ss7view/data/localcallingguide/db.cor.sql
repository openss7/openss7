--===================================
UPDATE ocndata    SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE pcdata     SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE rcdata     SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE rndata     SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE exdata     SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE lcadata    SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE wcdata     SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE switchdata SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE npadata    SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE prefixdata SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE pooldata   SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
UPDATE linedata   SET sect='lcg' WHERE sect NOT NULL and sect!='lcg';
--===================================
--=============================================
UPDATE exdata SET region='SF' WHERE exch='219480' AND region='SM'; -- invalid region
--=============================================
UPDATE prefixdata SET switch='ADA OKMADS1' WHERE switch=='ADA-OKMADS1'; -- FIXED: improper clli format character s/-/ / (580) [580-235]
UPDATE OR IGNORE switchdata SET switch='ADA OKMADS1' WHERE switch=='ADA-OKMADS1'; -- FIXED: improper clli format character s/-/ / (580) [ADA-OKMADS1]
DELETE FROM switchdata WHERE switch=='ADA-OKMADS1'; -- FIXED: improper clli format character s/-/ / (580) [ADA-OKMADS1]
UPDATE OR IGNORE wcdata SET wc='ADA OKMA' WHERE wc=='ADA-OKMA'; -- FIXED: improper clli format character s/-/ / (580)
DELETE FROM wcdata WHERE wc=='ADA-OKMA'; -- FIXED: improper clli format character s/-/ / (580)
---------------------
UPDATE prefixdata SET switch='ADA OKMADS0' WHERE switch=='ADA-OKMADS0'; -- FIXED: improper clli format character s/-/ / (580) [580-272]
UPDATE OR IGNORE switchdata SET switch='ADA OKMADS0' WHERE switch=='ADA-OKMADS0'; -- FIXED: improper clli format character s/-/ / (580) [ADA-OKMADS0]
DELETE FROM switchdata WHERE switch=='ADA-OKMADS0'; -- FIXED: improper clli format character s/-/ / (580) [ADA-OKMADS0]
UPDATE OR IGNORE wcdata SET wc='ADA OKMA' WHERE wc=='ADA-OKMA'; -- FIXED: improper clli format character s/-/ / (580)
DELETE FROM wcdata WHERE wc=='ADA-OKMA'; -- FIXED: improper clli format character s/-/ / (580)
---------------------
UPDATE OR IGNORE switchdata SET switch='EEK AKXA536' WHERE switch=='EEK_AKXA536'; -- FIXED: improper clli format character s/_/ / (EEK_AKXA536) [EEK_AKXA536]
DELETE FROM switchdata WHERE switch=='EEK_AKXA536'; -- FIXED: improper clli format character s/_/ / (EEK_AKXA536) [EEK_AKXA536]
UPDATE OR IGNORE wcdata SET wc='EEK AKXA' WHERE wc=='EEK_AKXA'; -- FIXED: improper clli format character s/_/ / (EEK_AKXA536)
DELETE FROM wcdata WHERE wc=='EEK_AKXA'; -- FIXED: improper clli format character s/_/ / (EEK_AKXA536)
---------------------
UPDATE OR IGNORE switchdata SET switch='TOK AKXADS2' WHERE switch=='TOK_AKXADS2'; -- FIXED: improper clli format character s/_/ / (TOK_AKXADS2) [TOK_AKXADS2]
DELETE FROM switchdata WHERE switch=='TOK_AKXADS2'; -- FIXED: improper clli format character s/_/ / (TOK_AKXADS2) [TOK_AKXADS2]
UPDATE OR IGNORE wcdata SET wc='TOK AKXA' WHERE wc=='TOK_AKXA'; -- FIXED: improper clli format character s/_/ / (TOK_AKXADS2)
DELETE FROM wcdata WHERE wc=='TOK_AKXA'; -- FIXED: improper clli format character s/_/ / (TOK_AKXADS2)
---------------------
UPDATE OR IGNORE switchdata SET switch='OPP AL08GT0' WHERE switch=='OPP_AL08GT0'; -- FIXED: improper clli format character s/_/ / (OPP_AL08GT0) [OPP_AL08GT0]
DELETE FROM switchdata WHERE switch=='OPP_AL08GT0'; -- FIXED: improper clli format character s/_/ / (OPP_AL08GT0) [OPP_AL08GT0]
UPDATE OR IGNORE wcdata SET wc='OPP AL08' WHERE wc=='OPP_AL08'; -- FIXED: improper clli format character s/_/ / (OPP_AL08GT0)
DELETE FROM wcdata WHERE wc=='OPP_AL08'; -- FIXED: improper clli format character s/_/ / (OPP_AL08GT0)
---------------------
UPDATE OR IGNORE switchdata SET switch='OPP ALXADS0' WHERE switch=='OPP_ALXADS0'; -- FIXED: improper clli format character s/_/ / (OPP_ALXADS0) [OPP_ALXADS0]
DELETE FROM switchdata WHERE switch=='OPP_ALXADS0'; -- FIXED: improper clli format character s/_/ / (OPP_ALXADS0) [OPP_ALXADS0]
UPDATE OR IGNORE wcdata SET wc='OPP ALXA' WHERE wc=='OPP_ALXA'; -- FIXED: improper clli format character s/_/ / (OPP_ALXADS0)
DELETE FROM wcdata WHERE wc=='OPP_ALXA'; -- FIXED: improper clli format character s/_/ / (OPP_ALXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='COY ARXARS0' WHERE switch=='COY_ARXARS0'; -- FIXED: improper clli format character s/_/ / (COY_ARXARS0) [COY_ARXARS0]
DELETE FROM switchdata WHERE switch=='COY_ARXARS0'; -- FIXED: improper clli format character s/_/ / (COY_ARXARS0) [COY_ARXARS0]
UPDATE OR IGNORE wcdata SET wc='COY ARXA' WHERE wc=='COY_ARXA'; -- FIXED: improper clli format character s/_/ / (COY_ARXARS0)
DELETE FROM wcdata WHERE wc=='COY_ARXA'; -- FIXED: improper clli format character s/_/ / (COY_ARXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='FOX ARXADS0' WHERE switch=='FOX_ARXADS0'; -- FIXED: improper clli format character s/_/ / (FOX_ARXADS0) [FOX_ARXADS0]
DELETE FROM switchdata WHERE switch=='FOX_ARXADS0'; -- FIXED: improper clli format character s/_/ / (FOX_ARXADS0) [FOX_ARXADS0]
UPDATE OR IGNORE wcdata SET wc='FOX ARXA' WHERE wc=='FOX_ARXA'; -- FIXED: improper clli format character s/_/ / (FOX_ARXADS0)
DELETE FROM wcdata WHERE wc=='FOX_ARXA'; -- FIXED: improper clli format character s/_/ / (FOX_ARXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='OLA ARXADS0' WHERE switch=='OLA_ARXADS0'; -- FIXED: improper clli format character s/_/ / (OLA_ARXADS0) [OLA_ARXADS0]
DELETE FROM switchdata WHERE switch=='OLA_ARXADS0'; -- FIXED: improper clli format character s/_/ / (OLA_ARXADS0) [OLA_ARXADS0]
UPDATE OR IGNORE wcdata SET wc='OLA ARXA' WHERE wc=='OLA_ARXA'; -- FIXED: improper clli format character s/_/ / (OLA_ARXADS0)
DELETE FROM wcdata WHERE wc=='OLA_ARXA'; -- FIXED: improper clli format character s/_/ / (OLA_ARXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ROE ARXARS0' WHERE switch=='ROE_ARXARS0'; -- FIXED: improper clli format character s/_/ / (ROE_ARXARS0) [ROE_ARXARS0]
DELETE FROM switchdata WHERE switch=='ROE_ARXARS0'; -- FIXED: improper clli format character s/_/ / (ROE_ARXARS0) [ROE_ARXARS0]
UPDATE OR IGNORE wcdata SET wc='ROE ARXA' WHERE wc=='ROE_ARXA'; -- FIXED: improper clli format character s/_/ / (ROE_ARXARS0)
DELETE FROM wcdata WHERE wc=='ROE_ARXA'; -- FIXED: improper clli format character s/_/ / (ROE_ARXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='AJO AZXCDS0' WHERE switch=='AJO_AZXCDS0'; -- FIXED: improper clli format character s/_/ / (AJO_AZXCDS0) [AJO_AZXCDS0]
DELETE FROM switchdata WHERE switch=='AJO_AZXCDS0'; -- FIXED: improper clli format character s/_/ / (AJO_AZXCDS0) [AJO_AZXCDS0]
UPDATE OR IGNORE wcdata SET wc='AJO AZXC' WHERE wc=='AJO_AZXC'; -- FIXED: improper clli format character s/_/ / (AJO_AZXCDS0)
DELETE FROM wcdata WHERE wc=='AJO_AZXC'; -- FIXED: improper clli format character s/_/ / (AJO_AZXCDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ELK CA11RS1' WHERE switch=='ELK_CA11RS1'; -- FIXED: improper clli format character s/_/ / (ELK_CA11RS1) [ELK_CA11RS1]
DELETE FROM switchdata WHERE switch=='ELK_CA11RS1'; -- FIXED: improper clli format character s/_/ / (ELK_CA11RS1) [ELK_CA11RS1]
UPDATE OR IGNORE wcdata SET wc='ELK CA11' WHERE wc=='ELK_CA11'; -- FIXED: improper clli format character s/_/ / (ELK_CA11RS1)
DELETE FROM wcdata WHERE wc=='ELK_CA11'; -- FIXED: improper clli format character s/_/ / (ELK_CA11RS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='KIM COXCDS1' WHERE switch=='KIM_COXCDS1'; -- FIXED: improper clli format character s/_/ / (KIM_COXCDS1) [KIM_COXCDS1]
DELETE FROM switchdata WHERE switch=='KIM_COXCDS1'; -- FIXED: improper clli format character s/_/ / (KIM_COXCDS1) [KIM_COXCDS1]
UPDATE OR IGNORE wcdata SET wc='KIM COXC' WHERE wc=='KIM_COXC'; -- FIXED: improper clli format character s/_/ / (KIM_COXCDS1)
DELETE FROM wcdata WHERE wc=='KIM_COXC'; -- FIXED: improper clli format character s/_/ / (KIM_COXCDS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='RYE COXCRS0' WHERE switch=='RYE_COXCRS0'; -- FIXED: improper clli format character s/_/ / (RYE_COXCRS0) [RYE_COXCRS0]
DELETE FROM switchdata WHERE switch=='RYE_COXCRS0'; -- FIXED: improper clli format character s/_/ / (RYE_COXCRS0) [RYE_COXCRS0]
UPDATE OR IGNORE wcdata SET wc='RYE COXC' WHERE wc=='RYE_COXC'; -- FIXED: improper clli format character s/_/ / (RYE_COXCRS0)
DELETE FROM wcdata WHERE wc=='RYE_COXC'; -- FIXED: improper clli format character s/_/ / (RYE_COXCRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='JAY FLMARS0' WHERE switch=='JAY_FLMARS0'; -- FIXED: improper clli format character s/_/ / (JAY_FLMARS0) [JAY_FLMARS0]
DELETE FROM switchdata WHERE switch=='JAY_FLMARS0'; -- FIXED: improper clli format character s/_/ / (JAY_FLMARS0) [JAY_FLMARS0]
UPDATE OR IGNORE wcdata SET wc='JAY FLMA' WHERE wc=='JAY_FLMA'; -- FIXED: improper clli format character s/_/ / (JAY_FLMARS0)
DELETE FROM wcdata WHERE wc=='JAY_FLMA'; -- FIXED: improper clli format character s/_/ / (JAY_FLMARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='LEE FLXARS0' WHERE switch=='LEE_FLXARS0'; -- FIXED: improper clli format character s/_/ / (LEE_FLXARS0) [LEE_FLXARS0]
DELETE FROM switchdata WHERE switch=='LEE_FLXARS0'; -- FIXED: improper clli format character s/_/ / (LEE_FLXARS0) [LEE_FLXARS0]
UPDATE OR IGNORE wcdata SET wc='LEE FLXA' WHERE wc=='LEE_FLXA'; -- FIXED: improper clli format character s/_/ / (LEE_FLXARS0)
DELETE FROM wcdata WHERE wc=='LEE_FLXA'; -- FIXED: improper clli format character s/_/ / (LEE_FLXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='GAY GAMA53A' WHERE switch=='GAY_GAMA53A'; -- FIXED: improper clli format character s/_/ / (GAY_GAMA53A) [GAY_GAMA53A]
DELETE FROM switchdata WHERE switch=='GAY_GAMA53A'; -- FIXED: improper clli format character s/_/ / (GAY_GAMA53A) [GAY_GAMA53A]
UPDATE OR IGNORE wcdata SET wc='GAY GAMA' WHERE wc=='GAY_GAMA'; -- FIXED: improper clli format character s/_/ / (GAY_GAMA53A)
DELETE FROM wcdata WHERE wc=='GAY_GAMA'; -- FIXED: improper clli format character s/_/ / (GAY_GAMA53A)
---------------------
UPDATE OR IGNORE switchdata SET switch='ILA GAXARS1' WHERE switch=='ILA_GAXARS1'; -- FIXED: improper clli format character s/_/ / (ILA_GAXARS1) [ILA_GAXARS1]
DELETE FROM switchdata WHERE switch=='ILA_GAXARS1'; -- FIXED: improper clli format character s/_/ / (ILA_GAXARS1) [ILA_GAXARS1]
UPDATE OR IGNORE wcdata SET wc='ILA GAXA' WHERE wc=='ILA_GAXA'; -- FIXED: improper clli format character s/_/ / (ILA_GAXARS1)
DELETE FROM wcdata WHERE wc=='ILA_GAXA'; -- FIXED: improper clli format character s/_/ / (ILA_GAXARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='EWA HICORS1' WHERE switch=='EWA_HICORS1'; -- FIXED: improper clli format character s/_/ / (EWA_HICORS1) [EWA_HICORS1]
DELETE FROM switchdata WHERE switch=='EWA_HICORS1'; -- FIXED: improper clli format character s/_/ / (EWA_HICORS1) [EWA_HICORS1]
UPDATE OR IGNORE wcdata SET wc='EWA HICO' WHERE wc=='EWA_HICO'; -- FIXED: improper clli format character s/_/ / (EWA_HICORS1)
DELETE FROM wcdata WHERE wc=='EWA_HICO'; -- FIXED: improper clli format character s/_/ / (EWA_HICORS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='EWA HIXARS0' WHERE switch=='EWA_HIXARS0'; -- FIXED: improper clli format character s/_/ / (EWA_HIXARS0) [EWA_HIXARS0]
DELETE FROM switchdata WHERE switch=='EWA_HIXARS0'; -- FIXED: improper clli format character s/_/ / (EWA_HIXARS0) [EWA_HIXARS0]
UPDATE OR IGNORE wcdata SET wc='EWA HIXA' WHERE wc=='EWA_HIXA'; -- FIXED: improper clli format character s/_/ / (EWA_HIXARS0)
DELETE FROM wcdata WHERE wc=='EWA_HIXA'; -- FIXED: improper clli format character s/_/ / (EWA_HIXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ELY IAXORS8' WHERE switch=='ELY_IAXORS8'; -- FIXED: improper clli format character s/_/ / (ELY_IAXORS8) [ELY_IAXORS8]
DELETE FROM switchdata WHERE switch=='ELY_IAXORS8'; -- FIXED: improper clli format character s/_/ / (ELY_IAXORS8) [ELY_IAXORS8]
UPDATE OR IGNORE wcdata SET wc='ELY IAXO' WHERE wc=='ELY_IAXO'; -- FIXED: improper clli format character s/_/ / (ELY_IAXORS8)
DELETE FROM wcdata WHERE wc=='ELY_IAXO'; -- FIXED: improper clli format character s/_/ / (ELY_IAXORS8)
---------------------
UPDATE OR IGNORE switchdata SET switch='OTO IAXODS0' WHERE switch=='OTO_IAXODS0'; -- FIXED: improper clli format character s/_/ / (OTO_IAXODS0) [OTO_IAXODS0]
DELETE FROM switchdata WHERE switch=='OTO_IAXODS0'; -- FIXED: improper clli format character s/_/ / (OTO_IAXODS0) [OTO_IAXODS0]
UPDATE OR IGNORE wcdata SET wc='OTO IAXO' WHERE wc=='OTO_IAXO'; -- FIXED: improper clli format character s/_/ / (OTO_IAXODS0)
DELETE FROM wcdata WHERE wc=='OTO_IAXO'; -- FIXED: improper clli format character s/_/ / (OTO_IAXODS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='UTE IAXORS0' WHERE switch=='UTE_IAXORS0'; -- FIXED: improper clli format character s/_/ / (UTE_IAXORS0) [UTE_IAXORS0]
DELETE FROM switchdata WHERE switch=='UTE_IAXORS0'; -- FIXED: improper clli format character s/_/ / (UTE_IAXORS0) [UTE_IAXORS0]
UPDATE OR IGNORE wcdata SET wc='UTE IAXO' WHERE wc=='UTE_IAXO'; -- FIXED: improper clli format character s/_/ / (UTE_IAXORS0)
DELETE FROM wcdata WHERE wc=='UTE_IAXO'; -- FIXED: improper clli format character s/_/ / (UTE_IAXORS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='MAY IDXCRS1' WHERE switch=='MAY_IDXCRS1'; -- FIXED: improper clli format character s/_/ / (MAY_IDXCRS1) [MAY_IDXCRS1]
DELETE FROM switchdata WHERE switch=='MAY_IDXCRS1'; -- FIXED: improper clli format character s/_/ / (MAY_IDXCRS1) [MAY_IDXCRS1]
UPDATE OR IGNORE wcdata SET wc='MAY IDXC' WHERE wc=='MAY_IDXC'; -- FIXED: improper clli format character s/_/ / (MAY_IDXCRS1)
DELETE FROM wcdata WHERE wc=='MAY_IDXC'; -- FIXED: improper clli format character s/_/ / (MAY_IDXCRS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='AVA ILXERS0' WHERE switch=='AVA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (AVA_ILXERS0) [AVA_ILXERS0]
DELETE FROM switchdata WHERE switch=='AVA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (AVA_ILXERS0) [AVA_ILXERS0]
UPDATE OR IGNORE wcdata SET wc='AVA ILXE' WHERE wc=='AVA_ILXE'; -- FIXED: improper clli format character s/_/ / (AVA_ILXERS0)
DELETE FROM wcdata WHERE wc=='AVA_ILXE'; -- FIXED: improper clli format character s/_/ / (AVA_ILXERS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='DOW ILXERS0' WHERE switch=='DOW_ILXERS0'; -- FIXED: improper clli format character s/_/ / (DOW_ILXERS0) [DOW_ILXERS0]
DELETE FROM switchdata WHERE switch=='DOW_ILXERS0'; -- FIXED: improper clli format character s/_/ / (DOW_ILXERS0) [DOW_ILXERS0]
UPDATE OR IGNORE wcdata SET wc='DOW ILXE' WHERE wc=='DOW_ILXE'; -- FIXED: improper clli format character s/_/ / (DOW_ILXERS0)
DELETE FROM wcdata WHERE wc=='DOW_ILXE'; -- FIXED: improper clli format character s/_/ / (DOW_ILXERS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='INA ILXERS0' WHERE switch=='INA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (INA_ILXERS0) [INA_ILXERS0]
DELETE FROM switchdata WHERE switch=='INA_ILXERS0'; -- FIXED: improper clli format character s/_/ / (INA_ILXERS0) [INA_ILXERS0]
UPDATE OR IGNORE wcdata SET wc='INA ILXE' WHERE wc=='INA_ILXE'; -- FIXED: improper clli format character s/_/ / (INA_ILXERS0)
DELETE FROM wcdata WHERE wc=='INA_ILXE'; -- FIXED: improper clli format character s/_/ / (INA_ILXERS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='JOY ILXDRS0' WHERE switch=='JOY_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (JOY_ILXDRS0) [JOY_ILXDRS0]
DELETE FROM switchdata WHERE switch=='JOY_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (JOY_ILXDRS0) [JOY_ILXDRS0]
UPDATE OR IGNORE wcdata SET wc='JOY ILXD' WHERE wc=='JOY_ILXD'; -- FIXED: improper clli format character s/_/ / (JOY_ILXDRS0)
DELETE FROM wcdata WHERE wc=='JOY_ILXD'; -- FIXED: improper clli format character s/_/ / (JOY_ILXDRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='RIO ILABRS0' WHERE switch=='RIO_ILABRS0'; -- FIXED: improper clli format character s/_/ / (RIO_ILABRS0) [RIO_ILABRS0]
DELETE FROM switchdata WHERE switch=='RIO_ILABRS0'; -- FIXED: improper clli format character s/_/ / (RIO_ILABRS0) [RIO_ILABRS0]
UPDATE OR IGNORE wcdata SET wc='RIO ILAB' WHERE wc=='RIO_ILAB'; -- FIXED: improper clli format character s/_/ / (RIO_ILABRS0)
DELETE FROM wcdata WHERE wc=='RIO_ILAB'; -- FIXED: improper clli format character s/_/ / (RIO_ILABRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='RIO ILXDRS0' WHERE switch=='RIO_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (RIO_ILXDRS0) [RIO_ILXDRS0]
DELETE FROM switchdata WHERE switch=='RIO_ILXDRS0'; -- FIXED: improper clli format character s/_/ / (RIO_ILXDRS0) [RIO_ILXDRS0]
UPDATE OR IGNORE wcdata SET wc='RIO ILXD' WHERE wc=='RIO_ILXD'; -- FIXED: improper clli format character s/_/ / (RIO_ILXDRS0)
DELETE FROM wcdata WHERE wc=='RIO_ILXD'; -- FIXED: improper clli format character s/_/ / (RIO_ILXDRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='LEO INXARS3' WHERE switch=='LEO_INXARS3'; -- FIXED: improper clli format character s/_/ / (LEO_INXARS3) [LEO_INXARS3]
DELETE FROM switchdata WHERE switch=='LEO_INXARS3'; -- FIXED: improper clli format character s/_/ / (LEO_INXARS3) [LEO_INXARS3]
UPDATE OR IGNORE wcdata SET wc='LEO INXA' WHERE wc=='LEO_INXA'; -- FIXED: improper clli format character s/_/ / (LEO_INXARS3)
DELETE FROM wcdata WHERE wc=='LEO_INXA'; -- FIXED: improper clli format character s/_/ / (LEO_INXARS3)
---------------------
UPDATE OR IGNORE switchdata SET switch='POE INXARS0' WHERE switch=='POE_INXARS0'; -- FIXED: improper clli format character s/_/ / (POE_INXARS0) [POE_INXARS0]
DELETE FROM switchdata WHERE switch=='POE_INXARS0'; -- FIXED: improper clli format character s/_/ / (POE_INXARS0) [POE_INXARS0]
UPDATE OR IGNORE wcdata SET wc='POE INXA' WHERE wc=='POE_INXA'; -- FIXED: improper clli format character s/_/ / (POE_INXARS0)
DELETE FROM wcdata WHERE wc=='POE_INXA'; -- FIXED: improper clli format character s/_/ / (POE_INXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='IDA LAXARS1' WHERE switch=='IDA_LAXARS1'; -- FIXED: improper clli format character s/_/ / (IDA_LAXARS1) [IDA_LAXARS1]
DELETE FROM switchdata WHERE switch=='IDA_LAXARS1'; -- FIXED: improper clli format character s/_/ / (IDA_LAXARS1) [IDA_LAXARS1]
UPDATE OR IGNORE wcdata SET wc='IDA LAXA' WHERE wc=='IDA_LAXA'; -- FIXED: improper clli format character s/_/ / (IDA_LAXARS1)
DELETE FROM wcdata WHERE wc=='IDA_LAXA'; -- FIXED: improper clli format character s/_/ / (IDA_LAXARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='LEE MAHIDS0' WHERE switch=='LEE_MAHIDS0'; -- FIXED: improper clli format character s/_/ / (LEE_MAHIDS0) [LEE_MAHIDS0]
DELETE FROM switchdata WHERE switch=='LEE_MAHIDS0'; -- FIXED: improper clli format character s/_/ / (LEE_MAHIDS0) [LEE_MAHIDS0]
UPDATE OR IGNORE wcdata SET wc='LEE MAHI' WHERE wc=='LEE_MAHI'; -- FIXED: improper clli format character s/_/ / (LEE_MAHIDS0)
DELETE FROM wcdata WHERE wc=='LEE_MAHI'; -- FIXED: improper clli format character s/_/ / (LEE_MAHIDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ADA MIMNDS0' WHERE switch=='ADA_MIMNDS0'; -- FIXED: improper clli format character s/_/ / (ADA_MIMNDS0) [ADA_MIMNDS0]
DELETE FROM switchdata WHERE switch=='ADA_MIMNDS0'; -- FIXED: improper clli format character s/_/ / (ADA_MIMNDS0) [ADA_MIMNDS0]
UPDATE OR IGNORE wcdata SET wc='ADA MIMN' WHERE wc=='ADA_MIMN'; -- FIXED: improper clli format character s/_/ / (ADA_MIMNDS0)
DELETE FROM wcdata WHERE wc=='ADA_MIMN'; -- FIXED: improper clli format character s/_/ / (ADA_MIMNDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='IDA MIXHRS0' WHERE switch=='IDA_MIXHRS0'; -- FIXED: improper clli format character s/_/ / (IDA_MIXHRS0) [IDA_MIXHRS0]
DELETE FROM switchdata WHERE switch=='IDA_MIXHRS0'; -- FIXED: improper clli format character s/_/ / (IDA_MIXHRS0) [IDA_MIXHRS0]
UPDATE OR IGNORE wcdata SET wc='IDA MIXH' WHERE wc=='IDA_MIXH'; -- FIXED: improper clli format character s/_/ / (IDA_MIXHRS0)
DELETE FROM wcdata WHERE wc=='IDA_MIXH'; -- FIXED: improper clli format character s/_/ / (IDA_MIXHRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='MIO MIXGDS0' WHERE switch=='MIO_MIXGDS0'; -- FIXED: improper clli format character s/_/ / (MIO_MIXGDS0) [MIO_MIXGDS0]
DELETE FROM switchdata WHERE switch=='MIO_MIXGDS0'; -- FIXED: improper clli format character s/_/ / (MIO_MIXGDS0) [MIO_MIXGDS0]
UPDATE OR IGNORE wcdata SET wc='MIO MIXG' WHERE wc=='MIO_MIXG'; -- FIXED: improper clli format character s/_/ / (MIO_MIXGDS0)
DELETE FROM wcdata WHERE wc=='MIO_MIXG'; -- FIXED: improper clli format character s/_/ / (MIO_MIXGDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ADA MN010MD' WHERE switch=='ADA_MN010MD'; -- FIXED: improper clli format character s/_/ / (ADA_MN010MD) [ADA_MN010MD]
DELETE FROM switchdata WHERE switch=='ADA_MN010MD'; -- FIXED: improper clli format character s/_/ / (ADA_MN010MD) [ADA_MN010MD]
UPDATE OR IGNORE wcdata SET wc='ADA MN01' WHERE wc=='ADA_MN01'; -- FIXED: improper clli format character s/_/ / (ADA_MN010MD)
DELETE FROM wcdata WHERE wc=='ADA_MN01'; -- FIXED: improper clli format character s/_/ / (ADA_MN010MD)
---------------------
UPDATE OR IGNORE switchdata SET switch='ADA MNXADS0' WHERE switch=='ADA_MNXADS0'; -- FIXED: improper clli format character s/_/ / (ADA_MNXADS0) [ADA_MNXADS0]
DELETE FROM switchdata WHERE switch=='ADA_MNXADS0'; -- FIXED: improper clli format character s/_/ / (ADA_MNXADS0) [ADA_MNXADS0]
UPDATE OR IGNORE wcdata SET wc='ADA MNXA' WHERE wc=='ADA_MNXA'; -- FIXED: improper clli format character s/_/ / (ADA_MNXADS0)
DELETE FROM wcdata WHERE wc=='ADA_MNXA'; -- FIXED: improper clli format character s/_/ / (ADA_MNXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ELY MNXEDS0' WHERE switch=='ELY_MNXEDS0'; -- FIXED: improper clli format character s/_/ / (ELY_MNXEDS0) [ELY_MNXEDS0]
DELETE FROM switchdata WHERE switch=='ELY_MNXEDS0'; -- FIXED: improper clli format character s/_/ / (ELY_MNXEDS0) [ELY_MNXEDS0]
UPDATE OR IGNORE wcdata SET wc='ELY MNXE' WHERE wc=='ELY_MNXE'; -- FIXED: improper clli format character s/_/ / (ELY_MNXEDS0)
DELETE FROM wcdata WHERE wc=='ELY_MNXE'; -- FIXED: improper clli format character s/_/ / (ELY_MNXEDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ORR MNXADS0' WHERE switch=='ORR_MNXADS0'; -- FIXED: improper clli format character s/_/ / (ORR_MNXADS0) [ORR_MNXADS0]
DELETE FROM switchdata WHERE switch=='ORR_MNXADS0'; -- FIXED: improper clli format character s/_/ / (ORR_MNXADS0) [ORR_MNXADS0]
UPDATE OR IGNORE wcdata SET wc='ORR MNXA' WHERE wc=='ORR_MNXA'; -- FIXED: improper clli format character s/_/ / (ORR_MNXADS0)
DELETE FROM wcdata WHERE wc=='ORR_MNXA'; -- FIXED: improper clli format character s/_/ / (ORR_MNXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='AVA MOXADS0' WHERE switch=='AVA_MOXADS0'; -- FIXED: improper clli format character s/_/ / (AVA_MOXADS0) [AVA_MOXADS0]
DELETE FROM switchdata WHERE switch=='AVA_MOXADS0'; -- FIXED: improper clli format character s/_/ / (AVA_MOXADS0) [AVA_MOXADS0]
UPDATE OR IGNORE wcdata SET wc='AVA MOXA' WHERE wc=='AVA_MOXA'; -- FIXED: improper clli format character s/_/ / (AVA_MOXADS0)
DELETE FROM wcdata WHERE wc=='AVA_MOXA'; -- FIXED: improper clli format character s/_/ / (AVA_MOXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ROY MTXCCM1' WHERE switch=='ROY_MTXCCM1'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCCM1) [ROY_MTXCCM1]
DELETE FROM switchdata WHERE switch=='ROY_MTXCCM1'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCCM1) [ROY_MTXCCM1]
UPDATE OR IGNORE wcdata SET wc='ROY MTXC' WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCCM1)
DELETE FROM wcdata WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCCM1)
---------------------
UPDATE OR IGNORE switchdata SET switch='ROY MTXCRS1' WHERE switch=='ROY_MTXCRS1'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCRS1) [ROY_MTXCRS1]
DELETE FROM switchdata WHERE switch=='ROY_MTXCRS1'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCRS1) [ROY_MTXCRS1]
UPDATE OR IGNORE wcdata SET wc='ROY MTXC' WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCRS1)
DELETE FROM wcdata WHERE wc=='ROY_MTXC'; -- FIXED: improper clli format character s/_/ / (ROY_MTXCRS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='ULM MTMARS1' WHERE switch=='ULM_MTMARS1'; -- FIXED: improper clli format character s/_/ / (ULM_MTMARS1) [ULM_MTMARS1]
DELETE FROM switchdata WHERE switch=='ULM_MTMARS1'; -- FIXED: improper clli format character s/_/ / (ULM_MTMARS1) [ULM_MTMARS1]
UPDATE OR IGNORE wcdata SET wc='ULM MTMA' WHERE wc=='ULM_MTMA'; -- FIXED: improper clli format character s/_/ / (ULM_MTMARS1)
DELETE FROM wcdata WHERE wc=='ULM_MTMA'; -- FIXED: improper clli format character s/_/ / (ULM_MTMARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='MAX NDXARS6' WHERE switch=='MAX_NDXARS6'; -- FIXED: improper clli format character s/_/ / (MAX_NDXARS6) [MAX_NDXARS6]
DELETE FROM switchdata WHERE switch=='MAX_NDXARS6'; -- FIXED: improper clli format character s/_/ / (MAX_NDXARS6) [MAX_NDXARS6]
UPDATE OR IGNORE wcdata SET wc='MAX NDXA' WHERE wc=='MAX_NDXA'; -- FIXED: improper clli format character s/_/ / (MAX_NDXARS6)
DELETE FROM wcdata WHERE wc=='MAX_NDXA'; -- FIXED: improper clli format character s/_/ / (MAX_NDXARS6)
---------------------
UPDATE OR IGNORE switchdata SET switch='RAY NDXADS2' WHERE switch=='RAY_NDXADS2'; -- FIXED: improper clli format character s/_/ / (RAY_NDXADS2) [RAY_NDXADS2]
DELETE FROM switchdata WHERE switch=='RAY_NDXADS2'; -- FIXED: improper clli format character s/_/ / (RAY_NDXADS2) [RAY_NDXADS2]
UPDATE OR IGNORE wcdata SET wc='RAY NDXA' WHERE wc=='RAY_NDXA'; -- FIXED: improper clli format character s/_/ / (RAY_NDXADS2)
DELETE FROM wcdata WHERE wc=='RAY_NDXA'; -- FIXED: improper clli format character s/_/ / (RAY_NDXADS2)
---------------------
UPDATE OR IGNORE switchdata SET switch='ZAP NDXARS9' WHERE switch=='ZAP_NDXARS9'; -- FIXED: improper clli format character s/_/ / (ZAP_NDXARS9) [ZAP_NDXARS9]
DELETE FROM switchdata WHERE switch=='ZAP_NDXARS9'; -- FIXED: improper clli format character s/_/ / (ZAP_NDXARS9) [ZAP_NDXARS9]
UPDATE OR IGNORE wcdata SET wc='ZAP NDXA' WHERE wc=='ZAP_NDXA'; -- FIXED: improper clli format character s/_/ / (ZAP_NDXARS9)
DELETE FROM wcdata WHERE wc=='ZAP_NDXA'; -- FIXED: improper clli format character s/_/ / (ZAP_NDXARS9)
---------------------
UPDATE OR IGNORE switchdata SET switch='DIX NEXSRS0' WHERE switch=='DIX_NEXSRS0'; -- FIXED: improper clli format character s/_/ / (DIX_NEXSRS0) [DIX_NEXSRS0]
DELETE FROM switchdata WHERE switch=='DIX_NEXSRS0'; -- FIXED: improper clli format character s/_/ / (DIX_NEXSRS0) [DIX_NEXSRS0]
UPDATE OR IGNORE wcdata SET wc='DIX NEXS' WHERE wc=='DIX_NEXS'; -- FIXED: improper clli format character s/_/ / (DIX_NEXSRS0)
DELETE FROM wcdata WHERE wc=='DIX_NEXS'; -- FIXED: improper clli format character s/_/ / (DIX_NEXSRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ONG NEXLRS0' WHERE switch=='ONG_NEXLRS0'; -- FIXED: improper clli format character s/_/ / (ONG_NEXLRS0) [ONG_NEXLRS0]
DELETE FROM switchdata WHERE switch=='ONG_NEXLRS0'; -- FIXED: improper clli format character s/_/ / (ONG_NEXLRS0) [ONG_NEXLRS0]
UPDATE OR IGNORE wcdata SET wc='ONG NEXL' WHERE wc=='ONG_NEXL'; -- FIXED: improper clli format character s/_/ / (ONG_NEXLRS0)
DELETE FROM wcdata WHERE wc=='ONG_NEXL'; -- FIXED: improper clli format character s/_/ / (ONG_NEXLRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ORD NEXHDS0' WHERE switch=='ORD_NEXHDS0'; -- FIXED: improper clli format character s/_/ / (ORD_NEXHDS0) [ORD_NEXHDS0]
DELETE FROM switchdata WHERE switch=='ORD_NEXHDS0'; -- FIXED: improper clli format character s/_/ / (ORD_NEXHDS0) [ORD_NEXHDS0]
UPDATE OR IGNORE wcdata SET wc='ORD NEXH' WHERE wc=='ORD_NEXH'; -- FIXED: improper clli format character s/_/ / (ORD_NEXHDS0)
DELETE FROM wcdata WHERE wc=='ORD_NEXH'; -- FIXED: improper clli format character s/_/ / (ORD_NEXHDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='JAL NMXARS1' WHERE switch=='JAL_NMXARS1'; -- FIXED: improper clli format character s/_/ / (JAL_NMXARS1) [JAL_NMXARS1]
DELETE FROM switchdata WHERE switch=='JAL_NMXARS1'; -- FIXED: improper clli format character s/_/ / (JAL_NMXARS1) [JAL_NMXARS1]
UPDATE OR IGNORE wcdata SET wc='JAL NMXA' WHERE wc=='JAL_NMXA'; -- FIXED: improper clli format character s/_/ / (JAL_NMXARS1)
DELETE FROM wcdata WHERE wc=='JAL_NMXA'; -- FIXED: improper clli format character s/_/ / (JAL_NMXARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='ROY NMXCRS0' WHERE switch=='ROY_NMXCRS0'; -- FIXED: improper clli format character s/_/ / (ROY_NMXCRS0) [ROY_NMXCRS0]
DELETE FROM switchdata WHERE switch=='ROY_NMXCRS0'; -- FIXED: improper clli format character s/_/ / (ROY_NMXCRS0) [ROY_NMXCRS0]
UPDATE OR IGNORE wcdata SET wc='ROY NMXC' WHERE wc=='ROY_NMXC'; -- FIXED: improper clli format character s/_/ / (ROY_NMXCRS0)
DELETE FROM wcdata WHERE wc=='ROY_NMXC'; -- FIXED: improper clli format character s/_/ / (ROY_NMXCRS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ELY NV01DS1' WHERE switch=='ELY_NV01DS1'; -- FIXED: improper clli format character s/_/ / (ELY_NV01DS1) [ELY_NV01DS1]
DELETE FROM switchdata WHERE switch=='ELY_NV01DS1'; -- FIXED: improper clli format character s/_/ / (ELY_NV01DS1) [ELY_NV01DS1]
UPDATE OR IGNORE wcdata SET wc='ELY NV01' WHERE wc=='ELY_NV01'; -- FIXED: improper clli format character s/_/ / (ELY_NV01DS1)
DELETE FROM wcdata WHERE wc=='ELY_NV01'; -- FIXED: improper clli format character s/_/ / (ELY_NV01DS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='RYE NYRYDS0' WHERE switch=='RYE_NYRYDS0'; -- FIXED: improper clli format character s/_/ / (RYE_NYRYDS0) [RYE_NYRYDS0]
DELETE FROM switchdata WHERE switch=='RYE_NYRYDS0'; -- FIXED: improper clli format character s/_/ / (RYE_NYRYDS0) [RYE_NYRYDS0]
UPDATE OR IGNORE wcdata SET wc='RYE NYRY' WHERE wc=='RYE_NYRY'; -- FIXED: improper clli format character s/_/ / (RYE_NYRYDS0)
DELETE FROM wcdata WHERE wc=='RYE_NYRY'; -- FIXED: improper clli format character s/_/ / (RYE_NYRYDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ADA OHXARS1' WHERE switch=='ADA_OHXARS1'; -- FIXED: improper clli format character s/_/ / (ADA_OHXARS1) [ADA_OHXARS1]
DELETE FROM switchdata WHERE switch=='ADA_OHXARS1'; -- FIXED: improper clli format character s/_/ / (ADA_OHXARS1) [ADA_OHXARS1]
UPDATE OR IGNORE wcdata SET wc='ADA OHXA' WHERE wc=='ADA_OHXA'; -- FIXED: improper clli format character s/_/ / (ADA_OHXARS1)
DELETE FROM wcdata WHERE wc=='ADA_OHXA'; -- FIXED: improper clli format character s/_/ / (ADA_OHXARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='NEY OHXARS0' WHERE switch=='NEY_OHXARS0'; -- FIXED: improper clli format character s/_/ / (NEY_OHXARS0) [NEY_OHXARS0]
DELETE FROM switchdata WHERE switch=='NEY_OHXARS0'; -- FIXED: improper clli format character s/_/ / (NEY_OHXARS0) [NEY_OHXARS0]
UPDATE OR IGNORE wcdata SET wc='NEY OHXA' WHERE wc=='NEY_OHXA'; -- FIXED: improper clli format character s/_/ / (NEY_OHXARS0)
DELETE FROM wcdata WHERE wc=='NEY_OHXA'; -- FIXED: improper clli format character s/_/ / (NEY_OHXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ADA OKBSDS0' WHERE switch=='ADA_OKBSDS0'; -- FIXED: improper clli format character s/_/ / (ADA_OKBSDS0) [ADA_OKBSDS0]
DELETE FROM switchdata WHERE switch=='ADA_OKBSDS0'; -- FIXED: improper clli format character s/_/ / (ADA_OKBSDS0) [ADA_OKBSDS0]
UPDATE OR IGNORE wcdata SET wc='ADA OKBS' WHERE wc=='ADA_OKBS'; -- FIXED: improper clli format character s/_/ / (ADA_OKBSDS0)
DELETE FROM wcdata WHERE wc=='ADA_OKBS'; -- FIXED: improper clli format character s/_/ / (ADA_OKBSDS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ADA OKMADS1' WHERE switch=='ADA_OKMADS1'; -- FIXED: improper clli format character s/_/ / (ADA_OKMADS1) [ADA_OKMADS1]
DELETE FROM switchdata WHERE switch=='ADA_OKMADS1'; -- FIXED: improper clli format character s/_/ / (ADA_OKMADS1) [ADA_OKMADS1]
UPDATE OR IGNORE wcdata SET wc='ADA OKMA' WHERE wc=='ADA_OKMA'; -- FIXED: improper clli format character s/_/ / (ADA_OKMADS1)
DELETE FROM wcdata WHERE wc=='ADA_OKMA'; -- FIXED: improper clli format character s/_/ / (ADA_OKMADS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='EVA OKXARS1' WHERE switch=='EVA_OKXARS1'; -- FIXED: improper clli format character s/_/ / (EVA_OKXARS1) [EVA_OKXARS1]
DELETE FROM switchdata WHERE switch=='EVA_OKXARS1'; -- FIXED: improper clli format character s/_/ / (EVA_OKXARS1) [EVA_OKXARS1]
UPDATE OR IGNORE wcdata SET wc='EVA OKXA' WHERE wc=='EVA_OKXA'; -- FIXED: improper clli format character s/_/ / (EVA_OKXARS1)
DELETE FROM wcdata WHERE wc=='EVA_OKXA'; -- FIXED: improper clli format character s/_/ / (EVA_OKXARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='FAY OKXARS1' WHERE switch=='FAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (FAY_OKXARS1) [FAY_OKXARS1]
DELETE FROM switchdata WHERE switch=='FAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (FAY_OKXARS1) [FAY_OKXARS1]
UPDATE OR IGNORE wcdata SET wc='FAY OKXA' WHERE wc=='FAY_OKXA'; -- FIXED: improper clli format character s/_/ / (FAY_OKXARS1)
DELETE FROM wcdata WHERE wc=='FAY_OKXA'; -- FIXED: improper clli format character s/_/ / (FAY_OKXARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='JAY OKXADS0' WHERE switch=='JAY_OKXADS0'; -- FIXED: improper clli format character s/_/ / (JAY_OKXADS0) [JAY_OKXADS0]
DELETE FROM switchdata WHERE switch=='JAY_OKXADS0'; -- FIXED: improper clli format character s/_/ / (JAY_OKXADS0) [JAY_OKXADS0]
UPDATE OR IGNORE wcdata SET wc='JAY OKXA' WHERE wc=='JAY_OKXA'; -- FIXED: improper clli format character s/_/ / (JAY_OKXADS0)
DELETE FROM wcdata WHERE wc=='JAY_OKXA'; -- FIXED: improper clli format character s/_/ / (JAY_OKXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='MAY OKXARS1' WHERE switch=='MAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (MAY_OKXARS1) [MAY_OKXARS1]
DELETE FROM switchdata WHERE switch=='MAY_OKXARS1'; -- FIXED: improper clli format character s/_/ / (MAY_OKXARS1) [MAY_OKXARS1]
UPDATE OR IGNORE wcdata SET wc='MAY OKXA' WHERE wc=='MAY_OKXA'; -- FIXED: improper clli format character s/_/ / (MAY_OKXARS1)
DELETE FROM wcdata WHERE wc=='MAY_OKXA'; -- FIXED: improper clli format character s/_/ / (MAY_OKXARS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='BLY ORXARS0' WHERE switch=='BLY_ORXARS0'; -- FIXED: improper clli format character s/_/ / (BLY_ORXARS0) [BLY_ORXARS0]
DELETE FROM switchdata WHERE switch=='BLY_ORXARS0'; -- FIXED: improper clli format character s/_/ / (BLY_ORXARS0) [BLY_ORXARS0]
UPDATE OR IGNORE wcdata SET wc='BLY ORXA' WHERE wc=='BLY_ORXA'; -- FIXED: improper clli format character s/_/ / (BLY_ORXARS0)
DELETE FROM wcdata WHERE wc=='BLY_ORXA'; -- FIXED: improper clli format character s/_/ / (BLY_ORXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='GAP PAXGRS1' WHERE switch=='GAP_PAXGRS1'; -- FIXED: improper clli format character s/_/ / (GAP_PAXGRS1) [GAP_PAXGRS1]
DELETE FROM switchdata WHERE switch=='GAP_PAXGRS1'; -- FIXED: improper clli format character s/_/ / (GAP_PAXGRS1) [GAP_PAXGRS1]
UPDATE OR IGNORE wcdata SET wc='GAP PAXG' WHERE wc=='GAP_PAXG'; -- FIXED: improper clli format character s/_/ / (GAP_PAXGRS1)
DELETE FROM wcdata WHERE wc=='GAP_PAXG'; -- FIXED: improper clli format character s/_/ / (GAP_PAXGRS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='REW PARERS1' WHERE switch=='REW_PARERS1'; -- FIXED: improper clli format character s/_/ / (REW_PARERS1) [REW_PARERS1]
DELETE FROM switchdata WHERE switch=='REW_PARERS1'; -- FIXED: improper clli format character s/_/ / (REW_PARERS1) [REW_PARERS1]
UPDATE OR IGNORE wcdata SET wc='REW PARE' WHERE wc=='REW_PARE'; -- FIXED: improper clli format character s/_/ / (REW_PARERS1)
DELETE FROM wcdata WHERE wc=='REW_PARE'; -- FIXED: improper clli format character s/_/ / (REW_PARERS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='IVA SCXARS0' WHERE switch=='IVA_SCXARS0'; -- FIXED: improper clli format character s/_/ / (IVA_SCXARS0) [IVA_SCXARS0]
DELETE FROM switchdata WHERE switch=='IVA_SCXARS0'; -- FIXED: improper clli format character s/_/ / (IVA_SCXARS0) [IVA_SCXARS0]
UPDATE OR IGNORE wcdata SET wc='IVA SCXA' WHERE wc=='IVA_SCXA'; -- FIXED: improper clli format character s/_/ / (IVA_SCXARS0)
DELETE FROM wcdata WHERE wc=='IVA_SCXA'; -- FIXED: improper clli format character s/_/ / (IVA_SCXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='TEA SDCORS1' WHERE switch=='TEA_SDCORS1'; -- FIXED: improper clli format character s/_/ / (TEA_SDCORS1) [TEA_SDCORS1]
DELETE FROM switchdata WHERE switch=='TEA_SDCORS1'; -- FIXED: improper clli format character s/_/ / (TEA_SDCORS1) [TEA_SDCORS1]
UPDATE OR IGNORE wcdata SET wc='TEA SDCO' WHERE wc=='TEA_SDCO'; -- FIXED: improper clli format character s/_/ / (TEA_SDCORS1)
DELETE FROM wcdata WHERE wc=='TEA_SDCO'; -- FIXED: improper clli format character s/_/ / (TEA_SDCORS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='ARP TXXADS0' WHERE switch=='ARP_TXXADS0'; -- FIXED: improper clli format character s/_/ / (ARP_TXXADS0) [ARP_TXXADS0]
DELETE FROM switchdata WHERE switch=='ARP_TXXADS0'; -- FIXED: improper clli format character s/_/ / (ARP_TXXADS0) [ARP_TXXADS0]
UPDATE OR IGNORE wcdata SET wc='ARP TXXA' WHERE wc=='ARP_TXXA'; -- FIXED: improper clli format character s/_/ / (ARP_TXXADS0)
DELETE FROM wcdata WHERE wc=='ARP_TXXA'; -- FIXED: improper clli format character s/_/ / (ARP_TXXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='HUB TXXARS0' WHERE switch=='HUB_TXXARS0'; -- FIXED: improper clli format character s/_/ / (HUB_TXXARS0) [HUB_TXXARS0]
DELETE FROM switchdata WHERE switch=='HUB_TXXARS0'; -- FIXED: improper clli format character s/_/ / (HUB_TXXARS0) [HUB_TXXARS0]
UPDATE OR IGNORE wcdata SET wc='HUB TXXA' WHERE wc=='HUB_TXXA'; -- FIXED: improper clli format character s/_/ / (HUB_TXXARS0)
DELETE FROM wcdata WHERE wc=='HUB_TXXA'; -- FIXED: improper clli format character s/_/ / (HUB_TXXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='JOY TXXARS0' WHERE switch=='JOY_TXXARS0'; -- FIXED: improper clli format character s/_/ / (JOY_TXXARS0) [JOY_TXXARS0]
DELETE FROM switchdata WHERE switch=='JOY_TXXARS0'; -- FIXED: improper clli format character s/_/ / (JOY_TXXARS0) [JOY_TXXARS0]
UPDATE OR IGNORE wcdata SET wc='JOY TXXA' WHERE wc=='JOY_TXXA'; -- FIXED: improper clli format character s/_/ / (JOY_TXXARS0)
DELETE FROM wcdata WHERE wc=='JOY_TXXA'; -- FIXED: improper clli format character s/_/ / (JOY_TXXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='MAY TXXARS0' WHERE switch=='MAY_TXXARS0'; -- FIXED: improper clli format character s/_/ / (MAY_TXXARS0) [MAY_TXXARS0]
DELETE FROM switchdata WHERE switch=='MAY_TXXARS0'; -- FIXED: improper clli format character s/_/ / (MAY_TXXARS0) [MAY_TXXARS0]
UPDATE OR IGNORE wcdata SET wc='MAY TXXA' WHERE wc=='MAY_TXXA'; -- FIXED: improper clli format character s/_/ / (MAY_TXXARS0)
DELETE FROM wcdata WHERE wc=='MAY_TXXA'; -- FIXED: improper clli format character s/_/ / (MAY_TXXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='TOW TXXARS0' WHERE switch=='TOW_TXXARS0'; -- FIXED: improper clli format character s/_/ / (TOW_TXXARS0) [TOW_TXXARS0]
DELETE FROM switchdata WHERE switch=='TOW_TXXARS0'; -- FIXED: improper clli format character s/_/ / (TOW_TXXARS0) [TOW_TXXARS0]
UPDATE OR IGNORE wcdata SET wc='TOW TXXA' WHERE wc=='TOW_TXXA'; -- FIXED: improper clli format character s/_/ / (TOW_TXXARS0)
DELETE FROM wcdata WHERE wc=='TOW_TXXA'; -- FIXED: improper clli format character s/_/ / (TOW_TXXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='VAN TXXADS0' WHERE switch=='VAN_TXXADS0'; -- FIXED: improper clli format character s/_/ / (VAN_TXXADS0) [VAN_TXXADS0]
DELETE FROM switchdata WHERE switch=='VAN_TXXADS0'; -- FIXED: improper clli format character s/_/ / (VAN_TXXADS0) [VAN_TXXADS0]
UPDATE OR IGNORE wcdata SET wc='VAN TXXA' WHERE wc=='VAN_TXXA'; -- FIXED: improper clli format character s/_/ / (VAN_TXXADS0)
DELETE FROM wcdata WHERE wc=='VAN_TXXA'; -- FIXED: improper clli format character s/_/ / (VAN_TXXADS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='LOA UTXCRS1' WHERE switch=='LOA_UTXCRS1'; -- FIXED: improper clli format character s/_/ / (LOA_UTXCRS1) [LOA_UTXCRS1]
DELETE FROM switchdata WHERE switch=='LOA_UTXCRS1'; -- FIXED: improper clli format character s/_/ / (LOA_UTXCRS1) [LOA_UTXCRS1]
UPDATE OR IGNORE wcdata SET wc='LOA UTXC' WHERE wc=='LOA_UTXC'; -- FIXED: improper clli format character s/_/ / (LOA_UTXCRS1)
DELETE FROM wcdata WHERE wc=='LOA_UTXC'; -- FIXED: improper clli format character s/_/ / (LOA_UTXCRS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='ROY UTMARS3' WHERE switch=='ROY_UTMARS3'; -- FIXED: improper clli format character s/_/ / (ROY_UTMARS3) [ROY_UTMARS3]
DELETE FROM switchdata WHERE switch=='ROY_UTMARS3'; -- FIXED: improper clli format character s/_/ / (ROY_UTMARS3) [ROY_UTMARS3]
UPDATE OR IGNORE wcdata SET wc='ROY UTMA' WHERE wc=='ROY_UTMA'; -- FIXED: improper clli format character s/_/ / (ROY_UTMARS3)
DELETE FROM wcdata WHERE wc=='ROY_UTMA'; -- FIXED: improper clli format character s/_/ / (ROY_UTMARS3)
---------------------
UPDATE OR IGNORE switchdata SET switch='ELK WA01DS0' WHERE switch=='ELK_WA01DS0'; -- FIXED: improper clli format character s/_/ / (ELK_WA01DS0) [ELK_WA01DS0]
DELETE FROM switchdata WHERE switch=='ELK_WA01DS0'; -- FIXED: improper clli format character s/_/ / (ELK_WA01DS0) [ELK_WA01DS0]
UPDATE OR IGNORE wcdata SET wc='ELK WA01' WHERE wc=='ELK_WA01'; -- FIXED: improper clli format character s/_/ / (ELK_WA01DS0)
DELETE FROM wcdata WHERE wc=='ELK_WA01'; -- FIXED: improper clli format character s/_/ / (ELK_WA01DS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='ROY WA01DS0' WHERE switch=='ROY_WA01DS0'; -- FIXED: improper clli format character s/_/ / (ROY_WA01DS0) [ROY_WA01DS0]
DELETE FROM switchdata WHERE switch=='ROY_WA01DS0'; -- FIXED: improper clli format character s/_/ / (ROY_WA01DS0) [ROY_WA01DS0]
UPDATE OR IGNORE wcdata SET wc='ROY WA01' WHERE wc=='ROY_WA01'; -- FIXED: improper clli format character s/_/ / (ROY_WA01DS0)
DELETE FROM wcdata WHERE wc=='ROY_WA01'; -- FIXED: improper clli format character s/_/ / (ROY_WA01DS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='RIO WIXARS0' WHERE switch=='RIO_WIXARS0'; -- FIXED: improper clli format character s/_/ / (RIO_WIXARS0) [RIO_WIXARS0]
DELETE FROM switchdata WHERE switch=='RIO_WIXARS0'; -- FIXED: improper clli format character s/_/ / (RIO_WIXARS0) [RIO_WIXARS0]
UPDATE OR IGNORE wcdata SET wc='RIO WIXA' WHERE wc=='RIO_WIXA'; -- FIXED: improper clli format character s/_/ / (RIO_WIXARS0)
DELETE FROM wcdata WHERE wc=='RIO_WIXA'; -- FIXED: improper clli format character s/_/ / (RIO_WIXARS0)
---------------------
UPDATE OR IGNORE switchdata SET switch='MAN WVJURS1' WHERE switch=='MAN_WVJURS1'; -- FIXED: improper clli format character s/_/ / (MAN_WVJURS1) [MAN_WVJURS1]
DELETE FROM switchdata WHERE switch=='MAN_WVJURS1'; -- FIXED: improper clli format character s/_/ / (MAN_WVJURS1) [MAN_WVJURS1]
UPDATE OR IGNORE wcdata SET wc='MAN WVJU' WHERE wc=='MAN_WVJU'; -- FIXED: improper clli format character s/_/ / (MAN_WVJURS1)
DELETE FROM wcdata WHERE wc=='MAN_WVJU'; -- FIXED: improper clli format character s/_/ / (MAN_WVJURS1)
---------------------
UPDATE OR IGNORE switchdata SET switch='WAR WVXADS0' WHERE switch=='WAR_WVXADS0'; -- FIXED: improper clli format character s/_/ / (WAR_WVXADS0) [WAR_WVXADS0]
DELETE FROM switchdata WHERE switch=='WAR_WVXADS0'; -- FIXED: improper clli format character s/_/ / (WAR_WVXADS0) [WAR_WVXADS0]
UPDATE OR IGNORE wcdata SET wc='WAR WVXA' WHERE wc=='WAR_WVXA'; -- FIXED: improper clli format character s/_/ / (WAR_WVXADS0)
DELETE FROM wcdata WHERE wc=='WAR_WVXA'; -- FIXED: improper clli format character s/_/ / (WAR_WVXADS0)
--=============================================
UPDATE prefixdata SET switch='SPFDILSDPMD' WHERE switch=='SPFDLSDPMD'; -- FIXED: bad clli length 10 (lcg) [217-306]
UPDATE OR IGNORE switchdata SET switch='SPFDILSDPMD' WHERE switch=='SPFDLSDPMD'; -- FIXED: bad clli length 10 (lcg) [SPFDLSDPMD]
DELETE FROM switchdata WHERE switch=='SPFDLSDPMD'; -- FIXED: bad clli length 10 (lcg) [SPFDLSDPMD]
UPDATE OR IGNORE wcdata SET wc='SPFDILSD' WHERE wc=='SPFDLSD'; -- FIXED: bad clli length 10 (lcg) [SPFDLSDPMD]
DELETE FROM wcdata WHERE wc=='SPFDLSD'; -- FIXED: bad clli length 10 (lcg) [SPFDLSDPMD]
---------------------
UPDATE prefixdata SET switch='HNVIALQUCM2' WHERE switch=='HNVIALQUCM'; -- FIXED: bad clli length 10 (256) [256-678]
UPDATE OR IGNORE switchdata SET switch='HNVIALQUCM2' WHERE switch=='HNVIALQUCM'; -- FIXED: bad clli length 10 (256) [HNVIALQUCM]
DELETE FROM switchdata WHERE switch=='HNVIALQUCM'; -- FIXED: bad clli length 10 (256) [HNVIALQUCM]
---------------------
UPDATE prefixdata SET switch='WNYKNYBCCM2' WHERE switch=='NYKNYBCCM2'; -- FIXED: bad clli length 10 (917) [917-232]
UPDATE OR IGNORE switchdata SET switch='WNYKNYBCCM2' WHERE switch=='NYKNYBCCM2'; -- FIXED: bad clli length 10 (917) [NYKNYBCCM2]
DELETE FROM switchdata WHERE switch=='NYKNYBCCM2'; -- FIXED: bad clli length 10 (917) [NYKNYBCCM2]
UPDATE OR IGNORE wcdata SET wc='WNYKNYBC' WHERE wc=='NYKNYBCC'; -- FIXED: bad clli length 10 (917)
DELETE FROM wcdata WHERE wc=='NYKNYBCC'; -- FIXED: bad clli length 10 (917)
--=============================================
UPDATE prefixdata SET switch='ENWDCO47DS0' WHERE switch=='ENWDC047DS0'; -- FIXED: invalid clli 'ENWDC047DS0' (720) [720-367]
UPDATE OR IGNORE switchdata SET switch='ENWDCO47DS0' WHERE switch=='ENWDC047DS0'; -- FIXED: invalid clli 'ENWDC047DS0' (720,ENWDC047DS0) [ENWDC047DS0]
DELETE FROM switchdata WHERE switch=='ENWDC047DS0'; -- FIXED: invalid clli 'ENWDC047DS0' (720,ENWDC047DS0) [ENWDC047DS0]
UPDATE OR IGNORE wcdata SET wc='ENWDCO47' WHERE wc=='ENWDC047'; -- FIXED: invalid clli 'ENWDC047DS0' (720,ENWDC047DS0)
DELETE FROM wcdata WHERE wc=='ENWDC047'; -- FIXED: invalid clli 'ENWDC047DS0' (720,ENWDC047DS0)
---------------------
UPDATE prefixdata SET switch='MSCWIDDL0MD' WHERE switch=='MSCW1DDL0MD'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data) [208-669]
UPDATE OR IGNORE switchdata SET switch='MSCWIDDL0MD' WHERE switch=='MSCW1DDL0MD'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data) [MSCW1DDL0MD]
DELETE FROM switchdata WHERE switch=='MSCW1DDL0MD'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data) [MSCW1DDL0MD]
UPDATE OR IGNORE wcdata SET wc='MSCWIDDL' WHERE wc=='MSCW1DDL'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data)
DELETE FROM wcdata WHERE wc=='MSCW1DDL'; -- FIXED: invalid clli 'MSCW1DDL0MD' (data)
---------------------
UPDATE prefixdata SET switch='CLSQCO01DS0' WHERE switch=='CLSQC001DS0'; -- FIXED: invalid clli 'CLSQC001DS0' (data) [719-559]
UPDATE OR IGNORE switchdata SET switch='CLSQCO01DS0' WHERE switch=='CLSQC001DS0'; -- FIXED: invalid clli 'CLSQC001DS0' (data) [CLSQC001DS0]
DELETE FROM switchdata WHERE switch=='CLSQC001DS0'; -- FIXED: invalid clli 'CLSQC001DS0' (data) [CLSQC001DS0]
UPDATE OR IGNORE wcdata SET wc='CLSQCO01' WHERE wc=='CLSQC001'; -- FIXED: invalid clli 'CLSQC001DS0' (data)
DELETE FROM wcdata WHERE wc=='CLSQC001'; -- FIXED: invalid clli 'CLSQC001DS0' (data)
---------------------
UPDATE prefixdata SET switch='LJNTCO26DS0' WHERE switch=='LJNTC026DS0'; -- FIXED: invalid clli 'LJNTC026DS0' (data) [719-363]
UPDATE OR IGNORE switchdata SET switch='LJNTCO26DS0' WHERE switch=='LJNTC026DS0'; -- FIXED: invalid clli 'LJNTC026DS0' (data) [LJNTC026DS0]
DELETE FROM switchdata WHERE switch=='LJNTC026DS0'; -- FIXED: invalid clli 'LJNTC026DS0' (data) [LJNTC026DS0]
UPDATE OR IGNORE wcdata SET wc='LJNTCO26' WHERE wc=='LJNTC026'; -- FIXED: invalid clli 'LJNTC026DS0' (data)
DELETE FROM wcdata WHERE wc=='LJNTC026'; -- FIXED: invalid clli 'LJNTC026DS0' (data)
---------------------
UPDATE prefixdata SET switch=NULL WHERE npa=='500' OR npa=='710' OR npa=='900'; -- FIXED: invalid clli
DELETE from switchdata WHERE switch LIKE 'ASGNPCS%'; -- FIXED: invalid clli
DELETE from switchdata WHERE switch=='RTGETSAGENT'; -- FIXED: invalid clli
DELETE from switchdata WHERE switch LIKE 'ASGNIC%'; -- FIXED: invalid clli
DELETE from wcdata WHERE wc LIKE 'ASGNPCS%'; -- FIXED: invalid clli
DELETE from wcdata WHERE wc=='RTGETSAG'; -- FIXED: invalid clli
DELETE from wcdata WHERE wc LIKE 'ASGNIC%'; -- FIXED: invalid clli
---------------------
UPDATE pooldata   SET switch=NULL WHERE switch=='NOCLLIKNOWN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data) [201-204-0,201-204-1,201-204-3,201-204-9,201-205-1,201-205-2,201-210-0,201-210-2,201-210-4,201-210-5,...]
UPDATE prefixdata SET switch=NULL WHERE switch=='NOCLLIKNOWN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data) [201-201,201-211,201-260,201-311,201-351,201-411,201-479,201-509,201-511,201-550,...]
DELETE FROM switchdata WHERE switch=='NOCLLIKNOWN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data) [NOCLLIKNOWN]
DELETE FROM wcdata WHERE wc=='NOCLLIKN'; -- FIXED: invalid clli 'NOCLLIKNOWN' (data)
---------------------
UPDATE prefixdata SET switch='PRTGMI01DS0' WHERE switch=='PRTGM101DS0'; -- FIXED: invalid clli 'PRTGM101DS0' (data) [616-855]
UPDATE OR IGNORE switchdata SET switch='PRTGMI01DS0' WHERE switch=='PRTGM101DS0'; -- FIXED: invalid clli 'PRTGM101DS0' (data) [PRTGM101DS0]
DELETE FROM switchdata WHERE switch=='PRTGM101DS0'; -- FIXED: invalid clli 'PRTGM101DS0' (data) [PRTGM101DS0]
UPDATE OR IGNORE wcdata SET wc='PRTGMI01' WHERE wc=='PRTGM101'; -- FIXED: invalid clli 'PRTGM101DS0' (data)
DELETE FROM wcdata WHERE wc=='PRTGM101'; -- FIXED: invalid clli 'PRTGM101DS0' (data)
---------------------
-- UPDATE switchdata SET switch='H109NCXARS0' WHERE switch=='H109NCXARS0'; -- FIXME: invalid clli 'H109NCXARS0' (H109NCXARS0) [H109NCXARS0]
-- UPDATE wcdata SET wc='H109NCXA' WHERE wc=='H109NCXA'; -- FIXME: invalid clli 'H109NCXARS0' (H109NCXARS0)
---------------------
-- UPDATE switchdata SET switch='S162NDXARS2' WHERE switch=='S162NDXARS2'; -- FIXME: invalid clli 'S162NDXARS2' (S162NDXARS2) [S162NDXARS2]
-- UPDATE wcdata SET wc='S162NDXA' WHERE wc=='S162NDXA'; -- FIXME: invalid clli 'S162NDXARS2' (S162NDXARS2)
---------------------
UPDATE prefixdata SET switch=NULL WHERE switch=='__VARIOUS__'; -- FIXED: invalid clli '__VARIOUS__' (203,206,303,304,310,323,360,364,385,408,...) [203-976,206-976,303-976,304-950,304-976,310-976,323-976,360-976,364-555,385-211,...]
DELETE FROM switchdata WHERE switch=='__VARIOUS__'; -- FIXED: invalid clli '__VARIOUS__' (203,206,303,304,310,323,360,364,385,408,...) [__VARIOUS__]
DELETE FROM wcdata WHERE wc=='__VARIOU'; -- FIXED: invalid clli '__VARIOUS__' (203,206,303,304,310,323,360,364,385,408,...)
---------------------
-- UPDATE switchdata SET switch='H151SCXARS0' WHERE switch=='H151SCXARS0'; -- FIXME: invalid clli 'H151SCXARS0' (H151SCXARS0) [H151SCXARS0]
-- UPDATE wcdata SET wc='H151SCXA' WHERE wc=='H151SCXA'; -- FIXME: invalid clli 'H151SCXARS0' (H151SCXARS0)
---------------------
-- UPDATE switchdata SET switch='H9CHSCXARS0' WHERE switch=='H9CHSCXARS0'; -- FIXME: invalid clli 'H9CHSCXARS0' (H9CHSCXARS0) [H9CHSCXARS0]
-- UPDATE wcdata SET wc='H9CHSCXA' WHERE wc=='H9CHSCXA'; -- FIXME: invalid clli 'H9CHSCXARS0' (H9CHSCXARS0)
---------------------
-- UPDATE switchdata SET switch='H9PGSCXARS0' WHERE switch=='H9PGSCXARS0'; -- FIXME: invalid clli 'H9PGSCXARS0' (H9PGSCXARS0) [H9PGSCXARS0]
-- UPDATE wcdata SET wc='H9PGSCXA' WHERE wc=='H9PGSCXA'; -- FIXME: invalid clli 'H9PGSCXARS0' (H9PGSCXARS0)
--=============================================
UPDATE prefixdata SET switch='MOBLAL02XFX' WHERE switch=='MOBLAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (data) [251-583]
UPDATE OR IGNORE switchdata SET switch='MOBLAL02XFX' WHERE switch=='MOBLAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (data) [MOBLAAL02XF]
DELETE FROM switchdata WHERE switch=='MOBLAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (data) [MOBLAAL02XF]
UPDATE OR IGNORE wcdata SET wc='MOBLAL02' WHERE wc=='MOBLAAL0'; -- FIXED: invalid state/province/territory 'AA' (data)
DELETE FROM wcdata WHERE wc=='MOBLAAL0'; -- FIXED: invalid state/province/territory 'AA' (data)
---------------------
UPDATE prefixdata SET switch='MMPHTNMACM0' WHERE switch=='MMMPHTNMACM'; -- FIXED: invalid state/province/territory 'HT' (731) [731-613]
UPDATE OR IGNORE switchdata SET switch='MMPHTNMACM0' WHERE switch=='MMMPHTNMACM'; -- FIXED: invalid state/province/territory 'HT' (731,MMMPHTNMACM) [MMMPHTNMACM]
DELETE FROM switchdata WHERE switch=='MMMPHTNMACM'; -- FIXED: invalid state/province/territory 'HT' (731,MMMPHTNMACM) [MMMPHTNMACM]
UPDATE OR IGNORE wcdata SET wc='MMPHTNMA' WHERE wc=='MMMPHTNM'; -- FIXED: invalid state/province/territory 'HT' (731,MMMPHTNMACM)
DELETE FROM wcdata WHERE wc=='MMMPHTNM'; -- FIXED: invalid state/province/territory 'HT' (731,MMMPHTNMACM)
---------------------
UPDATE prefixdata SET switch='FYVLNC01DS0' WHERE switch=='FYVLMC01DS0'; -- FIXED: invalid state/province/territory 'MC' (910) [910-446]
UPDATE OR IGNORE switchdata SET switch='FYVLNC01DS0' WHERE switch=='FYVLMC01DS0'; -- FIXED: invalid state/province/territory 'MC' (910,FYVLMC01DS0) [FYVLMC01DS0]
DELETE FROM switchdata WHERE switch=='FYVLMC01DS0'; -- FIXED: invalid state/province/territory 'MC' (910,FYVLMC01DS0) [FYVLMC01DS0]
UPDATE OR IGNORE wcdata SET wc='FYVLNC01' WHERE wc=='FYVLMC01'; -- FIXED: invalid state/province/territory 'MC' (910,FYVLMC01DS0)
DELETE FROM wcdata WHERE wc=='FYVLMC01'; -- FIXED: invalid state/province/territory 'MC' (910,FYVLMC01DS0)
---------------------
UPDATE prefixdata SET switch='RCMTNCXAX5X' WHERE switch=='RCMTMCXAX5X'; -- FIXED: invalid state/province/territory 'MC' (252) [252-668]
UPDATE OR IGNORE switchdata SET switch='RCMTNCXAX5X' WHERE switch=='RCMTMCXAX5X'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAX5X) [RCMTMCXAX5X]
DELETE FROM switchdata WHERE switch=='RCMTMCXAX5X'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAX5X) [RCMTMCXAX5X]
UPDATE OR IGNORE wcdata SET wc='RCMTNCXA' WHERE wc=='RCMTMCXA'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAX5X)
DELETE FROM wcdata WHERE wc=='RCMTMCXA'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAX5X)
---------------------
UPDATE prefixdata SET switch='RCMTNCXAXVX' WHERE switch=='RCMTMCXAXVX'; -- FIXED: invalid state/province/territory 'MC' (252) [252-822]
UPDATE OR IGNORE switchdata SET switch='RCMTNCXAXVX' WHERE switch=='RCMTMCXAXVX'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAXVX) [RCMTMCXAXVX]
DELETE FROM switchdata WHERE switch=='RCMTMCXAXVX'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAXVX) [RCMTMCXAXVX]
UPDATE OR IGNORE wcdata SET wc='RCMTNCXA' WHERE wc=='RCMTMCXA'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAXVX)
DELETE FROM wcdata WHERE wc=='RCMTMCXA'; -- FIXED: invalid state/province/territory 'MC' (252,RCMTMCXAXVX)
---------------------
UPDATE prefixdata SET switch='RCSNTXHVGT0' WHERE switch=='RCSNTHXVGT0'; -- FIXED: invalid state/province/territory 'TH' (469) [469-202]
UPDATE OR IGNORE switchdata SET switch='RCSNTXHVGT0' WHERE switch=='RCSNTHXVGT0'; -- FIXED: invalid state/province/territory 'TH' (469,RCSNTHXVGT0) [RCSNTHXVGT0]
DELETE FROM switchdata WHERE switch=='RCSNTHXVGT0'; -- FIXED: invalid state/province/territory 'TH' (469,RCSNTHXVGT0) [RCSNTHXVGT0]
UPDATE OR IGNORE wcdata SET wc='RCSNTXHV' WHERE wc=='RCSNTHXV'; -- FIXED: invalid state/province/territory 'TH' (469,RCSNTHXVGT0)
DELETE FROM wcdata WHERE wc=='RCSNTHXV'; -- FIXED: invalid state/province/territory 'TH' (469,RCSNTHXVGT0)
---------------------
UPDATE prefixdata SET switch='BUMTTXTEWMD' WHERE switch=='BUMMTTXEWMD'; -- FIXED: invalid state/province/territory 'TT' (409) [409-223]
UPDATE OR IGNORE switchdata SET switch='BUMTTXTEWMD' WHERE switch=='BUMMTTXEWMD'; -- FIXED: invalid state/province/territory 'TT' (409,BUMMTTXEWMD) [BUMMTTXEWMD]
DELETE FROM switchdata WHERE switch=='BUMMTTXEWMD'; -- FIXED: invalid state/province/territory 'TT' (409,BUMMTTXEWMD) [BUMMTTXEWMD]
UPDATE OR IGNORE wcdata SET wc='BUMTTXTE' WHERE wc=='BUMMTTXE'; -- FIXED: invalid state/province/territory 'TT' (409,BUMMTTXEWMD)
DELETE FROM wcdata WHERE wc=='BUMMTTXE'; -- FIXED: invalid state/province/territory 'TT' (409,BUMMTTXEWMD)
---------------------
UPDATE pooldata   SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [843-606-5,843-606-6]
UPDATE prefixdata SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [843-606]
UPDATE OR IGNORE switchdata SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [CHTNXDCTX4Y]
DELETE FROM switchdata WHERE switch=='CHTNXDCTX4Y'; -- FIXED: invalid state/province/territory 'XD' (data) [CHTNXDCTX4Y]
UPDATE OR IGNORE wcdata SET wc='CHTNSCDT' WHERE wc=='CHTNXDCT'; -- FIXED: invalid state/province/territory 'XD' (data)
DELETE FROM wcdata WHERE wc=='CHTNXDCT'; -- FIXED: invalid state/province/territory 'XD' (data)
---------------------
UPDATE prefixdata SET switch='MOBLAL02XFX' WHERE switch=='MOBXAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (GOLD) [251-583]
UPDATE OR IGNORE switchdata SET switch='MOBLAL02XFX' WHERE switch=='MOBXAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (GOLD) [MOBXAAL02XF]
DELETE FROM switchdata WHERE switch=='MOBXAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (GOLD) [MOBXAAL02XF]
UPDATE OR IGNORE wcdata SET wc='MOBLAL02' WHERE wc=='MOBXAAL0'; -- FIXED: invalid state/province/territory 'AA' (GOLD)
DELETE FROM wcdata WHERE wc=='MOBXAAL0'; -- FIXED: invalid state/province/territory 'AA' (GOLD)
---------------------
UPDATE prefixdata SET switch='ALBYNY80DS0' WHERE switch=='ALBYBYBODS0'; -- FIXED: invalid state/province/territory 'BY' (data) [518-874]
UPDATE OR IGNORE switchdata SET switch='ALBYNY80DS0' WHERE switch=='ALBYBYBODS0'; -- FIXED: invalid state/province/territory 'BY' (data) [ALBYBYBODS0]
DELETE FROM switchdata WHERE switch=='ALBYBYBODS0'; -- FIXED: invalid state/province/territory 'BY' (data) [ALBYBYBODS0]
UPDATE OR IGNORE wcdata SET wc='ALBYNY80' WHERE wc=='ALBYBYBO'; -- FIXED: invalid state/province/territory 'BY' (data)
DELETE FROM wcdata WHERE wc=='ALBYBYBO'; -- FIXED: invalid state/province/territory 'BY' (data)
---------------------
UPDATE prefixdata SET switch='CLSPCO63DS0' WHERE switch=='CLSPCP63DS0'; -- FIXED: invalid state/province/territory 'CP' (data) [719-234]
UPDATE OR IGNORE switchdata SET switch='CLSPCO63DS0' WHERE switch=='CLSPCP63DS0'; -- FIXED: invalid state/province/territory 'CP' (data) [CLSPCP63DS0]
DELETE FROM switchdata WHERE switch=='CLSPCP63DS0'; -- FIXED: invalid state/province/territory 'CP' (data) [CLSPCP63DS0]
UPDATE OR IGNORE wcdata SET wc='CLSPCO63' WHERE wc=='CLSPCP63'; -- FIXED: invalid state/province/territory 'CP' (data)
DELETE FROM wcdata WHERE wc=='CLSPCP63'; -- FIXED: invalid state/province/territory 'CP' (data)
---------------------
UPDATE pooldata   SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [323-366-3,323-366-4]
UPDATE prefixdata SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [323-366]
UPDATE OR IGNORE switchdata SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [LSANDAJWGT0]
DELETE FROM switchdata WHERE switch=='LSANDAJWGT0'; -- FIXED: invalid state/province/territory 'DA' (data) [LSANDAJWGT0]
UPDATE OR IGNORE wcdata SET wc='LSANCAJW' WHERE wc=='LSANDAJW'; -- FIXED: invalid state/province/territory 'DA' (data)
DELETE FROM wcdata WHERE wc=='LSANDAJW'; -- FIXED: invalid state/province/territory 'DA' (data)
---------------------
UPDATE prefixdata SET switch='SVNHGABSIMD' WHERE switch=='SVNGHABSIMD'; -- FIXED: invalid state/province/territory 'HA' (data) [912-596]
UPDATE OR IGNORE switchdata SET switch='SVNHGABSIMD' WHERE switch=='SVNGHABSIMD'; -- FIXED: invalid state/province/territory 'HA' (data) [SVNGHABSIMD]
DELETE FROM switchdata WHERE switch=='SVNGHABSIMD'; -- FIXED: invalid state/province/territory 'HA' (data) [SVNGHABSIMD]
UPDATE OR IGNORE wcdata SET wc='SVNHGABS' WHERE wc=='SVNGHABS'; -- FIXED: invalid state/province/territory 'HA' (data)
DELETE FROM wcdata WHERE wc=='SVNGHABS'; -- FIXED: invalid state/province/territory 'HA' (data)
---------------------
UPDATE prefixdata SET switch='LENYKSCJCM2' WHERE switch=='LENYHSCJCM2'; -- FIXED: invalid state/province/territory 'HS' (data) [913-302]
UPDATE OR IGNORE switchdata SET switch='LENYKSCJCM2' WHERE switch=='LENYHSCJCM2'; -- FIXED: invalid state/province/territory 'HS' (data) [LENYHSCJCM2]
DELETE FROM switchdata WHERE switch=='LENYHSCJCM2'; -- FIXED: invalid state/province/territory 'HS' (data) [LENYHSCJCM2]
UPDATE OR IGNORE wcdata SET wc='LENYKSCJ' WHERE wc=='LENYHSCJ'; -- FIXED: invalid state/province/territory 'HS' (data)
DELETE FROM wcdata WHERE wc=='LENYHSCJ'; -- FIXED: invalid state/province/territory 'HS' (data)
---------------------
UPDATE prefixdata SET switch='ULYSKS03CM0' WHERE switch=='ULYSHS03CM0'; -- FIXED: invalid state/province/territory 'HS' (data) [620-934]
UPDATE OR IGNORE switchdata SET switch='ULYSKS03CM0' WHERE switch=='ULYSHS03CM0'; -- FIXED: invalid state/province/territory 'HS' (data) [ULYSHS03CM0]
DELETE FROM switchdata WHERE switch=='ULYSHS03CM0'; -- FIXED: invalid state/province/territory 'HS' (data) [ULYSHS03CM0]
UPDATE OR IGNORE wcdata SET wc='ULYSKS03' WHERE wc=='ULYSHS03'; -- FIXED: invalid state/province/territory 'HS' (data)
DELETE FROM wcdata WHERE wc=='ULYSHS03'; -- FIXED: invalid state/province/territory 'HS' (data)
---------------------
UPDATE pooldata   SET switch='SXCTIAXO1MD' WHERE switch=='SXCTIZXO1MD'; -- FIXED: invalid state/province/territory 'IZ' (27-100) [712-463-1]
UPDATE OR IGNORE switchdata SET switch='SXCTIAXO1MD' WHERE switch=='SXCTIZXO1MD'; -- FIXED: invalid state/province/territory 'IZ' (27-100) [SXCTIZXO1MD]
DELETE FROM switchdata WHERE switch=='SXCTIZXO1MD'; -- FIXED: invalid state/province/territory 'IZ' (27-100) [SXCTIZXO1MD]
UPDATE OR IGNORE wcdata SET wc='SXCTIAXO' WHERE wc=='SXCTIZXO'; -- FIXED: invalid state/province/territory 'IZ' (27-100)
DELETE FROM wcdata WHERE wc=='SXCTIZXO'; -- FIXED: invalid state/province/territory 'IZ' (27-100)
---------------------
UPDATE prefixdata SET switch='GAVLIDAJCM0' WHERE switch=='GAVILDAJCM0'; -- FIXED: invalid state/province/territory 'LD' (data) [208-507]
UPDATE OR IGNORE switchdata SET switch='GAVLIDAJCM0' WHERE switch=='GAVILDAJCM0'; -- FIXED: invalid state/province/territory 'LD' (data) [GAVILDAJCM0]
DELETE FROM switchdata WHERE switch=='GAVILDAJCM0'; -- FIXED: invalid state/province/territory 'LD' (data) [GAVILDAJCM0]
UPDATE OR IGNORE wcdata SET wc='GAVLIDAJ' WHERE wc=='GAVILDAJ'; -- FIXED: invalid state/province/territory 'LD' (data)
DELETE FROM wcdata WHERE wc=='GAVILDAJ'; -- FIXED: invalid state/province/territory 'LD' (data)
---------------------
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [828-217-8,828-217-9,828-270-4,828-270-5,828-270-6,828-270-8,828-404-4,828-404-7,828-404-9,828-409-5,...]
UPDATE prefixdata SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [828-238]
UPDATE OR IGNORE switchdata SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [HCLRMCXA2MD]
DELETE FROM switchdata WHERE switch=='HCLRMCXA2MD'; -- FIXED: invalid state/province/territory 'MC' (data) [HCLRMCXA2MD]
UPDATE OR IGNORE wcdata SET wc='HCLRNCXA' WHERE wc=='HCLRMCXA'; -- FIXED: invalid state/province/territory 'MC' (data)
DELETE FROM wcdata WHERE wc=='HCLRMCXA'; -- FIXED: invalid state/province/territory 'MC' (data)
---------------------
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [314-737-0,314-737-1,314-737-2,314-737-3,314-737-6,314-737-7,314-737-8,314-737-9]
UPDATE prefixdata SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [314-737]
UPDATE OR IGNORE switchdata SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [NWTPMPABCM8]
DELETE FROM switchdata WHERE switch=='NWTPMPABCM8'; -- FIXED: invalid state/province/territory 'MP' (data) [NWTPMPABCM8]
UPDATE OR IGNORE wcdata SET wc='NWTPMOAB' WHERE wc=='NWTPMPAB'; -- FIXED: invalid state/province/territory 'MP' (data)
DELETE FROM wcdata WHERE wc=='NWTPMPAB'; -- FIXED: invalid state/province/territory 'MP' (data)
---------------------
UPDATE pooldata   SET switch='NYCSNYOODS0' WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [646-503-1,646-570-1]
UPDATE prefixdata SET switch='NYCSNYOODS0' WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [646-503,646-570]
UPDATE OR IGNORE switchdata SET switch='NYCSNYOODS0' WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [NYCMMYYJOMD]
DELETE FROM switchdata WHERE switch=='NYCMMYYJOMD'; -- FIXED: invalid state/province/territory 'MY' (data) [NYCMMYYJOMD]
UPDATE OR IGNORE wcdata SET wc='NYCSNYOO' WHERE wc=='NYCMMYYJ'; -- FIXED: invalid state/province/territory 'MY' (data)
DELETE FROM wcdata WHERE wc=='NYCMMYYJ'; -- FIXED: invalid state/province/territory 'MY' (data)
---------------------
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [405-881-0,405-881-1,405-881-2,405-881-3,405-881-4,405-881-5,405-881-6,405-881-7,405-881-8,405-881-9]
UPDATE prefixdata SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [405-881]
UPDATE OR IGNORE switchdata SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [OKCYPKCEX8X]
DELETE FROM switchdata WHERE switch=='OKCYPKCEX8X'; -- FIXED: invalid state/province/territory 'PK' (data) [OKCYPKCEX8X]
UPDATE OR IGNORE wcdata SET wc='OKCYOKCE' WHERE wc=='OKCYPKCE'; -- FIXED: invalid state/province/territory 'PK' (data)
DELETE FROM wcdata WHERE wc=='OKCYPKCE'; -- FIXED: invalid state/province/territory 'PK' (data)
---------------------
UPDATE prefixdata SET switch='TXRKTXAD1MD' WHERE switch=='TXRKRXAD1MD'; -- FIXED: invalid state/province/territory 'RX' (data) [903-219]
UPDATE OR IGNORE switchdata SET switch='TXRKTXAD1MD' WHERE switch=='TXRKRXAD1MD'; -- FIXED: invalid state/province/territory 'RX' (data) [TXRKRXAD1MD]
DELETE FROM switchdata WHERE switch=='TXRKRXAD1MD'; -- FIXED: invalid state/province/territory 'RX' (data) [TXRKRXAD1MD]
UPDATE OR IGNORE wcdata SET wc='TXRKTXAD' WHERE wc=='TXRKRXAD'; -- FIXED: invalid state/province/territory 'RX' (data)
DELETE FROM wcdata WHERE wc=='TXRKRXAD'; -- FIXED: invalid state/province/territory 'RX' (data)
---------------------
UPDATE prefixdata SET switch=NULL WHERE switch=='ALL SWITCHE'; -- FIXED: invalid state/province/territory 'SW' (data) [205-203,205-204]
DELETE FROM switchdata WHERE switch=='ALL SWITCHE'; -- FIXED: invalid state/province/territory 'SW' (data) [ALL SWITCHE]
DELETE FROM wcdata WHERE wc=='ALL SWIT'; -- FIXED: invalid state/province/territory 'SW' (data)
---------------------
UPDATE pooldata   SET switch='ELGNILOT0MD' WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [773-816-0,773-816-1,773-816-2,773-816-3,773-816-4,773-816-6,773-816-7,773-816-8,773-816-9]
UPDATE prefixdata SET switch='ELGNILOT0MD' WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [773-816]
UPDATE OR IGNORE switchdata SET switch='ELGNILOT0MD' WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [CHICTGOZN10]
DELETE FROM switchdata WHERE switch=='CHICTGOZN10'; -- FIXED: invalid state/province/territory 'TG' (data) [CHICTGOZN10]
UPDATE OR IGNORE wcdata SET wc='ELGNILOT' WHERE wc=='CHICTGOZ'; -- FIXED: invalid state/province/territory 'TG' (data)
DELETE FROM wcdata WHERE wc=='CHICTGOZ'; -- FIXED: invalid state/province/territory 'TG' (data)
---------------------
UPDATE prefixdata SET switch='CJTGTN100MD' WHERE switch=='CJTGTM100MD'; -- FIXED: invalid state/province/territory 'TM' (data) [423-503]
UPDATE OR IGNORE switchdata SET switch='CJTGTN100MD' WHERE switch=='CJTGTM100MD'; -- FIXED: invalid state/province/territory 'TM' (data) [CJTGTM100MD]
DELETE FROM switchdata WHERE switch=='CJTGTM100MD'; -- FIXED: invalid state/province/territory 'TM' (data) [CJTGTM100MD]
UPDATE OR IGNORE wcdata SET wc='CJTGTN10' WHERE wc=='CJTGTM10'; -- FIXED: invalid state/province/territory 'TM' (data)
DELETE FROM wcdata WHERE wc=='CJTGTM10'; -- FIXED: invalid state/province/territory 'TM' (data)
---------------------
UPDATE prefixdata SET switch='DCTRILDCGMD' WHERE switch=='DCTRUKBXCM8'; -- FIXED: invalid state/province/territory 'UK' (data) [217-412]
UPDATE OR IGNORE switchdata SET switch='DCTRILDCGMD' WHERE switch=='DCTRUKBXCM8'; -- FIXED: invalid state/province/territory 'UK' (data) [DCTRUKBXCM8]
DELETE FROM switchdata WHERE switch=='DCTRUKBXCM8'; -- FIXED: invalid state/province/territory 'UK' (data) [DCTRUKBXCM8]
UPDATE OR IGNORE wcdata SET wc='DCTRILDC' WHERE wc=='DCTRUKBX'; -- FIXED: invalid state/province/territory 'UK' (data)
DELETE FROM wcdata WHERE wc=='DCTRUKBX'; -- FIXED: invalid state/province/territory 'UK' (data)
--=============================================
UPDATE pooldata   SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [256-853-6]
UPDATE prefixdata SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [256-853]
UPDATE OR IGNORE switchdata SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [ALCYALAYBMX]
DELETE FROM switchdata WHERE switch=='ALCYALAYBMX'; -- FIXED: poor equipment code 'BMX' (data) [ALCYALAYBMX]
---------------------
UPDATE prefixdata SET switch='MTGMALIR5MD' WHERE switch=='MTGMALMLMTX'; -- FIXED: poor equipment code 'MTX' (334) [334-549]
UPDATE OR IGNORE switchdata SET switch='MTGMALIR5MD' WHERE switch=='MTGMALMLMTX'; -- FIXED: poor equipment code 'MTX' (334,MTGMALMLMTX) [MTGMALMLMTX]
DELETE FROM switchdata WHERE switch=='MTGMALMLMTX'; -- FIXED: poor equipment code 'MTX' (334,MTGMALMLMTX) [MTGMALMLMTX]
UPDATE OR IGNORE wcdata SET wc='MTGMALIR' WHERE wc=='MTGMALML'; -- FIXED: poor equipment code 'MTX' (334,MTGMALMLMTX) [MTGMALMLMTX]
DELETE FROM wcdata WHERE wc=='MTGMALML'; -- FIXED: poor equipment code 'MTX' (334,MTGMALMLMTX) [MTGMALMLMTX]
---------------------
UPDATE prefixdata SET switch='BNTVARCRDS0' WHERE switch=='BNTVARCRGEL'; -- FIXED: poor equipment code 'GEL' (479) [479-203,479-205]
UPDATE OR IGNORE switchdata SET switch='BNTVARCRDS0' WHERE switch=='BNTVARCRGEL'; -- FIXED: poor equipment code 'GEL' (479,BNTVARCRGEL) [BNTVARCRGEL]
DELETE FROM switchdata WHERE switch=='BNTVARCRGEL'; -- FIXED: poor equipment code 'GEL' (479,BNTVARCRGEL) [BNTVARCRGEL]
---------------------
UPDATE pooldata   SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [916-259-4]
UPDATE prefixdata SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [916-259]
UPDATE OR IGNORE switchdata SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [SCRMCA01XPT]
DELETE FROM switchdata WHERE switch=='SCRMCA01XPT'; -- FIXED: poor equipment code 'XPT' (data) [SCRMCA01XPT]
---------------------
UPDATE prefixdata SET switch='SNJSCAJRDS0' WHERE switch=='SNJSCAJRD0X'; -- FIXED: poor equipment code 'D0X' (707) [707-243]
UPDATE OR IGNORE switchdata SET switch='SNJSCAJRDS0' WHERE switch=='SNJSCAJRD0X'; -- FIXED: poor equipment code 'D0X' (707,SNJSCAJRD0X) [SNJSCAJRD0X]
DELETE FROM switchdata WHERE switch=='SNJSCAJRD0X'; -- FIXED: poor equipment code 'D0X' (707,SNJSCAJRD0X) [SNJSCAJRD0X]
---------------------
UPDATE prefixdata SET switch='SNJSCAJRDS0' WHERE switch=='SNJSCAJRDX0'; -- FIXED: poor equipment code 'DX0' (707) [707-294]
UPDATE OR IGNORE switchdata SET switch='SNJSCAJRDS0' WHERE switch=='SNJSCAJRDX0'; -- FIXED: poor equipment code 'DX0' (707,SNJSCAJRDX0) [SNJSCAJRDX0]
DELETE FROM switchdata WHERE switch=='SNJSCAJRDX0'; -- FIXED: poor equipment code 'DX0' (707,SNJSCAJRDX0) [SNJSCAJRDX0]
---------------------
-- UPDATE prefixdata SET switch='DNVRCO26WBA' WHERE switch=='DNVRCO26WBA'; -- FIXME: poor equipment code 'WBA' (303) [303-330]
-- UPDATE switchdata SET switch='DNVRCO26WBA' WHERE switch=='DNVRCO26WBA'; -- FIXME: poor equipment code 'WBA' (303,DNVRCO26WBA) [DNVRCO26WBA]
---------------------
UPDATE prefixdata SET switch='NWCSDEBCCM2' WHERE switch=='NWCSDEBCGM2'; -- FIXED: poor equipment code 'GM2' (data) [610-368]
UPDATE OR IGNORE switchdata SET switch='NWCSDEBCCM2' WHERE switch=='NWCSDEBCGM2'; -- FIXED: poor equipment code 'GM2' (data) [NWCSDEBCGM2]
DELETE FROM switchdata WHERE switch=='NWCSDEBCGM2'; -- FIXED: poor equipment code 'GM2' (data) [NWCSDEBCGM2]
---------------------
UPDATE prefixdata SET switch='GSVLFLBSCM1' WHERE switch=='GSVLFLBASCM'; -- FIXED: poor equipment code 'SCM' (data) [352-517]
UPDATE OR IGNORE switchdata SET switch='GSVLFLBSCM1' WHERE switch=='GSVLFLBASCM'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
DELETE FROM switchdata WHERE switch=='GSVLFLBASCM'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
UPDATE OR IGNORE wcdata SET wc='GSVLFLBS' WHERE wc=='GSVLFLBA'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
DELETE FROM wcdata WHERE wc=='GSVLFLBA'; -- FIXED: poor equipment code 'SCM' (data) [GSVLFLBASCM]
---------------------
UPDATE pooldata   SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [478-220-4]
UPDATE prefixdata SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [478-220]
UPDATE OR IGNORE switchdata SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [MACNGAMTXUS]
DELETE FROM switchdata WHERE switch=='MACNGAMTXUS'; -- FIXED: poor equipment code 'XUS' (data) [MACNGAMTXUS]
---------------------
UPDATE prefixdata SET switch='DVNPIAEASMD' WHERE switch=='DVNPIAEATDM'; -- FIXED: poor equipment code 'TDM' (data) [309-771]
UPDATE OR IGNORE switchdata SET switch='DVNPIAEASMD' WHERE switch=='DVNPIAEATDM'; -- FIXED: poor equipment code 'TDM' (data) [DVNPIAEATDM]
DELETE FROM switchdata WHERE switch=='DVNPIAEATDM'; -- FIXED: poor equipment code 'TDM' (data) [DVNPIAEATDM]
---------------------
UPDATE pooldata   SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [515-441-2,515-441-4,515-441-5,515-822-0,515-822-7,515-822-9]
UPDATE prefixdata SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [515-441,515-822]
UPDATE OR IGNORE switchdata SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [URDLIASDIKD]
DELETE FROM switchdata WHERE switch=='URDLIASDIKD'; -- FIXED: poor equipment code 'IKD' (data) [URDLIASDIKD]
---------------------
UPDATE prefixdata SET switch='OKBRILOAIMD' WHERE switch=='OKBRILOAIMG'; -- FIXED: poor equipment code 'IMG' (630) [630-343]
UPDATE OR IGNORE switchdata SET switch='OKBRILOAIMD' WHERE switch=='OKBRILOAIMG'; -- FIXED: poor equipment code 'IMG' (630,OKBRILOAIMG) [OKBRILOAIMG]
DELETE FROM switchdata WHERE switch=='OKBRILOAIMG'; -- FIXED: poor equipment code 'IMG' (630,OKBRILOAIMG) [OKBRILOAIMG]
---------------------
UPDATE prefixdata SET switch='LXTNKYXAXUX' WHERE switch=='LXTNKYAXAUX'; -- FIXED: poor equipment code 'AUX' (data) [606-655]
UPDATE OR IGNORE switchdata SET switch='LXTNKYXAXUX' WHERE switch=='LXTNKYAXAUX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
DELETE FROM switchdata WHERE switch=='LXTNKYAXAUX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
UPDATE OR IGNORE wcdata SET wc='LXTNKYXA' WHERE wc=='LXTNKYAX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
DELETE FROM wcdata WHERE wc=='LXTNKYAX'; -- FIXED: poor equipment code 'AUX' (data) [LXTNKYAXAUX]
---------------------
UPDATE prefixdata SET switch='BLTMMDCH13Z' WHERE switch=='BLTMMDCHI3Z'; -- FIXED: poor equipment code 'I3Z' (data) [410-800]
UPDATE OR IGNORE switchdata SET switch='BLTMMDCH13Z' WHERE switch=='BLTMMDCHI3Z'; -- FIXED: poor equipment code 'I3Z' (data) [BLTMMDCHI3Z]
DELETE FROM switchdata WHERE switch=='BLTMMDCHI3Z'; -- FIXED: poor equipment code 'I3Z' (data) [BLTMMDCHI3Z]
---------------------
UPDATE prefixdata SET switch='STPLMNHXCM1' WHERE switch=='STPLMNHXHCM'; -- FIXED: poor equipment code 'HCM' (952) [952-244]
UPDATE OR IGNORE switchdata SET switch='STPLMNHXCM1' WHERE switch=='STPLMNHXHCM'; -- FIXED: poor equipment code 'HCM' (952,STPLMNHXHCM) [STPLMNHXHCM]
DELETE FROM switchdata WHERE switch=='STPLMNHXHCM'; -- FIXED: poor equipment code 'HCM' (952,STPLMNHXHCM) [STPLMNHXHCM]
---------------------
UPDATE prefixdata SET switch='KSCYMOMCDC0' WHERE switch=='KSCYMOMCDCO'; -- FIXED: poor equipment code 'DCO' (816) [816-817,816-949]
UPDATE OR IGNORE switchdata SET switch='KSCYMOMCDC0' WHERE switch=='KSCYMOMCDCO'; -- FIXED: poor equipment code 'DCO' (816,KSCYMOMCDCO) [KSCYMOMCDCO]
DELETE FROM switchdata WHERE switch=='KSCYMOMCDCO'; -- FIXED: poor equipment code 'DCO' (816,KSCYMOMCDCO) [KSCYMOMCDCO]
---------------------
UPDATE prefixdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (data) [601-233]
UPDATE OR IGNORE switchdata SET switch='MCCMMSBLAMD' WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (data) [MCCMMSBLAMB]
DELETE FROM switchdata WHERE switch=='MCCMMSBLAMB'; -- FIXED: poor equipment code 'AMB' (data) [MCCMMSBLAMB]
---------------------
UPDATE prefixdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (data) [601-216]
UPDATE OR IGNORE switchdata SET switch='MTOLMSABAMD' WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (data) [MTOLMSABAND]
DELETE FROM switchdata WHERE switch=='MTOLMSABAND'; -- FIXED: poor equipment code 'AND' (data) [MTOLMSABAND]
---------------------
UPDATE prefixdata SET switch='FYVLNCXA49F' WHERE switch=='FYVLNCXA49D'; -- FIXED: poor equipment code '49D' (910) [910-927]
UPDATE OR IGNORE switchdata SET switch='FYVLNCXA49F' WHERE switch=='FYVLNCXA49D'; -- FIXED: poor equipment code '49D' (910,FYVLNCXA49D) [FYVLNCXA49D]
DELETE FROM switchdata WHERE switch=='FYVLNCXA49D'; -- FIXED: poor equipment code '49D' (910,FYVLNCXA49D) [FYVLNCXA49D]
---------------------
UPDATE prefixdata SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1'; -- FIXED: poor equipment code 'EC1' (data) [575-416,575-909]
UPDATE OR IGNORE switchdata SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1'; -- FIXED: poor equipment code 'EC1' (data) [HBBSNMCREC1]
DELETE FROM switchdata WHERE switch=='HBBSNMCREC1'; -- FIXED: poor equipment code 'EC1' (data) [HBBSNMCREC1]
---------------------
UPDATE prefixdata SET switch='TULSOKTBXQX' WHERE switch=='TULSOKTBHH5'; -- FIXED: poor equipment code 'HH5' (918) [918-728]
UPDATE OR IGNORE switchdata SET switch='TULSOKTBXQX' WHERE switch=='TULSOKTBHH5'; -- FIXED: poor equipment code 'HH5' (918,TULSOKTBHH5) [TULSOKTBHH5]
DELETE FROM switchdata WHERE switch=='TULSOKTBHH5'; -- FIXED: poor equipment code 'HH5' (918,TULSOKTBHH5) [TULSOKTBHH5]
---------------------
UPDATE prefixdata SET switch='BMBGPABLX7X' WHERE switch=='BMBGPABLHVA'; -- FIXED: poor equipment code 'HVA' (data) [570-416]
UPDATE OR IGNORE switchdata SET switch='BMBGPABLX7X' WHERE switch=='BMBGPABLHVA'; -- FIXED: poor equipment code 'HVA' (data) [BMBGPABLHVA]
DELETE FROM switchdata WHERE switch=='BMBGPABLHVA'; -- FIXED: poor equipment code 'HVA' (data) [BMBGPABLHVA]
---------------------
UPDATE prefixdata SET switch='HZTNPAHZAMD' WHERE switch=='HZTNPAHZHVA'; -- FIXED: poor equipment code 'HVA' (570) [570-501]
UPDATE OR IGNORE switchdata SET switch='HZTNPAHZAMD' WHERE switch=='HZTNPAHZHVA'; -- FIXED: poor equipment code 'HVA' (570,HZTNPAHZHVA) [HZTNPAHZHVA]
DELETE FROM switchdata WHERE switch=='HZTNPAHZHVA'; -- FIXED: poor equipment code 'HVA' (570,HZTNPAHZHVA) [HZTNPAHZHVA]
---------------------
UPDATE prefixdata SET switch='LNCSPALASMD' WHERE switch=='LNCSPALAHPC'; -- FIXED: poor equipment code 'HPC' (717) [717-509]
UPDATE OR IGNORE switchdata SET switch='LNCSPALASMD' WHERE switch=='LNCSPALAHPC'; -- FIXED: poor equipment code 'HPC' (717,LNCSPALAHPC) [LNCSPALAHPC]
DELETE FROM switchdata WHERE switch=='LNCSPALAHPC'; -- FIXED: poor equipment code 'HPC' (717,LNCSPALAHPC) [LNCSPALAHPC]
---------------------
UPDATE prefixdata SET switch='PITBPAIWPS0' WHERE switch=='PITBPAIWPSO'; -- FIXED: poor equipment code 'PSO' (724) [724-702]
UPDATE OR IGNORE switchdata SET switch='PITBPAIWPS0' WHERE switch=='PITBPAIWPSO'; -- FIXED: poor equipment code 'PSO' (724,PITBPAIWPSO) [PITBPAIWPSO]
DELETE FROM switchdata WHERE switch=='PITBPAIWPSO'; -- FIXED: poor equipment code 'PSO' (724,PITBPAIWPSO) [PITBPAIWPSO]
---------------------
UPDATE prefixdata SET switch='PTTWPAPT3MD' WHERE switch=='PTTWPAPTHPF'; -- FIXED: poor equipment code 'HPF' (data) [484-941]
UPDATE OR IGNORE switchdata SET switch='PTTWPAPT3MD' WHERE switch=='PTTWPAPTHPF'; -- FIXED: poor equipment code 'HPF' (data) [PTTWPAPTHPF]
DELETE FROM switchdata WHERE switch=='PTTWPAPTHPF'; -- FIXED: poor equipment code 'HPF' (data) [PTTWPAPTHPF]
---------------------
UPDATE prefixdata SET switch='RDNGPAREUMD' WHERE switch=='RDNGPAREHVA'; -- FIXED: poor equipment code 'HVA' (610) [610-685]
UPDATE OR IGNORE switchdata SET switch='RDNGPAREUMD' WHERE switch=='RDNGPAREHVA'; -- FIXED: poor equipment code 'HVA' (610,RDNGPAREHVA) [RDNGPAREHVA]
DELETE FROM switchdata WHERE switch=='RDNGPAREHVA'; -- FIXED: poor equipment code 'HVA' (610,RDNGPAREHVA) [RDNGPAREHVA]
---------------------
UPDATE pooldata   SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [570-233-4,570-233-8,570-233-9,570-579-5,570-751-2]
UPDATE prefixdata SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [570-233]
UPDATE OR IGNORE switchdata SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [SCTNPASCCOM]
DELETE FROM switchdata WHERE switch=='SCTNPASCCOM'; -- FIXED: poor equipment code 'COM' (data) [SCTNPASCCOM]
---------------------
-- UPDATE prefixdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724) [724-914]
-- UPDATE switchdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- FIXME: poor equipment code 'HPT' (724,WASHPAWAHPT) [WASHPAWAHPT]
---------------------
UPDATE prefixdata SET switch='WLPTPAWIYMD' WHERE switch=='WLPTPAWIHVA'; -- FIXED: poor equipment code 'HVA' (570) [570-601]
UPDATE OR IGNORE switchdata SET switch='WLPTPAWIYMD' WHERE switch=='WLPTPAWIHVA'; -- FIXED: poor equipment code 'HVA' (570,WLPTPAWIHVA) [WLPTPAWIHVA]
DELETE FROM switchdata WHERE switch=='WLPTPAWIHVA'; -- FIXED: poor equipment code 'HVA' (570,WLPTPAWIHVA) [WLPTPAWIHVA]
---------------------
UPDATE prefixdata SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA'; -- FIXED: poor equipment code 'HMA' (data) [267-475,267-992]
UPDATE OR IGNORE switchdata SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
DELETE FROM switchdata WHERE switch=='WNRTPAACHMA'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
UPDATE OR IGNORE wcdata SET wc='WNRTPAAH' WHERE wc=='WNRTPAAC'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
DELETE FROM wcdata WHERE wc=='WNRTPAAC'; -- FIXED: poor equipment code 'HMA' (data) [WNRTPAACHMA]
---------------------
-- UPDATE prefixdata SET switch='RDOVSCWLTP2' WHERE switch=='RDOVSCWLTP2'; -- FIXME: poor equipment code 'TP2' (803) [803-493,803-517,803-984]
-- UPDATE switchdata SET switch='RDOVSCWLTP2' WHERE switch=='RDOVSCWLTP2'; -- FIXME: poor equipment code 'TP2' (803,RDOVSCWLTP2) [RDOVSCWLTP2]
---------------------
UPDATE prefixdata SET switch='FDULSK04DS0' WHERE switch=='FDULSK04ET1'; -- FIXED: poor equipment code 'ET1' (data) [306-686]
UPDATE OR IGNORE switchdata SET switch='FDULSK04DS0' WHERE switch=='FDULSK04ET1'; -- FIXED: poor equipment code 'ET1' (data) [FDULSK04ET1]
DELETE FROM switchdata WHERE switch=='FDULSK04ET1'; -- FIXED: poor equipment code 'ET1' (data) [FDULSK04ET1]
---------------------
-- UPDATE prefixdata SET switch='HLNQSK01CTA' WHERE switch=='HLNQSK01CTA'; -- FIXME: poor equipment code 'CTA' (306) [306-553]
-- UPDATE switchdata SET switch='HLNQSK01CTA' WHERE switch=='HLNQSK01CTA'; -- FIXME: poor equipment code 'CTA' (306,HLNQSK01CTA) [HLNQSK01CTA]
---------------------
UPDATE prefixdata SET switch='WCHTKSBRXFX' WHERE switch=='WCHTSKSBRXF'; -- FIXED: poor equipment code 'RXF' (data) [620-490]
UPDATE OR IGNORE switchdata SET switch='WCHTKSBRXFX' WHERE switch=='WCHTSKSBRXF'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
DELETE FROM switchdata WHERE switch=='WCHTSKSBRXF'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
UPDATE OR IGNORE wcdata SET wc='WCHTKSBR' WHERE wc=='WCHTSKSB'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
DELETE FROM wcdata WHERE wc=='WCHTSKSB'; -- FIXED: poor equipment code 'RXF' (data) [WCHTSKSBRXF]
---------------------
-- UPDATE prefixdata SET switch='WEBBSK02CTA' WHERE switch=='WEBBSK02CTA'; -- FIXME: poor equipment code 'CTA' (306) [306-674]
-- UPDATE switchdata SET switch='WEBBSK02CTA' WHERE switch=='WEBBSK02CTA'; -- FIXME: poor equipment code 'CTA' (306,WEBBSK02CTA) [WEBBSK02CTA]
---------------------
UPDATE prefixdata SET switch='NSVLTNMT56Z' WHERE switch=='NSVLTNMTH0S'; -- FIXED: poor equipment code 'H0S' (931) [931-557]
UPDATE OR IGNORE switchdata SET switch='NSVLTNMT56Z' WHERE switch=='NSVLTNMTH0S'; -- FIXED: poor equipment code 'H0S' (931,NSVLTNMTH0S) [NSVLTNMTH0S]
DELETE FROM switchdata WHERE switch=='NSVLTNMTH0S'; -- FIXED: poor equipment code 'H0S' (931,NSVLTNMTH0S) [NSVLTNMTH0S]
---------------------
UPDATE prefixdata SET switch='PLANTXYLCM0' WHERE switch=='PLANTXYCLM0'; -- FIXED: poor equipment code 'LM0' (data) [214-898]
UPDATE OR IGNORE switchdata SET switch='PLANTXYLCM0' WHERE switch=='PLANTXYCLM0'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
DELETE FROM switchdata WHERE switch=='PLANTXYCLM0'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
UPDATE OR IGNORE wcdata SET wc='PLANTXYL' WHERE wc=='PLANTXYC'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
DELETE FROM wcdata WHERE wc=='PLANTXYC'; -- FIXED: poor equipment code 'LM0' (data) [PLANTXYCLM0]
---------------------
UPDATE prefixdata SET switch='SANGTXXCCM0' WHERE switch=='SANGTXXCCCM'; -- FIXED: poor equipment code 'CCM' (data) [979-295,979-322]
UPDATE OR IGNORE switchdata SET switch='SANGTXXCCM0' WHERE switch=='SANGTXXCCCM'; -- FIXED: poor equipment code 'CCM' (data) [SANGTXXCCCM]
DELETE FROM switchdata WHERE switch=='SANGTXXCCCM'; -- FIXED: poor equipment code 'CCM' (data) [SANGTXXCCCM]
---------------------
UPDATE pooldata   SET switch='AUSUTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [512-961-2,512-961-9]
UPDATE prefixdata SET switch='AUSUTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [512-961]
UPDATE OR IGNORE switchdata SET switch='AUSUTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
DELETE FROM switchdata WHERE switch=='AUSTUTXZMCM'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
UPDATE OR IGNORE wcdata SET wc='AUSUTXZM' WHERE wc=='AUSTUTXZ'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
DELETE FROM wcdata WHERE wc=='AUSTUTXZ'; -- FIXED: poor equipment code 'MCM' (data) [AUSTUTXZMCM]
---------------------
UPDATE pooldata   SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [509-385-0]
UPDATE prefixdata SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [509-385]
UPDATE OR IGNORE switchdata SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [SPKNWAOBWT1]
DELETE FROM switchdata WHERE switch=='SPKNWAOBWT1'; -- FIXED: poor equipment code 'WT1' (data) [SPKNWAOBWT1]
---------------------
UPDATE prefixdata SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP'; -- FIXED: poor equipment code 'WTP' (206) [206-420]
UPDATE OR IGNORE switchdata SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP'; -- FIXED: poor equipment code 'WTP' (206,STTLWAWBWTP) [STTLWAWBWTP]
DELETE FROM switchdata WHERE switch=='STTLWAWBWTP'; -- FIXED: poor equipment code 'WTP' (206,STTLWAWBWTP) [STTLWAWBWTP]
---------------------
UPDATE OR IGNORE switchdata SET switch='BRHMALHW0GT' WHERE switch=='BRHMALHWOGT'; -- FIXED: poor equipment code 'OGT' (12-12) [BRHMALHWOGT]
DELETE FROM switchdata WHERE switch=='BRHMALHWOGT'; -- FIXED: poor equipment code 'OGT' (12-12) [BRHMALHWOGT]
---------------------
UPDATE prefixdata SET switch='GNBRARXAPS0' WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [501-287]
UPDATE OR IGNORE switchdata SET switch='GNBRARXAPS0' WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
DELETE FROM switchdata WHERE switch=='LTRKARTLPSX'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
UPDATE OR IGNORE wcdata SET wc='GNBRARXA' WHERE wc=='LTRKARTL'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
DELETE FROM wcdata WHERE wc=='LTRKARTL'; -- FIXED: poor equipment code 'PSX' (501287) [LTRKARTLPSX]
---------------------
UPDATE pooldata   SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [760-592-5]
UPDATE prefixdata SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [760-592]
UPDATE OR IGNORE switchdata SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [SNDGCA02W0P]
DELETE FROM switchdata WHERE switch=='SNDGCA02W0P'; -- FIXED: poor equipment code 'W0P' (telc) [SNDGCA02W0P]
---------------------
DELETE FROM switchdata WHERE switch=='ABLNTXOR1ST'; -- FIXED: poor equipment code '1ST' (ABLNTXOR1ST) [ABLNTXOR1ST]
DELETE FROM switchdata WHERE switch=='ALBRNMJW0ND'; -- FIXED: poor equipment code '0ND' (ALBRNMJW0ND) [ALBRNMJW0ND]
DELETE FROM switchdata WHERE switch=='ALLSWITCHES'; -- FIXED: poor equipment code 'HES' (ALLSWITCHES) [ALLSWITCHES]
DELETE FROM switchdata WHERE switch=='ALTOTXXOHPI'; -- FIXED: poor equipment code 'HPI' (ALTOTXXOHPI) [ALTOTXXOHPI]
DELETE FROM switchdata WHERE switch=='AMRLTX02CGO'; -- FIXED: poor equipment code 'CGO' (AMRLTX02CGO) [AMRLTX02CGO]
DELETE FROM switchdata WHERE switch=='AMRLTX92WSM'; -- FIXED: poor equipment code 'WSM' (AMRLTX92WSM) [AMRLTX92WSM]
DELETE FROM switchdata WHERE switch=='ANPKNV10HBC'; -- FIXED: poor equipment code 'HBC' (ANPKNV10HBC) [ANPKNV10HBC]
DELETE FROM switchdata WHERE switch=='APSPTXXPHPI'; -- FIXED: poor equipment code 'HPI' (APSPTXXPHPI) [APSPTXXPHPI]
DELETE FROM switchdata WHERE switch=='ATLNGACSBC2'; -- FIXED: poor equipment code 'BC2' (ATLNGACSBC2) [ATLNGACSBC2]
DELETE FROM switchdata WHERE switch=='AUSTTXGR8X8'; -- FIXED: poor equipment code '8X8' (AUSTTXGR8X8) [AUSTTXGR8X8]
DELETE FROM switchdata WHERE switch=='AUSTTXGRHC1'; -- FIXED: poor equipment code 'HC1' (AUSTTXGRHC1) [AUSTTXGRHC1]
DELETE FROM switchdata WHERE switch=='BDTMMOHIDCM'; -- FIXED: poor equipment code 'DCM' (BDTMMOHIDCM) [BDTMMOHIDCM]
DELETE FROM switchdata WHERE switch=='BEREKYXAPSO'; -- FIXED: poor equipment code 'PSO' (BEREKYXAPSO) [BEREKYXAPSO]
DELETE FROM switchdata WHERE switch=='BGTMMTXCHS0'; -- FIXED: poor equipment code 'HS0' (BGTMMTXCHS0) [BGTMMTXCHS0]
DELETE FROM switchdata WHERE switch=='BKFDCAKTIKD'; -- FIXED: poor equipment code 'IKD' (BKFDCAKTIKD) [BKFDCAKTIKD]
DELETE FROM switchdata WHERE switch=='BKSHTXXAHPI'; -- FIXED: poor equipment code 'HPI' (BKSHTXXAHPI) [BKSHTXXAHPI]
DELETE FROM switchdata WHERE switch=='BLARMDBLHPC'; -- FIXED: poor equipment code 'HPC' (BLARMDBLHPC) [BLARMDBLHPC]
DELETE FROM switchdata WHERE switch=='BLTMMDSNBBI'; -- FIXED: poor equipment code 'BBI' (BLTMMDSNBBI) [BLTMMDSNBBI]
DELETE FROM switchdata WHERE switch=='BLTNILXDPSO'; -- FIXED: poor equipment code 'PSO' (BLTNILXDPSO) [BLTNILXDPSO]
DELETE FROM switchdata WHERE switch=='BNTXARCRGEL'; -- FIXED: poor equipment code 'GEL' (BNTXARCRGEL) [BNTXARCRGEL]
DELETE FROM switchdata WHERE switch=='BOISIDMAKXX'; -- FIXED: poor equipment code 'KXX' (BOISIDMAKXX) [BOISIDMAKXX]
DELETE FROM switchdata WHERE switch=='BRHMALMTW6C'; -- FIXED: poor equipment code 'W6C' (BRHMALMTW6C) [BRHMALMTW6C]
DELETE FROM switchdata WHERE switch=='BRHXALRTWPS'; -- FIXED: poor equipment code 'WPS' (BRHXALRTWPS) [BRHXALRTWPS]
DELETE FROM switchdata WHERE switch=='BSMRNDBCFH5'; -- FIXED: poor equipment code 'FH5' (BSMRNDBCFH5) [BSMRNDBCFH5]
DELETE FROM switchdata WHERE switch=='BSTNMACVWDC'; -- FIXED: poor equipment code 'WDC' (BSTNMACVWDC) [BSTNMACVWDC]
DELETE FROM switchdata WHERE switch=='CENTTXXCHPI'; -- FIXED: poor equipment code 'HPI' (CENTTXXCHPI) [CENTTXXCHPI]
DELETE FROM switchdata WHERE switch=='CHCGILCLTDS'; -- FIXED: poor equipment code 'TDS' (CHCGILCLTDS) [CHCGILCLTDS]
DELETE FROM switchdata WHERE switch=='CHRLMT02WOO'; -- FIXED: poor equipment code 'WOO' (CHRLMT02WOO) [CHRLMT02WOO]
DELETE FROM switchdata WHERE switch=='CKVLTNVA1ND'; -- FIXED: poor equipment code '1ND' (CKVLTNVA1ND) [CKVLTNVA1ND]
DELETE FROM switchdata WHERE switch=='CLBGWVMAPLU'; -- FIXED: poor equipment code 'PLU' (CLBGWVMAPLU) [CLBGWVMAPLU]
DELETE FROM switchdata WHERE switch=='CLDLNV09HBC'; -- FIXED: poor equipment code 'HBC' (CLDLNV09HBC) [CLDLNV09HBC]
DELETE FROM switchdata WHERE switch=='CLMASCCHKXX'; -- FIXED: poor equipment code 'KXX' (CLMASCCHKXX) [CLMASCCHKXX]
DELETE FROM switchdata WHERE switch=='CLMASCSNXG4'; -- FIXED: poor equipment code 'XG4' (CLMASCSNXG4) [CLMASCSNXG4]
DELETE FROM switchdata WHERE switch=='CLMASCTSDCO'; -- FIXED: poor equipment code 'DCO' (CLMASCTSDCO) [CLMASCTSDCO]
DELETE FROM switchdata WHERE switch=='CLMBINAKWM1'; -- FIXED: poor equipment code 'WM1' (CLMBINAKWM1) [CLMBINAKWM1]
DELETE FROM switchdata WHERE switch=='CLMBOHIAWPY'; -- FIXED: poor equipment code 'WPY' (CLMBOHIAWPY) [CLMBOHIAWPY]
DELETE FROM switchdata WHERE switch=='CLMSSCARKXX'; -- FIXED: poor equipment code 'KXX' (CLMSSCARKXX) [CLMSSCARKXX]
DELETE FROM switchdata WHERE switch=='CLSMPAXCRU1'; -- FIXED: poor equipment code 'RU1' (CLSMPAXCRU1) [CLSMPAXCRU1]
DELETE FROM switchdata WHERE switch=='CLSMPAXCRU2'; -- FIXED: poor equipment code 'RU2' (CLSMPAXCRU2) [CLSMPAXCRU2]
DELETE FROM switchdata WHERE switch=='CLSTTXADQO1'; -- FIXED: poor equipment code 'QO1' (CLSTTXADQO1) [CLSTTXADQO1]
DELETE FROM switchdata WHERE switch=='CMDNNJCEPHA'; -- FIXED: poor equipment code 'PHA' (CMDNNJCEPHA) [CMDNNJCEPHA]
DELETE FROM switchdata WHERE switch=='CONRTXXAHPI'; -- FIXED: poor equipment code 'HPI' (CONRTXXAHPI) [CONRTXXAHPI]
DELETE FROM switchdata WHERE switch=='CTSHTXXCHPI'; -- FIXED: poor equipment code 'HPI' (CTSHTXXCHPI) [CTSHTXXCHPI]
DELETE FROM switchdata WHERE switch=='DANVILT1AXA'; -- FIXED: poor equipment code 'AXA' (DANVILT1AXA) [DANVILT1AXA]
DELETE FROM switchdata WHERE switch=='DBLLTXXDHPI'; -- FIXED: poor equipment code 'HPI' (DBLLTXXDHPI) [DBLLTXXDHPI]
DELETE FROM switchdata WHERE switch=='DRBHFLMADMS'; -- FIXED: poor equipment code 'DMS' (DRBHFLMADMS) [DRBHFLMADMS]
DELETE FROM switchdata WHERE switch=='EGVGILEGTDS'; -- FIXED: poor equipment code 'TDS' (EGVGILEGTDS) [EGVGILEGTDS]
DELETE FROM switchdata WHERE switch=='ELDRPAXEPSO'; -- FIXED: poor equipment code 'PSO' (ELDRPAXEPSO) [ELDRPAXEPSO]
DELETE FROM switchdata WHERE switch=='EPHRPAXLPSO'; -- FIXED: poor equipment code 'PSO' (EPHRPAXLPSO) [EPHRPAXLPSO]
DELETE FROM switchdata WHERE switch=='ETOLTXXEHPI'; -- FIXED: poor equipment code 'HPI' (ETOLTXXEHPI) [ETOLTXXEHPI]
DELETE FROM switchdata WHERE switch=='FARGNDBCF2E'; -- FIXED: poor equipment code 'F2E' (FARGNDBCF2E) [FARGNDBCF2E]
DELETE FROM switchdata WHERE switch=='FLLNNVCTHBC'; -- FIXED: poor equipment code 'HBC' (FLLNNVCTHBC) [FLLNNVCTHBC]
DELETE FROM switchdata WHERE switch=='FLLNNVXDHBC'; -- FIXED: poor equipment code 'HBC' (FLLNNVXDHBC) [FLLNNVXDHBC]
DELETE FROM switchdata WHERE switch=='FLSPTXXFHPI'; -- FIXED: poor equipment code 'HPI' (FLSPTXXFHPI) [FLSPTXXFHPI]
DELETE FROM switchdata WHERE switch=='FRLKMNFRLDS'; -- FIXED: poor equipment code 'LDS' (FRLKMNFRLDS) [FRLKMNFRLDS]
DELETE FROM switchdata WHERE switch=='FTDDIAXCPOI'; -- FIXED: poor equipment code 'POI' (FTDDIAXCPOI) [FTDDIAXCPOI]
DELETE FROM switchdata WHERE switch=='FTLDFLCRDMS'; -- FIXED: poor equipment code 'DMS' (FTLDFLCRDMS) [FTLDFLCRDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLCYDMS'; -- FIXED: poor equipment code 'DMS' (FTLDFLCYDMS) [FTLDFLCYDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLMRDMS'; -- FIXED: poor equipment code 'DMS' (FTLDFLMRDMS) [FTLDFLMRDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLOADMS'; -- FIXED: poor equipment code 'DMS' (FTLDFLOADMS) [FTLDFLOADMS]
DELETE FROM switchdata WHERE switch=='FTLDFLPLDMS'; -- FIXED: poor equipment code 'DMS' (FTLDFLPLDMS) [FTLDFLPLDMS]
DELETE FROM switchdata WHERE switch=='FTLDFLPLG1W'; -- FIXED: poor equipment code 'G1W' (FTLDFLPLG1W) [FTLDFLPLG1W]
DELETE FROM switchdata WHERE switch=='GDRPMIBLH1C'; -- FIXED: poor equipment code 'H1C' (GDRPMIBLH1C) [GDRPMIBLH1C]
DELETE FROM switchdata WHERE switch=='GNLDTXXGHPI'; -- FIXED: poor equipment code 'HPI' (GNLDTXXGHPI) [GNLDTXXGHPI]
DELETE FROM switchdata WHERE switch=='GRVEOKMARLC'; -- FIXED: poor equipment code 'RLC' (GRVEOKMARLC) [GRVEOKMARLC]
DELETE FROM switchdata WHERE switch=='HAVRMTXCHS0'; -- FIXED: poor equipment code 'HS0' (HAVRMTXCHS0) [HAVRMTXCHS0]
DELETE FROM switchdata WHERE switch=='HAYSKASLD1M'; -- FIXED: poor equipment code 'D1M' (HAYSKASLD1M) [HAYSKASLD1M]
DELETE FROM switchdata WHERE switch=='HCKRNCXAX6C'; -- FIXED: poor equipment code 'X6C' (HCKRNCXAX6C) [HCKRNCXAX6C]
DELETE FROM switchdata WHERE switch=='HDSNOHXAPSO'; -- FIXED: poor equipment code 'PSO' (HDSNOHXAPSO) [HDSNOHXAPSO]
DELETE FROM switchdata WHERE switch=='HDSNTXXHHPI'; -- FIXED: poor equipment code 'HPI' (HDSNTXXHHPI) [HDSNTXXHHPI]
DELETE FROM switchdata WHERE switch=='HLWDFLHADMS'; -- FIXED: poor equipment code 'DMS' (HLWDFLHADMS) [HLWDFLHADMS]
DELETE FROM switchdata WHERE switch=='HLWDFLWHDMS'; -- FIXED: poor equipment code 'DMS' (HLWDFLWHDMS) [HLWDFLWHDMS]
DELETE FROM switchdata WHERE switch=='HNDLILHNTDS'; -- FIXED: poor equipment code 'TDS' (HNDLILHNTDS) [HNDLILHNTDS]
DELETE FROM switchdata WHERE switch=='HNLLHIMNLAD'; -- FIXED: poor equipment code 'LAD' (HNLLHIMNLAD) [HNLLHIMNLAD]
DELETE FROM switchdata WHERE switch=='HNVIALUNOGT'; -- FIXED: poor equipment code 'OGT' (HNVIALUNOGT) [HNVIALUNOGT]
DELETE FROM switchdata WHERE switch=='HNVXALUNOGT'; -- FIXED: poor equipment code 'OGT' (HNVXALUNOGT) [HNVXALUNOGT]
DELETE FROM switchdata WHERE switch=='HRLNTXHGIX4'; -- FIXED: poor equipment code 'IX4' (HRLNTXHGIX4) [HRLNTXHGIX4]
DELETE FROM switchdata WHERE switch=='IDFLIDMAKXX'; -- FIXED: poor equipment code 'KXX' (IDFLIDMAKXX) [IDFLIDMAKXX]
DELETE FROM switchdata WHERE switch=='INDNIABFELD'; -- FIXED: poor equipment code 'ELD' (INDNIABFELD) [INDNIABFELD]
DELETE FROM switchdata WHERE switch=='JCVLFLARDMS'; -- FIXED: poor equipment code 'DMS' (JCVLFLARDMS) [JCVLFLARDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLCLDMS'; -- FIXED: poor equipment code 'DMS' (JCVLFLCLDMS) [JCVLFLCLDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLFCDMS'; -- FIXED: poor equipment code 'DMS' (JCVLFLFCDMS) [JCVLFLFCDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLMSDMS'; -- FIXED: poor equipment code 'DMS' (JCVLFLMSDMS) [JCVLFLMSDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLRVDMS'; -- FIXED: poor equipment code 'DMS' (JCVLFLRVDMS) [JCVLFLRVDMS]
DELETE FROM switchdata WHERE switch=='JCVLFLSJDMS'; -- FIXED: poor equipment code 'DMS' (JCVLFLSJDMS) [JCVLFLSJDMS]
DELETE FROM switchdata WHERE switch=='JHCYTNXAHS0'; -- FIXED: poor equipment code 'HS0' (JHCYTNXAHS0) [JHCYTNXAHS0]
DELETE FROM switchdata WHERE switch=='JMTWNYXAPSO'; -- FIXED: poor equipment code 'PSO' (JMTWNYXAPSO) [JMTWNYXAPSO]
DELETE FROM switchdata WHERE switch=='JNCYKSXCHS0'; -- FIXED: poor equipment code 'HS0' (JNCYKSXCHS0) [JNCYKSXCHS0]
DELETE FROM switchdata WHERE switch=='KATYTXXAHPI'; -- FIXED: poor equipment code 'HPI' (KATYTXXAHPI) [KATYTXXAHPI]
DELETE FROM switchdata WHERE switch=='KATYTXXAJF1'; -- FIXED: poor equipment code 'JF1' (KATYTXXAJF1) [KATYTXXAJF1]
DELETE FROM switchdata WHERE switch=='KATYTXXBHPI'; -- FIXED: poor equipment code 'HPI' (KATYTXXBHPI) [KATYTXXBHPI]
DELETE FROM switchdata WHERE switch=='KATYTXXCHPI'; -- FIXED: poor equipment code 'HPI' (KATYTXXCHPI) [KATYTXXCHPI]
DELETE FROM switchdata WHERE switch=='KGFSOKXAO1W'; -- FIXED: poor equipment code 'O1W' (KGFSOKXAO1W) [KGFSOKXAO1W]
DELETE FROM switchdata WHERE switch=='LFKNTXXAHPI'; -- FIXED: poor equipment code 'HPI' (LFKNTXXAHPI) [LFKNTXXAHPI]
DELETE FROM switchdata WHERE switch=='LKCFTXXLHIP'; -- FIXED: poor equipment code 'HIP' (LKCFTXXLHIP) [LKCFTXXLHIP]
DELETE FROM switchdata WHERE switch=='LKPTUTBHOOT'; -- FIXED: poor equipment code 'OOT' (LKPTUTBHOOT) [LKPTUTBHOOT]
DELETE FROM switchdata WHERE switch=='LNNGMIMNH0A'; -- FIXED: poor equipment code 'H0A' (LNNGMIMNH0A) [LNNGMIMNH0A]
DELETE FROM switchdata WHERE switch=='LSANCARCOSZ'; -- FIXED: poor equipment code 'OSZ' (LSANCARCOSZ) [LSANCARCOSZ]
DELETE FROM switchdata WHERE switch=='LSVGNV03HBC'; -- FIXED: poor equipment code 'HBC' (LSVGNV03HBC) [LSVGNV03HBC]
DELETE FROM switchdata WHERE switch=='LSVLKYKYAPX'; -- FIXED: poor equipment code 'APX' (LSVLKYKYAPX) [LSVLKYKYAPX]
DELETE FROM switchdata WHERE switch=='LTRKARCAHB1'; -- FIXED: poor equipment code 'HB1' (LTRKARCAHB1) [LTRKARCAHB1]
DELETE FROM switchdata WHERE switch=='LTRKARFRHD1'; -- FIXED: poor equipment code 'HD1' (LTRKARFRHD1) [LTRKARFRHD1]
DELETE FROM switchdata WHERE switch=='LTRKARMOHB1'; -- FIXED: poor equipment code 'HB1' (LTRKARMOHB1) [LTRKARMOHB1]
DELETE FROM switchdata WHERE switch=='LWBGPAXLPSO'; -- FIXED: poor equipment code 'PSO' (LWBGPAXLPSO) [LWBGPAXLPSO]
DELETE FROM switchdata WHERE switch=='MEXCMOJUHC1'; -- FIXED: poor equipment code 'HC1' (MEXCMOJUHC1) [MEXCMOJUHC1]
DELETE FROM switchdata WHERE switch=='MIAMFLAEDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLAEDMS) [MIAMFLAEDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLBADMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLBADMS) [MIAMFLBADMS]
DELETE FROM switchdata WHERE switch=='MIAMFLBRDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLBRDMS) [MIAMFLBRDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLCADMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLCADMS) [MIAMFLCADMS]
DELETE FROM switchdata WHERE switch=='MIAMFLGRDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLGRDMS) [MIAMFLGRDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLHLDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLHLDMS) [MIAMFLHLDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLICDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLICDMS) [MIAMFLICDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLNMDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLNMDMS) [MIAMFLNMDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLPLDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLPLDMS) [MIAMFLPLDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLRRDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLRRDMS) [MIAMFLRRDMS]
DELETE FROM switchdata WHERE switch=='MIAMFLSODMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLSODMS) [MIAMFLSODMS]
DELETE FROM switchdata WHERE switch=='MIAMFLWMDMS'; -- FIXED: poor equipment code 'DMS' (MIAMFLWMDMS) [MIAMFLWMDMS]
DELETE FROM switchdata WHERE switch=='MNCHNHCOXE7'; -- FIXED: poor equipment code 'XE7' (MNCHNHCOXE7) [MNCHNHCOXE7]
DELETE FROM switchdata WHERE switch=='MOBXALVAUC1'; -- FIXED: poor equipment code 'UC1' (MOBXALVAUC1) [MOBXALVAUC1]
DELETE FROM switchdata WHERE switch=='MONRNYXAJUD'; -- FIXED: poor equipment code 'JUD' (MONRNYXAJUD) [MONRNYXAJUD]
DELETE FROM switchdata WHERE switch=='MPLSMNCD1KM'; -- FIXED: poor equipment code '1KM' (MPLSMNCD1KM) [MPLSMNCD1KM]
DELETE FROM switchdata WHERE switch=='MPLSMNDTKDZ'; -- FIXED: poor equipment code 'KDZ' (MPLSMNDTKDZ) [MPLSMNDTKDZ]
DELETE FROM switchdata WHERE switch=='MTGMALMTW6C'; -- FIXED: poor equipment code 'W6C' (MTGMALMTW6C) [MTGMALMTW6C]
DELETE FROM switchdata WHERE switch=='MTGMTXXMHPI'; -- FIXED: poor equipment code 'HPI' (MTGMTXXMHPI) [MTGMTXXMHPI]
DELETE FROM switchdata WHERE switch=='NBRKILNTTDS'; -- FIXED: poor equipment code 'TDS' (NBRKILNTTDS) [NBRKILNTTDS]
DELETE FROM switchdata WHERE switch=='NDADFLACDMS'; -- FIXED: poor equipment code 'DMS' (NDADFLACDMS) [NDADFLACDMS]
DELETE FROM switchdata WHERE switch=='NDADFLBRDMS'; -- FIXED: poor equipment code 'DMS' (NDADFLBRDMS) [NDADFLBRDMS]
DELETE FROM switchdata WHERE switch=='NWMDWIXAPOI'; -- FIXED: poor equipment code 'POI' (NWMDWIXAPOI) [NWMDWIXAPOI]
DELETE FROM switchdata WHERE switch=='NWRKNJ4IWI0'; -- FIXED: poor equipment code 'WI0' (NWRKNJ4IWI0) [NWRKNJ4IWI0]
DELETE FROM switchdata WHERE switch=='NWRKNJMDHC8'; -- FIXED: poor equipment code 'HC8' (NWRKNJMDHC8) [NWRKNJMDHC8]
DELETE FROM switchdata WHERE switch=='ORLDFLAPDMS'; -- FIXED: poor equipment code 'DMS' (ORLDFLAPDMS) [ORLDFLAPDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLCLDMS'; -- FIXED: poor equipment code 'DMS' (ORLDFLCLDMS) [ORLDFLCLDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLMADMS'; -- FIXED: poor equipment code 'DMS' (ORLDFLMADMS) [ORLDFLMADMS]
DELETE FROM switchdata WHERE switch=='ORLDFLPCDMS'; -- FIXED: poor equipment code 'DMS' (ORLDFLPCDMS) [ORLDFLPCDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLPHDMS'; -- FIXED: poor equipment code 'DMS' (ORLDFLPHDMS) [ORLDFLPHDMS]
DELETE FROM switchdata WHERE switch=='ORLDFLSADMS'; -- FIXED: poor equipment code 'DMS' (ORLDFLSADMS) [ORLDFLSADMS]
DELETE FROM switchdata WHERE switch=='PCTLIDMAKXX'; -- FIXED: poor equipment code 'KXX' (PCTLIDMAKXX) [PCTLIDMAKXX]
DELETE FROM switchdata WHERE switch=='PEORILPJTDS'; -- FIXED: poor equipment code 'TDS' (PEORILPJTDS) [PEORILPJTDS]
DELETE FROM switchdata WHERE switch=='PHLAPAFG5ES'; -- FIXED: poor equipment code '5ES' (PHLAPAFG5ES) [PHLAPAFG5ES]
DELETE FROM switchdata WHERE switch=='PHLAPAFGSW2'; -- FIXED: poor equipment code 'SW2' (PHLAPAFGSW2) [PHLAPAFGSW2]
DELETE FROM switchdata WHERE switch=='PHLAPAFGWS2'; -- FIXED: poor equipment code 'WS2' (PHLAPAFGWS2) [PHLAPAFGWS2]
DELETE FROM switchdata WHERE switch=='PHNXAZMACCM'; -- FIXED: poor equipment code 'CCM' (PHNXAZMACCM) [PHNXAZMACCM]
DELETE FROM switchdata WHERE switch=='PHNXAZNO0W1'; -- FIXED: poor equipment code '0W1' (PHNXAZNO0W1) [PHNXAZNO0W1]
DELETE FROM switchdata WHERE switch=='PITBPADTHVE'; -- FIXED: poor equipment code 'HVE' (PITBPADTHVE) [PITBPADTHVE]
DELETE FROM switchdata WHERE switch=='PMBHFLFEDMS'; -- FIXED: poor equipment code 'DMS' (PMBHFLFEDMS) [PMBHFLFEDMS]
DELETE FROM switchdata WHERE switch=='PMBHFLMADMS'; -- FIXED: poor equipment code 'DMS' (PMBHFLMADMS) [PMBHFLMADMS]
DELETE FROM switchdata WHERE switch=='POMTNV10HBC'; -- FIXED: poor equipment code 'HBC' (POMTNV10HBC) [POMTNV10HBC]
DELETE FROM switchdata WHERE switch=='PRLKKYAAJ00'; -- FIXED: poor equipment code 'J00' (PRLKKYAAJ00) [PRLKKYAAJ00]
DELETE FROM switchdata WHERE switch=='PRVSMSAMAND'; -- FIXED: poor equipment code 'AND' (PRVSMSAMAND) [PRVSMSAMAND]
DELETE FROM switchdata WHERE switch=='PSVLNJPL2GN'; -- FIXED: poor equipment code '2GN' (PSVLNJPL2GN) [PSVLNJPL2GN]
DELETE FROM switchdata WHERE switch=='PTLDORPBWID'; -- FIXED: poor equipment code 'WID' (PTLDORPBWID) [PTLDORPBWID]
DELETE FROM switchdata WHERE switch=='RCFRILRTTDS'; -- FIXED: poor equipment code 'TDS' (RCFRILRTTDS) [RCFRILRTTDS]
DELETE FROM switchdata WHERE switch=='RCISILRIKA1'; -- FIXED: poor equipment code 'KA1' (RCISILRIKA1) [RCISILRIKA1]
DELETE FROM switchdata WHERE switch=='RONKVALKXRK'; -- FIXED: poor equipment code 'XRK' (RONKVALKXRK) [RONKVALKXRK]
DELETE FROM switchdata WHERE switch=='RSMYOHROWV1'; -- FIXED: poor equipment code 'WV1' (RSMYOHROWV1) [RSMYOHROWV1]
DELETE FROM switchdata WHERE switch=='RSVTUTTANCM'; -- FIXED: poor equipment code 'NCM' (RSVTUTTANCM) [RSVTUTTANCM]
DELETE FROM switchdata WHERE switch=='RVBKTXXRHPI'; -- FIXED: poor equipment code 'HPI' (RVBKTXXRHPI) [RVBKTXXRHPI]
DELETE FROM switchdata WHERE switch=='SCRMCAAM0S0'; -- FIXED: poor equipment code '0S0' (SCRMCAAM0S0) [SCRMCAAM0S0]
DELETE FROM switchdata WHERE switch=='SFLDMIMNVS0'; -- FIXED: poor equipment code 'VS0' (SFLDMIMNVS0) [SFLDMIMNVS0]
DELETE FROM switchdata WHERE switch=='SFLDMIUCTCM'; -- FIXED: poor equipment code 'TCM' (SFLDMIUCTCM) [SFLDMIUCTCM]
DELETE FROM switchdata WHERE switch=='SGLDTXAQWWM'; -- FIXED: poor equipment code 'WWM' (SGLDTXAQWWM) [SGLDTXAQWWM]
DELETE FROM switchdata WHERE switch=='SNANCACZDZ0'; -- FIXED: poor equipment code 'DZ0' (SNANCACZDZ0) [SNANCACZDZ0]
DELETE FROM switchdata WHERE switch=='SNANTXCAHC1'; -- FIXED: poor equipment code 'HC1' (SNANTXCAHC1) [SNANTXCAHC1]
DELETE FROM switchdata WHERE switch=='SNDGCA023DK'; -- FIXED: poor equipment code '3DK' (SNDGCA023DK) [SNDGCA023DK]
DELETE FROM switchdata WHERE switch=='SPNCIAMURLO'; -- FIXED: poor equipment code 'RLO' (SPNCIAMURLO) [SPNCIAMURLO]
DELETE FROM switchdata WHERE switch=='SSKTNCA01XT'; -- FIXED: poor equipment code '1XT' (SSKTNCA01XT) [SSKTNCA01XT]
DELETE FROM switchdata WHERE switch=='STCDMNOTOXR'; -- FIXED: poor equipment code 'OXR' (STCDMNOTOXR) [STCDMNOTOXR]
DELETE FROM switchdata WHERE switch=='STPLMNMKKDZ'; -- FIXED: poor equipment code 'KDZ' (STPLMNMKKDZ) [STPLMNMKKDZ]
DELETE FROM switchdata WHERE switch=='SXCYIADTSB3'; -- FIXED: poor equipment code 'SB3' (SXCYIADTSB3) [SXCYIADTSB3]
DELETE FROM switchdata WHERE switch=='TACMWA43XM2'; -- FIXED: poor equipment code 'XM2' (TACMWA43XM2) [TACMWA43XM2]
DELETE FROM switchdata WHERE switch=='TACMWAFAKXX'; -- FIXED: poor equipment code 'KXX' (TACMWAFAKXX) [TACMWAFAKXX]
DELETE FROM switchdata WHERE switch=='THTNGAXAPSO'; -- FIXED: poor equipment code 'PSO' (THTNGAXAPSO) [THTNGAXAPSO]
DELETE FROM switchdata WHERE switch=='TSC LALBX0M'; -- FIXED: poor equipment code 'X0M' (TSC LALBX0M) [TSC LALBX0M]
DELETE FROM switchdata WHERE switch=='TWFLIDMAKXX'; -- FIXED: poor equipment code 'KXX' (TWFLIDMAKXX) [TWFLIDMAKXX]
DELETE FROM switchdata WHERE switch=='WCLMSCMAKXX'; -- FIXED: poor equipment code 'KXX' (WCLMSCMAKXX) [WCLMSCMAKXX]
DELETE FROM switchdata WHERE switch=='WLDMTXXWHPI'; -- FIXED: poor equipment code 'HPI' (WLDMTXXWHPI) [WLDMTXXWHPI]
DELETE FROM switchdata WHERE switch=='WLLSTXXWHPI'; -- FIXED: poor equipment code 'HPI' (WLLSTXXWHPI) [WLLSTXXWHPI]
DELETE FROM switchdata WHERE switch=='WNDSCOMAZ01'; -- FIXED: poor equipment code 'Z01' (WNDSCOMAZ01) [WNDSCOMAZ01]
DELETE FROM switchdata WHERE switch=='WNHNFLBUVM1'; -- FIXED: poor equipment code 'VM1' (WNHNFLBUVM1) [WNHNFLBUVM1]
DELETE FROM switchdata WHERE switch=='WOTNMN14WOO'; -- FIXED: poor equipment code 'WOO' (WOTNMN14WOO) [WOTNMN14WOO]
DELETE FROM switchdata WHERE switch=='WPBHFLANDMS'; -- FIXED: poor equipment code 'DMS' (WPBHFLANDMS) [WPBHFLANDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLGADMS'; -- FIXED: poor equipment code 'DMS' (WPBHFLGADMS) [WPBHFLGADMS]
DELETE FROM switchdata WHERE switch=='WPBHFLGRDMS'; -- FIXED: poor equipment code 'DMS' (WPBHFLGRDMS) [WPBHFLGRDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLHHDMS'; -- FIXED: poor equipment code 'DMS' (WPBHFLHHDMS) [WPBHFLHHDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLLEDMS'; -- FIXED: poor equipment code 'DMS' (WPBHFLLEDMS) [WPBHFLLEDMS]
DELETE FROM switchdata WHERE switch=='WPBHFLRBDMS'; -- FIXED: poor equipment code 'DMS' (WPBHFLRBDMS) [WPBHFLRBDMS]
DELETE FROM switchdata WHERE switch=='WSCRCABYESS'; -- FIXED: poor equipment code 'ESS' (WSCRCABYESS) [WSCRCABYESS]
DELETE FROM switchdata WHERE switch=='WTWPIN01RLO'; -- FIXED: poor equipment code 'RLO' (WTWPIN01RLO) [WTWPIN01RLO]
DELETE FROM switchdata WHERE switch=='WVCYUTCK1JD'; -- FIXED: poor equipment code '1JD' (WVCYUTCK1JD) [WVCYUTCK1JD]
DELETE FROM switchdata WHERE switch=='WWFDNYXAPSO'; -- FIXED: poor equipment code 'PSO' (WWFDNYXAPSO) [WWFDNYXAPSO]
--=============================================
