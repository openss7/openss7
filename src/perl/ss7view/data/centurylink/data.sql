-- information from Centurylink Network Disclosure Announcement No. 11-002
-- pinglo is missing this change an still reports CLTNMOXADS0 instead of CLTNMOXARS0.
INSERT OR IGNORE INTO wcdata wc VALUES('CNTNMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WRBGMOXA');
UPDATE wcdata SET wcst='MO',wccity='Clinton' WHERE wc=='CNTNMOXA';
UPDATE wcdata SET wcst='MO',wccity='Warrensburg' WHERE wc=='WRBGMOXA';
INSERT OR IGNORE INTO switchdata switch VALUES ('CNTNMOXADS0');
INSERT OR IGNORE INTO switchdata switch VALUES ('CNTNMOXARS0');
INSERT OR IGNORE INTO switchdata switch VALUES ('WRBGMOXADS0');
UPDATE switchdata SET switchname='Clinton',switchtype='DMH',switchdesc='GENBAND DMS100',spc='239-012-017' WHERE switch=='CNTNMOXADS0';
UPDATE switchdata SET switchname='Clinton',switchdesc='CALIX Digital Line Concentrator',swfunc='REMOTE',host='WRBGMOXADS0' WHERE switch=='CNTNMOXARS0';
UPDATE switchdata SET switchname='Warrensburg',switchtype='DMH',switchdesc='GENBAND DMS100',swfunc='HOST',spc='239-012-002' WHERE switch=='WRBGMOXADS0';
INSERT OR IGNORE INTO prefixdata npa,nxx VALUES('660','885');
INSERT OR IGNORE INTO prefixdata npa,nxx VALUES('660','890');
UPDATE prefixdata SET switch='CNTNMOXARS0',effdate='1317168000' WHERE npa=='660' and (nxx=='885' or nxx=='890') IF date('now','unixepoch') >= 1317168000;
-- information from Centurylink Network Disclosure Announcement No. 11-003
-- note that npanxxsource is missing this change (it still shows the old host)
INSERT OR IGNORE INTO wcdata wc VALUES('CNTNMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('WRBGMOXA');
INSERT OR IGNORE INTO wcdata wc VALUES('DPWRMOXA');
UPDATE wcdata SET wcst='MO',wccity='Clinton' WHERE wc=='CNTNMOXA';
UPDATE wcdata SET wcst='MO',wccity='Warrensburg' WHERE wc=='WRBGMOXA';
UPDATE wcdata SET wcst='MO',wccity='Deepwater' WHERE wc=='DPWRMOXA';
INSERT OR IGNORE INTO switchdata switch VALUES ('CNTNMOXADS0');
INSERT OR IGNORE INTO switchdata switch VALUES ('WRBGMOXADS0');
INSERT OR IGNORE INTO switchdata switch VALUES ('DPWRMOXARS0');
UPDATE switchdata SET switchname='Clinton',switchtype='DMH',switchdesc='GENBAND DMS100',spc='239-012-017',swfunc='HOST' WHERE switch=='CLTNMOXADS0';
UPDATE switchdata SET switchname='Warrensburg',switchtype='DMH',switchdesc='GENBAND DMS100',spc='239-012-002',swfunc='HOST' WHERE switch=='WRBGMOXADS0';
UPDATE switchdata SET switchname='Deepwater',switchtype='RLC',switchdesc='GENBAND DMS RLC Remote',spc='239-012-017',host='CLTNMOXADS0' where switch='DPWRMOXARS0';
UPDATE switchdata SET switchname='Deepwater',switchtype='RLC',switchdesc='GENBAND DMS RLC Remote',spc='239-012-002',host='WRBGMOXADS0' where switch='DPWRMOXARS0' IF date('now','unixepoch') >= 1313366400;
INSERT or IGNORE INTO prefixdata npa,nxx VALUES('660','696');
INSERT or IGNORE INTO pooldata npa,nxx,x VALUES('660','696','0');
INSERT or IGNORE INTO pooldata npa,nxx,x VALUES('660','696','2');
UPDATE pooldata SET switch='DPWRMOXARS0',effdate='1313366400' WHERE npa=='660' and nxx=='696' and (x=='0' or x=='2');
-- "2-14-2012JacksonvilleWestArkansasSwitchReplacementrev-3.doc" 2012-02-28
-- Jacksonville West switch JCVLARXBDS0 is decomissioned and lines migrated to a new Jacksonville
-- West remote JCVLARXBRS0
INSERT or IGNORE INTO wcdata wc VALUES('JCVLARXA');
UPDATE wcdata SET wcst='AR',wcname='Jacksonville Main' WHERE wc=='JCVLARXA';
INSERT or IGNORE INTO switchdata switch VALUES('JCVLARXADS0');
UPDATE switchdata SET switchname='Jacksonville Main',switchdesc='Alcatel-Lucent GTD5',spc='232-029-003' WHERE switch=='JCVLARXADS0';
INSERT or IGNORE INTO wcdata wc VALUES('JCVLARXB');
UPDATE wcdata SET wcst='AR',wcname='Jacksonville West' WHERE wc=='JCVLARXB';
INSERT or IGNORE INTO switchdata switch VALUES('JCVLARXBRS0');
UPDATE switchdata SET switchname='Jacksonville West',switchdesc='GTD5 Remote',host='JCVLARXADS0',swfunc='REMOTE' WHERE switch=='JCVLARXBRS0';
INSERT or IGNORE INTO switchdata switch VALUES('JCVLARXBDS0');
UPDATE switchdata SET switchname='Jacksonville West',switchdesc='Alcatel-Lucent GTD5',swfunc='HOST' WHERE switch='JCVLARXBDS0';
INSERT or IGNORE INTO prefixdata npa,nxx VALUES('501','983');
INSERT or IGNORE INTO prefixdata npa,nxx VALUES('501','987');
INSERT or IGNORE INTO prefixdata npa,nxx VALUES('501','988');
UPDATE prefixdata SET switch='JCVLARXBRS0' WHERE (switch is null or switch=='JCVLARXBDS0') and npa=='501' and (nxx=='983' or nxx=='987' or nxx=='988') IF date('now','unixepoch') >= 1337126400; -- 2012-05-16
