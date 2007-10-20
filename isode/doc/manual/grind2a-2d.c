...

static int  ros_lose (td)
struct TSAPdisconnect *td;
{
    if (td -> td_cc > 0)
	fatal ("TNetAccept: [%s] %*.*s", TErrString (td -> td_reason),
		td -> td_cc, td -> td_cc, td -> td_data);
    else
	fatal ("TNetAccept: [%s]", TErrString (td -> td_reason));
}
