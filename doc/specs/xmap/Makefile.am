## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
## Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU Affero General Public License as published by the Free
## Software Foundation; version 3 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
## details.
##
## You should have received a copy of the GNU Affero General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>, or
## write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
## 02139, USA.
##
## -----------------------------------------------------------------------------
##
## U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
## behalf of the U.S. Government ("Government"), the following provisions apply
## to you.  If the Software is supplied by the Department of Defense ("DoD"), it
## is classified as "Commercial Computer Software" under paragraph 252.227-7014
## of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
## successor regulations) and the Government is acquiring only the license rights
## granted herein (the license rights customarily provided to non-Government
## users).  If the Software is supplied to any unit or agency of the Government
## other than DoD, it is classified as "Restricted Computer Software" and the
## Government's rights in the Software are defined in paragraph 52.227-19 of the
## Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
## the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
## (or any successor regulations).
##
## -----------------------------------------------------------------------------
##
## Commercial licensing and support of this software is available from OpenSS7
## Corporation at a fee.  See http://www.openss7.com/
##
## -----------------------------------------------------------------------------
##
## Last Modified $Date$ by $Author$
##
## =============================================================================

include ../../../am/openss7.am

info_TEXINFOS		=

if DEVELOPMENT
## DEVELOPMENT

info_TEXINFOS		+= xmap.texi

xmap.h: $(top_srcdir)/src/include/xmap.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

xmap_gsm.h: $(top_srcdir)/src/include/xmap_gsm.h
	$(AM_V_GEN)cat $< | sed -e '1,/\*\//d;/doxygen/d;/#ident/d;/^\/\/\//d' | cat -s | expand > $@

CLEANFILES += xmap.h xmap_gsm.h

xmap_TEXINFOS		= \
		  	  xmap.h \
		  	  xmap_gsm.h \
			  example.c \
			  $(top_srcdir)/scripts/texi/macros.texi \
			  $(top_srcdir)/scripts/texi/versions.texi \
			  $(top_srcdir)/scripts/texi/fdl.texi

if MAINTAINER_MODE
if AMDEP
xmap_TEXINFOS		+= \
			  $(top_builddir)/scripts/texi/args.texi
endif
endif

EXTRA_info_TEXINFOS	= \
			  $(top_builddir)/scripts/texi/args.texi \
			  $(info_TEXINFOS) \
			  $(xmap_TEXINFOS)

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

FIG_FIGS		= 

