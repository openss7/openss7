/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.ss7.inap.operation;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * This class represents the ManageTriggerData Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ManageTriggerData  extends Operation   implements java.io.Serializable
{
    private ActionIndicator actionIndicator;
    private TriggerDataIdentifier triggerDataIdentifier;
    private String registratorIdentifier;
    private boolean isRegistratorIdentifier = false ;
    private ExtensionField extensions[];
    private boolean isExtensions = false ;         
    /**
      * Constructor For ManageTriggerData.
      * @param actionIndicator
      * @param triggerDataIdentifier
      */
    public ManageTriggerData(ActionIndicator actionIndicator, TriggerDataIdentifier triggerDataIdentifier) {
        operationCode = OperationCode.MANAGE_TRIGGER_DATA;
        setActionIndicator(actionIndicator);
        setTriggerDataIdentifier(triggerDataIdentifier);
    }
    /** Gets Operation Code.
      * @return
      * The operation code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets the Action Indicator.
      * @return
      * One of the following: <ul>
      * <li>ACTIVATE - gets an activated  TDP
      * <li>DEACTIVATE - gets a deactivated TDP
      * <li>RETRIEVE - retrieves the status of a TDP </ul>
      */
    public ActionIndicator getActionIndicator() { 
        return actionIndicator;
    }
    /** Sets Action Indicator.
      * @param actionIndicator
      * The action indicator to set.
      */
    public void setActionIndicator(ActionIndicator actionIndicator) {
        this.actionIndicator = actionIndicator;
    }
    /** Gets Trigger Data Identifier.
      * @return
      * The trigger data identifier. */
    public TriggerDataIdentifier getTriggerDataIdentifier() {
        return triggerDataIdentifier;
    }
    /** Sets Trigger Data Identifier.
      * @param triggerDataIdentifier
      * The trigger data identifier. */
    public void setTriggerDataIdentifier(TriggerDataIdentifier triggerDataIdentifier) {
        this.triggerDataIdentifier = triggerDataIdentifier;
    }
    /** Gets Registrator Identifier.
      * @return
      * The registration identifier. */
    public java.lang.String getRegistratorIdentifier()
        throws ParameterNotSetException {
        if(isRegistratorIdentifierPresent())
            return registratorIdentifier;
        throw new ParameterNotSetException();
    }
    /** Sets Registrator Identifier.
      * @param registratorIdentifier
      * The registration identifier. */
    public void setRegistratorIdentifier(java.lang.String registratorIdentifier) {
        this.registratorIdentifier = registratorIdentifier;
        isRegistratorIdentifier = true;
    }
    /** Indicates if the Registrator Identifier optional parameter is
      * present.
      * @return
      * True if present, false otherwise.  */
    public boolean isRegistratorIdentifierPresent() {
        return isRegistratorIdentifier;
    }
    /** Gets Extensions Parameter.
      * @return
      * The extensions parameter. */
    public ExtensionField[] getExtensions()
        throws ParameterNotSetException {
        if(isExtensionsPresent())
            return extensions;
        throw new ParameterNotSetException();
    }
    /** Gets a particular Extension Parameter.
      * @param index
      * Index of the extension parameter.
      * @return
      * The indexed extension parameter. */
    public ExtensionField getExtension(int index) {
        return extensions[index];
    }
    /** Sets Extensions Parameter.
      * @param extensions
      * The extensions to set. */
    public void  setExtensions (ExtensionField extensions[]) {
        this.extensions = extensions;
        isExtensions = true;
    }
    /** Sets a particular Extensions Parameter.
      * @param index
      * Index of the extension parameter. */
    public void setExtension(int index, ExtensionField extension) {
        this.extensions[index] = extension;
    }
    /** Indicates if the Extensions optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isExtensionsPresent() {
        return isExtensions;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
