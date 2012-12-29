--===================================
UPDATE ocndata    SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE pcdata     SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE rcdata     SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE rndata     SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE exdata     SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE lcadata    SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE wcdata     SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE switchdata SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE npadata    SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE prefixdata SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE pooldata   SET sect='area' WHERE sect NOT NULL and sect!='area';
UPDATE linedata   SET sect='area' WHERE sect NOT NULL and sect!='area';
--===================================
UPDATE prefixdata SET region='AR' WHERE region='FL' and rcshort='BENTONVL';
UPDATE OR IGNORE rcdata SET region='AR' WHERE region='FL' and rcshort='BENTONVL';
DELETE FROM rcdata WHERE region='FL' and rcshort='BENTONVL';
--===================================
UPDATE prefixdata SET switch='MOBXAL02DS1' WHERE switch=='MOBXALVAUC1'; -- FIXED: poor equipment code 'UC1' (GOLD) [251-378]
UPDATE OR IGNORE switchdata SET switch='MOBXAL02DS1' WHERE switch=='MOBXALVAUC1'; -- FIXED: poor equipment code 'UC1' (GOLD) [MOBXALVAUC1]
DELETE FROM switchdata WHERE switch=='MOBXALVAUC1'; -- FIXED: poor equipment code 'UC1' (GOLD) [MOBXALVAUC1]
UPDATE OR IGNORE wcdata SET wc='MOBXAL02' WHERE wc=='MOBXALVA'; -- FIXED: poor equipment code 'UC1' (GOLD) [MOBXALVAUC1]
DELETE FROM wcdata WHERE wc=='MOBXALVA'; -- FIXED: poor equipment code 'UC1' (GOLD) [MOBXALVAUC1]
---------------------
UPDATE prefixdata SET switch='MOBLAL02XFX' WHERE switch=='MOBXAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (GOLD) [251-583]
UPDATE OR IGNORE switchdata SET switch='MOBLAL02XFX' WHERE switch=='MOBXAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (GOLD) [MOBXAAL02XF]
DELETE FROM switchdata WHERE switch=='MOBXAAL02XF'; -- FIXED: invalid state/province/territory 'AA' (GOLD) [MOBXAAL02XF]
UPDATE OR IGNORE wcdata SET wc='MOBLAL02' WHERE wc=='MOBXAAL0'; -- FIXED: invalid state/province/territory 'AA' (GOLD)
DELETE FROM wcdata WHERE wc=='MOBXAAL0'; -- FIXED: invalid state/province/territory 'AA' (GOLD)
---------------------
-- UPDATE prefixdata SET switch='BRHXALRTWPS' WHERE switch=='BRHXALRTWPS'; -- FIXME: poor equipment code 'WPS' (GOLD) [256-291]
-- UPDATE switchdata SET switch='BRHXALRTWPS' WHERE switch=='BRHXALRTWPS'; -- FIXME: poor equipment code 'WPS' (GOLD) [BRHXALRTWPS]
---------------------
UPDATE prefixdata SET switch='HNVIALUN0GT' WHERE switch=='HNVXALUNOGT'; -- FIXED: poor equipment code 'OGT' (GOLD) [256-631]
UPDATE OR IGNORE switchdata SET switch='HNVIALUN0GT' WHERE switch=='HNVXALUNOGT'; -- FIXED: poor equipment code 'OGT' (GOLD) [HNVXALUNOGT]
DELETE FROM switchdata WHERE switch=='HNVXALUNOGT'; -- FIXED: poor equipment code 'OGT' (GOLD) [HNVXALUNOGT]
UPDATE OR IGNORE wcdata SET wc='HNVIALUN' WHERE wc=='HNVXALUN'; -- FIXED: poor equipment code 'OGT' (GOLD) [HNVXALUNOGT]
DELETE FROM wcdata WHERE wc=='HNVXALUN'; -- FIXED: poor equipment code 'OGT' (GOLD) [HNVXALUNOGT]
---------------------
UPDATE prefixdata SET switch='BNTVARCRDS0' WHERE switch=='BNTXARCRGEL'; -- FIXED: poor equipment code 'GEL' (GOLD) [479-205]
UPDATE OR IGNORE switchdata SET switch='BNTVARCRDS0' WHERE switch=='BNTXARCRGEL'; -- FIXED: poor equipment code 'GEL' (GOLD) [BNTXARCRGEL]
DELETE FROM switchdata WHERE switch=='BNTXARCRGEL'; -- FIXED: poor equipment code 'GEL' (GOLD) [BNTXARCRGEL]
UPDATE OR IGNORE wcdata SET wc='BNTVARCR' WHERE wc=='BNTXARCR'; -- FIXED: poor equipment code 'GEL' (GOLD) [BNTXARCRGEL]
DELETE FROM wcdata WHERE wc=='BNTXARCR'; -- FIXED: poor equipment code 'GEL' (GOLD) [BNTXARCRGEL]
--===================================
