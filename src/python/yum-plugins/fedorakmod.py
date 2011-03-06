#!/usr/bin/python

# fedorakmod.py - Fedora Kmod -- Yum Kernel Module Support
# Copyright 2006 - 2008 NC State University
# Written by Jack Neely <jjneely@ncsu.edu>
#
# SDG
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

import os
import rpmUtils
from yum import packages
from yum.constants import TS_INSTALL
from yum.plugins import TYPE_CORE
from rpm import RPMPROB_FILTER_OLDPACKAGE

requires_api_version = '2.4'
plugin_type = (TYPE_CORE,)

kernelProvides = set([ "kernel-%s" % a for a in rpmUtils.arch.arches.keys() ])

# We shouldn't need this if we didn't have to fake stuff so much
kernelVariants = ["bigmem", "enterprise", "smp", "hugemem", "PAE",
                  "guest", "hypervisor", "xen0", "xenU", "xen"]

def getRunningKernel():
    # Taken from the installonlyn.py plugin writen by Jeremy Katz
    # Copyright 2005  Red Hat, Inc. 
    # Modified by Jack Neely to return a kernel provides tuple
    """This takes the output of uname and figures out the (version, release)
    tuple for the running kernel."""
    ver = os.uname()[2]
    #for s in kernelVariants:
    #    if ver.endswith(s):
    #        ver = ver.replace(s, "")
    if ver.find("-") != -1:
        (v, r) = ver.split("-", 1)
        # XXX: Gah, this assumes epoch
        return ('kernel-%s' % os.uname()[4], 'EQ', ('0', v, r))
    return None

def _whatProvides(c, provides):
    """Return a list of POs of installed kernels."""

    bag = []
    
    rpmdb = c.getRpmDB()
    for i in provides:
        tuples = rpmdb.whatProvides(i, None, None)
        for pkgtuple in tuples:
            # XXX: what do we do for duplicate packages?
            #po = rpmdb.packagesByTuple(pkgtuple)[0]
            po = rpmdb.searchPkgTuple(pkgtuple)[0]
            bag.append(po)

    return bag

def _getKernelDeps(po, match):
      
    reqs = po.returnPrco(match)
    return [ r for r in reqs if r[0] in kernelProvides ]

def getInstalledKernels(c):
    return _whatProvides(c, kernelProvides)

def getInstalledModules(c):
    # "kernel-modules-for-kernel" is supposed to mark things installonly, see:
    # https://bugzilla.redhat.com/show_bug.cgi?id=482893
    return _whatProvides(c, ["kernel-modules"])

def getKernelProvides(po):
    """Pass in a package header.  This function will return a list of
       tuples (name, flags, ver) representing any kernel provides.
       Assumed that the PO is a kernel package."""
     
    return _getKernelDeps(po, "provides")

def getKernelReqs(po):
    """Pass in a package header.  This function will return a list of
       tuples (name, flags, ver) representing any kernel requires."""
      
    return _getKernelDeps(po, "requires")

def fakeName(po):
    """When Yum wont give us full PRCO information we yell
          "Say my name, bitch!"
       and fake it hard."""

    # Normally, I should be able to pull the <name>-kmod provide

    fields = po.name.split('-')
    if fields[0] == "kmod":
        del fields[0]
    if fields[-1] in kernelVariants:
        del fields[-1]

    return ('-'.join(fields + ['kmod']), 'EQ', 
            (po.epoch, po.version, po.release))

def resolveVersions(packageList):
    """The packageDict is a dict of pkgtuple -> PO
       We return a dict of kernel version -> list of kmod POs
          where the list contains only one PO for each kmod name"""

    pdict = {}
    for po in packageList:
        kernel = getKernelReqs(po)
        if len(kernel) == 0:
            print "Bad kmod package '%s' does not require a kernel" % po
            continue
        elif len(kernel) == 1:
            kernel = kernel[0]
        else:
            print "Bad kmod package: Must require only one kernel"
            continue

        # Figure out the real name of this kmod
        name = []
        for r in po.prco["provides"]:
            if r[0].endswith('-kmod'):
                name.append(r[0])
        if len(name) == 0:
            # Yum bug
            name = fakeName(po)
        elif len(name) != 1:
            print "Non compliant kmod package: %s" % po
            continue
        po.kmodName = name[0]

        if kernel not in pdict:
            pdict[kernel] = [po]
        else:
            sameName = None
            for tempPo in pdict[kernel]:
                if po.name == tempPo.name:
                    sameName = tempPo
                    break
            if sameName and packages.comparePoEVR(sameName, po) < 0:
                pdict[kernel].remove(sameName)
                pdict[kernel].append(po)
            elif sameName is None:
                pdict[kernel].append(po)

    return pdict

def installKernelModules(c, newModules, installedModules):
    """Figure out what special magic needs to be done to install/upgrade
       this kernel module.  This doesn't actually initiate an install
       as the module is already in the package sack to be applied."""

    tsInfo = c.getTsInfo()

    for modpo in newModules:
        c.info(4, "Installing kernel module: %s" % modpo.name)
        # Should only ever be 1 element to this list
        te = tsInfo.getMembers(modpo.pkgtup)[0] 
        tsCheck(te)

        kernelReqs = getKernelReqs(modpo)
        instPkgs = filter(lambda p: p.name == modpo.name, installedModules)
        for po in instPkgs:
            instKernelReqs = getKernelReqs(po)

            for r in kernelReqs:
                if r in instKernelReqs:
                    # we know that an incoming kernel module requires the
                    # same kernel as an already installed moulde of the
                    # same name.  "Upgrade" this module instead of install.
                    tsInfo.addErase(po)
                    c.info(2, 'Removing kernel module %s upgraded to %s' %
                           (po, modpo))
                    break

