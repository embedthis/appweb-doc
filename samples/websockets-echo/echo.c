/*
    echo.c - WebSockets echo server
 */
#include "esp.h"

/*
    Event callback. Invoked for incoming web socket messages and other events of interest.
 */
static void echo_callback(HttpStream *stream, int event, int arg)
{
    HttpPacket      *packet;

    if (event == HTTP_EVENT_READABLE) {
        /*
            Grab the packet off the read queue.
         */
        packet = httpGetPacket(stream->readq);
        if (packet->type == WS_MSG_TEXT || packet->type == WS_MSG_BINARY) {
            /*
                Echo back the contents
             */
            httpSendBlock(stream, packet->type, httpGetPacketStart(packet), httpGetPacketLength(packet), 0);
        }
    } else if (event == HTTP_EVENT_APP_CLOSE) {
        mprLog("info echo", 0, "close event. Status status %d, orderly closed %d, reason %s", arg,
        httpWebSocketOrderlyClosed(stream), httpGetWebSocketCloseReason(stream));

    } else if (event == HTTP_EVENT_ERROR) {
        mprLog("info echo", 0, "error event");

    } else if (event == HTTP_EVENT_DESTROY) {
        mprLog("info echo", 0, "client disconnected");
    }
}


/*
    Action to run in response to the "test/echo" URI
 */
static void echo_action() {
    /*
        Don't automatically finalize (complete) the request when this routine returns. This keeps the connection open.
     */
    dontAutoFinalize();

    /*
        Establish the event callback
     */
    espSetNotifier(getConn(), echo_callback);
}


/*
    Initialize the "echo" loadable module
    The default ESP name for a controller is "app" if there is no esp.json to define the app name.
 */
ESP_EXPORT int esp_controller_app_echo(HttpRoute *route, MprModule *module) {
    /*
        Define the "echo" action that will run when the "test/echo" URI is invoked
     */
    espDefineAction(route, "test/echo", echo_action);
    return 0;
}
