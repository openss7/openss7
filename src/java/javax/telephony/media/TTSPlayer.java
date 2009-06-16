/*
 * TTSPlayer.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media;

import java.util.Dictionary;

/**
 * Extensions of Player to support Text-To-Speech (TTS) coders.
 * <p>
 * Text-to-speech data may be encoded in Speech API (SAPI)
 * or Telephone Device for the Deaf (TDD) formats.
 * <p>
 * <h3>Container_CoderType</h3>
 * TTS data is stored in a MDO with parameter Container_CoderType
 * having a value of: 
 * <ul><li>
 * <tt>Container_ASCII</tt>,
 * </li><li>
 * <tt>Container_SAPI</tt>,
 * </li><li>
 * <tt>Container_TDD</tt>, 
 * </li><li>
 * <tt>Container_Unicode</tt>, 
 * </li></ul>
 * or other textual speech encoding.

 *
 * <h3> TTS Dictionary </h3>
 * Some TTS Player Resources (Players that support text-to-speech (TTS) coders)
 * have the ability to customize the pronunciation of text by the use of
 * loadable dictionaries. The dictionaries that may be loaded into such
 * a player resource are selected by the attribute
 * <tt>p_LoadedDictionary</tt>.  Dictionaries loaded in a Player
 * resource are arranged conceptually as a stack. They are loaded and
 * unloaded by the client in FIFO order. When input text is processed by
 * the player, the dictionaries are searched in stack order,
 * top-to-bottom, for pronunciation rules. The number of user
 * dictionaries a player may have loaded simultaneously is determined by
 * the value of the parameter <tt>p_DictionaryList</tt>.
 
 * User dictionaries are represented as TVM objects of type
 * <tt>Container_SAPI</tt>, and contain pronunciation rules, 
 * filter directives, and prosody directives.
 * This container format is discussed <a href="#SAPI Container">below</a>.


 * <p>
 * A TTS dictionary is loaded via the comand loadTTSDictionary().
 * <p>
 
 * When initially loaded, it is deactivated, and must be activated by
 * the command <tt>activateTTSDictionary()</tt> before its rules are
 * used by the player.  MDO objects encoded in TDD or SAPI formats have
 * as an additional <tt>ESymbol.Container_ContainerInfo</tt> information
 * element the language in which the data is written. The player is
 * expected to use this information to determine which of its loaded
 * dictionaries are to be used to play the object. 

 * <p>
 * The Media Data Object containing a TTS text
 * has additional information in its ContainerInfo parameter.
 * The value of ContainerInfo is a Dictionary 
 * with the following elements:
 * <table border="1" cellpadding="3">
 * <tr><td>
 * Key	</td><td>	Value Type	</td><td>Description
 * </td></tr><tr><td>
 * Language_Language	</td><td>Symbol	</td><td>the language in which the text is to be spoken.
 * One of many languages identified by <tt>ESymbol.Language_*</tt>
 * </td></tr><tr><td>
 * Language_Variant	</td><td>Symbol	</td><td>a few dialects
 * </td></tr><tr><td>
 * Language_SubVariant	</td><td>Symbol	</td><td>a few subdialects
 * </td></tr><tr><td>
 * Container_Sentences	</td><td>Integer</td><td>number of sentences in the text
 * </td></tr><tr><td>
 * Container_Duration	</td><td>Integer</td><td>time to play at normal speed in milliseconds
 * </td></tr></table>
 * <p>
 * The Media Data Object containing a TTS dictionary
 * has additional information available via getParameters().
 * The container parameters include
 * the following elements:
 * <table border="1" cellpadding="3">
 * <tr><td>
 * Key			</td><td>Value Type</td><td>Description
 * </td></tr><tr><td>
 * Player_Name		</td><td>String[]</td><td>A string
 * identifying the words/phrases that the dictionary contains.
 * These names are yet to be standardized, but are expected
 * to be formatted as scoped names, like ECTF Symbol names.
 * </td></tr><tr><td>
 * Player_WordList	</td><td>String[]</td><td>The actual words/phrases in the dictionary.
 * This is used for administrative and documentation.
 * The value of Player_Name is a shorthand way to identify this list.
 * </td></tr><tr><td>
 * Player_Size		</td><td>Integer </td><td>
 * A vendor specific heuristic indicating the in-resource space required.
 * </td></tr><tr><td>
 * Language_Language	</td><td>Symbol[]</td><td>
 * The language the dictionary pronounciations are for.
 * </td></tr><tr><td>
 * Language_Variant	</td><td>Symbol[]</td><td>
 * The language dialect(s) the dictionary pronounciations are for.
 * </td></tr><tr><td>
 * Language_SubVariant	</td><td>Symbol[]</td><td>
 * The language subdialect(s) the dictionary pronounciations are for.
 * </td></tr></table>
 *
 * <a name="SAPI Container"><!-- --></a>
 * <h3> SAPI support </h3>
 *
 * The coder type <tt>ESymbol.Container_SAPI</tt> is of particular note; 
 * this coder is conformant with the raw text format of the 
 * Speech API (SAPI) specification <a href="SAPIref">[SAPI]</a>.  
 * <p>
 * As mentioned above, containers in this format are also used
 * to store TTS dictionaries.
 * <p>
 * Data encoded according to SAPI consists of ASCII characters with
 * embedded escape sequences which specify volume, speed, pitch, mode
 * (grapheme or phonetic), or emphasis of a particular word or phoneme.
 * <p>
 * The TTS Player supports at least a minimum subset of the SAPI tags.
 * For details see: <a href="doc-files/SAPItags.html">SAPI tags for S.410</a>.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */

public interface TTSPlayer extends Player {

    // Need to get attributes for this, too.
    // a_LoadedDictionary

    /**
     * Load a collection of TTS dictionaries into a Player.
     * The dictionary is placed at the top of the dictionary stack,
     * and is deactivated.
     *
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a PlayerEvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    PlayerEvent loadTTSDictionary(String[] dictNames, Dictionary optargs)
	throws MediaResourceException;

    /**
     * Activates a collection of TTS dictionaries.
     * The dictionaries must already be loaded.
     *
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a PlayerEvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    
    PlayerEvent activateTTSDictionary(String[] dictNames, Dictionary optargs)
	throws MediaResourceException;

    /**
     * Deactivates a collection of loaded TTS dictionaries.
     * 
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a PlayerEvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    PlayerEvent deactivateTTSDictionary(String dictNames[], Dictionary optargs)
	throws MediaResourceException;
    /**
     * Unloads a collection of TTS dictionaries from a Player.
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a PlayerEvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    PlayerEvent unloadTTSDictionary(String dictNames[], Dictionary optargs)
	throws MediaResourceException;
}
