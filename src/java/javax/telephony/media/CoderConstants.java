/*
 * CoderConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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


/**
 * Define the Symbol constants for Coders.
 * <p>
 * These are inherited into the Player and Recorder interfaces.
 * These are used as values for <tt>Player.a_Coder</tt> 
 * or <tt>Recorder.a_Coder</tt>.
 * <p>
 * Also used in the ContainerService methods.
 * <p>
 * <b>Note:</b> there are many more Symbols defined to
 * identify Coders and Languages (especially for TTS).
 * Consult and if necessary directly use Symbols of the form:
 * <tt>{@link ESymbol}.Container_<i>Coder</i></tt>
 * or
 * <tt>{@link ESymbol}.Language_<i>Language</i></tt>.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface CoderConstants {
    /* pre-release spellings, remove for *final* publication. */
    /** 24k ADPCM. @deprecated use v_24kADPCM */
    Symbol v_ADPCM_24k	 		= ESymbol.Container_24kADPCM;
    /** 32k ADPCM. @deprecated use v_32kADPCM */
    Symbol v_ADPCM_32k	 		= ESymbol.Container_32kADPCM;
    /** 44k ADPCM. */
    Symbol v_ADPCM_44k	 		= ESymbol.Container_44kADPCM;
    /** 48k ALawPCM. */
    Symbol v_ALawPCM_48k		= ESymbol.Container_48kALawPCM;
    /** 64k ALawPCM. */
    Symbol v_ALawPCM_64k		= ESymbol.Container_64kALawPCM;
    /** 88k ALawPCM. */
    Symbol v_ALawPCM_88k 		= ESymbol.Container_88kALawPCM;
    /** 48k MuLawPCM. */
    Symbol v_MuLawPCM_48k 		= ESymbol.Container_48kMuLawPCM;
    /** 64k MuLawPCM. */
    Symbol v_MuLawPCM_64k		= ESymbol.Container_64kMuLawPCM;
    /** 88k MuLawPCM. */
    Symbol v_MuLawPCM_88k		= ESymbol.Container_88kMuLawPCM;
    /** 48k 8Bit Linear. */
    Symbol v_Linear8Bit_48k		= ESymbol.Container_48k8BitLinear;
    /** 64k 8Bit Linear. */
    Symbol v_Linear8Bit_64k		= ESymbol.Container_64k8BitLinear;
    /** 88k 8Bit Linear. */
    Symbol v_Linear8Bit_88k		= ESymbol.Container_88k8BitLinear;

    /* S.100-R2: */
    /** 24k ADPCM. */
    Symbol v_24kADPCM	 		= ESymbol.Container_24kADPCM;
    /** 32k ADPCM. */
    Symbol v_32kADPCM	 		= ESymbol.Container_32kADPCM;
    /** 44k ADPCM. */
    Symbol v_44kADPCM	 		= ESymbol.Container_44kADPCM;
    /** 48k ALaw PCM. */
    Symbol v_48kALawPCM			= ESymbol.Container_48kALawPCM;
    /** 64k ALaw PCM. */
    Symbol v_64kALawPCM			= ESymbol.Container_64kALawPCM;
    /** 88k ALaw PCM. */
    Symbol v_88kALawPCM 		= ESymbol.Container_88kALawPCM;
    /** 48k MuLaw PCM. */
    Symbol v_48kMuLawPCM 		= ESymbol.Container_48kMuLawPCM;
    /** 64k MuLaw PCM. */
    Symbol v_64kMuLawPCM		= ESymbol.Container_64kMuLawPCM;
    /** 88k MuLaw PCM. */
    Symbol v_88kMuLawPCM		= ESymbol.Container_88kMuLawPCM;
    /** 48k 8Bit Linear. */
    Symbol v_48k8BitLinear		= ESymbol.Container_48k8BitLinear;
    /** 64k 8Bit Linear. */
    Symbol v_64k8BitLinear		= ESymbol.Container_64k8BitLinear;
    /** 88k 8Bit Linear. */
    Symbol v_88k8BitLinear		= ESymbol.Container_88k8BitLinear;

    /* These are for TTS (and there are more) */
    /** ADSI for TTS. */
    Symbol v_ADSI			= ESymbol.Container_ADSI;
    /** TDD for TTS. */
    Symbol v_TDD			= ESymbol.Container_TDD;
    /** ASCII for TTS. */
    Symbol v_ASCII			= ESymbol.Container_ASCII;
}
