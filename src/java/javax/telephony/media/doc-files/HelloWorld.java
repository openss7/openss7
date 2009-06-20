import javax.telephony.media.*;
public class HelloWorld extends BasicMediaService {

    public static void main(java.lang.String[] argv) {
	new HelloWorld().run();
    }
    /**
     * Wait for calls to be delivered to "HelloWorld",
     * and say "Hello" to the caller.
     */
    public void run() {
	try {
	    bindToServiceName(ConfigSpec.basicConfig, "HelloWorld");
	    play("Hello", 0, null, null);
	    release();
	} catch (Exception ex) {
	    System.err.println(ex);
	}
    }
}
