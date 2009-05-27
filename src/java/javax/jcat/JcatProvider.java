
package javax.jcat;

import javax.csapi.cc.jcc.*;

/**
  * JcatProvider interface extends the JccProvider interface. This
  * interface is expected to be used when advanced call control features
  * are desired. JcatProvider has the same finite state machine as
  * JccProvider.
  *
  * A JcatProvider is associated with JcatCall, JcatConnection and
  * JcatAddress which are extensions of corresponding JCC entities. In
  * addition, a JcatProvider also has JcatTerminal objects and
  * JcatTerminalConnection objects associated with it during a call.
  */
public interface JcatProvider extends JccProvider {
    /**
      * Returns a Set of JcatCall objects in which the JcatAddress
      * participates. The call are currently associated with the
      * JcatProvider. When a JcatCall moves into the JccCall.INVALID
      * state, the JcatProvider loses its reference to this JcatCall.
      * Therefore, all Calls returned by this method must either be in
      * the JccCall.ACTIVE state. This method returns an empty Set if
      * zero calls match the request.
      *
      * @param address  The address that participates in the calls
      */
    public java.util.Set getCalls(JcatAddress address);
    /**
      * Returns a Set of JcatTerminal objects administered by the
      * JcatProvider who's name satisfies the regular expression.
      *
      * @param nameRegex  denotes the regular expression (for the
      * purpose of this specification, the platform will use the Perl5
      * regular expressions).
      */
    public java.util.Set getTerminals(java.lang.String nameRegex);
    /**
      * This method returns a standard EventFilter which is implemented
      * by the JCAT platform. For all events that require filtering by
      * this EventFilter, apply the following:
      *
      * <ul>
      * <li>If the terminal name is matched, the filter returns the
      * value matchDisposition.
      * <li>If the terminal name is not matched, then return
      * nomatchDisposition.
      * </ul>
      *
      * @param terminalNameRegex  denotes the regular expression (for
      * the purpose of this specification, the platform will use the
      * Perl5 regular expressions).
      * @param matchDisposition  indicates the disposition of the
      * JcatAddressEvent.TERMINAL_REGISTERED event with equal cause
      * code. This should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition  indicates the disposition the
      * JcatAddressEvent.TERMINAL_REGISTERED event with other cause
      * code. This should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return EventFilter standard EventFilter provided by the JCAT
      * platform to enable filtering of events based on the
      * application's requirements.
      *
      * @exception ResourceUnavailableException  An internal resource
      * for completing this call is unavailable.
      * @exception InvalidArgumentException  One or more of the provided
      * argument is not valid
      *
      * @since 1.0
      */
    public EventFilter createEventFilterRegistration(java.lang.String terminalNameRegex, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
