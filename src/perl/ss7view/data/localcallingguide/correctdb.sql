-- UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='251'; -- #
-- UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='464'; -- #
-- UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='527'; -- #
-- UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='632'; -- #
-- UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='794'; -- #
-- UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='804'; -- #
-- UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='973'; -- #

-- Actually lcg is using Postal State.
-- UPDATE rcdata SET region='NN' WHERE region=='MP'; -- correct region quirks
-- UPDATE rcdata SET region='NF' WHERE region=='NL'; -- correct region quirks
-- UPDATE rcdata SET region='VU' WHERE region=='NU'; -- correct region quirks
-- UPDATE rcdata SET region='PQ' WHERE region=='QC'; -- correct region quirks
UPDATE rcdata SET region='SF' WHERE region=='SM'; -- correct region quirks

UPDATE pooldata   SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- #letter mangling, 256-853
UPDATE pooldata   SET switch='AUSTTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- #letter insertion, 512-961
UPDATE pooldata   SET switch='BEREKYXAPS0' WHERE switch=='BEREKYXAPSO'; -- #letter confusion O/0 (29-4)
UPDATE pooldata   SET switch='BLTMMDCH13Z' WHERE switch=='BLTMMDCHI3Z'; -- #letter confusion I/1, 410-800
UPDATE pooldata   SET switch='BLTMMDCHXOY' WHERE switch=='BLTMMDCHC01' AND npa=='443' AND nxx=='207' AND x=='3'; -- #corrected
UPDATE pooldata   SET switch='BLTNILXDPS0' WHERE switch=='BLTNILXDPSO'; -- #letter confusion O/0 (25-13)
UPDATE pooldata   SET switch='BMBGPABLX7X' WHERE switch=='BMBGPABLHVA'; -- #corrected, 570-416
UPDATE pooldata   SET switch='BNTVARCRDS0' WHERE switch=='BNTVARCRGEL'; -- #corrected, 479-203,479-205
UPDATE pooldata   SET switch='BNTXARCRDS0' WHERE switch=='BNTXARCRGEL'; -- #fixed'
UPDATE pooldata   SET switch='BRHMALEW95E' WHERE switch=='BRHMALEWRPA'; -- #fixed (12-8)
UPDATE pooldata   SET switch='BRHMALTA84E' WHERE switch=='BRHMALTARPA'; -- #fixed (12-15)
UPDATE pooldata   SET switch='BRYNTXAYCM0' WHERE switch=='SANGTXXCCCM'; -- #corrected, 979-295,979-322
UPDATE pooldata   SET switch='BUMTTXTEWMD' WHERE switch=='BUMMTTXEWMD'; -- #letter insertion, 409-223'
UPDATE pooldata   SET switch='CHCGILCPDS5' WHERE switch=='CHICTGOZN10'; -- #rate center instead of clli
UPDATE pooldata   SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- #corrected, 843-606'
UPDATE pooldata   SET switch='CLMASCSNXGY' WHERE switch=='CLMASCSNXG4'; -- #letter confusion 4/Y
UPDATE pooldata   SET switch='CLMASCTSDC0' WHERE switch=='CLMASCTSDCO'; -- #letter confusion O/0 (53-23)
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='1'; -- #corrected
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='2'; -- #corrected
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='4'; -- #corrected
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='5'; -- #corrected
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='6'; -- #corrected
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='7'; -- #corrected
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='8'; -- #corrected
UPDATE pooldata   SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915' AND x=='9'; -- #corrected
UPDATE pooldata   SET switch='CRVLTNAOGT1' WHERE switch=='MMMPHTNMACM'; -- #corrected?, 731-613'
UPDATE pooldata   SET switch='DESMIA726MD' WHERE switch=='URDLIASD1KD' AND npa=='515' AND nxx=='822' AND x=='0'; -- #corrected
UPDATE pooldata   SET switch='DESMIA726MD' WHERE switch=='URDLIASD1KD' AND npa=='515' AND nxx=='822' AND x=='7'; -- #corrected
UPDATE pooldata   SET switch='DESMIA726MD' WHERE switch=='URDLIASD1KD' AND npa=='515' AND nxx=='822' AND x=='9'; -- #corrected
UPDATE pooldata   SET switch='DESMIA726MD' WHERE switch=='URDLIASD1KD'; -- #corrected, 515-822
UPDATE pooldata   SET switch='DVNPIAEASMD' WHERE switch=='DVNPIAEATDM'; -- #corrected?, 309-771
UPDATE pooldata   SET switch='ELDRPAXEPS0' WHERE switch=='ELDRPAXEPSO'; -- #letter confusion O/0 (50-44)
UPDATE pooldata   SET switch='ENWDCO47DS0' WHERE switch=='ENWDC047DS0'; -- #letter confusion O/0, 720-367'
UPDATE pooldata   SET switch='EPHRPAXLPS0' WHERE switch=='EPHRPAXLPSO'; -- #letter confusion O/0 (50-46)
UPDATE pooldata   SET switch='FDULSK04ET1' WHERE switch=='FDULSK04ET1'; -- #OK?, 306-686
UPDATE pooldata   SET switch='FTDDIAXCPOI' WHERE switch=='FTDDIAXCPOI'; -- FIXME: poor equip (27-43)
UPDATE pooldata   SET switch='FYVLNC01DS0' WHERE switch=='FYVLMC01DS0'; -- #corrected, 910-446'
UPDATE pooldata   SET switch='FYVLNCXD49F' WHERE switch=='FYVLNCXA49D'; -- #corrected, 910-927
UPDATE pooldata   SET switch='GSVLFLBSCM1' WHERE switch=='GSVLFLBASCM'; -- #letter insertion, 352-517
UPDATE pooldata   SET switch='HAYSKSLD1MD' WHERE switch=='HAYSKASLD1M' AND npa=='785' AND nxx=='513' AND x=='0'; -- #letter insertion
UPDATE pooldata   SET switch='HAYSKSLD1MD' WHERE switch=='HAYSKASLD1M' AND npa=='785' AND nxx=='626' AND x=='2'; -- #letter insertion
UPDATE pooldata   SET switch='HAYSKSLD1MD' WHERE switch=='HAYSKASLD1M' AND npa=='785' AND nxx=='626' AND x=='7'; -- #letter insertion
UPDATE pooldata   SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1'; -- #letter insertion, 575-416,575-909
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='217' AND x=='8'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='217' AND x=='9'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='270' AND x=='4'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='270' AND x=='5'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='270' AND x=='6'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='270' AND x=='8'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='404' AND x=='4'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='404' AND x=='7'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='404' AND x=='9'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='409' AND x=='5'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='409' AND x=='7'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='409' AND x=='9'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='449' AND x=='0'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='449' AND x=='4'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='449' AND x=='5'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='449' AND x=='6'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='449' AND x=='7'; -- #letter confusion
UPDATE pooldata   SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='449' AND x=='9'; -- #letter confusion
UPDATE pooldata   SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO' AND npa=='234' AND nxx=='284' AND x=='0'; -- #letter confusion O/0
UPDATE pooldata   SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO' AND npa=='234' AND nxx=='284' AND x=='6'; -- #letter confusion O/0
UPDATE pooldata   SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO'; -- #letter confusion O/0 (47-98)
UPDATE pooldata   SET switch='HNSNCO02PS0' WHERE switch=='DNVRCO26WBA'; -- #corrected?, 303-330
UPDATE pooldata   SET switch='HNVXALQUCM2' WHERE switch=='HNVXALUNOGT'; -- #fixed'
UPDATE pooldata   SET switch='HRLNTXHGX4X' WHERE switch=='HRLNTXHGIX4' AND npa=='956' AND nxx=='336' AND x=='0'; -- #letter insertion
UPDATE pooldata   SET switch='HRLNTXHGX4X' WHERE switch=='HRLNTXHGIX4' AND npa=='956' AND nxx=='336' AND x=='1'; -- #letter insertion
UPDATE pooldata   SET switch='HRLNTXHGX4X' WHERE switch=='HRLNTXHGIX4' AND npa=='956' AND nxx=='336' AND x=='2'; -- #letter insertion
UPDATE pooldata   SET switch='HSTNTXNAYLX' WHERE switch=='KATYTXXAJF1' AND npa=='713' AND nxx=='969' AND x=='0'; -- #corrected
UPDATE pooldata   SET switch='HZTNPAHZAMD' WHERE switch=='HZTNPAHZHVA'; -- #corrected, 570-501
UPDATE pooldata   SET switch='INDNIACODS1' WHERE switch=='INDNIABFELD'; -- ? (27-54)
UPDATE pooldata   SET switch='INDNIACODS1' WHERE switch=='INDNIABFELD' AND npa=='515' AND nxx=='962' AND x=='5'; -- #corrected
UPDATE pooldata   SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB' AND npa=='214' AND nxx=='785' AND x=='9'; -- ?DLLSTXSQDS0 (DLLSTXSQDS0?)
UPDATE pooldata   SET switch='JMTWNYXAPS0' WHERE switch=='JMTWNYXAPSO'; -- #letter confusion O/0 (44-71)
UPDATE pooldata   SET switch='KSCYMOMCDC0' WHERE switch=='KSCYMOMCDCO'; -- #letter confusion O/0, 816-817,816-949
UPDATE pooldata   SET switch='LNCSPALASMD' WHERE switch=='LNCSPALAHPC'; -- #corrected, 717-509
UPDATE pooldata   SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0' AND npa=='323' AND nxx=='366' AND x=='3'; -- #region mangled
UPDATE pooldata   SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0' AND npa=='323' AND nxx=='366' AND x=='4'; -- #region mangled
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='0'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='1'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='2'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='3'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='4'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='5'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='6'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='7'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='8'; -- #OK?
UPDATE pooldata   SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295' AND x=='9'; -- #OK?
UPDATE pooldata   SET switch='LTRKARTLPSX' WHERE switch=='LTRKARTLPSX' AND npa=='501' AND nxx=='287' AND x=='1'; -- #OK
UPDATE pooldata   SET switch='LWBGPAXLPS0' WHERE switch=='LWBGPAXLPSO'; -- #letter confusion O/0 (50-91)
UPDATE pooldata   SET switch='LXTNKYXAXUX' WHERE switch=='LXTNKYAXAUX'; -- #letter insertion, 606-655
UPDATE pooldata   SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS' AND npa=='478' AND nxx=='220' AND x=='4'; -- #corrected
UPDATE pooldata   SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- #letter mangling, 478-220
UPDATE pooldata   SET switch='MCCMMSBLAMB' WHERE switch=='MCCMMSBLAMB'; -- #OK?, 601-233
UPDATE pooldata   SET switch='MNCHNHCOXEZ' WHERE switch=='MNCHNHCOXE7' AND npa=='603' AND nxx=='757' AND x=='5'; -- #letter confusion 7/Z
UPDATE pooldata   SET switch='MNCHNHCOXEZ' WHERE switch=='MNCHNHCOXE7' AND npa=='603' AND nxx=='903' AND x=='0'; -- #letter confusion 7/Z
UPDATE pooldata   SET switch='MNCHNHCOXEZ' WHERE switch=='MNCHNHCOXE7' AND npa=='603' AND nxx=='903' AND x=='1'; -- #letter confusion 7/Z
UPDATE pooldata   SET switch='MNNTMN58CM0' WHERE switch=='STPLMNHXHCM'; -- #corrected, 952-244
UPDATE pooldata   SET switch='MOBLAL02XFX' WHERE switch=='MOBLAAL02XF'; -- #letter insertion, 251-583'
UPDATE pooldata   SET switch='MOBXAL02DS1' WHERE switch=='MOBXALVAUC1'; -- #fixed'
UPDATE pooldata   SET switch='MOBXAL02XFX' WHERE switch=='MOBXAAL02XF'; -- #fixed'
UPDATE pooldata   SET switch='MTGMALIR5MD' WHERE switch=='MTGMALMLMTX'; -- #corrected, 334-549
UPDATE pooldata   SET switch='MTOLMSABAND' WHERE switch=='MTOLMSABAND'; -- #OK?, 601-216
UPDATE pooldata   SET switch='NSVLTNMT56Z' WHERE switch=='NSVLTNMTH0S'; -- #corrected, 931-557
UPDATE pooldata   SET switch='NWCSDEBCCM2' WHERE switch=='NWCSDEBCGM2'; -- #letter reversal, 610-368
UPDATE pooldata   SET switch='NWMDWIXAPOI' WHERE switch=='NWMDWIXAPOI'; -- FIXME: poor equip (63-82)
UPDATE pooldata   SET switch='NWRKNJMDGMD' WHERE switch=='NWRKNJMDHC8' AND npa=='862' AND nxx=='944' AND x=='1'; -- #corrected
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='0'; -- #region mangled
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='1'; -- #region mangled
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='2'; -- #region mangled
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='3'; -- #region mangled
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='6'; -- #region mangled
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='7'; -- #region mangled
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='8'; -- #region mangled
UPDATE pooldata   SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737' AND x=='9'; -- #region mangled
UPDATE pooldata   SET switch='NYCMNYYJOMD' WHERE switch=='NYCMMYYJOMD' AND npa=='646' AND nxx=='503' AND x=='1'; -- #mangled region
UPDATE pooldata   SET switch='NYCMNYYJOMD' WHERE switch=='NYCMMYYJOMD' AND npa=='646' AND nxx=='570' AND x=='1'; -- #mangled region
UPDATE pooldata   SET switch='NYCMNYZRDFS' WHERE switch=='NYCMNYZRDFS' AND npa=='917' AND nxx=='268' AND x=='9'; -- #OK?
UPDATE pooldata   SET switch='OKBRILOAIMD' WHERE switch=='OKBRILOAIMG'; -- #letter mangling, 630-343
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='0'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='1'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='2'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='3'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='4'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='5'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='6'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='7'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='8'; -- #mangled region
UPDATE pooldata   SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881' AND x=='9'; -- #mangled region
UPDATE pooldata   SET switch='PHNXAZMACM6' WHERE switch=='PHNXAZMACCM' AND npa=='928' AND nxx=='363' AND x=='0'; -- #letter insertion
UPDATE pooldata   SET switch='PITBPAIWPS0' WHERE switch=='PITBPAIWPSO'; -- #letter confusion O/0, 724-702
UPDATE pooldata   SET switch='PLANTXYLCM0' WHERE switch=='PLANTXYCLM0'; -- #letter reversal, 214-898
UPDATE pooldata   SET switch='PTTWPAPT3MD' WHERE switch=='PTTWPAPTHPF'; -- #corrected, 484-941
UPDATE pooldata   SET switch='RCHLSC110MD' WHERE switch=='RDOVSCWLTP2'; -- #corrected, 803-493,803-517,803-984
UPDATE pooldata   SET switch='RCMTNCXAX5X' WHERE switch=='RCMTMCXAX5X'; -- #letter confusion M/N, 252-668'
UPDATE pooldata   SET switch='RCMTNCXAXVX' WHERE switch=='RCMTMCXAXVX'; -- #letter confusion M/N, 252-822'
UPDATE pooldata   SET switch='RCSNTXHVGT0' WHERE switch=='RCSNTHXVGT0'; -- #letter reversal, 469-202'
UPDATE pooldata   SET switch='RDNGPAREUMD' WHERE switch=='RDNGPAREHVA'; -- #corrected, 610-685
UPDATE pooldata   SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK' AND npa=='540' AND nxx=='466' AND x=='0'; -- #letter confusion K/X
UPDATE pooldata   SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK' AND npa=='540' AND nxx=='466' AND x=='2'; -- #letter confusion K/X
UPDATE pooldata   SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK' AND npa=='540' AND nxx=='466' AND x=='6'; -- #letter confusion K/X
UPDATE pooldata   SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK' AND npa=='540' AND nxx=='466' AND x=='9'; -- #letter confusion K/X
UPDATE pooldata   SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1'; -- #fixed (47-154)
UPDATE pooldata   SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT' AND npa=='916' AND nxx=='259' AND x=='4'; -- #letter confusion T/Y
UPDATE pooldata   SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- #letter confusion T/Y, 916-259
UPDATE pooldata   SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM' AND npa=='570' AND nxx=='233' AND x=='4'; -- #letter swap and confusion O/0
UPDATE pooldata   SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM' AND npa=='570' AND nxx=='233' AND x=='8'; -- #letter swap and confusion O/0
UPDATE pooldata   SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM' AND npa=='570' AND nxx=='233' AND x=='9'; -- #letter swap and confusion O/0
UPDATE pooldata   SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- #letter reversal/confusion, 570-233
UPDATE pooldata   SET switch='SCTNPASCCOM' WHERE switch=='SCTNPASCCOM' AND npa=='570' AND nxx=='579' AND x=='5'; -- FIXME: poor equip
UPDATE pooldata   SET switch='SCTNPASCCOM' WHERE switch=='SCTNPASCCOM' AND npa=='570' AND nxx=='751' AND x=='2'; -- FIXME: poor equip
UPDATE pooldata   SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P' AND npa=='760' AND nxx=='592' AND x=='5'; -- #corrected
UPDATE pooldata   SET switch='SNFCCAMMDSA' WHERE switch=='SNJSCAJRD0X'; -- #corrected, 707-243
UPDATE pooldata   SET switch='SNFCCAMMDSA' WHERE switch=='SNJSCAJRDX0'; -- #corrected, 707-294
UPDATE pooldata   SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1' AND npa=='509' AND nxx=='385' AND x=='0'; -- #corrected
UPDATE pooldata   SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- #corrected, 509-385
UPDATE pooldata   SET switch='SPNCIAMURL0' WHERE switch=='SPNCIAMURLO'; -- #letter confusion O/0 (27-3)
UPDATE pooldata   SET switch='STCDMNTOXRX' WHERE switch=='STCDMNOTOXR' AND npa=='320' AND nxx=='428' AND x=='0'; -- #letter insertion
UPDATE pooldata   SET switch='STCDMNTOXUX' WHERE switch=='STCDMITOXUX'; -- #fixed wrong state (35-131)
UPDATE pooldata   SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP' AND npa=='206' AND nxx=='420' AND x=='0'; -- #corrected
UPDATE pooldata   SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP'; -- #corrected, 206-420
UPDATE pooldata   SET switch='SXCTIAXO1MD' WHERE switch=='SXCTIZXO1MD'; -- #letter mangled (27-100)
UPDATE pooldata   SET switch='THTNGAXAPS0' WHERE switch=='THTNGAXAPSO'; -- #letter confusion O/0 (22-96)
UPDATE pooldata   SET switch='TULSOKTBXQX' WHERE switch=='TULSOKTBHH5'; -- #corrected, 918-728
UPDATE pooldata   SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- #letter substitution
UPDATE pooldata   SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- #letter substitution
UPDATE pooldata   SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT' AND npa=='724' AND nxx=='914' AND x=='6'; -- #OK
UPDATE pooldata   SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- #OK?, 724-914
UPDATE pooldata   SET switch='WCHSSCXARS0' WHERE switch=='SALDSCXARS0'; -- #corrected (53-69)
UPDATE pooldata   SET switch='WCHSSCXARS0' WHERE switch=='SALDSCXARS0'; -- #fixed wrong switch (53-69)
UPDATE pooldata   SET switch='WCHTKSBRXFX' WHERE switch=='WCHTSKSBRXF'; -- #letter reversal, 620-490
UPDATE pooldata   SET switch='WLPTPAWIYMD' WHERE switch=='WLPTPAWIHVA'; -- #corrected, 570-601
UPDATE pooldata   SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA'; -- #letter reversal, 267-475,267-992
UPDATE pooldata   SET switch='WTWPIN01RL0' WHERE switch=='WTWPIN01RLO'; -- #letter confusion O/0 (26-54)
UPDATE pooldata   SET switch='WWFDNYXAPS0' WHERE switch=='WWFDNYXAPSO'; -- #letter confusion O/0 (44-206)
UPDATE prefixdata SET switch='ALBYNY8ODS0' WHERE switch=='ALBYBYBODS0' AND npa=='518' AND nxx=='874'; -- #letter confusion B/N B/8
UPDATE prefixdata SET switch='ALCYALAYBMD' WHERE switch=='ALCYALAYBMX'; -- #letter mangling, 256-853
UPDATE prefixdata SET switch='AMRLTX02DS0' WHERE switch=='AMRLTX02CGO' AND npa=='806' AND nxx=='640'; -- #corrected
UPDATE prefixdata SET switch='AMRLTX9843Z' WHERE switch=='AMRLTX92WSM' AND npa=='806' AND nxx=='344'; -- #corrected
UPDATE prefixdata SET switch='AUSTTXZMCM0' WHERE switch=='AUSTUTXZMCM'; -- #letter insertion, 512-961
UPDATE prefixdata SET switch='BDTMOHIDCM0' WHERE switch=='BDTMMOHIDCM' AND npa=='330' AND nxx=='531'; -- #letter insertion
UPDATE prefixdata SET switch='BEREKYXAPS0' WHERE switch=='BEREKYXAPSO'; -- #letter confusion O/0 (29-4)
UPDATE prefixdata SET switch='BLTMMDCH13Z' WHERE switch=='BLTMMDCHI3Z' AND npa=='410' AND nxx=='800'; -- #corrected
UPDATE prefixdata SET switch='BLTMMDCH13Z' WHERE switch=='BLTMMDCHI3Z'; -- #letter confusion I/1, 410-800
UPDATE prefixdata SET switch='BLTMMDCHXOY' WHERE switch=='BLTMMDCHC01' AND npa=='443' AND nxx=='207'; -- #corrected
UPDATE prefixdata SET switch='BLTNILXDPS0' WHERE switch=='BLTNILXDPSO'; -- #letter confusion O/0 (25-13)
UPDATE prefixdata SET switch='BMBGPABLX7X' WHERE switch=='BMBGPABLHVA' AND npa=='570' AND nxx=='416'; -- #corrected
UPDATE prefixdata SET switch='BMBGPABLX7X' WHERE switch=='BMBGPABLHVA'; -- #corrected, 570-416
UPDATE prefixdata SET switch='BNTVARCRDS0' WHERE switch=='BNTVARCRGEL'; -- #corrected, 479-203,479-205
UPDATE prefixdata SET switch='BNTXARCRDS0' WHERE switch=='BNTXARCRGEL'; -- #fixed'
UPDATE prefixdata SET switch='BRHMALEW95E' WHERE switch=='BRHMALEWRPA' AND npa=='205' AND nxx=='951'; -- #corrected
UPDATE prefixdata SET switch='BRHMALEW95E' WHERE switch=='BRHMALEWRPA' AND npa=='205' AND nxx=='956'; -- #corrected
UPDATE prefixdata SET switch='BRHMALEW95E' WHERE switch=='BRHMALEWRPA' AND npa=='205' AND nxx=='957'; -- #corrected
UPDATE prefixdata SET switch='BRHMALEW95E' WHERE switch=='BRHMALEWRPA'; -- #fixed (12-8)
UPDATE prefixdata SET switch='BRHMALTA84E' WHERE switch=='BRHMALTARPA'; -- #fixed (12-15)
UPDATE prefixdata SET switch='BRHMALXBCM2' WHERE switch=='TSC LALBX0M' AND npa=='205' AND nxx=='246'; -- #corrected
UPDATE prefixdata SET switch='BRYNTXAYCM0' WHERE switch=='SANGTXXCCCM' AND npa=='979' AND nxx=='295'; -- #corrected
UPDATE prefixdata SET switch='BRYNTXAYCM0' WHERE switch=='SANGTXXCCCM' AND npa=='979' AND nxx=='322'; -- #corrected
UPDATE prefixdata SET switch='BRYNTXAYCM0' WHERE switch=='SANGTXXCCCM'; -- #corrected, 979-295,979-322
UPDATE prefixdata SET switch='BUMTTXTEWMD' WHERE switch=='BUMMTTXEWMD'; -- #letter insertion, 409-223'
UPDATE prefixdata SET switch='CHCGILCPDS5' WHERE switch=='CHICTGOZN10'; -- #rate center instead of clli
UPDATE prefixdata SET switch='CHRLNCVEBB0' WHERE switch=='CHRLNCVEBBO' AND npa=='704' AND nxx=='790'; -- #letter confusion
UPDATE prefixdata SET switch='CHTGTN100MD' WHERE switch=='CJTGTM100MD' AND npa=='423' AND nxx=='503'; -- #well mangled
UPDATE prefixdata SET switch='CHTNSCDTX4Y' WHERE switch=='CHTNXDCTX4Y'; -- #corrected, 843-606'
UPDATE prefixdata SET switch='CLMASCSNXGY' WHERE switch=='CLMASCSNXG4'; -- #letter confusion 4/Y
UPDATE prefixdata SET switch='CLMASCTSDC0' WHERE switch=='CLMASCTSDCO'; -- #letter confusion O/0 (53-23)
UPDATE prefixdata SET switch='CLMBOHIAX4X' WHERE switch=='CLMBOHIAWPY' AND npa=='614' AND nxx=='915'; -- #corrected
UPDATE prefixdata SET switch='CLSPCO53DS0' WHERE switch=='CLSPCP63DS0' AND npa=='719' AND nxx=='234'; -- #region mangling
UPDATE prefixdata SET switch='CLSQCO01DS0' WHERE switch=='CLSQC001DS0' AND npa=='719' AND nxx=='559'; -- #letter confusion
UPDATE prefixdata SET switch='CRVLTNAOGT1' WHERE switch=='MMMPHTNMACM'; -- #corrected?, 731-613'
UPDATE prefixdata SET switch='DCTRILBXCM8' WHERE switch=='DCTRUKBXCM8' AND npa=='217' AND nxx=='412'; -- #region mangled
UPDATE prefixdata SET switch='DESMIA726MD' WHERE switch=='URDLIASD1KD' AND npa=='515' AND nxx=='822'; -- #corrected, 515-822
UPDATE prefixdata SET switch='DVNPIAEASMD' WHERE switch=='DVNPIAEATDM' AND npa=='309' AND nxx=='771'; -- ?
UPDATE prefixdata SET switch='DVNPIAEASMD' WHERE switch=='DVNPIAEATDM'; -- #corrected?, 309-771
UPDATE prefixdata SET switch='ELDRPAXEPS0' WHERE switch=='ELDRPAXEPSO'; -- #letter confusion O/0 (50-44)
UPDATE prefixdata SET switch='ENWDCO47DS0' WHERE switch=='ENWDC047DS0'; -- #letter confusion O/0, 720-367'
UPDATE prefixdata SET switch='EPHRPAXLPS0' WHERE switch=='EPHRPAXLPSO'; -- #letter confusion O/0 (50-46)
UPDATE prefixdata SET switch='FDULSK04ET1' WHERE switch=='FDULSK04ET1' AND npa=='306' AND nxx=='686'; -- ?PRNASK01DS0
UPDATE prefixdata SET switch='FDULSK04ET1' WHERE switch=='FDULSK04ET1'; -- #OK? ?PRNASK01DS0, 306-686
UPDATE prefixdata SET switch='FRLKMNFLDS0' WHERE switch=='FRLKMNFRLDS' AND npa=='651' AND nxx=='466'; -- #letter insertion
UPDATE prefixdata SET switch='FTDDIAXCPOI' WHERE switch=='FTDDIAXCPOI'; -- FIXME: poor equip (27-43)
UPDATE prefixdata SET switch='FYVLNC01DS0' WHERE switch=='FYVLMC01DS0'; -- #corrected, 910-446'
UPDATE prefixdata SET switch='FYVLNCXD49F' WHERE switch=='FYVLNCXA49D'; -- #corrected, 910-927
UPDATE prefixdata SET switch='GAVLIDAJCM0' WHERE switch=='GAVILDAJCM0' AND npa=='208' AND nxx=='507'; -- #letter reversal
UPDATE prefixdata SET switch='GRVEOKMARL1' WHERE switch=='GRVEOKMARLC' AND npa=='918' AND nxx=='791'; -- #corrected
UPDATE prefixdata SET switch='GSVLFLBSCM1' WHERE switch=='GSVLFLBASCM' AND npa=='352' AND nxx=='517'; -- #corrected
UPDATE prefixdata SET switch='GSVLFLBSCM1' WHERE switch=='GSVLFLBASCM'; -- #letter insertion, 352-517
UPDATE prefixdata SET switch='HAYSKSLD1MD' WHERE switch=='HAYSKASLD1M' AND npa=='785' AND nxx=='513'; -- #letter insertion
UPDATE prefixdata SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1' AND npa=='575' AND nxx=='416'; -- #letter mangling
UPDATE prefixdata SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1' AND npa=='575' AND nxx=='909'; -- #letter mangling
UPDATE prefixdata SET switch='HBBSNMCRCM1' WHERE switch=='HBBSNMCREC1'; -- #letter insertion, 575-416,575-909
UPDATE prefixdata SET switch='HCKRNCXAX6X' WHERE switch=='HCKRNCXAX6C' AND npa=='828' AND nxx=='826'; -- #letter mangling
UPDATE prefixdata SET switch='HCLRNCXA2MD' WHERE switch=='HCLRMCXA2MD' AND npa=='828' AND nxx=='238'; -- #letter confusion
UPDATE prefixdata SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO' AND npa=='234' AND nxx=='284'; -- #letter confusion O/0
UPDATE prefixdata SET switch='HDSNOHXAPS0' WHERE switch=='HDSNOHXAPSO'; -- #letter confusion O/0 (47-98)
UPDATE prefixdata SET switch='HLNQSK01CTA' WHERE switch=='HLNQSK01CTA' AND npa=='306' AND nxx=='553'; -- #OK
UPDATE prefixdata SET switch='HLTPTXWICM0' WHERE switch=='SGLDTXAQWWM' AND npa=='281' AND nxx=='665'; -- #corrected
UPDATE prefixdata SET switch='HNSNCO02PS0' WHERE switch=='DNVRCO26WBA'; -- #corrected?, 303-330
UPDATE prefixdata SET switch='HNVIALQUCM2' WHERE switch==' HNVIALQUCM' AND npa=='256' AND nxx=='678'; -- #letter insertion
UPDATE prefixdata SET switch='HNVIALQUCM2' WHERE switch=='HNVIALUNOGT' AND npa=='256' AND nxx=='631'; -- #corrected
UPDATE prefixdata SET switch='HNVXALQUCM2' WHERE switch=='HNVXALUNOGT'; -- #fixed'
UPDATE prefixdata SET switch='HRLNTXHGX4X' WHERE switch=='HRLNTXHGIX4' AND npa=='956' AND nxx=='336'; -- #letter insertion
UPDATE prefixdata SET switch='HSTNTXNAYLX' WHERE switch=='KATYTXXAJF1' AND npa=='713' AND nxx=='969'; -- #corrected
UPDATE prefixdata SET switch='HWTHNYAFCM1' WHERE switch=='NYCMNYWSTL2' AND npa=='212' AND nxx=='470'; -- #corrected
UPDATE prefixdata SET switch='HZTNPAHZAMD' WHERE switch=='HZTNPAHZHVA'; -- #corrected, 570-501
UPDATE prefixdata SET switch='INDIPAASCM8' WHERE switch=='PITBPADTHVE' AND npa=='412' AND nxx=='340'; -- #corrected
UPDATE prefixdata SET switch='INDNIACODS1' WHERE switch=='INDNIABFELD'; -- ? (27-54)
UPDATE prefixdata SET switch='IRNGTXXBPSB' WHERE switch=='IRNGTXXBPSB' AND npa=='214' AND nxx=='785'; -- ?DLLSTXSQDS0 (DLLSTXSQDS0?)
UPDATE prefixdata SET switch='JMTWNYXAPS0' WHERE switch=='JMTWNYXAPSO'; -- #letter confusion O/0 (44-71)
UPDATE prefixdata SET switch='KSCYMOMCDC0' WHERE switch=='KSCYMOMCDCO'; -- #letter confusion O/0, 816-817,816-949
UPDATE prefixdata SET switch='LENYKSCJCM2' WHERE switch=='LENYHSCJCM2' AND npa=='913' AND nxx=='302'; -- #letter confusion
UPDATE prefixdata SET switch='LJNTCO26DS0' WHERE switch=='LJNTC026DS0' AND npa=='719' AND nxx=='363'; -- #letter confusion
UPDATE prefixdata SET switch='LNCSPALASMD' WHERE switch=='LNCSPALAHPC'; -- #corrected, 717-509
UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='251'; -- #corrected
UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='464'; -- #corrected
UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='527'; -- #corrected
UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='632'; -- #corrected
UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='794'; -- #corrected
UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='804'; -- #corrected
UPDATE prefixdata SET switch='LNNGMIMNXJY' WHERE switch=='LNNGMIMNZJY' AND npa=='517' AND nxx=='973'; -- #corrected
UPDATE prefixdata SET switch='LSANCAJWGT0' WHERE switch=='LSANDAJWGT0' AND npa=='323' AND nxx=='366'; -- #region mangled
UPDATE prefixdata SET switch='LSANCARCDDS' WHERE switch=='LSANCARCDDS' AND npa=='213' AND nxx=='295'; -- #OK?
UPDATE prefixdata SET switch='LSANCARCDSZ' WHERE switch=='LSANCARCOSZ' AND npa=='626' AND nxx=='370'; -- #letter confusion
UPDATE prefixdata SET switch='LSVLKYAPX0X' WHERE switch=='LSVLKYKYAPX' AND npa=='502' AND nxx=='236'; -- #letter insertions
UPDATE prefixdata SET switch='LSVLKYAPX0X' WHERE switch=='LSVLKYKYAPX' AND npa=='502' AND nxx=='237'; -- #letter insertions
UPDATE prefixdata SET switch='LSVLKYAPX0X' WHERE switch=='LSVLKYKYAPX' AND npa=='502' AND nxx=='317'; -- #letter insertions
UPDATE prefixdata SET switch='LSVLKYAPX0X' WHERE switch=='LSVLKYKYAPX' AND npa=='502' AND nxx=='334'; -- #letter insertions
UPDATE prefixdata SET switch='LSVLKYAPX0X' WHERE switch=='LSVLKYKYAPX' AND npa=='502' AND nxx=='462'; -- #letter insertions
UPDATE prefixdata SET switch='LTRKARTLPSX' WHERE switch=='LTRKARTLPSX' AND npa=='501' AND nxx=='287'; -- #OK
UPDATE prefixdata SET switch='LWBGPAXLPS0' WHERE switch=='LWBGPAXLPSO'; -- #letter confusion O/0 (50-91)
UPDATE prefixdata SET switch='LXTNKYXAXUX' WHERE switch=='LXTNKYAXAUX' AND npa=='606' AND nxx=='655'; -- #letter mangling
UPDATE prefixdata SET switch='LXTNKYXAXUX' WHERE switch=='LXTNKYAXAUX'; -- #letter insertion, 606-655
UPDATE prefixdata SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS' AND npa=='478' AND nxx=='220'; -- #corrected
UPDATE prefixdata SET switch='MACNGAMTXUX' WHERE switch=='MACNGAMTXUS'; -- #letter mangling, 478-220
UPDATE prefixdata SET switch='MCCMMSBLAMB' WHERE switch=='MCCMMSBLAMB' AND npa=='601' AND nxx=='233'; -- #OK
UPDATE prefixdata SET switch='MCCMMSBLAMB' WHERE switch=='MCCMMSBLAMB'; -- #OK?, 601-233
UPDATE prefixdata SET switch='MNCHNHCOXEZ' WHERE switch=='MNCHNHCOXE7' AND npa=='603' AND nxx=='903'; -- #letter confusion 7/Z
UPDATE prefixdata SET switch='MNNTMN58CM0' WHERE switch=='STPLMNHXHCM'; -- #corrected, 952-244
UPDATE prefixdata SET switch='MOBLAL02XFX' WHERE switch=='MOBLAAL02XF' AND npa=='251' AND nxx=='583'; -- #corrected
UPDATE prefixdata SET switch='MOBLAL02XFX' WHERE switch=='MOBLAAL02XF'; -- #letter insertion, 251-583'
UPDATE prefixdata SET switch='MOBXAL02DS1' WHERE switch=='MOBXALVAUC1'; -- #fixed'
UPDATE prefixdata SET switch='MOBXAL02XFX' WHERE switch=='MOBXAAL02XF'; -- #fixed'
UPDATE prefixdata SET switch='MPLSMNCD1KD' WHERE switch=='MPLSMNCD1KM' AND npa=='763' AND nxx=='286'; -- #letter mangled
UPDATE prefixdata SET switch='MPLSMNDT3IB' WHERE switch=='MPLSMNDT3IB' AND npa=='701' AND nxx=='555'; -- #should be deleted
UPDATE prefixdata SET switch='MRSHIL020MD' WHERE switch=='DANVILT1AXA' AND npa=='217' AND nxx=='822'; -- #corrected
UPDATE prefixdata SET switch='MSCWIDDL0MD' WHERE switch=='MSCW1DDL0MD' AND npa=='208' AND nxx=='669'; -- #letter confusion I/1
UPDATE prefixdata SET switch='MTGMAL13X1X' WHERE switch=='MTGMALMTW6C' AND npa=='334' AND nxx=='744'; -- #corrected
UPDATE prefixdata SET switch='MTGMAL13X1X' WHERE switch=='MTGMALMTW6C' AND npa=='334' AND nxx=='755'; -- #corrected
UPDATE prefixdata SET switch='MTGMAL13X1X' WHERE switch=='MTGMALMTW6C' AND npa=='334' AND nxx=='757'; -- #corrected
UPDATE prefixdata SET switch='MTGMAL13X1X' WHERE switch=='MTGMALMTW6C' AND npa=='334' AND nxx=='759'; -- #corrected
UPDATE prefixdata SET switch='MTGMALIR5MD' WHERE switch=='MTGMALMLMTX'; -- #corrected, 334-549
UPDATE prefixdata SET switch='MTOLMSABAND' WHERE switch=='MTOLMSABAND' AND npa=='601' AND nxx=='216'; -- #OK
UPDATE prefixdata SET switch='MTOLMSABAND' WHERE switch=='MTOLMSABAND'; -- #OK?, 601-216
UPDATE prefixdata SET switch='NSVLTNMT56Z' WHERE switch=='NSVLTNMTH0S'; -- #corrected, 931-557
UPDATE prefixdata SET switch='NWCSDEBCCM2' WHERE switch=='NWCSDEBCGM2'; -- #letter reversal, 610-368
UPDATE prefixdata SET switch='NWCSDEBGCM2' WHERE switch=='NWCSDEBCGM2' AND npa=='610' AND nxx=='368'; -- #letter reversal
UPDATE prefixdata SET switch='NWMDWIXAPOI' WHERE switch=='NWMDWIXAPOI'; -- FIXME: poor equip (63-82)
UPDATE prefixdata SET switch='NWRKNJ41W10' WHERE switch=='NWRKNJ4IWI0' AND npa=='908' AND nxx=='235'; -- #letter confusion
UPDATE prefixdata SET switch='NWRKNJMDGMD' WHERE switch=='NWRKNJMDHC8' AND npa=='862' AND nxx=='944'; -- #corrected
UPDATE prefixdata SET switch='NWTPMOABCM8' WHERE switch=='NWTPMPABCM8' AND npa=='314' AND nxx=='737'; -- #region mangled
UPDATE prefixdata SET switch='NYCMNYYJOMD' WHERE switch=='NYCMMYYJOMD' AND npa=='646' AND nxx=='503'; -- #mangled region
UPDATE prefixdata SET switch='NYCMNYYJOMD' WHERE switch=='NYCMMYYJOMD' AND npa=='646' AND nxx=='570'; -- #mangled region
UPDATE prefixdata SET switch='NYCMNYZRDFS' WHERE switch=='NYCMNYZRDFS' AND npa=='917' AND nxx=='268'; -- #OK?
UPDATE prefixdata SET switch='OKBRILOAIMD' WHERE switch=='OKBRILOAIMG'; -- #letter mangling, 630-343
UPDATE prefixdata SET switch='OKCYOKCEX8X' WHERE switch=='OKCYPKCEX8X' AND npa=='405' AND nxx=='881'; -- #mangled region
UPDATE prefixdata SET switch='PHLAPAFGW29' WHERE switch=='PHLAPAFGSW2' AND npa=='484' AND nxx=='261'; -- #letter insertion
UPDATE prefixdata SET switch='PHLAPAFGW29' WHERE switch=='PHLAPAFGWS2' AND npa=='484' AND nxx=='715'; -- #letter insertion
UPDATE prefixdata SET switch='PHNXAZMACM6' WHERE switch=='PHNXAZMACCM' AND npa=='928' AND nxx=='363'; -- #letter insertion
UPDATE prefixdata SET switch='PITBPAIWPS0' WHERE switch=='PITBPAIWPSO'; -- #letter confusion O/0, 724-702
UPDATE prefixdata SET switch='PLANTXYLCM0' WHERE switch=='PLANTXYCLM0' AND npa=='214' AND nxx=='898'; -- #letter reversal
UPDATE prefixdata SET switch='PLANTXYLCM0' WHERE switch=='PLANTXYCLM0'; -- #letter reversal, 214-898
UPDATE prefixdata SET switch='PRTGMI01DS0' WHERE switch=='PRTGM101DS0' AND npa=='616' AND nxx=='855'; -- #letter confusion
UPDATE prefixdata SET switch='PRVSMSAMAMD' WHERE switch=='PRVSMSAMAND' AND npa=='601' AND nxx=='464'; -- #letter confusion N/M
UPDATE prefixdata SET switch='PTTWPAPT3MD' WHERE switch=='PTTWPAPTHPF' AND npa=='484' AND nxx=='941'; -- #corrected
UPDATE prefixdata SET switch='PTTWPAPT3MD' WHERE switch=='PTTWPAPTHPF'; -- #corrected, 484-941
UPDATE prefixdata SET switch='RCHLSC110MD' WHERE switch=='RDOVSCWLTP2'; -- #corrected, 803-493,803-517,803-984
UPDATE prefixdata SET switch='RCMTNCXAX5X' WHERE switch=='RCMTMCXAX5X'; -- #letter confusion M/N, 252-668'
UPDATE prefixdata SET switch='RCMTNCXAXVX' WHERE switch=='RCMTMCXAXVX'; -- #letter confusion M/N, 252-822'
UPDATE prefixdata SET switch='RCSNTXHVGT0' WHERE switch=='RCSNTHXVGT0'; -- #letter reversal, 469-202'
UPDATE prefixdata SET switch='RDNGPAREUMD' WHERE switch=='RDNGPAREHVA'; -- #corrected, 610-685
UPDATE prefixdata SET switch='RONKVALKXRX' WHERE switch=='RONKVALKXRK' AND npa=='540' AND nxx=='466'; -- #letter confusion K/X
UPDATE prefixdata SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1' AND npa=='513' AND nxx=='229'; -- #corrected
UPDATE prefixdata SET switch='RSMYOHRODS0' WHERE switch=='RSMYOHROWV1'; -- #fixed (47-154)
UPDATE prefixdata SET switch='RSVTUTANCM0' WHERE switch=='RSVTUTTANCM' AND npa=='435' AND nxx=='364'; -- #letter insertion
UPDATE prefixdata SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT' AND npa=='916' AND nxx=='259'; -- #letter confusion T/Y
UPDATE prefixdata SET switch='SCRMCA01XPY' WHERE switch=='SCRMCA01XPT'; -- #letter confusion T/Y, 916-259
UPDATE prefixdata SET switch='SCRMCAAMDS0' WHERE switch=='SCRMCAAM0S0' AND npa=='916' AND nxx=='493'; -- #letter confusion 0/D
UPDATE prefixdata SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM' AND npa=='570' AND nxx=='233'; -- #letter swap and confusion O/0
UPDATE prefixdata SET switch='SCTNPASCCM0' WHERE switch=='SCTNPASCCOM'; -- #letter reversal/confusion, 570-233
UPDATE prefixdata SET switch='SFLDMICTCM2' WHERE switch=='SFLDMIUCTCM' AND npa=='734' AND nxx=='473'; -- #letter insertion
UPDATE prefixdata SET switch='SKTNCA01XTY' WHERE switch=='SSKTNCA01XT' AND npa=='209' AND nxx=='553'; -- #letter insertion
UPDATE prefixdata SET switch='SNDGCA023KD' WHERE switch=='SNDGCA023DK' AND npa=='760' AND nxx=='825'; -- #letter reversal
UPDATE prefixdata SET switch='SNDGCA02XHZ' WHERE switch=='SNDGCA02W0P' AND npa=='760' AND nxx=='592'; -- #corrected
UPDATE prefixdata SET switch='SNFCCAMMDSA' WHERE switch=='SNJSCAJRD0X'; -- #corrected, 707-243
UPDATE prefixdata SET switch='SNFCCAMMDSA' WHERE switch=='SNJSCAJRDX0'; -- #corrected, 707-294
UPDATE prefixdata SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1' AND npa=='509' AND nxx=='385'; -- #corrected
UPDATE prefixdata SET switch='SPKNWAOBX8X' WHERE switch=='SPKNWAOBWT1'; -- #corrected, 509-385
UPDATE prefixdata SET switch='SPNCIAMURL0' WHERE switch=='SPNCIAMURLO' AND npa=='515' AND nxx=='395'; -- #letter confusion O/0
UPDATE prefixdata SET switch='SPNCIAMURL0' WHERE switch=='SPNCIAMURLO'; -- #letter confusion O/0 (27-3)
UPDATE prefixdata SET switch='STCDMNTOXRX' WHERE switch=='STCDMNOTOXR' AND npa=='320' AND nxx=='428'; -- #letter insertion
UPDATE prefixdata SET switch='STCDMNTOXUX' WHERE switch=='STCDMITOXUX'; -- #fixed wrong state (35-131)
UPDATE prefixdata SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP' AND npa=='206' AND nxx=='420'; -- #corrected
UPDATE prefixdata SET switch='STTLWAWBX6Z' WHERE switch=='STTLWAWBWTP'; -- #corrected, 206-420
UPDATE prefixdata SET switch='SVNHGABSIMD' WHERE switch=='SVNGHABSIMD' AND npa=='912' AND nxx=='596'; -- #letter reversal
UPDATE prefixdata SET switch='SXCTIAXO1MD' WHERE switch=='SXCTIZXO1MD'; -- #letter mangled (27-100)
UPDATE prefixdata SET switch='TACMWA43CM2' WHERE switch=='TACMWA43XM2' AND npa=='253' AND nxx=='677'; -- #letter substitution
UPDATE prefixdata SET switch='THTNGAXAPS0' WHERE switch=='THTNGAXAPSO'; -- #letter confusion O/0 (22-96)
UPDATE prefixdata SET switch='TULSOKTBXQX' WHERE switch=='TULSOKTBHH5'; -- #corrected, 918-728
UPDATE prefixdata SET switch='TXRKTXAD1MD' WHERE switch=='TXRKRXAD1MD' AND npa=='903' AND nxx=='219'; -- #mangled region
UPDATE prefixdata SET switch='ULYSKS03CM0' WHERE switch=='ULYSHS03CM0' AND npa=='620' AND nxx=='934'; -- #letter confusion
UPDATE prefixdata SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- #letter subsitution, 515-822
UPDATE prefixdata SET switch='URDLIASD1KD' WHERE switch=='URDLIASDIKD'; -- #letter substitution
UPDATE prefixdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT' AND npa=='724' AND nxx=='914'; -- #OK
UPDATE prefixdata SET switch='WASHPAWAHPT' WHERE switch=='WASHPAWAHPT'; -- #OK?, 724-914
UPDATE prefixdata SET switch='WCHSSCXARS0' WHERE switch=='SALDSCXARS0'; -- #corrected (53-69)
UPDATE prefixdata SET switch='WCHSSCXARS0' WHERE switch=='SALDSCXARS0'; -- #fixed wrong switch (53-69)
UPDATE prefixdata SET switch='WCHTKSBRXFX' WHERE switch=='WCHTSKSBRXF' AND npa=='620' AND nxx=='490'; -- #letter insertion
UPDATE prefixdata SET switch='WCHTKSBRXFX' WHERE switch=='WCHTSKSBRXF'; -- #letter reversal, 620-490
UPDATE prefixdata SET switch='WEBBSK02CTA' WHERE switch=='WEBBSK02CTA' AND npa=='306' AND nxx=='674'; -- #OK
UPDATE prefixdata SET switch='WHPLNYWPXOY' WHERE switch=='NYCMNYWSTL2' AND npa=='917' AND nxx=='310'; -- #corrected??
UPDATE prefixdata SET switch='WLMRMN03F00' WHERE switch=='WLMRMN03F00' AND npa=='320' AND nxx=='222'; -- #OK
UPDATE prefixdata SET switch='WLPTPAWIYMD' WHERE switch=='WLPTPAWIHVA'; -- #corrected, 570-601
UPDATE prefixdata SET switch='WNDSCODCCM0' WHERE switch=='WNDSCOMAZ01' AND npa=='720' AND nxx=='658'; -- #rate center confusion
UPDATE prefixdata SET switch='WNHNFLBUCM1' WHERE switch=='WNHNFLBUVM1' AND npa=='941' AND nxx=='809'; -- #letter mangled
UPDATE prefixdata SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA' AND npa=='267' AND nxx=='475'; -- #letter reversal
UPDATE prefixdata SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA' AND npa=='267' AND nxx=='992'; -- #letter reversal
UPDATE prefixdata SET switch='WNRTPAAHCMA' WHERE switch=='WNRTPAACHMA'; -- #letter reversal, 267-475,267-992
UPDATE prefixdata SET switch='WTWPIN01RL0' WHERE switch=='WTWPIN01RLO' AND npa=='765' AND nxx=='445'; -- #letter confusion
UPDATE prefixdata SET switch='WTWPIN01RL0' WHERE switch=='WTWPIN01RLO'; -- #letter confusion O/0 (26-54)
UPDATE prefixdata SET switch='WVCYUTCL1KD' WHERE switch=='WVCYUTCK1JD' AND npa=='801' AND nxx=='630'; -- #letter mangling
UPDATE prefixdata SET switch='WWFDNYXAPS0' WHERE switch=='WWFDNYXAPSO'; -- #letter confusion O/0 (44-206)
UPDATE switchdata SET switch='ALCYALAYBMD',territory='AL' WHERE switch=='ALCYALAYBMX'; -- #letter mangling, 256-853
UPDATE switchdata SET switch='AUSTTXZMCM0',territory='TX' WHERE switch=='AUSTUTXZMCM'; -- #letter insertion, 512-961
UPDATE switchdata SET switch='BEREKYXAPS0',territory='KY' WHERE switch=='BEREKYXAPSO'; -- #letter confusion O/0 (29-4)
UPDATE switchdata SET switch='BLTMMDCH13Z',territory='MD' WHERE switch=='BLTMMDCHI3Z'; -- #letter confusion I/1, 410-800
UPDATE switchdata SET switch='BLTNILXDPS0',territory='IL' WHERE switch=='BLTNILXDPSO'; -- #letter confusion O/0 (25-13)
UPDATE switchdata SET switch='BMBGPABLX7X',territory='PA' WHERE switch=='BMBGPABLHVA'; -- #corrected, 570-416
UPDATE switchdata SET switch='BNTVARCRDS0',territory='AR' WHERE switch=='BNTVARCRGEL'; -- #corrected, 479-203,479-205
UPDATE switchdata SET switch='BNTXARCRDS0',territory='AR' WHERE switch=='BNTXARCRGEL'; -- #fixed'
UPDATE switchdata SET switch='BRHMALEW95E',territory='AL' WHERE switch=='BRHMALEWRPA'; -- #fixed (12-8)
UPDATE switchdata SET switch='BRHMALTA84E',territory='AL' WHERE switch=='BRHMALTARPA'; -- #fixed (12-15)
UPDATE switchdata SET switch='BRYNTXAYCM0',territory='TX' WHERE switch=='SANGTXXCCCM'; -- #corrected, 979-295,979-322
UPDATE switchdata SET switch='BUMTTXTEWMD',territory='TX' WHERE switch=='BUMMTTXEWMD'; -- #letter insertion, 409-223'
UPDATE switchdata SET switch='CHCGILCPDS5',territory='IL' WHERE switch=='CHICTGOZN10'; -- #rate center instead of clli
UPDATE switchdata SET switch='CHTNSCDTX4Y',territory='SC' WHERE switch=='CHTNXDCTX4Y'; -- #corrected, 843-606'
UPDATE switchdata SET switch='CLMASCSNXGY',territory='SC' WHERE switch=='CLMASCSNXG4'; -- #letter confusion 4/Y
UPDATE switchdata SET switch='CLMASCTSDC0',territory='SC' WHERE switch=='CLMASCTSDCO'; -- #letter confusion O/0 (53-23)
UPDATE switchdata SET switch='CRVLTNAOGT1',territory='TN' WHERE switch=='MMMPHTNMACM'; -- #corrected?, 731-613'
UPDATE switchdata SET switch='DVNPIAEASMD',territory='IA' WHERE switch=='DVNPIAEATDM'; -- #corrected?, 309-771
UPDATE switchdata SET switch='ELDRPAXEPS0',territory='PA' WHERE switch=='ELDRPAXEPSO'; -- #letter confusion O/0 (50-44)
UPDATE switchdata SET switch='ENWDCO47DS0',territory='CO' WHERE switch=='ENWDC047DS0'; -- #letter confusion O/0, 720-367'
UPDATE switchdata SET switch='EPHRPAXLPS0',territory='PA' WHERE switch=='EPHRPAXLPSO'; -- #letter confusion O/0 (50-46)
UPDATE switchdata SET switch='FDULSK04ET1',territory='SK' WHERE switch=='FDULSK04ET1'; -- #OK?, 306-686
UPDATE switchdata SET switch='FTDDIAXCPOI',territory='IA' WHERE switch=='FTDDIAXCPOI'; -- FIXME: poor equip (27-43)
UPDATE switchdata SET switch='FYVLNC01DS0',territory='NC' WHERE switch=='FYVLMC01DS0'; -- #corrected, 910-446'
UPDATE switchdata SET switch='FYVLNCXD49F',territory='NC' WHERE switch=='FYVLNCXA49D'; -- #corrected, 910-927
UPDATE switchdata SET switch='GSVLFLBSCM1',territory='FL' WHERE switch=='GSVLFLBASCM'; -- #letter insertion, 352-517
UPDATE switchdata SET switch='HBBSNMCRCM1',territory='NM' WHERE switch=='HBBSNMCREC1'; -- #letter insertion, 575-416,575-909
UPDATE switchdata SET switch='HDSNOHXAPS0',territory='OH' WHERE switch=='HDSNOHXAPSO'; -- #letter confusion O/0 (47-98)
UPDATE switchdata SET switch='HNSNCO02PS0',territory='CO' WHERE switch=='DNVRCO26WBA'; -- #corrected?, 303-330
UPDATE switchdata SET switch='HNVXALQUCM2',territory='AL' WHERE switch=='HNVXALUNOGT'; -- #fixed'
UPDATE switchdata SET switch='HZTNPAHZAMD',territory='PA' WHERE switch=='HZTNPAHZHVA'; -- #corrected, 570-501
UPDATE switchdata SET switch='INDNIACODS1',territory='IA' WHERE switch=='INDNIABFELD'; -- ? (27-54)
UPDATE switchdata SET switch='JMTWNYXAPS0',territory='NY' WHERE switch=='JMTWNYXAPSO'; -- #letter confusion O/0 (44-71)
UPDATE switchdata SET switch='KSCYMOMCDC0',territory='MO' WHERE switch=='KSCYMOMCDCO'; -- #letter confusion O/0, 816-817,816-949
UPDATE switchdata SET switch='LNCSPALASMD',territory='PA' WHERE switch=='LNCSPALAHPC'; -- #corrected, 717-509
UPDATE switchdata SET switch='LWBGPAXLPS0',territory='PA' WHERE switch=='LWBGPAXLPSO'; -- #letter confusion O/0 (50-91)
UPDATE switchdata SET switch='LXTNKYXAXUX',territory='KY' WHERE switch=='LXTNKYAXAUX'; -- #letter insertion, 606-655
UPDATE switchdata SET switch='MACNGAMTXUX',territory='GA' WHERE switch=='MACNGAMTXUS'; -- #letter mangling, 478-220
UPDATE switchdata SET switch='MCCMMSBLAMB',territory='MS' WHERE switch=='MCCMMSBLAMB'; -- #OK?, 601-233
UPDATE switchdata SET switch='MNNTMN58CM0',territory='MN' WHERE switch=='STPLMNHXHCM'; -- #corrected, 952-244
UPDATE switchdata SET switch='MOBLAL02XFX',territory='AL' WHERE switch=='MOBLAAL02XF'; -- #letter insertion, 251-583'
UPDATE switchdata SET switch='MOBXAL02DS1',territory='AL' WHERE switch=='MOBXALVAUC1'; -- #fixed'
UPDATE switchdata SET switch='MOBXAL02XFX',territory='AL' WHERE switch=='MOBXAAL02XF'; -- #fixed'
UPDATE switchdata SET switch='MTGMALIR5MD',territory='AL' WHERE switch=='MTGMALMLMTX'; -- #corrected, 334-549
UPDATE switchdata SET switch='MTOLMSABAND',territory='MS' WHERE switch=='MTOLMSABAND'; -- #OK?, 601-216
UPDATE switchdata SET switch='NSVLTNMT56Z',territory='TN' WHERE switch=='NSVLTNMTH0S'; -- #corrected, 931-557
UPDATE switchdata SET switch='NWCSDEBCCM2',territory='DE' WHERE switch=='NWCSDEBCGM2'; -- #letter reversal, 610-368
UPDATE switchdata SET switch='NWMDWIXAPOI',territory='WI' WHERE switch=='NWMDWIXAPOI'; -- FIXME: poor equip (63-82)
UPDATE switchdata SET switch='OKBRILOAIMD',territory='IL' WHERE switch=='OKBRILOAIMG'; -- #letter mangling, 630-343
UPDATE switchdata SET switch='PITBPAIWPS0',territory='PA' WHERE switch=='PITBPAIWPSO'; -- #letter confusion O/0, 724-702
UPDATE switchdata SET switch='PLANTXYLCM0',territory='TX' WHERE switch=='PLANTXYCLM0'; -- #letter reversal, 214-898
UPDATE switchdata SET switch='PTTWPAPT3MD',territory='PA' WHERE switch=='PTTWPAPTHPF'; -- #corrected, 484-941
UPDATE switchdata SET switch='RCHLSC110MD',territory='SC' WHERE switch=='RDOVSCWLTP2'; -- #corrected, 803-493,803-517,803-984
UPDATE switchdata SET switch='RCMTNCXAX5X',territory='NC' WHERE switch=='RCMTMCXAX5X'; -- #letter confusion M/N, 252-668'
UPDATE switchdata SET switch='RCMTNCXAXVX',territory='NC' WHERE switch=='RCMTMCXAXVX'; -- #letter confusion M/N, 252-822'
UPDATE switchdata SET switch='RCSNTXHVGT0',territory='TX' WHERE switch=='RCSNTHXVGT0'; -- #letter reversal, 469-202'
UPDATE switchdata SET switch='RDNGPAREUMD',territory='PA' WHERE switch=='RDNGPAREHVA'; -- #corrected, 610-685
UPDATE switchdata SET switch='RSMYOHRODS0',territory='OH' WHERE switch=='RSMYOHROWV1'; -- #fixed (47-154)
UPDATE switchdata SET switch='SCRMCA01XPY',territory='CA' WHERE switch=='SCRMCA01XPT'; -- #letter confusion T/Y, 916-259
UPDATE switchdata SET switch='SCTNPASCCM0',territory='PA' WHERE switch=='SCTNPASCCOM'; -- #letter reversal/confusion, 570-233
UPDATE switchdata SET switch='SNFCCAMMDSA',territory='CA' WHERE switch=='SNJSCAJRD0X'; -- #corrected, 707-243
UPDATE switchdata SET switch='SNFCCAMMDSA',territory='CA' WHERE switch=='SNJSCAJRDX0'; -- #corrected, 707-294
UPDATE switchdata SET switch='SPKNWAOBX8X',territory='WA' WHERE switch=='SPKNWAOBWT1'; -- #corrected, 509-385
UPDATE switchdata SET switch='SPNCIAMURL0',territory='IA' WHERE switch=='SPNCIAMURLO'; -- #letter confusion O/0 (27-3)
UPDATE switchdata SET switch='STCDMNTOXUX',territory='MN' WHERE switch=='STCDMITOXUX'; -- #fixed wrong state (35-131)
UPDATE switchdata SET switch='STTLWAWBX6Z',territory='WA' WHERE switch=='STTLWAWBWTP'; -- #corrected, 206-420
UPDATE switchdata SET switch='SXCTIAXO1MD',territory='IA' WHERE switch=='SXCTIZXO1MD'; -- #letter mangled (27-100)
UPDATE switchdata SET switch='THTNGAXAPS0',territory='GA' WHERE switch=='THTNGAXAPSO'; -- #letter confusion O/0 (22-96)
UPDATE switchdata SET switch='TULSOKTBXQX',territory='OK' WHERE switch=='TULSOKTBHH5'; -- #corrected, 918-728
UPDATE switchdata SET switch='URDLIASD1KD',territory='IA' WHERE switch=='URDLIASDIKD'; -- #letter substitudion, 515-822
UPDATE switchdata SET switch='WASHPAWAHPT',territory='PA' WHERE switch=='WASHPAWAHPT'; -- #OK?, 724-914
UPDATE switchdata SET switch='WCHSSCXARS0',territory='SC' WHERE switch=='SALDSCXARS0'; -- #corrected (53-69)
UPDATE switchdata SET switch='WCHSSCXARS0',territory='SC' WHERE switch=='SALDSCXARS0'; -- #fixed wrong switch (53-69)
UPDATE switchdata SET switch='WCHTKSBRXFX',territory='KS' WHERE switch=='WCHTSKSBRXF'; -- #letter reversal, 620-490
UPDATE switchdata SET switch='WLPTPAWIYMD',territory='PA' WHERE switch=='WLPTPAWIHVA'; -- #corrected, 570-601
UPDATE switchdata SET switch='WNRTPAAHCMA',territory='PA' WHERE switch=='WNRTPAACHMA'; -- #letter reversal, 267-475,267-992
UPDATE switchdata SET switch='WTWPIN01RL0',territory='IN' WHERE switch=='WTWPIN01RLO'; -- #letter confusion O/0 (26-54)
UPDATE switchdata SET switch='WWFDNYXAPS0',territory='NY' WHERE switch=='WWFDNYXAPSO'; -- #letter confusion O/0 (44-206)

