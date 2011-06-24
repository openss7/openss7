//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

var images = new Array;
images["blank"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAAAAAAAAACH5BAEAAAAALAAAAAAQABAAAAIOhI+py+0Po5y02ouzPgUAOw==";
images["obook"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI4nI+pB7DZhHAvSkmRvTcPEATcmDXhyFHAdKLYh7VpGYvYGmi1FXrr9Ot5DCtcoPH4gJBJ4rD5KAAAOw==";
images["obook-red"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook-red"] = "data:image/gif;base64,R0lGODlhEAAQAPIEAAAAAIsAAP8AAICAgP///wAAAAAAAAAAACH5BAEAAAUALAAAAAAQABAAAAM8WLrcvgC8FoWQs1qLmd5bVwBBAJ5dVJ4gBlwry41c3Ka1yQHE4OURQm8gel14w4jjxRsQRRTAc+KBUqkJADs=";
images["obook-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPIEAAAAAACAAACLAICAgP///wAAAAAAAAAAACH5BAEAAAUALAAAAAAQABAAAAM8WLrcvgC8FkOQs1qLmd5bVwCCAJ5dVJ4gBlwry41c3Ka1yQHE4OURQm8gel14w4jjxRsQRRTAc+KBUqkJADs=";
images["obook-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAEBAQL+/vwAAACH5BAEAAAMALAAAAAAQABAAAAI5nI+JAOoQRDts0igHE3lh+VUXJ5GgBZEpB6Gh2Y5sGLgqXHszG+uA9BPkEi1I4DhcFCuMh8UBVRQAADs=";
images["cbook-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPIEAAAAAEBAQICAgL+/v////wAAAAAAAAAAACH5BAEAAAUALAAAAAAQABAAAAM8WLrcvgC8FseQs1qLmd5bVwBBAJ5dVJ4gBlwry41c3Ka1yQGE4OURQk8gel14w4jjxRMQRRTAc+KBUqkJADs=";
images["ocols"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI8nA0Jxw0iQGhMWXreiwkPHQRJhGmQtThR+LBTFZ3QqkpiSMPblmfrHeghYkThB2V5MYC2kLKieHqc0kEBADs=";
images["ccols"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI6nA0Jxw0iQGhMWUXfizDO6gVS4DncWYLjmB4atLVIyErmFttqIPLy8ELpXBfLh9FL8o4VBZNiWD4HBQA7";
images["ocols-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI8nA0Jxw0iQGhMWXreiwkPHQRJhGmQtThR+LBTFZ3QqkpiSMPblmfrHeghYkThB2V5MYC2kLKieHqc0kEBADs=";
images["ccols-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nA0Jxw0iQGhMWUXfizDO6gVS4DncWYLjmB4atLVIyErmFttqIPLy8ELpXBfLh9FL8o4VBZNiWD4HBQA7";
images["ocols-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI8nA0Jxw0RgGhMWXreiwkPLQhJhGmQtThR+LBTFZ3QqkpiSMPblmfrLeghYkThB2V5MYC2kLKieHqc0kEBADs=";
images["ccols-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nA0Jxw0RgGhMWUXfizDO6gmS4DncWYLjmB4atLVIyErmFtuqIPLy8ELpXBfLh9FL8o4VBZNiWD4HBQA7";
images["odocs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI2nI+ZwA16gJiTwSGDFvIpSXHNJ2ibBgRLGKaraXIqArLyCwc3bVdzVMrtgD0XzYH8HXK5i7MAADs=";
images["cdocs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+ZwO0KhJwShCWC3sJiOnlItHHiEYHVNWZl0LGoumIvfBqpmg/kG/vwZDqHkWi4lRRMRAEAOw==";
images["odocs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI2nI+ZwA16gJiTwSGDFvIpSXHNJ2ibBgRLGKaraXIqArLyCwc3bVdzVMrtgD0XzYH8HXK5i7MAADs=";
images["cdocs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+ZwO0KhJwShCWC3sJiOnlItHHiEYHVNWZl0LGoumIvfBqpmg/kG/vwZDqHkWi4lRRMRAEAOw==";
images["orows"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwA16gJiTwSGDFvIpSXHNJ2ibBgQJGFaq4cQxzJl2kKYY11K5KgvqaLdATRPpuVbCIaJou0gHBQA7";
images["crows"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+JwO0KhJwShBSD3sJiSnmLsHHiEYHVZTiu23ZlaXlRqtbXy5/ZLNOgVCtM73SYzRTMRAEAOw==";
images["ohost"] = "data:image/gif;base64,R0lGODlhEAAQAPIAAAAAAICAgL+/v/X19QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAQABAAAAM+SLrcCzBKwIC4OAuqrFvbIwxkqQxhNzIlyhHWwJ4pPLZt7X2Erv0v2GTSoAQIxySsgmwqjxyjcwoVDq+8bAIAOw==";
images["chost"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAICAgAAAAAAAACH5BAEAAAIALAAAAAAQABAAAAI0lI95wO2AQJi0Bmjkc3fdzXTZB4qCBmpeqo6sKVkWlrF0JgT5rt+QDuSdcMHiDmJ7KJaIAgA7";
images["ohost-red"] = "data:image/gif;base64,R0lGODlhEAAQAPIAAAAAAIsAAP8AAPX19QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAQABAAAANCSLrcGzDKwIK4OAuqrFvbIwxkqQxhNzIlyhHWwJ4pPLZt7X2Erv0v2GTSAGyOFguAYUwij0tC8+lUSgHYrFbL6yYAADs=";
images["chost-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAIsAAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI4nI95wO2AgJi0Cmhk2JzfJXTil4Wihw3auZGqybprnEqWlarPk2jhH3C9gkQgyVdMfnZMhuKJKAAAOw==";
images["ohost-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPIAAAAAAACAAACLAPX19QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAQABAAAANCSLrcKzBKwUS4OAeqrFvbEwxkqQxhNzIlyhHWwJ4pPLZt7X2Erv0v2GTSAGyOFguAYUwij0tC8+lUSgHYrFbL6yYAADs=";
images["chost-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI4nI95wO2AQJi0Bmik2JzfFXTil4Wihw3auZGqybprnEqWlarPk2jhL3C9gkQgyVdMfnZMhuKJKAAAOw==";
images["oinds"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI7nI+JwA16gJiTwSGDFvIpWTneImhMGQBBAlKUypbNBpPhu5KmGnB59GrVgDuVL+bCsUSO30EDjTovigIAOw==";
images["cinds"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwO0KhJwShBSD3sLiyYDXImiM1o1HRIlf0KAeWbHpC1vBTXf2vCrlLLxgq6IyOJbAw+a5UUgTBQA7";
images["oinds-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI7nI+JwA16gJiTwSGDFvIpWTneImhMGQBBAlKUypbNBpPhu5KmGnB59GrVgDuVL+bCsUSO30EDjTovigIAOw==";
images["cinds-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwO0KhJwShBSD3sLiyYDXImiM1o1HRIlf0KAeWbHpC1vBTXf2vCrlLLxgq6IyOJbAw+a5UUgTBQA7";
images["oinds-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI7nI+JwA16QJiTwSGFDvIpWTneEmhMKQBCAlKUypbNBpPhu5KmKnB59GrVgDuVL+bCsUSO30EDjTovigIAOw==";
images["cinds-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAI6nI+JwO0KgpwSiBSF3sHiyYDXEmiM1o1HRImf0KAeWbHpC1vCTXf2vCrlLLxgq6IyOJbAw+a5UUgTBQA7";
images["onote"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+py40Ao4wHiIulAMFYfH0W54FhtHXDZ4LkKgSyHAehatH1nJati5sIX4OZ8ahyKJfMQQEAOw==";
images["cnote"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI1nI+py40Ao4wHiIulAMFYfH0W54FhtHXDZ4LkKgSyHAehatH1nJati5sIX4OZ8ahyKJfMQQEAOw==";
images["oobjs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["cobjs"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["oobjs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["cobjs-red"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAM0AAP8AAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["oobjs-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHAgUrklzAArrzCHetwnHSG5iWgmmw5YJ2r6i97iM8ACuvdNFhkREAQA7";
images["cobjs-grn"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAACAAACLAAAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHAgUrklzAArrzCHetwnHSG5iWgmmw5YJ2r6i97iM8ACuvdNFhkREAQA7";
images["oobjs-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAEBAQL+/vwAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["cobjs-blu"] = "data:image/gif;base64,R0lGODlhEAAQAPEAAAAAAEBAQL+/vwAAACH5BAEAAAMALAAAAAAQABAAAAIunI+pywbfHBAUrkmzAArrzCHetwXHSG5iWgWmw5YJ2r6i97hM8ACuvdNFhkREAQA7";
images["oparm"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI4nI+pCLDZhHAvykkZuOHmYQndhR2bSI6TeUqq1Jhu6rE0DASBjOL6bvPpFpvG7wHKDZGGJfNpKAAAOw==";
images["cparm"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAI4nI+pCLDZhHAvykkZuOHmYQndhR2bSI6TeUqq1Jhu6rE0DASBjOL6bvPpFpvG7wHKDZGGJfNpKAAAOw==";
images["openn"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAItnG+By8o/HBygWvuA2FyA3HEfo4UeeGlAwCJlOC5vF7vmSabYFPGL5OsFE48CADs=";
images["openl"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIqnG+By8o/HBygWvuA2FyA3HEfo4UeeGlAwCJlOC5vF7vmSabYxPf+3ysAADs=";
images["openf"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIqnI+py70AY3yiWgHorVmBzVHSBwQm8oFdkm4rCmLPODmBk9z4oe9DjygAADs=";
images["openo"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIlnI+py70AY3yiWgHorVmBzVHSBwQm8oFdkm4rCmLPODn2jedKAQA7";
images["clsdn"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAItnG+By8o/HBygWvuA2FyAzGnbx4heCF4aELSICYMjWnb0ol5Tsi9SHwEefocCADs=";
images["clsdl"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIpnG+By8o/HBygWvuA2FyAzGnbx4heCF4aELSICYMjWnb0ol7Tzvd+XwAAOw==";
images["clsdf"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAIrnI+py70AY3yiWgFoBTYrvnWUxAHBiYCY+HVsol4eTE5O4CR4fuz84EMUAAA7";
images["clsdo"] = "data:image/gif;base64,R0lGODlhEAAQAPECAAAAAICAgP///wAAACH5BAEAAAMALAAAAAAQABAAAAImnI+py70AY3yiWgFoBTYrvnWUxAHBiYCY+HVsol4eTE7Ojee6UgAAOw==";
images["nonen"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAICAgAAAACH5BAEAAAEALAAAAAAQABAAAAIdjG+Ay8ofHFxynmpjRtiCD35QN5HjdqGJqqnmQxYAOw==";
images["nonel"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAICAgAAAACH5BAEAAAEALAAAAAAQABAAAAIZjG+Ay8ofHFxynmpjRtiCD37bSJbmiaZIAQA7";
images["nonef"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAICAgAAAACH5BAEAAAEALAAAAAAQABAAAAIUjI+py+0Po5yg2soA3VJ3HnnhUgAAOw==";
images["noneo"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAICAgAAAACH5BAEAAAEALAAAAAAQABAAAAIRjI+py+0Po5yg2jqz3rx7UwAAOw==";
images["contn"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAICAgAAAACH5BAEAAAEALAAAAAAQABAAAAIbjG+Ay8ofHFxynmpjRtj2+UHhMzbbdSapln4FADs=";
images["contl"] = "data:image/gif;base64,R0lGODlhEAAQAPAAAAAAAAAAACH5BAEAAAAALAAAAAAQABAAAAIOhI+py+0Po5y02ouzPgUAOw==";
images["backb"] = "data:image/gif;base64,R0lGODlhFAAWAPIAAAAAADMzM2ZmZpmZmQAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAUABYAAANLSLrc/oKE8CoZM1O7os7c9WmcN04WdoKQdBIANypAHG5YbS/7kus1RlDxA+p4xqSRpmwCKE7nINqMwKi6wEAY1VaS3tBV/OiRz4sEADs=";
images["backg"] = "data:image/gif;base64,R0lGODlhFAAWAPIAAHBwcKOjo9bW1vn5+QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAUABYAAANLSLrc/oKE8CoZM1O7os7c9WmcN04WdoKQdBIANypAHG5YbS/7kus1RlDxA+p4xqSRpmwCKE7nINqMwKi6wEAY1VaS3tBV/OiRz4sEADs=";
images["nextb"] = "data:image/gif;base64,R0lGODlhFAAWAPIAAAAAADMzM2ZmZpmZmQAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAUABYAAANOSLrc/iEQ8Vy8YdSF79xEh2mV6FGQeZEOEJrfA7gvBs5ApkgggeO6Bm3xKwZ9Q2Rx+eMwnzPFAApVCKhPaQDbbAy21N532Wsly4oz+pEAADs=";
images["nextg"] = "data:image/gif;base64,R0lGODlhFAAWAPIAAHBwcKOjo9bW1vn5+QAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAUABYAAANOSLrc/iEQ8Vy8YdSF79xEh2mV6FGQeZEOEJrfA7gvBs5ApkgggeO6Bm3xKwZ9Q2Rx+eMwnzPFAApVCKhPaQDbbAy21N532Wsly4oz+pEAADs=";

// images["blank"] = "mini_blank.gif";
// images["obook"] = "mini_book1.gif";
// images["cbook"] = "mini_book2.gif";
// images["obook-red"] = "mini_book1.gif";
// images["cbook-red"] = "mini_book3.gif";
// images["obook-grn"] = "mini_book1.gif";
// images["cbook-grn"] = "mini_book4.gif";
// images["obook-blu"] = "mini_book6.gif";
// images["cbook-blu"] = "mini_book5.gif";
// images["ocols"] = "mini_col1.gif";
// images["ccols"] = "mini_col2.gif";
// images["ocols-red"] = "mini_col3.gif";
// images["ccols-red"] = "mini_col4.gif";
// images["ocols-grn"] = "mini_col5.gif";
// images["ccols-grn"] = "mini_col6.gif";
// images["odocs"] = "mini_doc1.gif";
// images["cdocs"] = "mini_doc2.gif";
// images["odocs-red"] = "mini_doc3.gif";
// images["cdocs-red"] = "mini_doc4.gif";
// images["orows"] = "mini_doc5.gif";
// images["crows"] = "mini_doc6.gif";
// images["ohost"] = "mini_host1.gif";
// images["chost"] = "mini_host2.gif";
// images["ohost-red"] = "mini_host3.gif";
// images["chost-red"] = "mini_host4.gif";
// images["ohost-grn"] = "mini_host5.gif";
// images["chost-grn"] = "mini_host6.gif";
// images["oinds"] = "mini_ind1.gif";
// images["cinds"] = "mini_ind2.gif";
// images["oinds-red"] = "mini_ind3.gif";
// images["cinds-red"] = "mini_ind4.gif";
// images["oinds-grn"] = "mini_ind5.gif";
// images["cinds-grn"] = "mini_ind6.gif";
// images["onote"] = "mini_note.gif";
// images["cnote"] = "mini_note.gif";
// images["oobjs"] = "mini_obj1.gif";
// images["cobjs"] = "mini_obj1.gif";
// images["oobjs-red"] = "mini_obj2.gif";
// images["cobjs-red"] = "mini_obj2.gif";
// images["oobjs-grn"] = "mini_obj3.gif";
// images["cobjs-grn"] = "mini_obj3.gif";
// images["oobjs-blu"] = "mini_obj4.gif";
// images["cobjs-blu"] = "mini_obj4.gif";
// images["oparm"] = "mini_parm.gif";
// images["cparm"] = "mini_parm.gif";
// images["openn"] = "mini_open.gif";
// images["openl"] = "mini_open_last.gif";
// images["openf"] = "mini_open_first.gif";
// images["openo"] = "mini_open_only.gif";
// images["clsdn"] = "mini_closed.gif";
// images["clsdl"] = "mini_closed_last.gif";
// images["clsdf"] = "mini_closed_first.gif";
// images["clsdo"] = "mini_closed_only.gif";
// images["nonen"] = "mini_none.gif";
// images["nonel"] = "mini_none_last.gif";
// images["nonef"] = "mini_none_first.gif";
// images["noneo"] = "mini_none_only.gif";
// images["contn"] = "mini_cont.gif";
// images["contl"] = "mini_cont_last.gif";

function getMenuFrame() {
	var menuframe = top.frames["menuframe"];
	if (menuframe == null) {
		alert("Menu frame disappeared.");
	}
	return menuframe;
}

function getMenuDivision() {
	var menudivision = null;
	var menuframe = getMenuFrame();
	if (menuframe != null) {
		menudivision = menuframe.document.getElementById("myMenuDivision");
		if (menudivision == null) {
			alert("Menu division disappeared.");
		}
	}
	return menudivision;
}

function getDataFrame() {
	var dataframe = top.frames["dataframe"];
	if (dataframe == null) {
		alert("Data frame disappeared.");
	}
	return dataframe;
}

function getDataDivision() {
	var datadivision = null;
	var dataframe = getDataFrame();
	if (dataframe != null) {
		datadivision = dataframe.document.getElementById("myDataDivision");
		if (datadivision == null) {
			alert("Data division disappeared.");
		}
	}
	return datadivision;
}

function writeem() {
	document.writeln("<TABLE CELLPADDING=0 CELLSPACING=0 BORDER=0>");
	for (image in images) {
		document.writeln("	<TR ID=\"" + image + "\">");
		document.writeln("		<TD VALIGN=\"MIDDLE\"><IMG BORDER=0 SRC=\"" + images[image] + "\" ALT=\"" + image + "\"></TD>");
		document.writeln("		<TD VALIGN=\"MIDDLE\" COLSPAN=20><FONT SIZE=-2>&nbsp;" + image + "</FONT></TD>");
		document.writeln("	</TR>");
	}
	document.writeln("</TABLE>");
}

var tree = new Array;
var tcs = new Array;

var prev = null;

function findMib(oidOrName) {
	var mib = tree[oidOrName];
	if (mib == null) {
		alert("Could not find Mib for: " + oidOrName);
	}
	return mib;
}

function MyTextualConvention(name, desc) {
	this.name = name;
	this.desc = desc;
	tcs[name] = this;
}

function findTags(desc,topName) {
	var words = desc.split(" ");
	var i; var word;
	for (i in words) {
		word = words[i];
		tag = word;
		tag = tag.replace(",","");
		tag = tag.replace("<p>","");
		tag = tag.replace("</p>","");
		tag = tag.replace("<li>","");
		tag = tag.replace("</li>","");
		tag = tag.replace(";","");
		tag = tag.replace(":","");
		tag = tag.replace(".","");
		if (tree[tag] != null) {
			word = "<a href='javascript:"+topName+".onTagClick(\"" + tag + "\")'>" + word + "</a>";
			words[i] = word;
		}
	}
	desc = words.join(" ");
	return desc;
}

function Mib(oid, name, sid, mid, mtype, access, status, syntax, desc, refs, image, color, superior) {
	this.SNMP = new Object;
	this.SNMP.next = null;
	this.SNMP.prev = prev;
	if (prev != null) { prev.SNMP.next = this; }
	prev = this;
	this.SNMP.objectID = "." + oid + "";
	this.SNMP.label = name;
	this.SNMP.subID = sid;
	this.SNMP.moduleID = mid;
	if (mtype != "") this.SNMP.type = mtype;
	if (access != "") this.SNMP.access = access;
	if (status != "") this.SNMP.status = status;
	if (syntax != "") this.SNMP.syntax = syntax;
	if (desc != "") this.SNMP.description = desc;
	if (refs != "") this.SNMP.reference = refs;
	this.oid = oid;
	tree[oid] = this;
	tree[name] = this;
	if (mid != "" && tree[mid] == null) { tree[mid] = this; }
	this.name = name;
	this.numchildren = 0;
	this.children = new Array;
	this.last = true;
	this.level = 0;
	this.superior = superior;
	this.image = image;
	this.color = color;
	this.state = "c";
	if (superior != null) {
		superior.addChild(this);
	}
	this.addChild = function(child) {
		if (this.numchildren > 0) {
			this.children[this.numchildren - 1].last = false;
		}
		this.children[this.numchildren] = child;
		this.numchildren = this.numchildren + 1;
		child.level = this.level + 1;
		child.last = true;
	}
	this.getCtrlName = function() {
		if (this.superior == null) {
			if (this.numchildren > 0) {
				if (this.last == true)
				{ if (this.state == "o") { iname = "openo"; } else { iname = "clsdo"; } } else
				{ if (this.state == "o") { iname = "openf"; } else { iname = "clsdf"; } }
			} else {
				if (this.last == true) { iname = "noneo"; } else { iname = "nonef"; }
			}
		} else {
			if (this.numchildren > 0) {
				if (this.last == true)
				{ if (this.state == "o") { iname = "openl"; } else { iname = "clsdl"; } } else
				{ if (this.state == "o") { iname = "openn"; } else { iname = "clsdn"; } }
			} else {
				if (this.last == true) { iname = "nonel"; } else { iname = "nonen"; }
			}
		}
		return iname;
	}
	this.getContName = function(obj) {
		if (obj.last == true) { iname = "contl"; } else { iname = "contn"; }
		return iname;
	}
	this.getIconName = function() {
		iname = ""+this.state+this.image+this.color+"";
		return iname;
	}
	this.returnChildrenHTML = function(topName) {
		if (topName == null) { topName = "top"; }
		html = "";
		if (this.numchildren > 0) {
			for (child in this.children) {
				html = html + this.children[child].returnHTML(topName);
			}
		}
		return html;
	}
	this.returnHTML = function(topName) {
		if (topName == null) { topName = "top"; }
		html = "";
		html = html + "<div id=\"" + this.oid + "\">\n";
		html = html + "\t<table border=0 cellpadding=0 cellspacing=0>\n";
		html = html + "\t\t<tr>\n";
		for (i=0, obj = this, spacers=""; obj.superior != null; i++) {
			obj = obj.superior;
			spacers = "\t\t\t<td valign=\"middle\"><img src=\"" + images[this.getContName(obj)] + "\" onmouseover='this.style.backgroundColor = \"cyan\"' onmouseout='this.style.backgroundColor = \"transparent\"' onclick='"+topName+".onImageClick(\""+obj.oid+"\")' alt=\""+obj.name+"("+obj.oid+")\" title=\""+obj.name+"("+obj.oid+")\"></td>\n" + spacers;
		}
		html = html + spacers;
		html = html + "\t\t\t<td valign=\"middle\"><img id=\"" + this.oid + ".ctrl\" src=\"" + images[this.getCtrlName()] + "\" onmouseover='this.style.backgroundColor = \"cyan\"' onmouseout='this.style.backgroundColor = \"transparent\"' onclick='"+topName+".onImageClick(\""+this.oid+"\")' alt=\""+this.name+"("+this.oid+")\" title=\""+this.name+"("+this.oid+")\"></td>\n";
		html = html + "\t\t\t<td valign=\"middle\"><img id=\"" + this.oid + ".icon\" src=\"" + images[this.getIconName()] + "\" onmouseover='this.style.backgroundColor = \"cyan\"' onmouseout='this.style.backgroundColor = \"transparent\"' onclick='"+topName+".onImageClick(\""+this.oid+"\")' alt=\""+this.name+"("+this.oid+")\" title=\""+this.name+"("+this.oid+")\"></td>\n";
		html = html + "\t\t\t<td valign=\"middle\" span=\"" + (30-i-2) + "\"><span id=\"" + this.oid + ".text\" onmouseover='this.style.backgroundColor = \"cyan\"' onmouseout='this.style.backgroundColor = "+topName+".transparentColor(\""+this.oid+"\")' ondblclick='"+topName+".onElementDoubleClick(\""+this.oid+"\")' onclick='"+topName+".onElementClick(\""+this.oid+"\")' title=\""+this.name+"("+this.oid+")\"><font size=-2>&nbsp;" + this.name + "</font></span></td>\n";
		html = html + "\t\t</tr>\n";
		html = html + "\t</table>\n";
		html = html + "</div>\n";
		html = html + "<div id=\"" + this.oid + ".children\">\n";
		if (this.state == "o") {
			html = html + this.returnChildrenHTML(topName);
		}
		html = html + "</div>\n";
		return html;
	}
	this.returnDataHTML = function(topName) {
		if (topName == null) { topName = "top"; }
		if (this.SNMP == null) { alert("Object " + this.oid + " SNMP sub-object disappeared!"); return; }
		html = "";
		html = html + "<table width=\"100%\">\n";
		html = html + "\t<tr>\n";
		html = html + "\t\t<th>\n";
		if (historySelection > 0) {
			html = html + "\t\t\t<a href='javascript:" + topName + ".historyBack()'><img src=\""+images["backb"]+"\"></a>\n";
		} else {
			html = html + "\t\t\t<img src=\""+images["backg"]+"\">\n";
		}
		html = html + "\t\t</th>\n";
		html = html + "\t\t<th>\n";
		selections = "";
		for (i in selectionHistory) {
			i = 1 * i;
			j = i + 1;
			if (i == historySelection) {
				selections = selections +  j + "&nbsp;";
			} else {
				selections = selections + "<a href='javascript:" + topName + ".historySelect(" + i + ")'>" + j + "</a>&nbsp;";
			}
		}
		html = html + "\t\t\t" + selections + "\n";
		html = html + "\t\t</th>\n";
		html = html + "\t\t<th>\n";
		if (historySelection < selectionHistory.length - 1) {
			html = html + "\t\t\t<a href='javascript:" + topName + ".historyForward()'><img src=\""+images["nextb"]+"\"></a>\n";
		} else {
			html = html + "\t\t\t<img src=\""+images["nextg"]+"\">\n";
		}
		html = html + "\t\t</th>\n";
		html = html + "\t</tr>\n";
		html = html + "</table>\n";
		html = html + "<table border=2 cellpadding=2 cellspacing=2 width=\"100%\">\n";
		html = html + "\t<tr><th width=\"50\" align=\"right\"><font size=\"-1\">Attribute</font></th><th align=\"left\"><font size=\"-1\">Value</font></th></tr>\n";
		if (this.SNMP.moduleID != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Module:</font></th> <td><font size=\"-1\"><a href='javascript:" + topName + ".onTagClick(\"" + this.SNMP.moduleID + "\")'>" + this.SNMP.moduleID + "</a></font></td></tr>\n";
		if (this.SNMP.label != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Label:</font></th>  <td><font size=\"-1\">" + this.SNMP.label +             "</font></td></tr>\n";
		if (this.SNMP.objectID != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Object:</font></th> <td><font size=\"-1\">" + this.SNMP.objectID +          "</font></td></tr>\n";
		if (this.SNMP.next != null)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Next:</font></th> <td><font size=\"-1\"><a href='javascript:"+topName+".onTagClick(\"" + this.SNMP.next.oid + "\")'>" + this.SNMP.next.SNMP.label + "</a></font></td></tr>\n";
		if (this.SNMP.prev != null)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Previous:</font></th> <td><font size=\"-1\"><a href='javascript:"+topName+".onTagClick(\"" + this.SNMP.prev.oid + "\")'>" + this.SNMP.prev.SNMP.label + "</a></font></td></tr>\n";
		if (this.superior != null)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Parent:</font></th> <td><font size=\"-1\"><a href='javascript:" + topName + ".onTagClick(\"" + this.superior.oid + "\")'>" + this.superior.SNMP.label + "</a></font></td></tr>\n";
		indexes = "";
		if (this.SNMP.indexes != undefined) {
			indArray = this.SNMP.indexes.split(", ");
			for (ind in indArray) {
				index = indArray[ind];
				if (index != "" && tree[index] != null) {
					if (indexes != "") { indexes = ""+indexes+", "; }
					indexes = indexes + "<a href='javascript:" + topName + ".onTagClick(\"" + index + "\")'>" + index + "</a>";
				}
			}
		}
		if (indexes != "")
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Indexes:</font></th><td><font size=\"-1\">" + indexes +                    "</font></td></tr>\n";
		children = "";
		if (this.numchildren > 0) {
			for (ind in this.children) {
				child = this.children[ind];
				if (child != null && tree[child.oid] != null) {
					if (children != "") { children = ""+children+", "; }
					children = children + "<a href='javascript:" + topName + ".onTagClick(\"" + child.oid + "\")'>" + child.name + "</a>";
				}
			}
		}
		if (children != "")
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Children:</font></th><td><font size=\"-1\">" + children +                  "</font></td></tr>\n";
		if (this.SNMP.type != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Type:</font></th>   <td><font size=\"-1\">" + this.SNMP.type +              "</font></td></tr>\n";
		if (this.SNMP.access != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Access:</font></th> <td><font size=\"-1\">" + this.SNMP.access +            "</font></td></tr>\n";
		if (this.SNMP.status != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Status:</font></th> <td><font size=\"-1\">" + this.SNMP.status +            "</font></td></tr>\n";
		if (this.SNMP.syntax != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Syntax:</font></th> <td><font size=\"-1\">" + this.SNMP.syntax +            "</font></td></tr>\n";
		if (this.SNMP.ranges != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Ranges:</font></th> <td><font size=\"-1\">" + this.SNMP.ranges +            "</font></td></tr>\n";
		if (this.SNMP.enums != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Enums:</font></th>  <td><font size=\"-1\">" + this.SNMP.enums +             "</font></td></tr>\n";
		if (this.SNMP.defaultValue != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Default:</font></th><td><font size=\"-1\">" + this.SNMP.defaultValue +      "</font></td></tr>\n";
		if (this.SNMP.units != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Units:</font></th>  <td><font size=\"-1\">" + this.SNMP.units +             "</font></td></tr>\n";
		if (this.SNMP.hint != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Hint:</font></th>   <td><font size=\"-1\">" + this.SNMP.hint +              "</font></td></tr>\n";
		if (this.SNMP.augment != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Augment:</font></th><td><font size=\"-1\">" + this.SNMP.augment +           "</font></td></tr>\n";
		if (this.SNMP.values != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Values:</font></th> <td><font size=\"-1\">" +                              "</font></td></tr>\n";
		if (this.SNMP.description != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Descrip:</font></th><td><font size=\"-1\">" + findTags(this.SNMP.description, topName) + "</font></td></tr>\n";
		if (this.SNMP.reference != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">Refer:</font></th>  <td><font size=\"-1\">" + findTags(this.SNMP.reference, topName) + "</font></td></tr>\n";
		if (this.SNMP.textualConvention != undefined) {
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">TC:</font></th>     <td><font size=\"-1\">" + this.SNMP.textualConvention + "</font></td></tr>\n";
		tc = tcs[this.SNMP.textualConvention];
		if (tc != undefined)
		html = html + "\t<tr><th align=\"right\" valign=\"top\"><font size=\"-1\">TC Desc:</font></th><td><font size=\"-1\">" + findTags(tc.desc, topName) +   "</font></td></tr>\n";
		}
		html = html + "</table>\n";
		return html;
	}
}

function getCookieVal(offset) {
	var endstr = document.cookie.indexOf(";", offset);
	if (endstr == -1) { endstr = document.cookie.length; }
	return unescape(document.cookie.substring(offset, endstr));
}

function GetCookie(name) {
	var arg = name + "=";
	var alen = arg.length;
	var clen = document.cookie.length;
	var i = 0;
	while (i < clen) {
		var j = i + alen;
		if (document.cookie.substring(i,j) == arg) {
			return getCookieVal(j);
		}
		i = document.cookie.indexOf(" ",i) + 1;
		if (i == 0) break;
	}
	return null;
}

function DeleteCookie(name) {
	if (GetCookie(name)) {
		document.cookie = name + "=" +
			"; expires=Thu, 01-Jan-70 00:00:01 GMT" +
			"; path=/";
	}
}

function SetCookie(name,value) {
	var expires = new Date();
	expires.setMonth(expires.getMonth()+1);
	document.cookie = name + "=" + escape(value) +
		"; expires=" + expires.toGMTString() +
		"; path=/";
}

function writeTree() {
	var cookie; var oids; var index; var oid; var obj; var html;

	cookie = GetCookie("mibmenu");
	if (cookie != null) {
		oids = cookie.split("^");
		for (index in oids) {
			oid = oids[index];
			if (oid != "") {
				obj = findMib(oid);
				if (obj != null) {
					obj.state = "o";
				}
			}
		}
	}

	menu = getMenuDivision();
	menu.innerHTML = obj0.returnHTML("top");
	renumberTree();

	cookie = GetCookie("history");
	if (cookie != null) {
		oids = cookie.split("^");
		historySelection = 1 * 0;
		for (index in oids) {
			oid = oids[index];
			if (oid != "") {
				selectionHistory[historySelection] = oid;
				historySelection = 1 * historySelection + 1;
			}
		}
	}
	cookie = GetCookie("position");
	if (cookie != null) {
		if (cookie > -1) {
			oid = selectionHistory[cookie];
			if (oid != null) {
				historySelection = 1 * cookie;
				obj = tree[oid];
				if (obj != null) { makeSelection(obj, false, true); }
			}
		}
	}
}

function saveMib(obj) {
	var cookie = "";
	var result;
	var i;
	if (obj != null) {
		if (obj.state == "o") {
			cookie = obj.oid;
		}
		if (obj.numchildren > 0) {
			for (i in obj.children) {
				result = saveMib(obj.children[i]);
				if (result != "") {
					if (cookie != "") {
						cookie = cookie + "^" + result;
					} else {
						cookie = result;
					}
				}
			}
		}
	}
	return cookie;
}

function saveTree() {
	var i; var oid;
	var win;
	for (win = dataWindows.pop(); win != null; win = dataWindows.pop()) {
		win.close();
	}
	var cookie = saveMib(obj0);
	SetCookie("mibmenu", cookie);
	cookie = "";
	for (i in selectionHistory) {
		oid = selectionHistory[i];
		if (cookie != "") {
			cookie = cookie + "^";
		}
		cookie = cookie + oid;
	}
	SetCookie("history", cookie);
	SetCookie("position", ""+historySelection+"");
}

function renumberSubTree(obj, start) {
	obj.number = start;
	start = 1 * start + 1;
	if (obj.state == "o") {
		for (i in obj.children) {
			start = renumberSubTree(obj.children[i], start);
		}
	}
	return start;
}

var totalNumber = 0;

function renumberTree() {
	totalNumber = renumberSubTree(obj0, 0);
}

function closeMib(obj) {
	var cookie; var ctrl; var icon; var children;
	var menuframe = getMenuFrame();
	if (obj.state == "c") { return; }
	obj.state = "c";
	ctrl = menuframe.document.getElementById(obj.oid+".ctrl");
	if (ctrl != null) { ctrl.src = images[obj.getCtrlName()]; }
	icon = menuframe.document.getElementById(obj.oid+".icon");
	if (icon != null) { icon.src = images[obj.getIconName()]; }
	children = menuframe.document.getElementById(obj.oid+".children");
	if (children != null) { children.innerHTML = ""; }
	renumberTree();
}
function openMib(obj) {
	var cookie; var ctrl; var icon; var children;
	var menuframe = getMenuFrame();
	if (obj.state == "o") { return; }
	obj.state = "o";
	ctrl = menuframe.document.getElementById(obj.oid+".ctrl");
	if (ctrl != null) { ctrl.src = images[obj.getCtrlName()]; }
	icon = menuframe.document.getElementById(obj.oid+".icon");
	if (icon != null) { icon.src = images[obj.getIconName()]; }
	children = menuframe.document.getElementById(obj.oid+".children");
	if (children != null) { children.innerHTML = obj.returnChildrenHTML("top"); }
	renumberTree();
}

function openUpwards(obj) {
	var superior = obj.superior;
	if (superior != null) { openUpwards(superior); }
	if (obj.state != "o") { openMib(obj); }
}

function onImageClick(oidOrName) {
	var obj = findMib(oidOrName);
	if (obj != null) {
		if (obj.state == "o") {
			closeMib(obj);
		} else {
			openMib(obj);
		}
	}
}

var selectedElement = null;
var selectionHistory = new Array();
var historySelection = 1 * -1;

function transparentColor(oidOrName) {
	obj = findMib(oidOrName);
	if (obj == null) { return "transparent"; }
	if (selectedElement != null && obj.oid == selectedElement.oid)
	{ return "lightgray"; } else { return "transparent"; }
}

function historyBack() {
	var oid; var obj; var ind;
	if (historySelection > -1) {
		historySelection = 1 * historySelection - 1;
		oid = selectionHistory[historySelection];
		if (oid == null) { return; }
		obj = tree[oid];
		if (obj == null) { return; }
		makeSelection(obj, false, true);
	}
}

function historySelect(index) {
	var oid; var obj; var ind;
	ind = 1 * index;
	if (selectionHistory[index] != null) {
		historySelection = ind;
		oid = selectionHistory[historySelection];
		if (oid == null) { return; }
		obj = tree[oid];
		if (obj == null) { return; }
		makeSelection(obj, false, true);
	}
}

function historyForward() {
	var oid; var obj;
	if (historySelection < selectionHistory.length) {
		historySelection = 1 * historySelection + 1;
		oid = selectionHistory[historySelection];
		if (oid == null) { return; }
		obj = tree[oid];
		if (obj == null) { return; }
		makeSelection(obj, false, true);
	}
}

var dataWindows = new Array();

function newDataWindow(obj) {
	var newwin;
	html = obj.returnDataHTML("opener");
	html = "<html><head><title>" + obj.name + "(" + obj.oid + ")</title></head><body>" + html + "</body></html>";
	newwin = open('about:blank', obj.name);
	newwin.txt = html;
	newwin.location.href = 'javascript:txt';
	newwin.document.title = obj.name + "(" + obj.oid + ")";
	return newwin;
}

function makeSelection(obj, makeHistory, jumpscroll) {
	var text; var html; var indexes; var children; var i; var cookie;
	var menuframe = getMenuFrame();
	if (selectedElement != null) {
		if (obj.oid == selectedElement.oid) { return; }
		text = menuframe.document.getElementById(selectedElement.oid + ".text");
		if (text != null) { text.style.backgroundColor='transparent'; }
	}
	if (makeHistory) {
		selectionHistory.push(obj.oid);
		while (selectionHistory.length > 20) {
			selectionHistory.shift();
		}
		historySelection = selectionHistory.length - 1;
	}
	text = menuframe.document.getElementById(obj.oid + ".text");
	if (text == null) {
		openUpwards(obj);
		text = menuframe.document.getElementById(obj.oid + ".text");
	}
	text.style.backgroundColor='lightgray';
	if (jumpscroll) {
		menuframe.document.body.scrollTop = menuframe.document.body.scrollHeight * obj.number / totalNumber - menuframe.innerHeight / 3;
	}
	menuframe.document.body.scrollLeft = (obj.level - 3) * 16;

	selectedElement = obj;
	if (obj.SNMP == null) { alert("Object " + obj.oid + " SNMP sub-object disappeared!"); return; }
	html = obj.returnDataHTML("top");
	data = getDataDivision();
	data.innerHTML = html;
}

function onElementClick(oidOrName) {
	var obj = findMib(oidOrName);
	if (obj == null) { return; }
	makeSelection(obj, true, false);
}

function onTagClick(oidOrName) {
	var obj = findMib(oidOrName);
	if (obj == null) { return; }
	makeSelection(obj, true, true);
}

function onElementDoubleClick(oidOrName) {
	var obj = findMib(oidOrName);
	if (obj == null) { return; }
	var win = newDataWindow(obj);
	dataWindows.push(win);
}

function getMib(oid, name, sid, mid, mtype, access, status, syntax, image, color, superior) {
	if (tree[oid] == undefined) {
		tree[oid] = new Mib(oid, name, sid, mid, mtype, access, status, syntax, image, color, superior);
	}
	return tree[oid];
}

function xbDetectBrowser()
{
	var oldOnError = window.onerror;
	window.onerror = null;

	// work around bug in xpcdom Mozilla 0.9.1
	window.saveNavigator = window.navigator;
	var ua = window.navigator.userAgent.toLowerCase();

	if ((ua.indexOf('msie') != -1) || (ua.indexOf('konqueror') != -1) || (ua.indexOf('gecko') != -1)) {
		browserVersion = 1;
	} else
	if ((ua.indexOf('mozilla') !=-1) && (ua.indexOf('spoofer')==-1) && (ua.indexOf('compatible') == -1) && (ua.indexOf('opera')==-1)&& (ua.indexOf('webtv')==-1) && (ua.indexOf('hotjava')==-1)) {
		browserVersion = 2;
	} else {
		browserVersion = 0;
	}

	window.onerror = oldOnError;
}

xbDetectBrowser();