TBL_TBLS		= \
			  xmap_tabxxx.tbl \
			  xmap_tab001.tbl \
			  xmap_tab002.tbl \
			  xmap_tab003.tbl \
			  xmap_tab004.tbl \
			  xmap_tab005.tbl \
			  xmap_tab006.tbl \
			  xmap_tab007.tbl \
			  xmap_tab008.tbl \
			  xmap_tab009.tbl \
			  xmap_tab010.tbl \
			  xmap_tab011.tbl \
			  xmap_tab012.tbl \
			  xmap_tab013.tbl \
			  xmap_tab014.tbl \
			  xmap_tab015.tbl \
			  xmap_tab016.tbl \
			  xmap_tab017.tbl \
			  xmap_tab018.tbl \
			  xmap_tab019.tbl \
			  xmap_tab020.tbl \
			  xmap_tab021.tbl \
			  xmap_tab022.tbl \
			  xmap_tab023.tbl \
			  xmap_tab024.tbl \
			  xmap_tab025.tbl \
			  xmap_tab026.tbl \
			  xmap_tab027.tbl \
			  xmap_tab028.tbl \
			  xmap_tab029.tbl \
			  xmap_tab030.tbl \
			  xmap_tab031.tbl \
			  xmap_tab032.tbl \
			  xmap_tab033.tbl \
			  xmap_tab034.tbl \
			  xmap_tab035.tbl \
			  xmap_tab036.tbl \
			  xmap_tab037.tbl \
			  xmap_tab038.tbl \
			  xmap_tab039.tbl \
			  xmap_tab040.tbl \
			  xmap_tab041.tbl \
			  xmap_tab042.tbl \
			  xmap_tab043.tbl \
			  xmap_tab044.tbl \
			  xmap_tab045.tbl \
			  xmap_tab046.tbl \
			  xmap_tab047.tbl \
			  xmap_tab048.tbl \
			  xmap_tab049.tbl \
			  xmap_tab050.tbl \
			  xmap_tab051.tbl \
			  xmap_tab052.tbl \
			  xmap_tab053.tbl \
			  xmap_tab054.tbl \
			  xmap_tab055.tbl \
			  xmap_tab056.tbl \
			  xmap_tab057.tbl \
			  xmap_tab058.tbl \
			  xmap_tab059.tbl \
			  xmap_tab060.tbl \
			  xmap_tab061.tbl \
			  xmap_tab062.tbl \
			  xmap_tab063.tbl \
			  xmap_tab064.tbl \
			  xmap_tab065.tbl \
			  xmap_tab066.tbl \
			  xmap_tab067.tbl \
			  xmap_tab068.tbl \
			  xmap_tab069.tbl \
			  xmap_tab070.tbl \
			  xmap_tab071.tbl \
			  xmap_tab072.tbl \
			  xmap_tab073.tbl \
			  xmap_tab074.tbl \
			  xmap_tab075.tbl \
			  xmap_tab076.tbl \
			  xmap_tab077.tbl \
			  xmap_tab078.tbl \
			  xmap_tab079.tbl \
			  xmap_tab080.tbl \
			  xmap_tab081.tbl \
			  xmap_tab082.tbl \
			  xmap_tab083.tbl \
			  xmap_tab084.tbl \
			  xmap_tab085.tbl \
			  xmap_tab086.tbl \
			  xmap_tab087.tbl \
			  xmap_tab088.tbl \
			  xmap_tab089.tbl \
			  xmap_tab090.tbl \
			  xmap_tab091.tbl \
			  xmap_tab092.tbl \
			  xmap_tab093.tbl \
			  xmap_tab094.tbl \
			  xmap_tab095.tbl \
			  xmap_tab096.tbl \
			  xmap_tab097.tbl \
			  xmap_tab098.tbl \
			  xmap_tab099.tbl \
			  xmap_tab100.tbl \
			  xmap_tab101.tbl \
			  xmap_tab102.tbl \
			  xmap_tab103.tbl \
			  xmap_tab104.tbl \
			  xmap_tab105.tbl \
			  xmap_tab106.tbl \
			  xmap_tab107.tbl \
			  xmap_tab108.tbl \
			  xmap_tab109.tbl \
			  xmap_tab110.tbl \
			  xmap_tab111.tbl \
			  xmap_tab112.tbl \
			  xmap_tab113.tbl \
			  xmap_tab114.tbl \
			  xmap_tab115.tbl \
			  xmap_tab116.tbl \
			  xmap_tab117.tbl \
			  xmap_tab118.tbl \
			  xmap_tab119.tbl \
			  xmap_tab120.tbl \
			  xmap_tab121.tbl \
			  xmap_tab122.tbl \
			  xmap_tab123.tbl \
			  xmap_tab124.tbl \
			  xmap_tab125.tbl \
			  xmap_tab126.tbl \
			  xmap_tab127.tbl \
			  xmap_tab128.tbl \
			  xmap_tab129.tbl \
			  xmap_tab130.tbl \
			  xmap_tab131.tbl \
			  xmap_tab132.tbl \
			  xmap_tab133.tbl \
			  xmap_tab134.tbl \
			  xmap_tab135.tbl \
			  xmap_tab136.tbl \
			  xmap_tab137.tbl \
			  xmap_tab138.tbl \
			  xmap_tab139.tbl \
			  xmap_tab140.tbl \
			  xmap_tab141.tbl \
			  xmap_tab142.tbl \
			  xmap_tab143.tbl \
			  xmap_tab144.tbl \
			  xmap_tab145.tbl \
			  xmap_tab146.tbl \
			  xmap_tab147.tbl \
			  xmap_tab148.tbl \
			  xmap_tab149.tbl \
			  xmap_tab150.tbl \
			  xmap_tab151.tbl \
			  xmap_tab152.tbl \
			  xmap_tab153.tbl \
			  xmap_tab154.tbl \
			  xmap_tab155.tbl \
			  xmap_tab156.tbl \
			  xmap_tab157.tbl \
			  xmap_tab158.tbl \
			  xmap_tab159.tbl \
			  xmap_tab160.tbl \
			  xmap_tab161.tbl \
			  xmap_tab162.tbl \
			  xmap_tab163.tbl \
			  xmap_tab164.tbl \
			  xmap_tab165.tbl \
			  xmap_tab166.tbl \
			  xmap_tab167.tbl \
			  xmap_tab168.tbl \
			  xmap_tab169.tbl \
			  xmap_tab170.tbl \
			  xmap_tab171.tbl \
			  xmap_tab172.tbl \
			  xmap_tab173.tbl \
			  xmap_tab174.tbl \
			  xmap_tab175.tbl \
			  xmap_tab176.tbl \
			  xmap_tab177.tbl \
			  xmap_tab178.tbl \
			  xmap_tab179.tbl \
			  xmap_tab180.tbl \
			  xmap_tab181.tbl \
			  xmap_tab182.tbl \
			  xmap_tab183.tbl \
			  xmap_tab184.tbl \
			  xmap_tab185.tbl \
			  xmap_tab186.tbl \
			  xmap_tab187.tbl \
			  xmap_tab188.tbl \
			  xmap_tab189.tbl \
			  xmap_tab190.tbl \
			  xmap_tab191.tbl \
			  xmap_tab192.tbl \
			  xmap_tab193.tbl \
			  xmap_tab194.tbl \
			  xmap_tab195.tbl \
			  xmap_tab196.tbl \
			  xmap_tab197.tbl \
			  xmap_tab198.tbl \
			  xmap_tab199.tbl \
			  xmap_tab200.tbl \
			  xmap_tab201.tbl \
			  xmap_tab202.tbl \
			  xmap_tab203.tbl \
			  xmap_tab204.tbl \
			  xmap_tab205.tbl \
			  xmap_tab206.tbl \
			  xmap_tab207.tbl \
			  xmap_tab208.tbl \
			  xmap_tab209.tbl \
			  xmap_tab210.tbl \
			  xmap_tab211.tbl \
			  xmap_tab212.tbl \
			  xmap_tab213.tbl \
			  xmap_tab214.tbl \
			  xmap_tab215.tbl \
			  xmap_tab216.tbl \
			  xmap_tab217.tbl \
			  xmap_tab218.tbl \
			  xmap_tab219.tbl \
			  xmap_tab220.tbl \
			  xmap_tab221.tbl \
			  xmap_tab222.tbl \
			  xmap_tab223.tbl \
			  xmap_tab224.tbl \
			  xmap_tab225.tbl \
			  xmap_tab226.tbl \
			  xmap_tab227.tbl \
			  xmap_tab228.tbl \
			  xmap_tab229.tbl \
			  xmap_tab230.tbl \
			  xmap_tab231.tbl \
			  xmap_tab232.tbl \
			  xmap_tab233.tbl \
			  xmap_tab234.tbl \
			  xmap_tab235.tbl \
			  xmap_tab236.tbl \
			  xmap_tab237.tbl \
			  xmap_tab238.tbl \
			  xmap_tab239.tbl \
			  xmap_tab240.tbl \
			  xmap_tab241.tbl \
			  xmap_tab242.tbl \
			  xmap_tab243.tbl \
			  xmap_tab244.tbl \
			  xmap_tab245.tbl \
			  xmap_tab246.tbl \
			  xmap_tab247.tbl \
			  xmap_tab248.tbl \
			  xmap_tab249.tbl \
			  xmap_tab250.tbl \
			  xmap_tab251.tbl \
			  xmap_tab252.tbl \
			  xmap_tab253.tbl \
			  xmap_tab254.tbl \
			  xmap_tab255.tbl \
			  xmap_tab256.tbl \
			  xmap_tab257.tbl \
			  xmap_tab258.tbl \
			  xmap_tab259.tbl \
			  xmap_tab260.tbl \
			  xmap_tab261.tbl \
			  xmap_tab262.tbl \
			  xmap_tab263.tbl \
			  xmap_tab264.tbl \
			  xmap_tab265.tbl \
			  xmap_tab266.tbl \
			  xmap_tab267.tbl \
			  xmap_tab268.tbl \
			  xmap_tab269.tbl \
			  xmap_tab270.tbl \
			  xmap_tab271.tbl \
			  xmap_tab272.tbl \
			  xmap_tab273.tbl \
			  xmap_tab274.tbl \
			  xmap_tab275.tbl \
			  xmap_tab276.tbl \
			  xmap_tab277.tbl \
			  xmap_tab278.tbl \
			  xmap_tab279.tbl \
			  xmap_tab280.tbl \
			  xmap_tab281.tbl \
			  xmap_tab282.tbl \
			  xmap_tab283.tbl \
			  xmap_tab284.tbl \
			  xmap_tab285.tbl \
			  xmap_tab286.tbl \
			  xmap_tab287.tbl \
			  xmap_tab288.tbl \
			  xmap_tab289.tbl \
			  xmap_tab290.tbl \
			  xmap_tab291.tbl \
			  xmap_tab292.tbl \
			  xmap_tab293.tbl \
			  xmap_tab294.tbl \
			  xmap_tab295.tbl \
			  xmap_tab296.tbl \
			  xmap_tab297.tbl \
			  xmap_tab298.tbl \
			  xmap_tab299.tbl \
			  xmap_tab300.tbl \
			  xmap_tab301.tbl \
			  xmap_tab302.tbl \
			  xmap_tab303.tbl \
			  xmap_tab304.tbl \
			  xmap_tab305.tbl \
			  xmap_tab306.tbl \
			  xmap_tab307.tbl \
			  xmap_tab308.tbl \
			  xmap_tab309.tbl \
			  xmap_tab310.tbl \
			  xmap_tab311.tbl \
			  xmap_tab312.tbl \
			  xmap_tab313.tbl \
			  xmap_tab314.tbl \
			  xmap_tab315.tbl \
			  xmap_tab316.tbl \
			  xmap_tab317.tbl \
			  xmap_tab318.tbl \
			  xmap_tab319.tbl \
			  xmap_tab320.tbl \
			  xmap_tab321.tbl \
			  xmap_tab322.tbl \
			  xmap_tab323.tbl \
			  xmap_tab324.tbl \
			  xmap_tab325.tbl \
			  xmap_tab326.tbl \
			  xmap_tab327.tbl \
			  xmap_tab328.tbl \
			  xmap_tab329.tbl \
			  xmap_tab330.tbl \
			  xmap_tab331.tbl \
			  xmap_tab332.tbl \
			  xmap_tab333.tbl \
			  xmap_tab334.tbl \
			  xmap_tab335.tbl \
			  xmap_tab336.tbl \
			  xmap_tab337.tbl \
			  xmap_tab338.tbl \
			  xmap_tab339.tbl \
			  xmap_tab340.tbl \
			  xmap_tab341.tbl \
			  xmap_tab342.tbl \
			  xmap_tab343.tbl \
			  xmap_tab344.tbl \
			  xmap_tab345.tbl \
			  xmap_tab346.tbl \
			  xmap_tab347.tbl \
			  xmap_tab348.tbl \
			  xmap_tab349.tbl \
			  xmap_tab350.tbl \
			  xmap_tab351.tbl \
			  xmap_tab352.tbl \
			  xmap_tab353.tbl \
			  xmap_tab354.tbl \
			  xmap_tab355.tbl \
			  xmap_tab356.tbl \
			  xmap_tab357.tbl \
			  xmap_tab358.tbl \
			  xmap_tab359.tbl \
			  xmap_tab360.tbl \
			  xmap_tab361.tbl \
			  xmap_tab362.tbl \
			  xmap_tab363.tbl \
			  xmap_tab364.tbl \
			  xmap_tab365.tbl \
			  xmap_tab366.tbl \
			  xmap_tab367.tbl \
			  xmap_tab368.tbl \
			  xmap_tab369.tbl \
			  xmap_tab370.tbl \
			  xmap_tab371.tbl \
			  xmap_tab372.tbl \
			  xmap_tab373.tbl \
			  xmap_tab374.tbl \
			  xmap_tab375.tbl \
			  xmap_tab376.tbl \
			  xmap_tab377.tbl \
			  xmap_tab378.tbl \
			  xmap_tab379.tbl \
			  xmap_tab380.tbl \
			  xmap_tab381.tbl \
			  xmap_tab382.tbl \
			  xmap_tab383.tbl \
			  xmap_tab384.tbl \
			  xmap_tab385.tbl \
			  xmap_tab386.tbl \
			  xmap_tab387.tbl \
			  xmap_tab388.tbl \
			  xmap_tab389.tbl \
			  xmap_tab390.tbl \
			  xmap_tab391.tbl \
			  xmap_tab392.tbl \
			  xmap_tab393.tbl \
			  xmap_tab394.tbl \
			  xmap_tab395.tbl \
			  xmap_tab396.tbl \
			  xmap_tab397.tbl \
			  xmap_tab398.tbl \
			  xmap_tab399.tbl \
			  xmap_tab400.tbl \
			  xmap_tab401.tbl \
			  xmap_tab402.tbl \
			  xmap_tab403.tbl \
			  xmap_tab404.tbl \
			  xmap_tab405.tbl \
			  xmap_tab406.tbl \
			  xmap_tab407.tbl \
			  xmap_tab408.tbl \
			  xmap_tab409.tbl \
			  xmap_tab410.tbl \
			  xmap_tab411.tbl \
			  xmap_tab412.tbl \
			  xmap_tab413.tbl \
			  xmap_tab414.tbl \
			  xmap_tab415.tbl \
			  xmap_tab416.tbl \
			  xmap_tab417.tbl \
			  xmap_tab418.tbl \
			  xmap_tab419.tbl \
			  xmap_tab420.tbl \
			  xmap_tab421.tbl \
			  xmap_tab422.tbl \
			  xmap_tab423.tbl \
			  xmap_tab424.tbl \
			  xmap_tab425.tbl \
			  xmap_tab426.tbl \
			  xmap_tab427.tbl \
			  xmap_tab428.tbl \
			  xmap_tab429.tbl \
			  xmap_tab430.tbl \
			  xmap_tab431.tbl \
			  xmap_tab432.tbl \
			  xmap_tab433.tbl \
			  xmap_tab434.tbl \
			  xmap_tab435.tbl \
			  xmap_tab436.tbl \
			  xmap_tab437.tbl \
			  xmap_tab438.tbl \
			  xmap_tab439.tbl \
			  xmap_tab440.tbl \
			  xmap_tab441.tbl \
			  xmap_tab442.tbl \
			  xmap_tab443.tbl \
			  xmap_tab444.tbl \
			  xmap_tab445.tbl \
			  xmap_tab446.tbl \
			  xmap_tab447.tbl \
			  xmap_tab448.tbl \
			  xmap_tab449.tbl \
			  xmap_tab450.tbl \
			  xmap_tab451.tbl \
			  xmap_tab452.tbl \
			  xmap_tab453.tbl \
			  xmap_tab454.tbl \
			  xmap_tab455.tbl \
			  xmap_tab456.tbl \
			  xmap_tab457.tbl \
			  xmap_tab458.tbl \
			  xmap_tab459.tbl \
			  xmap_tab460.tbl \
			  xmap_tab461.tbl \
			  xmap_tab462.tbl \
			  xmap_tab463.tbl \
			  xmap_tab464.tbl \
			  xmap_tab465.tbl \
			  xmap_tab466.tbl \
			  xmap_tab467.tbl \
			  xmap_tab468.tbl \
			  xmap_tab469.tbl \
			  xmap_tab470.tbl \
			  xmap_tab471.tbl \
			  xmap_tab472.tbl \
			  xmap_tab473.tbl \
			  xmap_tab474.tbl \
			  xmap_tab475.tbl \
			  xmap_tab476.tbl \
			  xmap_tab477.tbl \
			  xmap_tab478.tbl \
			  xmap_tab479.tbl \
			  xmap_tab480.tbl \
			  xmap_tab481.tbl \
			  xmap_tab482.tbl \
			  xmap_tab483.tbl \
			  xmap_tab484.tbl \
			  xmap_tab485.tbl \
			  xmap_tab486.tbl \
			  xmap_tab487.tbl \
			  xmap_tab488.tbl \
			  xmap_tab489.tbl \
			  xmap_tab490.tbl \
			  xmap_tab491.tbl \
			  xmap_tab492.tbl \
			  xmap_tab493.tbl \
			  xmap_tab494.tbl \
			  xmap_tab495.tbl \
			  xmap_tab496.tbl \
			  xmap_tab497.tbl \
			  xmap_tab498.tbl \
			  xmap_tab499.tbl \
			  xmap_tab500.tbl \
			  xmap_tab501.tbl \
			  xmap_tab502.tbl \
			  xmap_tab503.tbl \
			  xmap_tab504.tbl \
			  xmap_tab505.tbl \
			  xmap_tab506.tbl \
			  xmap_tab507.tbl \
			  xmap_tab508.tbl \
			  xmap_tab509.tbl \
			  xmap_tab510.tbl \
			  xmap_tab511.tbl \
			  xmap_tab512.tbl \
			  xmap_tab513.tbl \
			  xmap_tab514.tbl \
			  xmap_tab515.tbl \
			  xmap_tab516.tbl \
			  xmap_tab517.tbl \
			  xmap_tab518.tbl \
			  xmap_tab519.tbl \
			  xmap_tab520.tbl \
			  xmap_tab521.tbl \
			  xmap_tab522.tbl \
			  xmap_tab523.tbl \
			  xmap_tab524.tbl \
			  xmap_tab525.tbl \
			  xmap_tab526.tbl \
			  xmap_tab527.tbl \
			  xmap_tab528.tbl \
			  xmap_tab529.tbl \
			  xmap_tab530.tbl \
			  xmap_tab531.tbl \
			  xmap_tab532.tbl \
			  xmap_tab533.tbl \
			  xmap_tab534.tbl \
			  xmap_tab535.tbl \
			  xmap_tab536.tbl \
			  xmap_tab537.tbl \
			  xmap_tab538.tbl \
			  xmap_tab539.tbl \
			  xmap_tab540.tbl \
			  xmap_tab541.tbl \
			  xmap_tab542.tbl \
			  xmap_tab543.tbl \
			  xmap_tab544.tbl \
			  xmap_tab545.tbl

## PKG_BUILD_INDEP
endif

## DEVELOPMENT
endif

include ../../../am/info.am

## =============================================================================
##
## $Log$
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
