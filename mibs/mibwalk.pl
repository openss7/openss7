#!/usr/bin/perl -w
# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date$ by $Author$
#
# =============================================================================
eval 'exec perl -S $0 ${1+"@"}' if $running_under_some_shell;

use Tk;
use Tk::Xrm;
use Tk::Pretty;
use Tk::HList;
use Tk::Menu;

$rfont='-*-axhnr-*-*-*--*-100-*-*-*-*-*-*';
$bfont='-*-axhnb-*-*-*--*-100-*-*-*-*-*-*';

$rfont='-*-helvetica-medium-r-*--*-100-*-*-*-*-*-*';
$bfont='-*-helvetica-bold-r-*--*-100-*-*-*-*-*-*';

my $mw = MainWindow->new;
$mw->title("MIB Viewer");
$mw->geometry('800x600');
$mw->minsize(100,100);
$mw->optionAdd('*font'=>$rfont);
$mw->optionAdd('*.Button.*.font'=>$bfont);
$mw->optionAdd('*.Label.*.font'=>$bfont);
$mw->optionAdd('*BorderThickness'=>1);
$mw->optionAdd('*BorderWidth'=>1);
$mw->optionAdd('*Border'=>1);
$mw->optionAdd('*Scrollbar*Width'=>8);
$mw->optionAdd('*Menu*Width'=>2);
$mw->optionAdd('*Menu*Height'=>2);

$mw->Pixmap('doc',-file=>'/home/brian/perltk/mini-doc.xpm');
$mw->Pixmap('cbook',-file=>'/home/brian/perltk/mini-book1.xpm');
$mw->Pixmap('obook',-file=>'/home/brian/perltk/mini-book2.xpm');

my $fr = $mw->Frame(
	-relief=>'groove',
);
my $hl = $fr->Scrolled('HList',
	-scrollbars=>'osoe',
	-columns=>2,
);

my $mu = $mw->Menu(
	-type=>'menubar',
	-relief=>'groove',
);
my $fm = $mu->Menu(
	-type=>'normal',
	-title=>'File',
);
$fm->add('command',
	-label=>'New',
	-underline=>0,
	-accelerator=>'~N',
);
$fm->add('command',
	-label=>'Exit',
	-underline=>0,
	-command=>sub { exit },
	-accelerator=>'^X',
);
$mw->bind('<Control-x>', sub { exit });

$mw->configure(-menu=>$mu);

$fr->pack(
	-side=>'top',
	-expand=>'yes',
	-fill=>'both',
);

MainLoop;
