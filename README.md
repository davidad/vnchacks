Using VNC as a graphics library
===

VNC's protocol, Remote Frame Buffer (RFB) is a fairly sane way to stream
graphics and to receive keyboard and mouse events. A graphical application can
theoretically be implemented as an RFB server. This is a simple demo of how
such an app would work. Currently, it doesn't accept any input, but just
streams frames of a solid color rotating through the HCL color space (which I
devised and implemented for other projects). I also haven't tested it with many
implementations of VNC, but to my knowledge it conforms to the RFB 3.3
specification, so any conforming viewer should work.

    $ make
    $ ./color_rotate_zrle &
    $ vncviewer localhost:4092

Future work:
* Split out encoding to a `send_rect` function
* Parse client messages
* Provide user input handlers
* Consider an SDL backend: same `send_rect` and `register_handler` likely apply
* Implement a box model to route user input to interface elements
* Implement font rendering with FreeType
* Implement TeX+TikZ style graphics (big job)
* Create useful interface elements for this platform
