-- 11_001_Century_Link_Copper_Retirements_in_IL_040711.txt xxx
--===================================================================================
-- 11_002_Clinton_Missouri_Switch_Replacement_08152011.txt
-- CL11-002
-- 2011-09-28 (2011-05-24,2011-08-15)
-- information from Centurylink Network Disclosure Announcement No. 11-002
-- pinglo is missing this change an still reports CLTNMOXADS0 instead of CLTNMOXARS0.
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('CLTNMOXA','CL11-002');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('WRBGMOXA','CL11-002');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('239-012-017','CL11-002');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('239-012-002','CL11-002');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES ('CLTNMOXADS0','CL11-002');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES ('CLTNMOXARS0','CL11-002');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES ('WRBGMOXADS0','CL11-002');
INSERT OR IGNORE INTO prefixdata (npa,nxx,cref) VALUES('660','885','CL11-002');
INSERT OR IGNORE INTO prefixdata (npa,nxx,cref) VALUES('660','890','CL11-002');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','890','0','CL11-002');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','890','1','CL11-002');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','890','2','CL11-002');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','890','6','CL11-002');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','890','7','CL11-002');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','890','8','CL11-002');
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Clinton'     WHERE wc LIKE 'CLTNMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Warrensburg' WHERE wc LIKE 'WRBGMO%';
UPDATE pcdata SET switch='CLTNMOXADS0',cref='CL11-002' WHERE spc='239-012-017';
UPDATE pcdata SET switch='WRBGMOXADS0',cref='CL11-002' WHERE spc='239-012-002';
UPDATE switchdata SET switchname='Clinton',switchtype='DMH',swdesc='Genband DMS100',spc='239-012-017',ddate=strftime('%s','2011-09-28'),repl='CLTNMOXARS0',cref='CL11-002' WHERE switch='CLTNMOXADS0';
UPDATE switchdata SET switchname='Clinton',swdesc='CALIX Digital Line Concentrator',swfunc='REMOTE',host='WRBGMOXADS0',spc='239-012-002',idate=strftime('%s','2011-09-28'),repl='CLTNMOXADS0',cref='CL11-002' WHERE switch='CLTNMOXARS0';
UPDATE switchdata SET switchname='Warrensburg',switchtype='DMH',swdesc='Genband DMS100',swfunc='HOST',spc='239-012-002',cref='CL11-002' WHERE switch='WRBGMOXADS0';
UPDATE prefixdata SET switch='CLTNMOXADS0',cref='CL11-002' WHERE npa='660' and nxx='885';
UPDATE pooldata   SET switch='CLTNMOXADS0',cref='CL11-002' WHERE npa='660' and nxx='890' and (x='0' or x='1' or x='2' or x='6' or x='7' or x='8');
UPDATE prefixdata SET switch='CLTNMOXARS0',cref='CL11-002' WHERE switch='CLTNMOXADS0';
UPDATE pooldata   SET switch='CLTNMOXARS0',cref='CL11-002' WHERE switch='CLTNMOXADS0';
UPDATE linedata   SET switch='CLTNMOXARS0',cref='CL11-002' WHERE switch='CLTNMOXADS0';
-- 11_003_Deepwater_Missouri_Rehome_08152011.txt
-- CL11-003
-- information from Centurylink Network Disclosure Announcement No. 11-003
-- note that npanxxsource is missing this change (it still shows the old host)
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('DPWRMOXA','CL11-003');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('CLTNMOXA','CL11-003');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('WRBGMOXA','CL11-003');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('239-012-017','CL11-003');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('239-012-002','CL11-003');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('DPWRMOXARS0','CL11-003');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('CLTNMOXADS0','CL11-003');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('WRBGMOXADS0','CL11-003');
INSERT OR IGNORE INTO prefixdata (npa,nxx,cref) VALUES('660','696','CL11-003');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','696','0','CL11-003');
INSERT OR IGNORE INTO pooldata (npa,nxx,x,cref) VALUES('660','696','2','CL11-003');
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Deepwater'   WHERE wc LIKE 'DPWRMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Clinton'     WHERE wc LIKE 'CLTNMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Warrensburg' WHERE wc LIKE 'WRBGMO%';
UPDATE pcdata SET switch='CLTNMOXADS0',cref='CL11-003' WHERE spc='239-012-017';
UPDATE pcdata SET switch='WRBGMOXADS0',cref='CL11-003' WHERE spc='239-012-002';
UPDATE switchdata SET switchname='Deepwater',swdesc='Genband DMS RLC Remote',swfunc='REMOTE',host='WRBGMOXADS0',spc='239-012-002',cref='CL11-003' WHERE switch='DPWRMOXARS0';
UPDATE switchdata SET switchname='Clinton',switchtype='DMH',swdesc='Genband DMS100',swfunc='HOST',spc='239-012-017',cref='CL11-003' WHERE switch='CLTNMOXADS0';
UPDATE switchdata SET switchname='Warrensburg',switchtype='DMH',swdesc='Genband DMS100',swfunc='HOST',spc='239-012-002',cref='CL11-003' WHERE switch='WRBGMOXADS0';
UPDATE pooldata SET switch='DPWRMOXARS0',cref='CL11-003' WHERE npa='660' and nxx='696' and (x='0' or x='2');
-- 11-004_Wire_Center_Boundary_Change_Columbia_East_to_ColumbiaSunrise.txt XXX
-- 12_001_Century_Link_Copper_Retirements_in_OR_1_31_2012-1.txt XXX
-- 12_003_Century_Link_Copper_Retirements_in_AR__IL__PA.txt XXX
-- 12_004_Century_Link_Copper_Retirements_in_FL__IL__MO__NC__PA__and_WI.txt XXX
-- 2-14-2012JacksonvilleWestArkansasSwitchReplacementrev-3.txt
-- 2012-05-16 (2012-02-28)
-- CL12-002
-- Jacksonville West switch JCVLARXBDS0 is decomissioned and lines migrated to a new Jacksonville
-- West remote JCVLARXBRS0
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('JCVLARXA','CL12-002');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('JCVLARXB','CL12-002');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('232-029-004','CL12-002');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('232-029-003','CL12-002');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('JCVLARXBDS0','CL12-002');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('JCVLARXBRS0','CL12-002');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('JCVLARXADS0','CL12-002');
INSERT OR IGNORE INTO prefixdata (npa,nxx,cref) VALUES('501','983','CL12-002');
INSERT OR IGNORE INTO prefixdata (npa,nxx,cref) VALUES('501','987','CL12-002');
INSERT OR IGNORE INTO prefixdata (npa,nxx,cref) VALUES('501','988','CL12-002');
UPDATE wcdata SET wccc='US',wcst='AR',wccity='Jacksonville' WHERE wc LIKE 'JCVLAR%';
UPDATE wcdata SET wcname='Main' WHERE wc='JCVLARXA';
UPDATE wcdata SET wcname='West' WHERE wc='JCVLARXB';
UPDATE pcdata SET switch='JCVLARXBDS0',cref='CL12-002' WHERE spc='232-029-004';
UPDATE pcdata SET switch='JCVLARXADS0',cref='CL12-002' WHERE spc='232-029-003';
UPDATE switchdata SET switchname='Jacksonville West',swdesc='Alcatel-Lucent GTD5',spc='232-029-004',ddate=strftime('%s','2012-05-16'),repl='JCVLARXBRS0',cref='CL12-002' WHERE switch='JCVLARXBDS0';
UPDATE switchdata SET switchname='Jacksonville West',swfunc='REMOTE',swdesc='Alcatel-Lucent GTD5 Remote',host='JCVLARXADS0',spc='232-029-003',cref='CL12-002' WHERE switch='JCVLARXBRS0';
UPDATE switchdata SET switchname='Jacksonville Main',swfunc='HOST',swdesc='Alcatel-Lucent GTD5',spc='232-029-003',cref='CL12-002' WHERE switch='JCVLARXADS0';
UPDATE prefixdata SET switch='JCVLARXBDS0',cref='CL12-002' WHERE npa='501' and (nxx='983' or nxx='987' or nxx='988');
UPDATE prefixdata SET switch='JCVLARXBRS0',cref='CL12-002' WHERE switch='JCVLARXBDS0';
UPDATE pooldata   SET switch='JCVLARXBRS0',cref='CL12-002' WHERE switch='JCVLARXBDS0';
UPDATE linedata   SET switch='JCVLARXBRS0',cref='CL12-002' WHERE switch='JCVLARXBDS0';
-- CL_Notice_Letter_121310.txt XXX
-- CL_Notice_121310.txt XXX
-- CT_NNF_100109.txt
-- 2009-11-17 (2009-10-01)
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('GCVLMNXA','CT100109');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('CLTNMNXA','CT100109');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('BRDSMNXA','CT100109');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('MPLSMNCD','CT100109');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('PLMOMNBN','CT100109');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('STCDMNTO','CT100109');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('232-130-015','CT100109');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('222-101-006','CT100109');
INSERT OR IGNORE INTO pcdata (spc,cref) VALUES('222-102-002','CT100109');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('GCVLMNXADS0','CT100109');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('CLTNMNXARS0','CT100109');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('BRDSMNXARS0','CT100109');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('MPLSMNCD08T','CT100109');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('PLMOMNBN03T','CT100109');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('STCDMNTO12T','CT100109');
UPDATE wcdata SET wccc='US',wcst='MN',wccity='Graceville' WHERE wc LIKE 'GCVLMN%';
UPDATE wcdata SET wccc='US',wcst='MN',wccity='Clinton' WHERE wc LIKE 'CLTNMN%';
UPDATE wcdata SET wccc='US',wcst='MN',wccity='Beardsley' WHERE wc LIKE 'BRDSMN%';
UPDATE wcdata SET wccc='US',wcst='MN' WHERE wc LIKE 'MPLSMN%';
UPDATE wcdata SET wccc='US',wcst='MN' WHERE wc LIKE 'PLMOMN%';
UPDATE wcdata SET wccc='US',wcst='MN' WHERE wc LIKE 'STCDMN%';
UPDATE pcdata SET switch='GCVLMNXADS0',cref='CT100109' WHERE spc='232-130-015';
UPDATE pcdata SET cls45sw='MPLSMNCD08T',cref='CT100109' WHERE spc='222-101-006';
UPDATE pcdata SET cls45sw='PLMOMNBN03T',cref='CT100109' WHERE spc='222-102-002';
UPDATE switchdata SET tdmfgb='MPLSMNCD08T/PLMOMNBN03T',tdmfgd='MPLSMNCD08T/PLMOMNBN03T',swfunc='HOST',spc='232-130-015',cref='CT100109' WHERE switch='GCVLMNXADS0';
UPDATE switchdata SET tdmfgb='MPLSMNCD08T/PLMOMNBN03T',tdmfgd='MPLSMNCD08T/PLMOMNBN03T',swfunc='REMOTE',host='GCVLMNXADS0',spc='232-130-015',cref='CT100109' WHERE switch='CLTNMNXARS0';
UPDATE switchdata SET tdmfgb='MPLSMNCD08T/PLMOMNBN03T',tdmfgd='MPLSMNCD08T/PLMOMNBN03T',swfunc='REMOTE',host='GCVLMNXADS0',spc='232-130-015',cref='CT100109' WHERE switch='BRDSMNXARS0';
UPDATE switchdata SET swfunc='FGB,FGD',spc='222-101-006',cref='CT100109' WHERE switch='MPLSMNCD08T';
UPDATE switchdata SET swfunc='FGB,FGD',spc='222-102-002',cref='CT100109' WHERE switch='PLMOMNBN03T';
UPDATE switchdata SET swfunc='FGB,FGD',cref='CT100109' WHERE switch='STCDMNTO12T';
-- NCF_031510.txt
-- this disclosure provides some information about wire centers
INSERT OR IGNORE INTO wcdata (wc) VALUES('ADVLOHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('ALXNMNXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('ARP TXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('ARSNTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('AVLLINXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('BRKNVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('BRSTTNXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('BRTLOHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('BTLRPAXB');
INSERT OR IGNORE INTO wcdata (wc) VALUES('CABTARXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('CFTNTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('CHBGPAXC');
INSERT OR IGNORE INTO wcdata (wc) VALUES('CHVLVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('CKVLOHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('CRNNINXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('DLWYVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('ELKNNCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('ELTNTNXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('ESTLSCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('FRTNTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('GLSTOHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('HCKRNCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('HNVLMOXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('JNCYKSXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('JNCYOHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('KGPTTNXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('KGWDTXXC');
INSERT OR IGNORE INTO wcdata (wc) VALUES('KLLNTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('KNTWINXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('LAPLINXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('LKPETXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('LNJCMOXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('LOMTTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MFFTTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MKVLNCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MLBGOHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MRCHTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MRNNFLXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MTPLVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MTSTOHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MTVIVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('MTVROHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('NWOXPAXN');
INSERT OR IGNORE INTO wcdata (wc) VALUES('OSWTKSXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('PLMOINXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('PNHRNCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('PTTSTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('RCFROHXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('RCMTNCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('ROLLMOXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('SALDSCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('SALDTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('SCVLVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('SDVLVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('SLRKPAXS');
INSERT OR IGNORE INTO wcdata (wc) VALUES('STDLTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('TLHSFLXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WASHVAXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WCLMTXXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WHCYORXX');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WJSNNCXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WKSNINXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WRBGMOXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WRSWINXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WRSWMOXA');
INSERT OR IGNORE INTO wcdata (wc) VALUES('WSTROHXA');
UPDATE wcdata SET wccc='US',wcst='AR',wccity='Cabot' WHERE wc LIKE 'CABTAR%';
UPDATE wcdata SET wccc='US',wcst='FL',wccity='Marianna' WHERE wc LIKE 'MRNNFL%';
UPDATE wcdata SET wccc='US',wcst='FL',wccity='Tallahassee' WHERE wc LIKE 'TLHSFL%';
UPDATE wcdata SET wccc='US',wcst='IN',wccity='Avilla' WHERE wc LIKE 'AVLLIN%';
UPDATE wcdata SET wccc='US',wcst='IN',wccity='Corunna' WHERE wc LIKE 'CRNNIN%';
UPDATE wcdata SET wccc='US',wcst='IN',wccity='Knightstown' WHERE wc LIKE 'KNTWIN%';
UPDATE wcdata SET wccc='US',wcst='IN',wccity='Lapel' WHERE wc LIKE 'LAPLIN%';
UPDATE wcdata SET wccc='US',wcst='IN',wccity='Plymouth' WHERE wc LIKE 'PLMOIN%';
UPDATE wcdata SET wccc='US',wcst='IN',wccity='Warsaw' WHERE wc LIKE 'WRSWIN%';
UPDATE wcdata SET wccc='US',wcst='IN',wccity='Wilkinson' WHERE wc LIKE 'WKSNIN%';
UPDATE wcdata SET wccc='US',wcst='KS',wccity='Junction City' WHERE wc LIKE 'JNCYKS%';
UPDATE wcdata SET wccc='US',wcst='KS',wccity='Osawatomie' WHERE wc LIKE 'OSWTKS%';
UPDATE wcdata SET wccc='US',wcst='MN',wccity='Alexandria' WHERE wc LIKE 'ALXNMN%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Harrisonville' WHERE wc LIKE 'HNVLMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Lone Jack' WHERE wc LIKE 'LNJCMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Rolla' WHERE wc LIKE 'ROLLMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Warrensburg' WHERE wc LIKE 'WRBGMO%';
UPDATE wcdata SET wccc='US',wcst='MO',wccity='Warsaw' WHERE wc LIKE 'WRSWMO%';
UPDATE wcdata SET wccc='US',wcst='NC',wccity='Elkin' WHERE wc LIKE 'ELKNNC%';
UPDATE wcdata SET wccc='US',wcst='NC',wccity='Hickory' WHERE wc LIKE 'HCKRNC%';
UPDATE wcdata SET wccc='US',wcst='NC',wccity='Mocksville' WHERE wc LIKE 'MKVLNC%';
UPDATE wcdata SET wccc='US',wcst='NC',wccity='Pinehurst' WHERE wc LIKE 'PNHRNC%';
UPDATE wcdata SET wccc='US',wcst='NC',wccity='Rocky Mount' WHERE wc LIKE 'RCMTNC%';
UPDATE wcdata SET wccc='US',wcst='NC',wccity='West Jefferson' WHERE wc LIKE 'WJSNNC%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Adamsville' WHERE wc LIKE 'ADVLOH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Bartlett' WHERE wc LIKE 'BRTLOH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Crooksville' WHERE wc LIKE 'CKVLOH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Glouster' WHERE wc LIKE 'GLSTOH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Junction City' WHERE wc LIKE 'JNCYOH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Millersburg' WHERE wc LIKE 'MLBGOH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Mount Sterling' WHERE wc LIKE 'MTSTOH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Mount Vernon' WHERE wc LIKE 'MTVROH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Rockford' WHERE wc LIKE 'RCFROH%';
UPDATE wcdata SET wccc='US',wcst='OH',wccity='Wooster' WHERE wc LIKE 'WSTROH%';
UPDATE wcdata SET wccc='US',wcst='OR',wccity='White City' WHERE wc LIKE 'WHCYOR%';
UPDATE wcdata SET wccc='US',wcst='PA',wccity='Butler' WHERE wc LIKE 'BTLRPA%';
UPDATE wcdata SET wccc='US',wcst='PA',wccity='Chambersburg' WHERE wc LIKE 'CHBGPA%';
UPDATE wcdata SET wccc='US',wcst='PA',wccity='New Oxford' WHERE wc LIKE 'NWOXPA%';
UPDATE wcdata SET wccc='US',wcst='PA',wccity='Slippery Rock' WHERE wc LIKE 'SLRKPA%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Estill' WHERE wc LIKE 'ESTLSC%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Saluda' WHERE wc LIKE 'SALDSC%';
UPDATE wcdata SET wccc='US',wcst='TN',wccity='Bristol' WHERE wc LIKE 'BRSTTN%';
UPDATE wcdata SET wccc='US',wcst='TN',wccity='Elizabethton' WHERE wc LIKE 'ELTNTN%';
UPDATE wcdata SET wccc='US',wcst='TN',wccity='Kingsport' WHERE wc LIKE 'KGPTTN%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Anderson' WHERE wc LIKE 'ARSNTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Arp' WHERE wc LIKE 'ARP TX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Clifton' WHERE wc LIKE 'CFTNTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Frankston' WHERE wc LIKE 'FRTNTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Killeen' WHERE wc LIKE 'KLLNTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Kings Crossing' WHERE wc LIKE 'KGWDTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Lake Palestine East' WHERE wc LIKE 'LKPETX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Lometa' WHERE wc LIKE 'LOMTTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Moffat' WHERE wc LIKE 'MFFTTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Murchison' WHERE wc LIKE 'MRCHTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Pettus' WHERE wc LIKE 'PTTSTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Salado' WHERE wc LIKE 'SALDTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='Stockdale' WHERE wc LIKE 'STDLTX%';
UPDATE wcdata SET wccc='US',wcst='TX',wccity='West Columbia' WHERE wc LIKE 'WCLMTX%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Brookneal' WHERE wc LIKE 'BRKNVA%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Charlottesville' WHERE wc LIKE 'CHVLVA%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Dillwyn' WHERE wc LIKE 'DLWYVA%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Martinsville' WHERE wc LIKE 'MTVIVA%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Montpelier' WHERE wc LIKE 'MTPLVA%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Scottsville' WHERE wc LIKE 'SCVLVA%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Stanardsville' WHERE wc LIKE 'SDVLVA%';
UPDATE wcdata SET wccc='US',wcst='VA',wccity='Washington Rappahannock' WHERE wc LIKE 'WASHVA%';
-- ncg_090508.txt
-- this network disclosure provides some information on switch cllis
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('BUFTSCXA','NCG090508');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('LRBYSCXA','NCG090508');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('RDLDSCXA','NCG090508');
INSERT OR IGNORE INTO wcdata (wc,cref) VALUES('STHLSCXA','NCG090508');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('BUFTSCXADS0','NCG090508');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('LRBYSCXARS0','NCG090508');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('RDLDSCXARS0','NCG090508');
INSERT OR IGNORE INTO switchdata (switch,cref) VALUES('STHLSCXARS0','NCG090508');
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Beaufort'            WHERE wc LIKE 'BUFTSC%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Laurel Bay'          WHERE wc LIKE 'LRBYSC%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Ridgeland'           WHERE wc LIKE 'RDLDSC%';
UPDATE wcdata SET wccc='US',wcst='SC',wccity='Saint Helena Island' WHERE wc LIKE 'STHLSC%';
-- ncg_091107.txt XXX
