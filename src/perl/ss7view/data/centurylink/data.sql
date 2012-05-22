-- 11_001_Century_Link_Copper_Retirements_in_IL_040711.txt xxx
--===================================================================================
CREATE TEMP TRIGGER IF NOT EXISTS wcdata_sect_trig       BEFORE UPDATE of sect   on wcdata     FOR EACH ROW WHEN OLD.sect   IS NOT NULL BEGIN UPDATE wcdata     SET sect=OLD.sect','NEW.sect WHERE OLD.sect NOT LIKE '%'NEW.sect'%'; END;
CREATE TEMP TRIGGER IF NOT EXISTS pcdata_sect_trig       BEFORE UPDATE of sect   on pcdata     FOR EACH ROW WHEN OLD.sect   IS NOT NULL BEGIN UPDATE pcdata     SET sect=OLD.sect','NEW.sect WHERE OLD.sect NOT LIKE '%'NEW.sect'%'; END;
CREATE TEMP TRIGGER IF NOT EXISTS switchdata_sect_trig   BEFORE UPDATE of sect   on switchdata FOR EACH ROW WHEN OLD.sect   IS NOT NULL BEGIN UPDATE switchdata SET sect=OLD.sect','NEW.sect WHERE OLD.sect NOT LIKE '%'NEW.sect'%'; END;
CREATE TEMP TRIGGER IF NOT EXISTS prefixdata_sect_trig   BEFORE UPDATE of sect   on prefixdata FOR EACH ROW WHEN OLD.sect   IS NOT NULL BEGIN UPDATE prefixdata SET sect=OLD.sect','NEW.sect WHERE OLD.sect NOT LIKE '%'NEW.sect'%'; END;
CREATE TEMP TRIGGER IF NOT EXISTS pooldata_sect_trig     BEFORE UPDATE of sect   on pooldata   FOR EACH ROW WHEN OLD.sect   IS NOT NULL BEGIN UPDATE pooldata   SET sect=OLD.sect','NEW.sect WHERE OLD.sect NOT LIKE '%'NEW.sect'%'; END;
CREATE TEMP TRIGGER IF NOT EXISTS linedata_sect_trig     BEFORE UPDATE of sect   on linedata   FOR EACH ROW WHEN OLD.sect   IS NOT NULL BEGIN UPDATE linedata   SET sect=OLD.sect','NEW.sect WHERE OLD.sect NOT LIKE '%'NEW.sect'%'; END;
CREATE TEMP TRIGGER IF NOT EXISTS switchdata_swfunc_trig BEFORE UPDATE OF swfunc ON switchdata FOR EACH ROW WHEN OLD.swfunc IS NOT NULL BEGIN UPDATE switchdata SET swfunc=OLD.swfunc','NEW.swfunc WHERE OLD.swfunc NOT LIKE '%'NEW.swfunc'%'; END;
--===================================================================================
-- 11_002_Clinton_Missouri_Switch_Replacement_08152011.txt
-- CL11-002
-- 2011-09-28 (2011-05-24,2011-08-15)
-- information from Centurylink Network Disclosure Announcement No. 11-002
-- pinglo is missing this change an still reports CLTNMOXADS0 instead of CLTNMOXARS0.
INSERT OR IGNORE INTO wcdata wc,sect VALUES('CNTNMOXA','CL11-002');
INSERT OR IGNORE INTO wcdata wc,sect VALUES('WRBGMOXA','CL11-002');
INSERT OR IGNORE INTO pcdata spc,sect VALUES('239-012-017','CL11-002');
INSERT OR IGNORE INTO pcdata spc,sect VALUES('239-012-002','CL11-002');
INSERT OR IGNORE INTO switchdata switch,sect VALUES ('CNTNMOXADS0','CL11-002');
INSERT OR IGNORE INTO switchdata switch,sect VALUES ('CNTNMOXARS0','CL11-002');
INSERT OR IGNORE INTO switchdata switch,sect VALUES ('WRBGMOXADS0','CL11-002');
INSERT OR IGNORE INTO prefixdata npa,nxx,sect VALUES('660','885','CL11-002');
INSERT OR IGNORE INTO prefixdata npa,nxx,sect VALUES('660','890','CL11-002');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','890','0','CL11-002');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','890','1','CL11-002');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','890','2','CL11-002');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','890','6','CL11-002');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','890','7','CL11-002');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','890','8','CL11-002');
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Clinton'     WHERE wc LIKE 'CNTNMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Warrensburg' WHERE wc LIKE 'WRBGMO%';
UPDATE pcdata SET switch='CNTNMOXADS0',sect='CL11-002' WHERE spc='239-012-017';
UPDATE pcdata SET switch='WRBGMOXADS0',sect='CL11-002' WHERE spc='239-012-002';
UPDATE switchdata SET switchname='Clinton',switchtype='DMH',swdesc='Genband DMS100',spc='239-012-017',ddate=strftime('%s','2011-09-28'),repl='CNTNMOXARS0',sect='CL11-002' WHERE switch='CNTNMOXADS0';
UPDATE switchdata SET switchname='Clinton',swdesc='CALIX Digital Line Concentrator',swfunc='REMOTE',host='WRBGMOXADS0',spc='239-012-002',idate=strftime('%s','2011-09-28'),repl='CNTNMOXADS0',sect='CL11-002' WHERE switch='CNTNMOXARS0';
UPDATE switchdata SET switchname='Warrensburg',switchtype='DMH',swdesc='Genband DMS100',swfunc='HOST',spc='239-012-002',sect='CL11-002' WHERE switch='WRBGMOXADS0';
UPDATE prefixdata SET switch='CNTNMOXADS0',sect='CL11-002' WHERE npa='660' and nxx='885';
UPDATE pooldata   SET switch='CNTNMOXADS0',sect='CL11-002' WHERE npa='660' and nxx='890' and (x='0' or x='1' or x='2' or x='6' or x='7' or x='8');
UPDATE prefixdata SET switch='CNTNMOXARS0',sect='CL11-002' WHERE switch='CNTNMOXADS0';
UPDATE pooldata   SET switch='CNTNMOXARS0',sect='CL11-002' WHERE switch='CNTNMOXADS0';
UPDATE linedata   SET switch='CNTNMOXARS0',sect='CL11-002' WHERE switch='CNTNMOXADS0';
-- 11_003_Deepwater_Missouri_Rehome_08152011.txt
-- CL11-003
-- information from Centurylink Network Disclosure Announcement No. 11-003
-- note that npanxxsource is missing this change (it still shows the old host)
INSERT OR IGNORE INTO wcdata wc,sect VALUES('DPWRMOXA','CL11-003');
INSERT OR IGNORE INTO wcdata wc,sect VALUES('CLTNMOXA','CL11-003');
INSERT OR IGNORE INTO wcdata wc,sect VALUES('WRBGMOXA','CL11-003');
INSERT OR IGNORE INTO pcdata spc,sect VALUES('239-012-017','CL11-003');
INSERT OR IGNORE INTO pcdata spc,sect VALUES('239-012-002','CL11-003');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('DPWRMOXARS0','CL11-003');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('CLTNMOXADS0','CL11-003');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('WRBGMOXADS0','CL11-003');
INSERT OR IGNORE INTO prefixdata npa,nxx,sect VALUES('660','696','CL11-003');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','696','0','CL11-003');
INSERT OR IGNORE INTO pooldata npa,nxx,x,sect VALUES('660','696','2','CL11-003');
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Deepwater'   WHERE wc LIKE 'DPWRMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Clinton'     WHERE wc LIKE 'CLTNMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Warrensburg' WHERE wc LIKE 'WRBGMO%';
UPDATE pcdata SET switch='CLTNMOXADS0',sect='CL11-003' WHERE spc='239-012-017';
UPDATE pcdata SET switch='WRBGMOXADS0',sect='CL11-003' WHERE spc='239-012-002';
UPDATE switchdata SET switchname='Deepwater',swdesc='Genband DMS RLC Remote',swfunc='REMOTE',host='WRBGMOXADS0',spc='239-012-002',sect='CL11-003' WHERE switch='DPWRMOXARS0';
UPDATE switchdata SET switchname='Clinton',switchtype='DMH',swdesc='Genband DMS100',swfunc='HOST',spc='239-012-017',sect='CL11-003' WHERE switch='CLTNMOXADS0';
UPDATE switchdata SET switchname='Warrensburg',switchtype='DMH',swdesc='Genband DMS100',swfunc='HOST',spc='239-012-002',sect='CL11-003' WHERE switch='WRBGMOXADS0';
UPDATE pooldata SET switch='DPWRMOXARS0',sect='CL11-003' WHERE npa='660' and nxx='696' and (x='0' or x='2');
-- 11-004_Wire_Center_Boundary_Change_Columbia_East_to_ColumbiaSunrise.txt XXX
-- 12_001_Century_Link_Copper_Retirements_in_OR_1_31_2012-1.txt XXX
-- 12_003_Century_Link_Copper_Retirements_in_AR__IL__PA.txt XXX
-- 12_004_Century_Link_Copper_Retirements_in_FL__IL__MO__NC__PA__and_WI.txt XXX
-- 2-14-2012JacksonvilleWestArkansasSwitchReplacementrev-3.txt
-- 2012-05-16 (2012-02-28)
-- CL12-002
-- Jacksonville West switch JCVLARXBDS0 is decomissioned and lines migrated to a new Jacksonville
-- West remote JCVLARXBRS0
INSERT OR IGNORE INTO wcdata wc,sect VALUES('JCVLARXA','CL12-002');
INSERT OR IGNORE INTO wcdata wc,sect VALUES('JCVLARXB','CL12-002');
INSERT OR IGNORE INTO pcdata spc,sect VALUES('232-029-004','CL12-002');
INSERT OR IGNORE INTO pcdata spc,sect VALUES('232-029-003','CL12-002');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('JCVLARXBDS0','CL12-002');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('JCVLARXBRS0','CL12-002');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('JCVLARXADS0','CL12-002');
INSERT OR IGNORE INTO prefixdata npa,nxx,sect VALUES('501','983','CL12-002');
INSERT OR IGNORE INTO prefixdata npa,nxx,sect VALUES('501','987','CL12-002');
INSERT OR IGNORE INTO prefixdata npa,nxx,sect VALUES('501','988','CL12-002');
UPDATE wcdata SET wccc='US',wcst='AR',wccity='Jacksonville' WHERE wc LIKE 'JCVLAR%';
UPDATE wcdata SET wcname='Main' WHERE wc='JCVLARXA';
UPDATE wcdata SET wcname='West' WHERE wc='JCVLARXB';
UPDATE pcdata SET switch='JCVLARXBDS0',sect='CL12-002' WHERE spc='232-029-004';
UPDATE pcdata SET switch='JCVLARXADS0',sect='CL12-002' WHERE spc='232-029-003';
UPDATE switchdata SET switchname='Jacksonville West',swdesc='Alcatel-Lucent GTD5',spc='232-029-004',ddate=strftime('%s','2012-05-16'),repl='JCVLARXBRS0',sect='CL12-002' WHERE switch='JCVLARXBDS0';
UPDATE switchdata SET switchname='Jacksonville West',swfunc='REMOTE',swdesc='Alcatel-Lucent GTD5 Remote',host='JCVLARXADS0',spc='232-029-003',sect='CL12-002' WHERE switch='JCVLARXBRS0';
UPDATE switchdata SET switchname='Jacksonville Main',swfunc='HOST',swdesc='Alcatel-Lucent GTD5',spc='232-029-003',sect='CL12-002' WHERE switch='JCVLARXADS0';
UPDATE prefixdata SET switch='JCVLARXBDS0',sect='CL12-002' WHERE npa='501' and (nxx='983' or nxx='987' or nxx='988');
UPDATE prefixdata SET switch='JCVLARXBRS0',sect='CL12-002' WHERE switch='JCVLARXBDS0';
UPDATE pooldata   SET switch='JCVLARXBRS0',sect='CL12-002' WHERE switch='JCVLARXBDS0';
UPDATE linedata   SET switch='JCVLARXBRS0',sect='CL12-002' WHERE switch='JCVLARXBDS0';
-- CL_Notice_Letter_121310.txt XXX
-- CL_Notice_121310.txt XXX
-- CT_NNF_100109.txt
-- 2009-11-17 (2009-10-01)
INSERT OR IGNORE INTO wcdata wc,sect('GCVLMNXA','CT100109');
INSERT OR IGNORE INTO wcdata wc,sect('CLTNMNXA','CT100109');
INSERT OR IGNORE INTO wcdata wc,sect('BRDSMNXA','CT100109');
INSERT OR IGNORE INTO wcdata wc,sect('MPLSMNCD','CT100109');
INSERT OR IGNORE INTO wcdata wc,sect('PLMOMNBN','CT100109');
INSERT OR IGNORE INTO wcdata wc,sect('STCDMNTO','CT100109');
INSERT OR IGNORE INTO pcdata spc,sect('232-130-015','CT100109');
INSERT OR IGNORE INTO pcdata spc,sect('222-101-006','CT100109');
INSERT OR IGNORE INTO pcdata spc,sect('222-102-002','CT100109');
INSERT OR IGNORE INTO switchdata switch,sect('GCVLMNXADS0','CT100109');
INSERT OR IGNORE INTO switchdata switch,sect('CLTNMNXARS0','CT100109');
INSERT OR IGNORE INTO switchdata switch,sect('BRDSMNXARS0','CT100109');
INSERT OR IGNORE INTO switchdata switch,sect('MPLSMNCD08T','CT100109');
INSERT OR IGNORE INTO switchdata switch,sect('PLMOMNBN03T','CT100109');
INSERT OR IGNORE INTO switchdata switch,sect('STCDMNTO12T','CT100109');
UPDATE wcdata SET wccc='US',wcst='MN',wccity='Graceville' WHERE wc LIKE 'GCVLMN%';
UPDATE wcdata SET wccc='US',wcst='MN',wccity='Clinton' WHERE wc LIKE 'CLTNMN%';
UPDATE wcdata SET wccc='US',wcst='MN',wccity='Beardsley' WHERE wc LIKE 'BRDSMN%';
UPDATE wcdata SET wccc='US',wcst='MN' WHERE wc LIKE 'MPLSMN%';
UPDATE wcdata SET wccc='US',wcst='MN' WHERE wc LIKE 'PLMOMN%';
UPDATE wcdata SET wccc='US',wcst='MN' WHERE wc LIKE 'STCDMN%';
UPDATE pcdata SET switch='GCVLMNXADS0',sect='CT100109' WHERE spc='232-130-015';
UPDATE pcdata SET cls45sw='MPLSMNCD08T',sect='CT100109' WHERE spc='222-101-006';
UPDATE pcdata SET cls45sw='PLMOMNBN03T',sect='CT100109' WHERE spc='222-102-002';
UPDATE switchdata SET tdmfgb='MPLSMNCD08T/PLMOMNBN03T',tdmfgd='MPLSMNCD08T/PLMOMNBN03T',swfunc='HOST',spc='232-130-015',sect='CT100109' WHERE switch='GCVLMNXADS0';
UPDATE switchdata SET tdmfgb='MPLSMNCD08T/PLMOMNBN03T',tdmfgd='MPLSMNCD08T/PLMOMNBN03T',swfunc='REMOTE',host='GCVLMNXADS0',spc='232-130-015',sect='CT100109' WHERE switch='CLTNMNXARS0';
UPDATE switchdata SET tdmfgb='MPLSMNCD08T/PLMOMNBN03T',tdmfgd='MPLSMNCD08T/PLMOMNBN03T',swfunc='REMOTE',host='GCVLMNXADS0',spc='232-130-015',sect='CT100109' WHERE switch='BRDSMNXARS0';
UPDATE switchdata SET swfunc='FGB,FGD',spc='222-101-006',sect='CT100109' WHERE switch='MPLSMNCD08T';
UPDATE switchdata SET swfunc='FGB,FGD',spc='222-102-002',sect='CT100109' WHERE switch='PLMOMNBN03T';
UPDATE switchdata SET swfunc='FGB,FGD',sect='CT100109' WHERE switch='STCDMNTO12T';
-- NCF_031510.txt
-- this disclosure provides some information about wire centers
INSERT OR IGNORE INTO wcdata wc VALUES('ADVLOHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('ALXNMNXA');
INSERT OR IGNORE INTO wcdata wc VALUES('ARP TXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('ARSNTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('AVLLINXA');
INSERT OR IGNORE INTO wcdata wc VALUES('BRKNVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('BRSTTNXA');
INSERT OR IGNORE INTO wcdata wc VALUES('BRTLOHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('BTLRPAXB');
INSERT OR IGNORE INTO wcdata wc VALUES('CABTARXA');
INSERT OR IGNORE INTO wcdata wc VALUES('CFTNTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('CHBGPAXC');
INSERT OR IGNORE INTO wcdata wc VALUES('CHVLVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('CKVLOHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('CRNNINXA');
INSERT OR IGNORE INTO wcdata wc VALUES('DLWYVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('ELKNNCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('ELTNTNXA');
INSERT OR IGNORE INTO wcdata wc VALUES('ESTLSCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('FRTNTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('GLSTOHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('HCKRNCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('HNVLMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('JNCYKSXA');
INSERT OR IGNORE INTO wcdata wc VALUES('JNCYOHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('KGPTTNXA');
INSERT OR IGNORE INTO wcdata wc VALUES('KGWDTXXC');
INSERT OR IGNORE INTO wcdata wc VALUES('KLLNTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('KNTWINXA');
INSERT OR IGNORE INTO wcdata wc VALUES('LAPLINXA');
INSERT OR IGNORE INTO wcdata wc VALUES('LKPETXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('LNJCMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('LOMTTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MFFTTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MKVLNCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MLBGOHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MRCHTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MRNNFLXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MTPLVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MTSTOHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MTVIVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('MTVROHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('NWOXPAXN');
INSERT OR IGNORE INTO wcdata wc VALUES('OSWTKSXA');
INSERT OR IGNORE INTO wcdata wc VALUES('PLMOINXA');
INSERT OR IGNORE INTO wcdata wc VALUES('PNHRNCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('PTTSTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('RCFROHXA');
INSERT OR IGNORE INTO wcdata wc VALUES('RCMTNCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('ROLLMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('SALDSCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('SALDTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('SCVLVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('SDVLVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('SLRKPAXS');
INSERT OR IGNORE INTO wcdata wc VALUES('STDLTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('TLHSFLXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WASHVAXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WCLMTXXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WHCYORXX');
INSERT OR IGNORE INTO wcdata wc VALUES('WJSNNCXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WKSNINXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WRBGMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WRSWINXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WRSWMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WSTROHXA');
wccc='US',wcst='AR',wccity='Cabot' WHERE wc LIKE 'CABTAR%';
wccc='US',wcst='FL',wccity='Marianna' WHERE wc LIKE 'MRNNFL%';
wccc='US',wcst='FL',wccity='Tallahassee' WHERE wc LIKE 'TLHSFL%';
wccc='US',wcst='IN',wccity='Avilla' WHERE wc LIKE 'AVLLIN%';
wccc='US',wcst='IN',wccity='Corunna' WHERE wc LIKE 'CRNNIN%';
wccc='US',wcst='IN',wccity='Knightstown' WHERE wc LIKE 'KNTWIN%';
wccc='US',wcst='IN',wccity='Lapel' WHERE wc LIKE 'LAPLIN%';
wccc='US',wcst='IN',wccity='Plymouth' WHERE wc LIKE 'PLMOIN%';
wccc='US',wcst='IN',wccity='Warsaw' WHERE wc LIKE 'WRSWIN%';
wccc='US',wcst='IN',wccity='Wilkinson' WHERE wc LIKE 'WKSNIN%';
wccc='US',wcst='KS',wccity='Junction City' WHERE wc LIKE 'JNCYKS%';
wccc='US',wcst='KS',wccity='Osawatomie' WHERE wc LIKE 'OSWTKS%';
wccc='US',wcst='MN',wccity='Alexandria' WHERE wc LIKE 'ALXNMN%';
wccc='US',wcst='MO',wccity='Harrisonville' WHERE wc LIKE 'HNVLMO%';
wccc='US',wcst='MO',wccity='Lone Jack' WHERE wc LIKE 'LNJCMO%';
wccc='US',wcst='MO',wccity='Rolla' WHERE wc LIKE 'ROLLMO%';
wccc='US',wcst='MO',wccity='Warrensburg' WHERE wc LIKE 'WRBGMO%';
wccc='US',wcst='MO',wccity='Warsaw' WHERE wc LIKE 'WRSWMO%';
wccc='US',wcst='NC',wccity='Elkin' WHERE wc LIKE 'ELKNNC%';
wccc='US',wcst='NC',wccity='Hickory' WHERE wc LIKE 'HCKRNC%';
wccc='US',wcst='NC',wccity='Mocksville' WHERE wc LIKE 'MKVLNC%';
wccc='US',wcst='NC',wccity='Pinehurst' WHERE wc LIKE 'PNHRNC%';
wccc='US',wcst='NC',wccity='Rocky Mount' WHERE wc LIKE 'RCMTNC%';
wccc='US',wcst='NC',wccity='West Jefferson' WHERE wc LIKE 'WJSNNC%';
wccc='US',wcst='OH',wccity='Adamsville' WHERE wc LIKE 'ADVLOH%';
wccc='US',wcst='OH',wccity='Bartlett' WHERE wc LIKE 'BRTLOH%';
wccc='US',wcst='OH',wccity='Crooksville' WHERE wc LIKE 'CKVLOH%';
wccc='US',wcst='OH',wccity='Glouster' WHERE wc LIKE 'GLSTOH%';
wccc='US',wcst='OH',wccity='Junction City' WHERE wc LIKE 'JNCYOH%';
wccc='US',wcst='OH',wccity='Millersburg' WHERE wc LIKE 'MLBGOH%';
wccc='US',wcst='OH',wccity='Mount Sterling' WHERE wc LIKE 'MTSTOH%';
wccc='US',wcst='OH',wccity='Mount Vernon' WHERE wc LIKE 'MTVROH%';
wccc='US',wcst='OH',wccity='Rockford' WHERE wc LIKE 'RCFROH%';
wccc='US',wcst='OH',wccity='Wooster' WHERE wc LIKE 'WSTROH%';
wccc='US',wcst='OR',wccity='White City' WHERE wc LIKE 'WHCYOR%';
wccc='US',wcst='PA',wccity='Butler' WHERE wc LIKE 'BTLRPA%';
wccc='US',wcst='PA',wccity='Chambersburg' WHERE wc LIKE 'CHBGPA%';
wccc='US',wcst='PA',wccity='New Oxford' WHERE wc LIKE 'NWOXPA%';
wccc='US',wcst='PA',wccity='Slippery Rock' WHERE wc LIKE 'SLRKPA%';
wccc='US',wcst='SC',wccity='Estill' WHERE wc LIKE 'ESTLSC%';
wccc='US',wcst='SC',wccity='Saluda' WHERE wc LIKE 'SALDSC%';
wccc='US',wcst='TN',wccity='Bristol' WHERE wc LIKE 'BRSTTN%';
wccc='US',wcst='TN',wccity='Elizabethton' WHERE wc LIKE 'ELTNTN%';
wccc='US',wcst='TN',wccity='Kingsport' WHERE wc LIKE 'KGPTTN%';
wccc='US',wcst='TX',wccity='Anderson' WHERE wc LIKE 'ARSNTX%';
wccc='US',wcst='TX',wccity='Arp' WHERE wc LIKE 'ARP TX%';
wccc='US',wcst='TX',wccity='Clifton' WHERE wc LIKE 'CFTNTX%';
wccc='US',wcst='TX',wccity='Frankston' WHERE wc LIKE 'FRTNTX%';
wccc='US',wcst='TX',wccity='Killeen' WHERE wc LIKE 'KLLNTX%';
wccc='US',wcst='TX',wccity='Kings Crossing' WHERE wc LIKE 'KGWDTX%';
wccc='US',wcst='TX',wccity='Lake Palestine East' WHERE wc LIKE 'LKPETX%';
wccc='US',wcst='TX',wccity='Lometa' WHERE wc LIKE 'LOMTTX%';
wccc='US',wcst='TX',wccity='Moffat' WHERE wc LIKE 'MFFTTX%';
wccc='US',wcst='TX',wccity='Murchison' WHERE wc LIKE 'MRCHTX%';
wccc='US',wcst='TX',wccity='Pettus' WHERE wc LIKE 'PTTSTX%';
wccc='US',wcst='TX',wccity='Salado' WHERE wc LIKE 'SALDTX%';
wccc='US',wcst='TX',wccity='Stockdale' WHERE wc LIKE 'STDLTX%';
wccc='US',wcst='TX',wccity='West Columbia' WHERE wc LIKE 'WCLMTX%';
wccc='US',wcst='VA',wccity='Brookneal' WHERE wc LIKE 'BRKNVA%';
wccc='US',wcst='VA',wccity='Charlottesville' WHERE wc LIKE 'CHVLVA%';
wccc='US',wcst='VA',wccity='Dillwyn' WHERE wc LIKE 'DLWYVA%';
wccc='US',wcst='VA',wccity='Martinsville' WHERE wc LIKE 'MTVIVA%';
wccc='US',wcst='VA',wccity='Montpelier' WHERE wc LIKE 'MTPLVA%';
wccc='US',wcst='VA',wccity='Scottsville' WHERE wc LIKE 'SCVLVA%';
wccc='US',wcst='VA',wccity='Stanardsville' WHERE wc LIKE 'SDVLVA%';
wccc='US',wcst='VA',wccity='Washington Rappahannock' WHERE wc LIKE 'WASHVA%';
-- ncg_090508.txt
-- this network disclosure provides some information on switch cllis
INSERT OR IGNORE INTO wcdata wc,sect VALUES('BUFTSCXA','NCG090508');
INSERT OR IGNORE INTO wcdata wc,sect VALUES('LRBYSCXA','NCG090508');
INSERT OR IGNORE INTO wcdata wc,sect VALUES('RDLDSCXA','NCG090508');
INSERT OR IGNORE INTO wcdata wc,sect VALUES('STHLSCXA','NCG090508');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('BUFTSCXADS0','NCG090508');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('LRBYSCXARS0','NCG090508');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('RDLDSCXARS0','NCG090508');
INSERT OR IGNORE INTO switchdata switch,sect VALUES('STHLSCXARS0','NCG090508');
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Beaufort'            WHERE wc LIKE 'BUFTSC%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Laurel Bay'          WHERE wc LIKE 'LRBYSC%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Ridgeland'           WHERE wc LIKE 'RDLDSC%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Saint Helena Island' WHERE wc LIKE 'STHLSC%';
-- ncg_091107.txt XXX
--===================================================================================
DROP TRIGGER wcdata_sect_trig;
DROP TRIGGER pcdata_sect_trig;
DROP TRIGGER switchdata_sect_trig;
DROP TRIGGER prefixdata_sect_trig;
DROP TRIGGER pooldata_sect_trig;
DROP TRIGGER linedata_sect_trig;
DROP TRIGGER switchdata_swfunc_trig;
--===================================================================================