def pinKernels(c, newKernels, installedKernels, modules):
    """If we are using kernel modules, do not upgrade/install a new 
       kernel until matching modules are available."""
    
    runningKernel = getRunningKernel()
    if runningKernel is None:
        c.error(2, "Could not parse running kernel version.")
        return

    iKernels = [ getKernelProvides(p)[0] for p in installedKernels ]
    if runningKernel not in iKernels:
        # We have no knowledge of the running kernel -- its not installed
        # perhaps this is the anaconda %post environment or somebody
        # rpm -e'd the currently running kernel.  Choose a reasonable
        # kernel to go with.  IE, the greatest EVR we see.
        topkpo = None
        for p in installedKernels:
            if topkpo is None or packages.comparePoEVR(topkpo, p) < 0:
                topkpo = p
        runningKernel = getKernelProvides(topkpo)[0]
        c.info(2, "Unknown running kernel.  Using %s instead." % \
               str(runningKernel))

    table = resolveVersions(modules)
    if runningKernel not in table:
        c.info(2, "Trying to mimic %s which has no kernel modules installed" \
               % str(runningKernel))
        return
        
    names = [ p.kmodName for p in table[runningKernel] ]
    c.info(2, "kmods in %s: %s" % (str(runningKernel), str(names)))
    for kpo in newKernels:
        prov = getKernelProvides(kpo)[0]
        if prov in table:
            kmods = [ po.kmodName for po in table[prov] ]
        else:
            kmods = []
        if set(kmods) != set(names):
            c.info(2, "Removing kernel %s from install set" % str(prov))
            # XXX: This wants a pkgtuple which will probably change RSN
            c.getTsInfo().remove(kpo.pkgtup)

def installAllKmods(c, avaModules, modules, kernels):
    plist = []
    names = []
    interesting = []

    rModules = resolveVersions(modules)
    for group in rModules.values():
        for po in group:
            if po.kmodName not in names:
                names.append(po.kmodName)

    rAvaModules = resolveVersions(avaModules)
    for group in rAvaModules.values():
        for po in group:
            if po.kmodName in names:
                interesting.append(po)

    # If We have stuff in the interesting list its most likely a kmod for
    # an older kernel and therefore will have a VR < a VR of a kmod that
    # may already be installed.  RPM doesn't like installing packages
    # older than what's already installed.  Tell it to shutup.
    # XXX: If we thought hard enough we could do this more exactly.
    if len(interesting) > 0:
        tsInfo = c.getTsInfo()
        tsInfo.probFilterFlags.append(RPMPROB_FILTER_OLDPACKAGE)

    table = resolveVersions(interesting + modules)
    
    for kernel in [ getKernelProvides(k)[0] for k in kernels ]:
        if kernel not in table:
            continue
        for po in table[kernel]:
            if po not in modules:
                c.getTsInfo().addTrueInstall(po)
                plist.append(po)

    return plist

def tsCheck(te):
    "Make sure this transaction element is sane."

    if te.ts_state == 'u':
        te.ts_state = 'i'
        te.output_state = TS_INSTALL

def init_hook(c):
    if hasattr(c, 'registerPackageName'):
        c.registerPackageName("yum-plugin-fedorakmod")
    c.info(3, "Loading Fedora Extras kernel module support.")

def postresolve_hook(c):

    avaModules = []
    for name in ("kernel-modules", "kernel-modules-for-kernel"):
        avaModules += c.getRepos().getPackageSack().searchProvides(name)
    newModules = []
    newKernels = []

    installedKernels = getInstalledKernels(c)
    installedModules = getInstalledModules(c)

    for te in c.getTsInfo().getMembers():
        isKernel = len(kernelProvides.intersection(te.po.provides_names)) > 0
        if te.ts_state == 'e' and isKernel:
            # If a kernel is set to be erased then we don't want to
            # consider it in our dep checking.
            c.info(2, "Excluding to be erased: " + str(te.po))
            installedKernels.remove(te.po)
        if te.ts_state not in ('i', 'u'):
            continue
        if ("kernel-modules" in te.po.provides_names or
            "kernel-modules-for-kernel" in te.po.provides_names):
            newModules.append(te.po)
            for po in avaModules:
                if te.po.pkgtup == po.pkgtup:
                    avaModules.remove(po)
        if isKernel:
            newKernels.append(te.po)

    # Install modules for all kernels
    if c.confInt('main', 'installforallkernels', default=1) != 0:
        moreModules = installAllKmods(c, avaModules, 
                                      newModules + installedModules,
                                      newKernels + installedKernels)
        newModules = newModules + moreModules

    # Pin kernels
    if c.confInt('main', 'pinkernels', default=0) != 0:
        pinKernels(c, newKernels, installedKernels, 
                   newModules + installedModules)

    # Upgrade/Install kernel modules
    installKernelModules(c, newModules, installedModules)
           
# vim:ts=4:expandtab 
