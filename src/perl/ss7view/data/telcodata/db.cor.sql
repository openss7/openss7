UPDATE pooldata   SET switch=NULL WHERE switch=='NOCLLIKNOWN'; -- #FIXED: no use for this
UPDATE prefixdata SET switch=NULL WHERE switch=='NOCLLIKNOWN'; -- #FIXED: no use for this
DELETE FROM switchdata WHERE switch=='NOCLLIKNOWN'; -- #FIXED: no use for this
UPDATE pooldata   SET switch=NULL WHERE switch=='XXXXXXXXXXX'; -- #FIXED: no use for this
UPDATE prefixdata SET switch=NULL WHERE switch=='XXXXXXXXXXX'; -- #FIXED: no use for this
DELETE FROM switchdata WHERE switch=='XXXXXXXXXXX'; -- #FIXED: no use for this
UPDATE prefixdata SET rc=NULL WHERE rc=='XXXXXXXXXX'; -- #FIXED: no use for this
DELETE FROM rcdata WHERE rc=='XXXXXXXXXX'; -- #FIXED: no use for this