DELETE FROM switchdata WHERE switch=='ANDRNCXARS0'; -- #corrected (45-3)
DELETE FROM switchdata WHERE switch=='ARARNCXARS0'; -- #corrected (45-4)
DELETE FROM switchdata WHERE switch=='BREWARXARS0'; -- #corrected (15-6)
DELETE FROM switchdata WHERE switch=='HQTRNCXARS0'; -- #corrected (45-83)
DELETE FROM switchdata WHERE switch=='LMNTNCXARS0'; -- #corrected (45-105)
DELETE FROM switchdata WHERE switch=='PNGRNCXARS0'; -- #corrected (45-126)
DELETE FROM switchdata WHERE switch=='SLOMNCXARS0'; -- #corrected (45-153)
DELETE FROM switchdata WHERE switch=='STKNNCXARS0'; -- #corrected (45-158)
DELETE FROM switchdata WHERE switch=='TBAYARXARS0'; -- #corrected (15-57)
DELETE FROM switchdata WHERE switch=='TBAYARXARS0'; -- #delete erroneous record (15-57)
DELETE FROM switchdata WHERE switch=='WDRWARXARS0'; -- #corrected (15-60)
DELETE FROM switchdata WHERE switch=='WDRWARXARS0'; -- #delete erroneous record (15-60)
DELETE FROM switchdata WHERE switch=='WHCPNCXARS0'; -- #corrected (45-171)
UPDATE pooldata   SET switch=NULL WHERE switch=='ANDRNCXARS0'; -- #corrected (45-3)
UPDATE pooldata   SET switch=NULL WHERE switch=='ARARNCXARS0'; -- #corrected (45-4)
UPDATE pooldata   SET switch=NULL WHERE switch=='BREWARXARS0'; -- #corrected (15-6)
UPDATE pooldata   SET switch=NULL WHERE switch=='HQTRNCXARS0'; -- #corrected (45-83)
UPDATE pooldata   SET switch=NULL WHERE switch=='LMNTNCXARS0'; -- #corrected (45-105)
UPDATE pooldata   SET switch=NULL WHERE switch=='PNGRNCXARS0'; -- #corrected (45-126)
UPDATE pooldata   SET switch=NULL WHERE switch=='SLOMNCXARS0'; -- #corrected (45-153)
UPDATE pooldata   SET switch=NULL WHERE switch=='STKNNCXARS0'; -- #corrected (45-158)
UPDATE pooldata   SET switch=NULL WHERE switch=='TBAYARXARS0'; -- #corrected (15-57)
UPDATE pooldata   SET switch=NULL WHERE switch=='TBAYARXARS0'; -- #delete erroneous record (15-57)
UPDATE pooldata   SET switch=NULL WHERE switch=='WDRWARXARS0'; -- #corrected (15-60)
UPDATE pooldata   SET switch=NULL WHERE switch=='WDRWARXARS0'; -- #delete erroneous record (15-60)
UPDATE pooldata   SET switch=NULL WHERE switch=='WHCPNCXARS0'; -- #corrected (45-171)
UPDATE prefixdata SET switch=NULL WHERE switch=='ANDRNCXARS0'; -- #corrected (45-3)
UPDATE prefixdata SET switch=NULL WHERE switch=='ARARNCXARS0'; -- #corrected (45-4)
UPDATE prefixdata SET switch=NULL WHERE switch=='BREWARXARS0'; -- #corrected (15-6)
UPDATE prefixdata SET switch=NULL WHERE switch=='HQTRNCXARS0'; -- #corrected (45-83)
UPDATE prefixdata SET switch=NULL WHERE switch=='LMNTNCXARS0'; -- #corrected (45-105)
UPDATE prefixdata SET switch=NULL WHERE switch=='PNGRNCXARS0'; -- #corrected (45-126)
UPDATE prefixdata SET switch=NULL WHERE switch=='SLOMNCXARS0'; -- #corrected (45-153)
UPDATE prefixdata SET switch=NULL WHERE switch=='STKNNCXARS0'; -- #corrected (45-158)
UPDATE prefixdata SET switch=NULL WHERE switch=='TBAYARXARS0'; -- #corrected (15-57)
UPDATE prefixdata SET switch=NULL WHERE switch=='TBAYARXARS0'; -- #delete erroneous record (15-57)
UPDATE prefixdata SET switch=NULL WHERE switch=='WDRWARXARS0'; -- #corrected (15-60)
UPDATE prefixdata SET switch=NULL WHERE switch=='WDRWARXARS0'; -- #delete erroneous record (15-60)
UPDATE prefixdata SET switch=NULL WHERE switch=='WHCPNCXARS0'; -- #corrected (45-171)

