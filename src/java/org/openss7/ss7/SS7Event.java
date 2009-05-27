
package org.openss7.ss7;

public abstract class SS7Event extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {
    public SS7Event(java.lang.Object source) {
	super(source);
    }
    public void setSource(java.lang.Object source) {
	this.source = source;
    }
    public abstract int getPrimitiveType();
}
