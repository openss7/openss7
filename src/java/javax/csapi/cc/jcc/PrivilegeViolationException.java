package javax.csapi.cc.jcc;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.1
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
    This exception indicates that an action pertaining to a certain object failed because
    the application did not have the proper security permissions to execute that command.
    <p>
    This class stores the type of privilege not available which is obtained via the {@link #getType()}
    method in this class. 
    @since 1.0b
*/
public class PrivilegeViolationException extends Exception {
    /**
     * This private variable stores the type of privilege not available.
     * @serial
     */
    private int _type;

    /**
        Constructor takes no string. 

        @param type kind of violation. 
    */
    public PrivilegeViolationException(int type )
    {
    	super();
    	_type=type;
    }
    /**
        Constructor takes a string. 

        @param type kind of violation. 
        @param s description of the violation. 
    */
    public PrivilegeViolationException(int type, String s )
    {
    	super(s);
    	_type=type;

    }

/**
    Returns the type of privilege which is not available. 
    @return The type of privilege. 
*/
    public int getType()
    {
        return _type; 
    }

/**
    A privilege violation occurred at the origination. 
*/
public static final int ORIGINATOR_VIOLATION = 0;
/**
    A privilege violation occurred at the destination. 
*/
public static final int DESTINATION_VIOLATION = 1;
/**
    A privilege violation occurred at an unknown place. 
*/
public static final int UNKNOWN_VIOLATION = 2;
}