-- UPDATE switchdata SET switch='ABLNTXOR1ST',territory='TX' WHERE switch=='ABLNTXOR1ST'; -- FIXME: poor equip (56-1)
-- UPDATE switchdata SET switch='ALTOTXXOHPI',territory='TX' WHERE switch=='ALTOTXXOHPI'; -- FIXME: poor equip (56-5)
-- UPDATE switchdata SET switch='APSPTXXPHPI',territory='TX' WHERE switch=='APSPTXXPHPI'; -- FIXME: poor equip (56-9)
-- UPDATE switchdata SET switch='ATLNGACSBC2',territory='GA' WHERE switch=='ATLNGACSBC2'; -- FIXME: poor equip (22-13)
-- UPDATE switchdata SET switch='BGTMMTXCHS0',territory='MT' WHERE switch=='BGTMMTXCHS0'; -- FIXME: poor equip (38-4)
-- UPDATE switchdata SET switch='BKSHTXXAHPI',territory='TX' WHERE switch=='BKSHTXXAHPI'; -- FIXME: poor equip (56-41)
-- UPDATE switchdata SET switch='BOISIDMAKXX',territory='ID' WHERE switch=='BOISIDMAKXX'; -- FIXME: poor equip (24-4)
-- UPDATE switchdata SET switch='BRHMALHWOGT',territory='AL' WHERE switch=='BRHMALHWOGT'; -- FIXME: poor equip (12-12)
-- UPDATE switchdata SET switch='BSMRNDBCFH5',territory='ND' WHERE switch=='BSMRNDBCFH5'; -- FIXME: poor equip (46-5)
-- UPDATE switchdata SET switch='CENTTXXCHPI',territory='TX' WHERE switch=='CENTTXXCHPI'; -- FIXME: poor equip (56-53)
-- UPDATE switchdata SET switch='CHCGILCLTDS',territory='IL' WHERE switch=='CHCGILCLTDS'; -- FIXME: poor equip (25-25)
-- UPDATE switchdata SET switch='CHRLMT02WOO',territory='MT' WHERE switch=='CHRLMT02WOO'; -- FIXME: poor equip (38-9)
-- UPDATE switchdata SET switch='CLDLNV09HBC',territory='NV' WHERE switch=='CLDLNV09HBC'; -- FIXME: poor equip (40-3)
-- UPDATE switchdata SET switch='CLMASCCHKXX',territory='SC' WHERE switch=='CLMASCCHKXX'; -- FIXME: poor equip (53-17)
-- UPDATE switchdata SET switch='CLMSSCARKXX',territory='SC' WHERE switch=='CLMSSCARKXX'; -- FIXME: poor equip (53-24)
-- UPDATE switchdata SET switch='CLSMPAXCRU1',territory='PA' WHERE switch=='CLSMPAXCRU1'; -- FIXME: poor equip (50-27)
-- UPDATE switchdata SET switch='CLSMPAXCRU2',territory='PA' WHERE switch=='CLSMPAXCRU2'; -- FIXME: poor equip (50-27)
-- UPDATE switchdata SET switch='CONRTXXAHPI',territory='TX' WHERE switch=='CONRTXXAHPI'; -- FIXME: poor equip (56-60)
-- UPDATE switchdata SET switch='CTSHTXXCHPI',territory='TX' WHERE switch=='CTSHTXXCHPI'; -- FIXME: poor equip (56-67)
-- UPDATE switchdata SET switch='DBLLTXXDHPI',territory='TX' WHERE switch=='DBLLTXXDHPI'; -- FIXME: poor equip (56-102)
-- UPDATE switchdata SET switch='DRBHFLMADMS',territory='FL' WHERE switch=='DRBHFLMADMS'; -- FIXME: poor equip (21-29)
-- UPDATE switchdata SET switch='EGVGILEGTDS',territory='IL' WHERE switch=='EGVGILEGTDS'; -- FIXME: poor equip (25-74)
-- UPDATE switchdata SET switch='ETOLTXXEHPI',territory='TX' WHERE switch=='ETOLTXXEHPI'; -- FIXME: poor equip (56-113)
-- UPDATE switchdata SET switch='FARGNDBCF2E',territory='ND' WHERE switch=='FARGNDBCF2E'; -- FIXME: poor equip (46-17)
-- UPDATE switchdata SET switch='FLLNNVCTHBC',territory='NV' WHERE switch=='FLLNNVCTHBC'; -- FIXME: poor equip (40-5)
-- UPDATE switchdata SET switch='FLLNNVXDHBC',territory='NV' WHERE switch=='FLLNNVXDHBC'; -- FIXME: poor equip (40-5)
-- UPDATE switchdata SET switch='FLSPTXXFHPI',territory='TX' WHERE switch=='FLSPTXXFHPI'; -- FIXME: poor equip (56-134)
-- UPDATE switchdata SET switch='FTLDFLCRDMS',territory='FL' WHERE switch=='FTLDFLCRDMS'; -- FIXME: poor equip (21-22)
-- UPDATE switchdata SET switch='FTLDFLCYDMS',territory='FL' WHERE switch=='FTLDFLCYDMS'; -- FIXME: poor equip (21-24)
-- UPDATE switchdata SET switch='FTLDFLMRDMS',territory='FL' WHERE switch=='FTLDFLMRDMS'; -- FIXME: poor equip (21-91)
-- UPDATE switchdata SET switch='FTLDFLOADMS',territory='FL' WHERE switch=='FTLDFLOADMS'; -- FIXME: poor equip (21-131)
-- UPDATE switchdata SET switch='FTLDFLPLDMS',territory='FL' WHERE switch=='FTLDFLPLDMS'; -- FIXME: poor equip (21-158)
-- UPDATE switchdata SET switch='FTLDFLPLG1W',territory='FL' WHERE switch=='FTLDFLPLG1W'; -- FIXME: poor equip (21-41)
-- UPDATE switchdata SET switch='GDRPMIBLH1C',territory='MI' WHERE switch=='GDRPMIBLH1C'; -- FIXME: poor equip (34-55)
-- UPDATE switchdata SET switch='GNLDTXXGHPI',territory='TX' WHERE switch=='GNLDTXXGHPI'; -- FIXME: poor equip (56-142)
-- UPDATE switchdata SET switch='HAVRMTXCHS0',territory='MT' WHERE switch=='HAVRMTXCHS0'; -- FIXME: poor equip (38-23)
-- UPDATE switchdata SET switch='HDSNTXXHHPI',territory='TX' WHERE switch=='HDSNTXXHHPI'; -- FIXME: poor equip (56-184)
-- UPDATE switchdata SET switch='HLWDFLHADMS',territory='FL' WHERE switch=='HLWDFLHADMS'; -- FIXME: poor equip (21-58)
-- UPDATE switchdata SET switch='HLWDFLWHDMS',territory='FL' WHERE switch=='HLWDFLWHDMS'; -- FIXME: poor equip (21-208)
-- UPDATE switchdata SET switch='HNDLILHNTDS',territory='IL' WHERE switch=='HNDLILHNTDS'; -- FIXME: poor equip (25-95)
-- UPDATE switchdata SET switch='IDFLIDMAKXX',territory='ID' WHERE switch=='IDFLIDMAKXX'; -- FIXME: poor equip (24-15)
-- UPDATE switchdata SET switch='JCVLFLARDMS',territory='FL' WHERE switch=='JCVLFLARDMS'; -- FIXME: poor equip (21-4)
-- UPDATE switchdata SET switch='JCVLFLCLDMS',territory='FL' WHERE switch=='JCVLFLCLDMS'; -- FIXME: poor equip (21-18)
-- UPDATE switchdata SET switch='JCVLFLFCDMS',territory='FL' WHERE switch=='JCVLFLFCDMS'; -- FIXME: poor equip (21-36)
-- UPDATE switchdata SET switch='JCVLFLMSDMS',territory='FL' WHERE switch=='JCVLFLMSDMS'; -- FIXME: poor equip (21-73)
-- UPDATE switchdata SET switch='JCVLFLRVDMS',territory='FL' WHERE switch=='JCVLFLRVDMS'; -- FIXME: poor equip (21-165)
-- UPDATE switchdata SET switch='JCVLFLSJDMS',territory='FL' WHERE switch=='JCVLFLSJDMS'; -- FIXME: poor equip (21-171)
-- UPDATE switchdata SET switch='JHCYTNXAHS0',territory='TN' WHERE switch=='JHCYTNXAHS0'; -- FIXME: poor equip (55-48)
-- UPDATE switchdata SET switch='JNCYKSXCHS0',territory='KS' WHERE switch=='JNCYKSXCHS0'; -- FIXME: poor equip (28-29)
-- UPDATE switchdata SET switch='KATYTXXAHPI',territory='TX' WHERE switch=='KATYTXXAHPI'; -- FIXME: poor equip (56-196)
-- UPDATE switchdata SET switch='KATYTXXBHPI',territory='TX' WHERE switch=='KATYTXXBHPI'; -- FIXME: poor equip (56-197)
-- UPDATE switchdata SET switch='KATYTXXCHPI',territory='TX' WHERE switch=='KATYTXXCHPI'; -- FIXME: poor equip (56-196)
-- UPDATE switchdata SET switch='KGFSOKXAO1W',territory='OK' WHERE switch=='KGFSOKXAO1W'; -- FIXME: poor equip (48-36)
-- UPDATE switchdata SET switch='LFKNTXXAHPI',territory='TX' WHERE switch=='LFKNTXXAHPI'; -- FIXME: poor equip (56-220)
-- UPDATE switchdata SET switch='LKCFTXXLHIP',territory='TX' WHERE switch=='LKCFTXXLHIP'; -- FIXME: poor equip (56-205)
-- UPDATE switchdata SET switch='LKPTUTBHOOT',territory='UT' WHERE switch=='LKPTUTBHOOT'; -- FIXME: poor equip (58-12)
-- UPDATE switchdata SET switch='LNNGMIMNH0A',territory='MI' WHERE switch=='LNNGMIMNH0A'; -- FIXME: poor equip (34-83)
-- UPDATE switchdata SET switch='LSVGNV03HBC',territory='NV' WHERE switch=='LSVGNV03HBC'; -- FIXME: poor equip (40-10)
-- UPDATE switchdata SET switch='LTRKARCAHB1',territory='AR' WHERE switch=='LTRKARCAHB1'; -- FIXME: poor equip (15-33)
-- UPDATE switchdata SET switch='LTRKARFRHD1',territory='AR' WHERE switch=='LTRKARFRHD1'; -- FIXME: poor equip (15-34)
-- UPDATE switchdata SET switch='LTRKARMOHB1',territory='AR' WHERE switch=='LTRKARMOHB1'; -- FIXME: poor equip (15-36)
-- UPDATE switchdata SET switch='MEXCMOJUHC1',territory='MO' WHERE switch=='MEXCMOJUHC1'; -- FIXME: poor equip (37-67)
-- UPDATE switchdata SET switch='MIAMFLAEDMS',territory='FL' WHERE switch=='MIAMFLAEDMS'; -- FIXME: poor equip (21-1)
-- UPDATE switchdata SET switch='MIAMFLBADMS',territory='FL' WHERE switch=='MIAMFLBADMS'; -- FIXME: poor equip (21-6)
-- UPDATE switchdata SET switch='MIAMFLBRDMS',territory='FL' WHERE switch=='MIAMFLBRDMS'; -- FIXME: poor equip (21-6)
-- UPDATE switchdata SET switch='MIAMFLCADMS',territory='FL' WHERE switch=='MIAMFLCADMS'; -- FIXME: poor equip (21-14)
-- UPDATE switchdata SET switch='MIAMFLGRDMS',territory='FL' WHERE switch=='MIAMFLGRDMS'; -- FIXME: poor equip (21-56)
-- UPDATE switchdata SET switch='MIAMFLHLDMS',territory='FL' WHERE switch=='MIAMFLHLDMS'; -- FIXME: poor equip (21-59)
-- UPDATE switchdata SET switch='MIAMFLICDMS',territory='FL' WHERE switch=='MIAMFLICDMS'; -- FIXME: poor equip (21-64)
-- UPDATE switchdata SET switch='MIAMFLNMDMS',territory='FL' WHERE switch=='MIAMFLNMDMS'; -- FIXME: poor equip (21-130)
-- UPDATE switchdata SET switch='MIAMFLPLDMS',territory='FL' WHERE switch=='MIAMFLPLDMS'; -- FIXME: poor equip (21-150)
-- UPDATE switchdata SET switch='MIAMFLRRDMS',territory='FL' WHERE switch=='MIAMFLRRDMS'; -- FIXME: poor equip (21-165)
-- UPDATE switchdata SET switch='MIAMFLSODMS',territory='FL' WHERE switch=='MIAMFLSODMS'; -- FIXME: poor equip (21-176)
-- UPDATE switchdata SET switch='MIAMFLWMDMS',territory='FL' WHERE switch=='MIAMFLWMDMS'; -- FIXME: poor equip (21-208)
-- UPDATE switchdata SET switch='MONRNYXAJUD',territory='NY' WHERE switch=='MONRNYXAJUD'; -- FIXME: poor equip (44-89)
-- UPDATE switchdata SET switch='MPLSMNDTKDZ',territory='MN' WHERE switch=='MPLSMNDTKDZ'; -- FIXME: poor equip (35-93)
-- UPDATE switchdata SET switch='MTGMTXXMHPI',territory='TX' WHERE switch=='MTGMTXXMHPI'; -- FIXME: poor equip (56-234)
-- UPDATE switchdata SET switch='NBRKILNTTDS',territory='IL' WHERE switch=='NBRKILNTTDS'; -- FIXME: poor equip (25-127)
-- UPDATE switchdata SET switch='NDADFLACDMS',territory='FL' WHERE switch=='NDADFLACDMS'; -- FIXME: poor equip (21-3)
-- UPDATE switchdata SET switch='NDADFLBRDMS',territory='FL' WHERE switch=='NDADFLBRDMS'; -- FIXME: poor equip (21-12)
-- UPDATE switchdata SET switch='ORLDFLAPDMS',territory='FL' WHERE switch=='ORLDFLAPDMS'; -- FIXME: poor equip (21-5)
-- UPDATE switchdata SET switch='ORLDFLCLDMS',territory='FL' WHERE switch=='ORLDFLCLDMS'; -- FIXME: poor equip (21-22)
-- UPDATE switchdata SET switch='ORLDFLMADMS',territory='FL' WHERE switch=='ORLDFLMADMS'; -- FIXME: poor equip (21-140)
-- UPDATE switchdata SET switch='ORLDFLPCDMS',territory='FL' WHERE switch=='ORLDFLPCDMS'; -- FIXME: poor equip (21-157)
-- UPDATE switchdata SET switch='ORLDFLPHDMS',territory='FL' WHERE switch=='ORLDFLPHDMS'; -- FIXME: poor equip (21-158)
-- UPDATE switchdata SET switch='ORLDFLSADMS',territory='FL' WHERE switch=='ORLDFLSADMS'; -- FIXME: poor equip (21-171)
-- UPDATE switchdata SET switch='PCTLIDMAKXX',territory='ID' WHERE switch=='PCTLIDMAKXX'; -- FIXME: poor equip (24-24)
-- UPDATE switchdata SET switch='PEORILPJTDS',territory='IL' WHERE switch=='PEORILPJTDS'; -- FIXME: poor equip (25-137)
-- UPDATE switchdata SET switch='PHLAPAFG5ES',territory='PA' WHERE switch=='PHLAPAFG5ES'; -- FIXME: poor equip (19-3)
-- UPDATE switchdata SET switch='PHNXAZNO0W1',territory='AZ' WHERE switch=='PHNXAZNO0W1'; -- FIXME: poor equip (14-42)
-- UPDATE switchdata SET switch='PMBHFLFEDMS',territory='FL' WHERE switch=='PMBHFLFEDMS'; -- FIXME: poor equip (21-162)
-- UPDATE switchdata SET switch='PMBHFLMADMS',territory='FL' WHERE switch=='PMBHFLMADMS'; -- FIXME: poor equip (21-95)
-- UPDATE switchdata SET switch='POMTNV10HBC',territory='NV' WHERE switch=='POMTNV10HBC'; -- FIXME: poor equip (40-17)
-- UPDATE switchdata SET switch='PRLKKYAAJ00',territory='KY' WHERE switch=='PRLKKYAAJ00'; -- FIXME: poor equip (29-55)
-- UPDATE switchdata SET switch='PSVLNJPL2GN',territory='NJ' WHERE switch=='PSVLNJPL2GN'; -- FIXME: poor equip (42-63)
-- UPDATE switchdata SET switch='PTLDORPBWID',territory='OR' WHERE switch=='PTLDORPBWID'; -- FIXME: poor equip (49-45)
-- UPDATE switchdata SET switch='RCFRILRTTDS',territory='IL' WHERE switch=='RCFRILRTTDS'; -- FIXME: poor equip (25-147)
-- UPDATE switchdata SET switch='RCISILRIKA1',territory='IL' WHERE switch=='RCISILRIKA1'; -- FIXME: poor equip (25-146)
-- UPDATE switchdata SET switch='RVBKTXXRHPI',territory='TX' WHERE switch=='RVBKTXXRHPI'; -- FIXME: poor equip (56-267)
-- UPDATE switchdata SET switch='SFLDMIMNVS0',territory='MI' WHERE switch=='SFLDMIMNVS0'; -- FIXME: poor equip (34-142)
-- UPDATE switchdata SET switch='SNANCACZDZ0',territory='CA' WHERE switch=='SNANCACZDZ0'; -- FIXME: poor equip (16-338)
-- UPDATE switchdata SET switch='SNANTXCAHC1',territory='TX' WHERE switch=='SNANTXCAHC1'; -- FIXME: poor equip (56-276)
-- UPDATE switchdata SET switch='STPLMNMKKDZ',territory='MN' WHERE switch=='STPLMNMKKDZ'; -- FIXME: poor equip (35-139)
-- UPDATE switchdata SET switch='SXCYIADTSB3',territory='IA' WHERE switch=='SXCYIADTSB3'; -- FIXME: poor equip (27-102)
-- UPDATE switchdata SET switch='TACMWAFAKXX',territory='WA' WHERE switch=='TACMWAFAKXX'; -- FIXME: poor equip (61-90)
-- UPDATE switchdata SET switch='TWFLIDMAKXX',territory='ID' WHERE switch=='TWFLIDMAKXX'; -- FIXME: poor equip (24-30)
-- UPDATE switchdata SET switch='WCLMSCMAKXX',territory='SC' WHERE switch=='WCLMSCMAKXX'; -- FIXME: poor equip (53-24)
-- UPDATE switchdata SET switch='WLDMTXXWHPI',territory='TX' WHERE switch=='WLDMTXXWHPI'; -- FIXME: poor equip (56-323)
-- UPDATE switchdata SET switch='WLLSTXXWHPI',territory='TX' WHERE switch=='WLLSTXXWHPI'; -- FIXME: poor equip (56-326)
-- UPDATE switchdata SET switch='WOTNMN14WOO',territory='MN' WHERE switch=='WOTNMN14WOO'; -- FIXME: poor equip (35-158)
-- UPDATE switchdata SET switch='WPBHFLANDMS',territory='FL' WHERE switch=='WPBHFLANDMS'; -- FIXME: poor equip (21-208)
-- UPDATE switchdata SET switch='WPBHFLGADMS',territory='FL' WHERE switch=='WPBHFLGADMS'; -- FIXME: poor equip (21-214)
-- UPDATE switchdata SET switch='WPBHFLGRDMS',territory='FL' WHERE switch=='WPBHFLGRDMS'; -- FIXME: poor equip (21-214)
-- UPDATE switchdata SET switch='WPBHFLHHDMS',territory='FL' WHERE switch=='WPBHFLHHDMS'; -- FIXME: poor equip (21-214)
-- UPDATE switchdata SET switch='WPBHFLLEDMS',territory='FL' WHERE switch=='WPBHFLLEDMS'; -- FIXME: poor equip (21-87)
-- UPDATE switchdata SET switch='WPBHFLRBDMS',territory='FL' WHERE switch=='WPBHFLRBDMS'; -- FIXME: poor equip (21-165)
-- UPDATE switchdata SET switch='WSCRCABYESS',territory='CA' WHERE switch=='WSCRCABYESS'; -- FIXME: poor equip (16-388)

DELETE FROM pooldata   WHERE npa=='256' AND nxx=='291'; -- #not defined by NANPA
DELETE FROM prefixdata WHERE npa=='256' AND nxx=='291'; -- #not defined by NANPA
DELETE FROM pooldata   WHERE npa=='701' AND nxx=='555'; -- #should be deleted
DELETE FROM prefixdata WHERE npa=='701' AND nxx=='555'; -- #should be deleted

UPDATE rcdata SET rc='Lac du Bonnet' WHERE rc=='Lac Du Bonnet';

DELETE FROM pooldata   WHERE npa=='445' AND nxx=='445' AND x=='7'; -- NPA 445 not assigned by NANPA
DELETE FROM pooldata   WHERE npa=='515' AND nxx=='744' AND x=='4'; -- #not defined by NANPA
DELETE FROM pooldata   WHERE npa=='515' AND nxx=='744' AND x=='5'; -- #not defined by NANPA

UPDATE pooldata   SET switch='ALBRNMJW0MD' WHERE switch=='ALBRNMJW0ND' AND npa=='505' AND nxx=='806' AND x=='7'; -- #letter substitution
UPDATE pooldata   SET switch='ALBRNMJW0MD' WHERE switch=='ALBRNMJW0ND' AND npa=='505' AND nxx=='806' AND x=='8'; -- #letter substitution
UPDATE prefixdata SET switch='ALBRNMJW0MD' WHERE switch=='ALBRNMJW0ND' AND npa=='505' AND nxx=='806'; -- #letter substitution
